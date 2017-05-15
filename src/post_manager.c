#include "post.h"
#include "post_manager.h"
#include "mpi.h"
#include "map.h"
#include "map_iterator.h"

//period of decrementation of the score in hours
#define PERIOD_OF_DECR 24

//TODO: are timestams in seconds?
//factor of conversion from hours to timestamp unit of mesure (seconds)
#define CONVERSION_FACTOR 3600


void post_manager_run(){
// Post Manager
    thread_safe = false;
    int count;
    long int current_ts;
    struct {
        long post_id;
        int increment;
    }pair;
    MPI_Status stat;
    map_t posts = map_init();
    struct post* p = NULL;
    while(p = next_post())
    {
        map_put(posts, p);
        // TODO: SSsend or Send? Blocking
        MPI_Send(&(p->ts), 1, MPI_LONG, MASTER, TAG, MPI_COMM_WORLD);
        MPI_Recv(&current_ts, 1, MPI_LONG, MASTER, TAG, MPI_COMM_WORLD, &stat);
        // Update score of posts (24h decrement)
        update_scores(posts, current_ts);

        if (p->ts > current_ts)
        {
            // Wait for points coming from comments -> gets number of posts to update
            MPI_Recv(&count, 1, MPI_LONG, COMMENT_MANAGER, TAG, MPI_COMM_WORLD, &stat);
            // gets the pairs (post_id, delta_points)
            for(i=0; i<count; i++){
                MPI_Recv(&pair, count, MPI_LONG, COMMENT_MANAGER, TAG, MPI_COMM_WORLD, &stat);
                post* post = map_get(posts, pair.post_id);
                post->score = post->score + p.increment;
            }
        }
    }
}

void update_scores(map_t posts, long current_ts) {
    void *iterator = map_it_init(posts);
    while(map_it_hasnext(posts, iterator)){
        long * k;
        post* p;
        k = map_it_next(map, &iterator);
        p = map_get(map, *k);
        //time elapsed since last decrementation
        long lifetime = current_ts - p->ts;
        //period of decrementation in second
        long pod_in_sec = PERIOD_OF_DECR*CONVERSION_FACTOR;
        //correct number of decrementation
        long num_of_dec = lifetime % pod_in_sec;
        //num of points to be subtracted to the score
        long delta = num_of_dec - p->num_of_dec;

        p->score = p->score + delta;
        p->num_of_dec = num_of_dec;

    }
}