#include "mpi.h"
#include "map.h"
#include "map_iterator.h"
#include "utils.h"
#include "app_constants.h"
#include "comment_manager.h"
#include "comment.h"

/*
 * Set commented_post field to the real post id, if c is a reply to another comment
 * the value is retrieved from the parent comment which is always up to date
 */
void set_commented_post(map_t comments, comment* c) {
	if(c->commented_post == -1) {
		comment* comment_replied = map_get(comments, c->comment_replied);
		c->commented_post = comment_replied->commented_post;
	}
}

void comment_manager_run(){
	time_t stop_time = STOP;
    thread_safe = false;
    int count;
    long int current_ts;
    MPI_Status stat;
    map_t comments = map_init();
    map_t posts_to_update = map_init();
    struct comment* c = NULL;

    while(c = parser_next_comment()) {

    	//get the commented post
    	set_commented_post(comments, c);

    	//save new comment in the comments map
        map_put(comments, c->comment_id, c);
        // TODO: SSsend or Send? Blocking
        // Send timestamp of latest post
        MPI_Send(&(c->ts), 1, MPI_LONG, MASTER, GENERIC_TAG, MPI_COMM_WORLD);
        // Receive current timestamp from master
        MPI_Recv(&current_ts, 1, MPI_LONG, MASTER, GENERIC_TAG, MPI_COMM_WORLD, &stat);

        //if the received timestamp is greater or equal than my ts, then compute updates and read next
        //else if it's less than my ts, wait until the post manager reaches at least my ts
        while (c->ts > current_ts)
        {
        	//read next timestamp
        	MPI_Recv(&current_ts, 1, MPI_LONG, MASTER, GENERIC_TAG, MPI_COMM_WORLD, &stat);
        }

		void *iterator;
		long post_id;
		post_increment* pi;
		//calculate 24H decrements and fill posts_to_update list
		daily_decrement(comments, posts_to_update, current_ts);

		// Send number of posts that need to be updated
		count = map_size(posts_to_update) + 1;
		MPI_Send(&count, 1, MPI_LONG, POST_MANAGER, GENERIC_TAG, MPI_COMM_WORLD);


		iterator = map_it_init(posts_to_update);

		//send all the daily updates to post manager
		while(map_it_hasnext(posts_to_update, iterator)){

			post_id = map_it_next(posts_to_update, &iterator);
			pi = map_get(posts_to_update, post_id);

			MPI_Send(pi, 1, MPI_LONG_INT, POST_MANAGER, DECREMENT_UPDATE, MPI_COMM_WORLD);

		}
		//send the new comment to post manager
		pi->post_id = c->commented_post;
		pi->increment = STARTING_SCORE;
		MPI_Send(pi, 1, MPI_LONG_INT, POST_MANAGER, NEW_COMMENT_UPDATE, MPI_COMM_WORLD);
		//send the user id of the commenter
		MPI_Send(&c->user_id, 1, MPI_LONG, POST_MANAGER, GENERIC_TAG, MPI_COMM_WORLD);


    }
    MPI_Send(&stop_time, 1, MPI_LONG, MASTER, GENERIC_TAG, MPI_COMM_WORLD);
}


void daily_decrement(map_t comments, map_t posts_to_update,long current_ts) {
    void *iterator = map_it_init(comments);
    long k;
    comment *c;

    while(map_it_hasnext(comments, iterator)){

        k = map_it_next(comments, &iterator);
        c = map_get(comments, k);
    	//time elapsed since last decrementation
		long lifetime = current_ts - c->ts;
		//correct number of decrementation
		long num_of_dec = lifetime % PERIOD_OF_DECR;
		//num of points to be subtracted to the score
		long delta = num_of_dec + c->num_of_dec*DAILY_DECREMENT;

        if(delta != 0){
            bool is_active = comment_update_score(c, delta, false);

            long post_id = c->commented_post;

			post_increment *pi = map_get(posts_to_update, post_id);
			//if the post is not in the list, it means that it is the first time that
			//an increment is recorded for the post
			if( pi == NULL ) {
				pi = (post_increment*) malloc(sizeof(post_increment));
				pi->post = post_id;
				pi->increment = delta;
				map_put(posts_to_update, post_id, pi);
			}
			//the post is already recorded in the list, so we update the increment with
			//the one of the current comment
			else {
				pi->increment = pi->increment + delta;
			}

            if(!is_active){
                comment_delete(c);
                comments = map_remove(comments, k);
            }


        }

    }
}
