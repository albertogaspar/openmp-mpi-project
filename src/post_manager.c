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

void delete_inactive_posts(map_t posts_to_delete, map_t posts) {

	void *iterator = map_it_init(posts_to_delete);
	long k;
	post *p;
	while(map_it_hasnext(posts_to_delete, iterator)){
	        k = map_it_next(posts_to_delete, &iterator);
	        printf("POST MANAGER: deleting post %ld ...\n", k);
	        p = map_get(posts_to_delete, k);
	        printf("POST MANAGER: map size = %d\n", map_size(posts));
	        posts = map_remove(posts, k);
	        printf("POST MANAGER: map size = %d\n", map_size(posts));
	        post_delete(p);
	        printf("POST MANAGER: post %ld deleted\n", k);
	}
	posts_to_delete = map_empty(posts_to_delete);

}

void daily_decrement(map_t posts, long current_ts, post* best_three[]) {
    void *iterator = map_it_init(posts);
    long k;
    post *p;
    bool order_changed = false, is_daily_decrement = true;
    map_t posts_to_delete = map_init();

    //MPI_Datatype MPI_out_tuple = serialize_out_tuple();

    printf("POST MANAGER: daily decrement started\n");
    while(map_it_hasnext(posts, iterator)){
    	long lifetime, num_of_dec, pod = PERIOD_OF_DECR;
    	int delta;
    	k = map_it_next(posts, &iterator);
        printf("POST_ITERATOR: next=%ld\n", k);

        p = map_get(posts, k);

    	//time elapsed since last decrement
		lifetime = current_ts - p->ts;
		printf("lifetime= %ld\n", lifetime/(1000*3600));
		//correct number of decrements
		num_of_dec = lifetime / pod;
		//num of points to be subtracted to the score
		delta = (int) (num_of_dec - p->num_of_dec) * (-1);

        if(delta != 0){

        	bool is_active;
        	printf("POST MANAGER: post %ld is gonna be incremented by %d\n", p->post_id, delta);
            is_active = post_update_score(p, delta, is_daily_decrement);
            printf("POST MANAGER: post %ld is active? %d\n", p->post_id, is_active);
            if(!is_active){
            	posts_to_delete = map_put(posts_to_delete, k, p);
            }

        }
        if(out_compare_with_best(best_three, p))
        	order_changed = true;
    }
    //delete of the inactive posts
    printf("POST MANAGER: I have to delete %d posts\n", map_size(posts_to_delete));
    delete_inactive_posts(posts_to_delete, posts);
    //print the best 3
    if(order_changed) {
    	out_print_best(best_three, current_ts);
    }

}



void process_post (struct post *p, long current_ts, bool *inactive_post_exists)
{
    //MPI_Datatype MPI_out_tuple = serialize_out_tuple();
    //time elapsed since last decrementation
    long lifetime = current_ts - p->ts;
    //correct number of decrementation
    long num_of_dec = lifetime % PERIOD_OF_DECR;
    //num of points to be subtracted to the score
    long delta = num_of_dec - p->num_of_dec;

    if(delta != 0){
        // update score and check if post is till active after update, if not delte it.
        bool is_active = post_update_score(p, delta, false);
        if(!is_active){
            //post_delete(p);
            //posts = map_remove(posts, k);
        	*inactive_post_exists = true;
        }
        //TODO:
    }
}

void parallel_daily_decrement(map_t posts, long current_ts) {
    void *iterator = map_it_init(posts);
    long k;
    post *p;
    bool inactive_post_exists = false;
    #pragma omp parallel shared (posts, inactive_post_exists)
    {
        // one thread adds all taks to the queue
        #pragma omp single
            while(map_it_hasnext(posts, iterator)){
                k = map_it_next(posts, &iterator);
                p = map_get(posts, k);
                #pragma omp task firstprivate (p)
                    // task is inserted in the queue and an available
                    // thread will execute it
                    process_post(p, current_ts, &inactive_post_exists);
            }
    }
}

void post_manager_run(){
    int count, i=0;
    ts_rank current_tr;
    MPI_Status stat;
    map_t posts = map_init();
    struct post* p = NULL;
    post *best_three[NUM_OF_BEST] = {NULL, NULL, NULL};
    //MPI_Datatype MPI_out_tuple = serialize_out_tuple();

    FILE *file;
    file = fopen(POSTS_FILE, "r");
    if(!file) {
    		 printf("file: %s\n" , POSTS_FILE);
             perror("Error opening file");
             return;
    }

    while(p = parser_next_post(&file)) {
    	printf("POST_MANAGER: Post read: %ld, %ld\n", p->post_id, p->ts);
        posts = map_put(posts, p->post_id, p);
        printf("POST_MANAGER: Map size= %d\n", map_size(posts));
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
            MPI_Recv(&count, 1, MPI_LONG, COMMENT_MANAGER, GENERIC_TAG, MPI_COMM_WORLD, &stat);
            // gets the pairs (post_id, delta_points)
            for(i=0; i<count; i++){
            	//distinguish pair by tag
            	int increment;
                long post_id, commenter_id = -1;

                MPI_Recv(&post_id, 1, MPI_LONG, COMMENT_MANAGER, MPI_ANY_TAG, MPI_COMM_WORLD, &stat);

                post* post = map_get(posts, post_id);
                if(stat.MPI_TAG == NEW_COMMENT_UPDATE) {

                	MPI_Recv(&commenter_id, 1, MPI_LONG, COMMENT_MANAGER, MPI_ANY_TAG, MPI_COMM_WORLD, &stat);

                    increment = STARTING_SCORE;
                    //since we update the global timestamp only when a comment or a post is read
                    //and post_manager is waiting, current_ts is the timestamp of the comment we are receiving
                	post_add_comments_info(post, commenter_id, current_tr.ts);
                }
                else {
                	increment = DAILY_DECREMENT;
                }
                //since these update aren't due to post own daily drecrement, is_daily_decrement is false in both cases
                bool is_active = post_update_score(post, increment, false);
                if(!is_active){
                    posts = map_remove(posts, post_id);
                    post_delete(post);
                }

            }
            // Update score of posts (24h decrement) and the best three
            daily_decrement(posts, current_tr.ts, best_three);

            //read next timestamp
            MPI_Bcast(&current_tr, 1, MPI_LONG_INT, MASTER, MPI_COMM_WORLD);


        }
        daily_decrement(posts, current_tr.ts, best_three);
    }
    fclose(file);
}
