#import "comment.h"
#import  "post.h"
#import "mpi.h"

#define TAG 1

typedef enum {
    MASTER, POST_MANAGER, COMMENT_MANAGER, OUT_MANAGER
} proc;


int main(int argc, char* argv[]){
    int rank, size;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if(rank == 0)
    {
        //  Master
    }
    else if (rank == 1)
    {
        // Post Manager
        thread_safe = false;
        int count;
        long int current_ts;
        struct {
            long post_id;
            int increment;
        }pair;
        MPI_Status stat;
        struct post* *posts = NULL;
        posts = llist_create(NULL, NULL, thread_safe);
        struct post* p = NULL;
        while(p = next_post())
        {
            llist_insert_node(posts, (llist_node) p, thread_safe);
            // TODO: SSsend or Send? Blocking
            MPI_Send(&(p->ts), 1, MPI_LONG, MASTER, TAG, MPI_COMM_WORLD);
            MPI_Recv(&current_ts, 1, MPI_LONG, MASTER, TAG, MPI_COMM_WORLD, &stat);
            // Update score of posts (24h decrement)
            list_update_scores(posts, current_ts);

            if (p->ts > current_ts)
            {
                // Wait for points coming from comments -> gets number of posts to update
                MPI_Recv(&count, 1, MPI_LONG, COMMENT_MANAGER, TAG, MPI_COMM_WORLD, &stat);
                // gets the pairs (post_id, delta_points)
                for(i=0; i<count; i++){
                    MPI_Recv(&pair, count, MPI_LONG, COMMENT_MANAGER, TAG, MPI_COMM_WORLD, &stat);
                    post* post = list_get_by_id(posts, pair.post_id);
                    post->score = post->score + p.increment;
                }
            }


        }

    }
    else if (rank == 2)
    {
        // Comment Mangaer
    }
    else{
        // Output Manager
    }

    MPI_Finalize();
}
