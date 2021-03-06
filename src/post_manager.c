#include "post.h"
#include "post_manager.h"
#include "parser.h"
#include "mpi.h"
#include "map.h"
#include "map_iterator.h"
#include "utils.h"
#include "constants.h"
#include "types.h"
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include "output.h"

//just to avoid calling map_size on posts (low efficiency)
int posts_size;

void delete_inactive_posts(map_t posts_to_delete, map_t posts) {

	void *iterator = map_it_init(posts_to_delete);
	long k;
	post *p;
	while(map_it_hasnext(posts_to_delete, iterator)){
	        k = map_it_next(posts_to_delete, &iterator);
	        //printf("POST MANAGER: deleting post %ld ...\n", k);
	        p = map_get(posts_to_delete, k);
	        posts = map_remove(posts, k);
	        post_delete(p);
	        printf("POST MANAGER: post %ld deleted\n", k);
	}
	posts_to_delete = map_empty(posts_to_delete);

}

void daily_decrement(map_t posts, long current_ts, post* best_three[], FILE** output) {
    void *iterator = map_it_init(posts);
    long k;
    post *p;
    bool order_changed = false, is_daily_decrement = true;
    map_t posts_to_delete = map_init();
    int posts_to_delete_size = 0;

    //MPI_Datatype MPI_out_tuple = serialize_out_tuple();

    printf("POST MANAGER: daily decrement started\n");
    while(map_it_hasnext(posts, iterator)){
    	long lifetime, num_of_dec, pod = PERIOD_OF_DECR;
    	int delta;
    	k = map_it_next(posts, &iterator);
        //printf("POST_ITERATOR: next=%ld\n", k);

        p = map_get(posts, k);

    	//time elapsed since last decrement
		lifetime = current_ts - p->ts;
		//printf("lifetime= %ld\n", lifetime/(1000*3600));
		//correct number of decrements
		num_of_dec = lifetime / pod;
		//num of points to be subtracted to the score
		delta = (int) (num_of_dec - p->num_of_dec) * (-1);

        if(delta != 0){

        	bool is_active;
        	//printf("POST MANAGER: post %ld is gonna be incremented by %d\n", p->post_id, delta);
            is_active = post_update_score(p, delta, is_daily_decrement);
            //printf("POST MANAGER: post %ld is active? %d\n", p->post_id, is_active);
            if(!is_active){
            	posts_to_delete = map_put(posts_to_delete, k, p);
            }

        }
        if(out_compare_with_best(best_three, p))
        	order_changed = true;
    }
    //delete of the inactive posts
    posts_to_delete_size = map_size(posts_to_delete);
    printf("POST MANAGER: I have to delete %d posts\n", posts_to_delete_size);
    posts_size = posts_size - posts_to_delete_size;
    delete_inactive_posts(posts_to_delete, posts);
    //print the best 3
    if(order_changed) {
    	out_print_best(best_three, current_ts, output);
    }

}



void parallel_process_post (struct post *p, long current_ts, map_t *posts_to_delete, post* best_three[], bool *order_changed)
{
	long lifetime, num_of_dec, pod = PERIOD_OF_DECR;
	int delta;
	//time elapsed since last decrement
	lifetime = current_ts - p->ts;
	//printf("lifetime= %ld\n", lifetime/(1000*3600));
	//correct number of decrements
	num_of_dec = lifetime / pod;
	//num of points to be subtracted to the score
	delta = (int) (num_of_dec - p->num_of_dec) * (-1);

	bool is_active = true;
	if(delta != 0){
		//printf("POST MANAGER: post %ld is gonna be incremented by %d\n", p->post_id, delta);
		is_active = post_update_score(p, delta, true);
		//printf("POST MANAGER: post %ld is active? %d\n", p->post_id, is_active);
	}
	#pragma omp critical
	{
		if(!is_active){
			*posts_to_delete = map_put(*posts_to_delete, p->post_id, p);
		}
		if(out_compare_with_best(best_three, p))
			*order_changed = true;
	}
}


void parallel_daily_decrement(map_t posts, long current_ts, post* best_three[], FILE** output) {
    void *iterator = map_it_init(posts);
    long k;
    post *p;
    bool order_changed = false, is_daily_decrement = true;
    map_t posts_to_delete = map_init();
    int posts_to_delete_size;
    #pragma omp parallel shared (posts, order_changed, posts_to_delete) num_threads(NUM_OF_THREADS)
    {
        // one thread adds all tasks to the queue
        #pragma omp single
            while(map_it_hasnext(posts, iterator)){
                k = map_it_next(posts, &iterator);
                p = map_get(posts, k);
                #pragma omp task firstprivate (p)
                    // task is inserted in the queue and an available
                    // thread will execute it
                    parallel_process_post(p, current_ts, &posts_to_delete, best_three, &order_changed);
            }
    }
    //delete of the inactive posts
    posts_to_delete_size = map_size(posts_to_delete);
	printf("POST MANAGER: I have to delete %d posts\n", posts_to_delete_size);
	posts_size = posts_size - posts_to_delete_size;
	delete_inactive_posts(posts_to_delete, posts);
	//print the best 3
	if(order_changed) {
		out_print_best(best_three, current_ts, output);
	}
}

void post_manager_run(char *path){
	time_t stop_time = STOP;
    int count, i=0;
    ts_rank current_tr;
    MPI_Status stat;
    map_t posts = map_init();
    struct post* p = NULL;
    post *best_three[NUM_OF_BEST] = {NULL, NULL, NULL};
    post_increment pi;

    posts_size = 0;

    FILE *input, *output;
    if(path[0]!='\0')
    	input = fopen(path, "r");
    else
    	input = stdin;
    if(!input) {
    		 printf("file: %s\n" , POSTS_FILE);
             perror("Error opening file");
             return;
    }
    output = fopen(OUTPUT_FILE, "wb");
    if(!output) {
			 printf("file: %s\n" , OUTPUT_FILE);
			 perror("Error opening file");
			 return;
	}

    while(p = parser_next_post(&input)) {
    	//printf("POST_MANAGER: Post read: %ld, %ld\n", p->post_id, p->ts);
        posts = map_put(posts, p->post_id, p);
        posts_size++;
        printf("POST_MANAGER: Map size= %d\n", posts_size);
        // TODO: SSsend or Send? Blocking
        // Send timestamp of latest post
        MPI_Send(&(p->ts), 1, MPI_LONG, MASTER, GENERIC_TAG, MPI_COMM_WORLD);
        // Receive current timestamp from master
        MPI_Bcast(&current_tr, 1, MPI_LONG_INT, MASTER, MPI_COMM_WORLD);
        printf("POST_MANAGER: Current ts is: %ld, from process %d\n", current_tr.ts, current_tr.rank);

        while (p->ts > current_tr.ts || current_tr.rank == COMMENT_MANAGER)
        {

        	printf("POST_MANAGER: waiting for comments updates\n");
            // Wait for points coming from comments -> gets number of posts to update
            MPI_Recv(&count, 1, MPI_INT, COMMENT_MANAGER, GENERIC_TAG, MPI_COMM_WORLD, &stat);
            // gets the pairs (post_id, delta_points)
            for(i=0; i<count; i++){
            	//distinguish pair by tag
                long commenter_id = -1;

                MPI_Recv(&pi, 1, MPI_LONG_INT, COMMENT_MANAGER, MPI_ANY_TAG, MPI_COMM_WORLD, &stat);

                post* post = map_get(posts, pi.post_id);
                if(stat.MPI_TAG == NEW_COMMENT_UPDATE) {

                	MPI_Recv(&commenter_id, 1, MPI_LONG, COMMENT_MANAGER, MPI_ANY_TAG, MPI_COMM_WORLD, &stat);

                    //since we update the global timestamp only when a comment or a post is read
                    //and post_manager is waiting, current_ts is the timestamp of the comment we are receiving
                	post_add_comments_info(post, commenter_id, current_tr.ts);
                }

                //since these update aren't due to post own daily drecrement, is_daily_decrement is false in both cases
                bool is_active = post_update_score(post, pi.increment, false);
                if(!is_active){
                    posts = map_remove(posts, pi.post_id);
                    posts_size--;
                    post_delete(post);
                }

            }
            // Update score of posts (24h decrement) and the best three
            parallel_daily_decrement(posts, current_tr.ts, best_three, &output);

            //read next timestamp
            MPI_Bcast(&current_tr, 1, MPI_LONG_INT, MASTER, MPI_COMM_WORLD);


        }
        parallel_daily_decrement(posts, current_tr.ts, best_three, &output);
    }
    fclose(input);
    printf("POST_MANAGER: Sending stop to master\n");
    MPI_Send(&stop_time, 1, MPI_LONG, MASTER, GENERIC_TAG, MPI_COMM_WORLD);
}
