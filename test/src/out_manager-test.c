#include "mpi.h"
#include "../src/utils.h"
#include "../src/out_manager.h"

void test_1_master();
void test_1_post();

int main(int argc, char *argv[]){
	int rank;

	MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if(rank == MASTER)
    {
        test_1_master();
    }
    else if (rank == POST_MANAGER){
        test_1_post();
    }
    else if (rank == OUT_MANAGER){
        out_manager_run();
    }
    MPI_Finalize();
}

void test_1_master(){
	long timestamp = 0;
	MPI_Status stat;
	
	MPI_Send(&timestamp, 1, MPI_LONG, OUT_MANAGER, TAG, MPI_COMM_WORLD);
	MPI_Send(&timestamp, 1, MPI_LONG, POST_MANAGER, TAG, MPI_COMM_WORLD);
	
	MPI_Recv(&timestamp, 1, MPI_LONG, POST_MANAGER, TAG, MPI_COMM_WORLD, &stat);
	MPI_Send(&timestamp, 1, MPI_LONG, OUT_MANAGER, TAG, MPI_COMM_WORLD);
}

void test_1_post(){

	long timestamp = 0;
	MPI_Status stat;
	post_score first;
	user_num second;
	first.post_id = 1000;
	first.score = 1000;
	second.user_id = 1000;
	second.num_commenters = 1000;

	MPI_Recv(&timestamp, 1, MPI_LONG, MASTER, TAG, MPI_COMM_WORLD, &stat);
	MPI_Send(&first, 1, MPI_LONG_INT, OUT_MANAGER, TAG, MPI_COMM_WORLD);
	MPI_Send(&second, 1, MPI_LONG_INT, OUT_MANAGER, TAG, MPI_COMM_WORLD);

	timestamp = -1;
	MPI_Send(&timestamp, 1, MPI_LONG, MASTER, TAG, MPI_COMM_WORLD);
}