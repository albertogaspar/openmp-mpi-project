#include <stdio.h>
#include "mpi.h"
#include "post_manager.h"
#include "out_manager.h"
#include "app_constants.h"

int main(int argc, char* argv[]){
    process_t rank;
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
            out_manager_run();
            break;
        default:
            printf("Rank error! I'm the process with rank %d\n", rank);
            break;
    }

    MPI_Finalize();
}
