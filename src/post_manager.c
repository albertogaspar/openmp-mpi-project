#include "post.h"
#include "post_manager.h"
#include "mpi.h"
#include "map.h"
#include "map_iterator.h"
#include "utils.h"
#include "constants.h"
#include "types.h"
#include <omp.h>

//period of decrementation of the score in milliseconds
#define PERIOD_OF_DECR 24 * 3600 * 1000

void daily_decrement(map_t posts, long current_ts) {
    void *iterator = map_it_init(posts);
    long k;
    post *p;
    MPI_Datatype MPI_out_tuple = serialize_out_tuple();

    while(map_it_hasnext(posts, iterator)){
        k = map_it_next(posts, &iterator);
        p = map_get(posts, k);
    	//time elapsed since last decrementation
		long lifetime = current_ts - p->ts;
		//correct number of decrementation
		long num_of_dec = lifetime % PERIOD_OF_DECR;
		//num of points to be subtracted to the score
		long delta = num_of_dec - p->num_of_dec;

        if(delta != 0){
            bool is_active = post_update_score(p, delta, false);
            if(!is_active){
                post_delete(p);
                posts = map_remove(posts, k);
            }
            else{
                out_tuple ot = out_create_tuple(p);
                MPI_Send(&ot, 1, MPI_out_tuple, OUT_MANAGER, GENERIC_TAG, MPI_COMM_WORLD);
            }
        }
    }
}

void process_post (struct post *p, current_ts)
{
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
            post_delete(p);
            //posts = map_remove(posts, k);
        }
        else{
            // send data to out manager
            out_tuple ot = out_create_tuple(p);
            MPI_Send(&ot, 1, MPI_out_tuple, OUT_MANAGER, GENERIC_TAG, MPI_COMM_WORLD);
        }
    }
}

void parallel_daily_decrement(map_t posts, long current_ts) {
    void *iterator = map_it_init(posts);
    long k;
    post *p;
    MPI_Datatype MPI_out_tuple = serialize_out_tuple();
    #pragma omp paralell shared (posts, MPI_out_tuple)
    {
        // one thread adds all taks to the queue
        #pragma omp single
            while(map_it_hasnext(posts, iterator)){
                k = map_it_next(posts, &iterator);
                p = map_get(posts, k);
                #pragma omp task firstprivate (p)
                    // task is inserted in the queue and an available
                    // thread will execute it
                    process_post(p, current_ts);
            }
    }
}

void post_manager_run(){
    int count;
    long int current_ts;
    MPI_Status stat;
    map_t posts = map_init();
    struct post* p = NULL;
    MPI_Datatype MPI_out_tuple = serialize_out_tuple();

    while(p = parser_next_post()) {
        map_put(posts, p->post_id, p);
        // TODO: SSsend or Send? Blocking
        // Send timestamp of latest post
        MPI_Send(&(p->ts), 1, MPI_LONG, MASTER, GENERIC_TAG, MPI_COMM_WORLD);
        // Receive current timestamp from master
        MPI_Recv(&current_ts, 1, MPI_LONG, MASTER, GENERIC_TAG, MPI_COMM_WORLD, &stat);
        // Update score of posts (24h decrement)
        daily_decrement(posts, current_ts);
        while (p->ts > current_ts)
        {
            out_tuple ot = out_create_tuple(p);
            MPI_Send(&ot, 1, MPI_out_tuple, OUT_MANAGER, GENERIC_TAG, MPI_COMM_WORLD);
            // Wait for points coming from comments -> gets number of posts to update
            MPI_Recv(&count, 1, MPI_LONG, COMMENT_MANAGER, GENERIC_TAG, MPI_COMM_WORLD, &stat);
            // gets the pairs (post_id, delta_points)
            for(i=0; i<count; i++){
            	//distinguish pair by tag
            	int increment;
                long post_id, commenter_id = -1;

                MPI_Recv(&post_id, 1, MPI_LONG, COMMENT_MANAGER, MPI_ANY_TAG, MPI_COMM_WORLD, &stat);

                post* post = map_get(posts, post_id);
                if(stat.MPI_TAG == NEW_COMMENT) {
                    //TODO: *****add tags NEW_COMMENT and DECREMENT******
                    MPI_Recv(&commenter_id, 1, MPI_LONG, COMMENT_MANAGER, MPI_ANY_TAG, MPI_COMM_WORLD, &stat);

                    increment = STARTING_SCORE;
                    //since we update the global timestamp only when a comment or a post is read
                    //and post_manager is waiting, current_ts is the timestamp of the comment we are receiving
                	post_add_comments_info(post, commenter_id, current_ts);
                }
                else {
                	increment = DAILY_DECREMENT;
                }
                //since these update aren't due to post own daily drecrement, is_daily_decrement is false in both cases
                bool is_active = post_update_score(post, increment, false);
                if(!is_active){
                    post_delete(post);
                    posts = map_remove(posts, post_id);
                }
                else{
                    out_tuple ot = out_create_tuple(p);
                    MPI_Send(&ot, 1, MPI_out_tuple, OUT_MANAGER, GENERIC_TAG, MPI_COMM_WORLD);
                }

            }

            //read next timestamp
            MPI_Recv(&current_ts, 1, MPI_LONG, MASTER, GENERIC_TAG, MPI_COMM_WORLD, &stat);
            daily_decrement(posts, current_ts);

        }
    }
}
