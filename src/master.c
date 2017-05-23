#include "comment.h"
#include "post.h"
#include "mpi.h"
#include "map.h"
#include "map_iterator.h"
#include "post_manager.h"
#include "utils.h"

int main(int argc, char* argv[]){
    proc rank;
    int size;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    switch (rank)
    {
        case MASTER: 
            break;
        case POST_MANAGER:
            post_manager_run();
            break;
        case COMMENT_MANAGER:
            break;
        case OUT_MANAGER:
            break;
        default:
            break;
    }

    MPI_Finalize();
}
