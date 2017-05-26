#include "post.h"
#include "post_manager.h"
#include "mpi.h"
#include "map.h"
#include "map_iterator.h"
#include "utils.h"
#include "app_constants.h"

void post_manager_run(){
	time_t stop_time = STOP;
    int count;
    ts_rank current_tr;
    MPI_Status stat;
    map_t posts = map_init();
    struct post* p = NULL;
    MPI_Datatype MPI_out_tuple = serialize_out_tuple();

    FILE *file;
	file = fopen(POST_FILE,"r");
	if(!file)
	{
		printf("Error opening file\n");
		return -1;
	}
    while(p = parser_next_post(file)) {
        map_put(posts, p->post_id, p);
        // TODO: SSsend or Send? Blocking
        // Send timestamp of latest post
        MPI_Send(&(p->ts), 1, MPI_LONG, MASTER, GENERIC_TAG, MPI_COMM_WORLD);
        // Receive current timestamp from master
    	MPI_Bcast(&current_tr, 1, MPI_LONG_INT, MASTER, MPI_COMM_WORLD);
        // Update score of posts (24h decrement)
        daily_decrement(posts, current_ts);

        while (p->ts > current_tr->ts)
        {

            // Wait for points coming from comments -> gets number of posts to update
            MPI_Recv(&count, 1, MPI_LONG, COMMENT_MANAGER, GENERIC_TAG, MPI_COMM_WORLD, &stat);
            // gets the pairs (post_id, delta_points)
            for(i=0; i<count; i++){
            	//distinguish pair by tag
            	int increment;
                long commenter_id = -1;
                post_increment pi;
                post* post;

                MPI_Recv(&pi, 1, MPI_LONG_INT, COMMENT_MANAGER, MPI_ANY_TAG, MPI_COMM_WORLD, &stat);

                post = map_get(posts, pi.post_id);
                if(stat.MPI_TAG == NEW_COMMENT_UPDATE) {
                    //receive the commenter userid
                    MPI_Recv(&commenter_id, 1, MPI_LONG, COMMENT_MANAGER, GENERIC_TAG, MPI_COMM_WORLD, &stat);
                    //since we update the global timestamp only when a comment or a post is read
                    //and post_manager is waiting, current_ts is the timestamp of the comment we are receiving
                	post_add_comments_info(post, commenter_id, current_ts);
                }

                increment = pi.increment;

                //since these update aren't due to post own daily drecrement, is_daily_decrement is false in both cases
                bool is_active = post_update_score(post, increment, false);
                if(!is_active){
                    post_delete(post);
                    posts = map_remove(posts, post_id);
                }
                else{
                    out_tuple ot = out_create_tuple(post);
                    MPI_Send(&ot, 1, MPI_out_tuple, OUT_MANAGER, GENERIC_TAG, MPI_COMM_WORLD);
                }

            }

            //read next timestamp
            MPI_Bcast(&current_tr, 1, MPI_LONG_INT, MASTER, MPI_COMM_WORLD);
            daily_decrement(posts, current_ts);
            
        }
        //if my timestamp, send the new post to out and read next
        out_tuple ot = out_create_tuple(p);
        MPI_Send(&ot, 1, MPI_out_tuple, OUT_MANAGER, GENERIC_TAG, MPI_COMM_WORLD);
    }
    //finished reading posts from the file
    MPI_Send(&stop_time, 1, MPI_LONG, MASTER, GENERIC_TAG, MPI_COMM_WORLD);
}

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
		long delta = num_of_dec + p->num_of_dec*DAILY_DECREMENT;;

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
