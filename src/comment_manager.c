#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"
#include "map.h"
#include "map_iterator.h"
#include "utils.h"
#include "constants.h"
#include "types.h"
#include "comment_manager.h"
#include "comment.h"
#include "parser.h"

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

void delete_inactive_comments(map_t comments_to_delete, map_t comments) {

	void *iterator = map_it_init(comments_to_delete);
	long k;
	comment *c;
	while(map_it_hasnext(comments_to_delete, iterator)){
	        k = map_it_next(comments_to_delete, &iterator);
	        printf("COMMENT MANAGER: deleting comment %ld ...\n", k);
	        c = map_get(comments_to_delete, k);
	        comments = map_remove(comments, k);
	        comment_delete(c);
	        printf("COMMENT MANAGER: comment %ld deleted\n", k);
	}
	comments_to_delete = map_empty(comments_to_delete);
	printf("COMMENT MANAGER: delete completed\n");

}

void comment_daily_decrement(map_t comments, map_t posts_to_update,long current_ts) {
    void *iterator = map_it_init(comments);
    long k;
    comment *c;
    map_t comments_to_delete = map_init();

    printf("COMMENT MANAGER: daily decrement started\n");
    while(map_it_hasnext(comments, iterator)){

    	long lifetime, num_of_dec, pod = PERIOD_OF_DECR;
    	int delta;
        k = map_it_next(comments, &iterator);
        c = map_get(comments, k);
    	//time elapsed since last decrementation
		lifetime = current_ts - c->ts;
		//correct number of decrementation
		num_of_dec = lifetime / pod;
		//num of points to be subtracted to the score
		delta = (int) (num_of_dec - c->num_of_dec) * (-1);

        if(delta != 0){
        	long post_id;
        	post_increment *pi;
            bool is_active = comment_update_score(c, delta, false);
            post_id = c->commented_post;

			pi = map_get(posts_to_update, post_id);
			//if the post is not in the list, it means that it is the first time that
			//an increment is recorded for the post
			if( pi == NULL ) {
				pi = (post_increment*) malloc(sizeof(post_increment));
				pi->post_id = post_id;
				pi->increment = delta;
				posts_to_update = map_put(posts_to_update, post_id, pi);
			}
			//the post is already recorded in the list, so we update the increment with
			//the one of the current comment
			else {
				pi->increment = pi->increment + delta;
			}

            if(!is_active){
                comments_to_delete = map_put(comments_to_delete, k, c);
            }
        }
    }
    //delete of the inactive posts
    printf("COMMENT MANAGER: I have to delete %d comments\n", map_size(comments_to_delete));
    delete_inactive_comments(comments_to_delete, comments);
}



void comment_manager_run(){
	time_t stop_time = STOP;
    int count;
    ts_rank current_tr;
    MPI_Status stat;
    map_t comments = map_init();
    map_t posts_to_update = map_init();
    struct comment* c = NULL;
    post_increment pi;

    FILE *file;
	file = fopen(COMMENTS_FILE,"r");
	if(!file)
	{
		printf("Error opening file\n");
		return;
	}
    while(c = parser_next_comment(&file)) {

    	//get the commented post
    	printf("COMMENT_MANAGER: Comment read: %ld\n", c->comment_id);
    	set_commented_post(comments, c);

    	//save new comment in the comments map
        comments = map_put(comments, c->comment_id, c);
        printf("COMMENT_MANAGER: Comments size = %d\n", map_size(comments));
        // Send timestamp of latest post
        MPI_Send(&(c->ts), 1, MPI_LONG, MASTER, GENERIC_TAG, MPI_COMM_WORLD);
        // Receive current timestamp from master
        MPI_Bcast(&current_tr, 1, MPI_LONG_INT, MASTER, MPI_COMM_WORLD);
        printf("COMMENT_MANAGER: Current ts is: %ld, from process %d\n", current_tr.ts, current_tr.rank);
        //if the received timestamp is greater or equal than my ts, then compute updates and read next
        //else if it's less than my ts, wait until the post manager reaches at least my ts
        while (c->ts >= current_tr.ts && current_tr.rank != COMMENT_MANAGER )
        {
        	//read next timestamp
        	MPI_Bcast(&current_tr, 1, MPI_LONG_INT, MASTER, MPI_COMM_WORLD);
        }

		void *iterator;
		long post_id;
		//calculate 24H decrements and fill posts_to_update list
		comment_daily_decrement(comments, posts_to_update, current_tr.ts);

		// Send number of posts that need to be updated
		count = map_size(posts_to_update) + 1;
		MPI_Send(&count, 1, MPI_INT, POST_MANAGER, GENERIC_TAG, MPI_COMM_WORLD);


		iterator = map_it_init(posts_to_update);

		//send all the daily updates to post manager
		while(map_it_hasnext(posts_to_update, iterator)){

			post_increment *pi_ptr;
			post_id = map_it_next(posts_to_update, &iterator);
			pi_ptr = map_get(posts_to_update, post_id);
			pi.post_id = pi_ptr->post_id;
			pi.increment = pi_ptr->increment;
			MPI_Send(&pi, 1, MPI_LONG_INT, POST_MANAGER, DECREMENT_UPDATE, MPI_COMM_WORLD);

		}
		//send the new comment to post manager
		pi.post_id = c->commented_post;
		pi.increment = STARTING_SCORE;
		printf("COMMENT MANAGER: sending new comment for post %ld\n", pi.post_id);
		MPI_Send(&pi, 1, MPI_LONG_INT, POST_MANAGER, NEW_COMMENT_UPDATE, MPI_COMM_WORLD);
		//send the user id of the commenter
		MPI_Send(&(c->user_id), 1, MPI_LONG, POST_MANAGER, GENERIC_TAG, MPI_COMM_WORLD);


    }
    MPI_Send(&stop_time, 1, MPI_LONG, MASTER, GENERIC_TAG, MPI_COMM_WORLD);
}
