#include "post.h"
#include "post_manager.h"
#include "mpi.h"
#include "map.h"
#include "map_iterator.h"
#include "utils.h"

//period of decrementation of the score in hours
#define PERIOD_OF_DECR 24
//factor of conversion from hours to timestamp unit of measure (milliseconds)
#define CONVERSION_FACTOR 3600000

typedef struct {
    long post_id;
    long user_id;
}pair;

void post_manager_run(){
    thread_safe = false;
    int count;
    long int current_ts;
    MPI_Status stat;
    map_t posts = map_init();
    struct post* p = NULL;
    while(p = parser_next_post())
    {
        map_put(posts, p->post_id, p);
        // TODO: SSsend or Send? Blocking
        // Send timestamp of latest post
        MPI_Send(&(p->ts), 1, MPI_LONG, MASTER, TAG, MPI_COMM_WORLD);
        // Receive current timestamp from master
        MPI_Recv(&current_ts, 1, MPI_LONG, MASTER, TAG, MPI_COMM_WORLD, &stat);
        // Update score of posts (24h decrement)
        update_scores(posts, current_ts);

        while (p->ts > current_ts)
        {
            // Wait for points coming from comments -> gets number of posts to update
            MPI_Recv(&count, 1, MPI_LONG, COMMENT_MANAGER, TAG, MPI_COMM_WORLD, &stat);
            // gets the pairs (post_id, delta_points)
            for(i=0; i<count; i++){
            	//distinguish pair by tag
            	int increment;
                MPI_Recv(&pair, count, MPI_LONG_INT, COMMENT_MANAGER, MPI_ANY_TAG, MPI_COMM_WORLD, &stat);

                post* post = map_get(posts, pair.post_id);
                if(stat.MPI_TAG == NEW_COMMENT) {
                    //TODO: *****add tags NEW_COMMENT and DECREMENT******
                	increment = 10;
                	post_add_commenter(post, pair.user_id);
                }
                else {
                	increment = -1;
                }
                post_update_score(p, increment, false);
            }

            //read next timestamp
            MPI_Recv(&current_ts, 1, MPI_LONG, MASTER, TAG, MPI_COMM_WORLD, &stat);
            daily_decrement(posts, current_ts);
        }
    }
}

void daily_decrement(map_t posts, long current_ts) {
    void *iterator = map_it_init(posts);
    long *k;
    post *p;
    while(map_it_hasnext(posts, iterator)){
        k = map_it_next(map, &iterator);
        p = map_get(map, *k);
        if(p->is_active) {
        	//time elapsed since last decrementation
			long lifetime = current_ts - p->ts;
			//period of decrementation in second
			long pod_in_sec = PERIOD_OF_DECR*CONVERSION_FACTOR;
			//correct number of decrementation
			long num_of_dec = lifetime % pod_in_sec;
			//num of points to be subtracted to the score
			long delta = num_of_dec - p->num_of_dec;

            post_update_score(p, delta, true);
        }


    }
}
