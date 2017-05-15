#include "comment.h"
#include "post.h"
#include "mpi.h"
#include "map.h"
#include "map_iterator.h"
#include "post_manager.h"

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
        
        post_manager_run();

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
