#include <stdio.h>
#include "mpi.h"
#include "../../src/utils.h"
#include "../../src/out_manager.h"
#include "../../src/serialization.h"

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

	// Start managers
	MPI_Send(&timestamp, 1, MPI_LONG, OUT_MANAGER, GENERIC_TAG, MPI_COMM_WORLD);
	printf("Sending timestamps \n");
	MPI_Send(&timestamp, 1, MPI_LONG, POST_MANAGER, GENERIC_TAG, MPI_COMM_WORLD);

	// Receiving timestamp
	while(timestamp != -1)
	{
		MPI_Recv(&timestamp, 1, MPI_LONG, POST_MANAGER, GENERIC_TAG, MPI_COMM_WORLD, &stat);
		MPI_Send(&timestamp, 1, MPI_LONG, OUT_MANAGER, GENERIC_TAG, MPI_COMM_WORLD);
	}
}

void test_1_post(){

	long timestamp = 0;
	MPI_Status stat;
	out_tuple out;
	out.ts = 1;
	out.post_id = 100;
	out.score = 1000;
	out.user_id = out.ts;
	out.num_commenters = 27;
	// Genearte complex serialized type for MPI send
	MPI_Datatype MPI_out_tuple = serialize_out_tuple();

	// Read timestamp from master
	MPI_Recv(&timestamp, 1, MPI_LONG, MASTER, GENERIC_TAG, MPI_COMM_WORLD, &stat);
	// Send out tuple to out manager
	MPI_Send(&out, 1, MPI_out_tuple, OUT_MANAGER, GENERIC_TAG, MPI_COMM_WORLD);
	MPI_Send(&out.ts, 1, MPI_LONG, MASTER, GENERIC_TAG, MPI_COMM_WORLD);
	out.ts++;
	out.score = 100;
	out.user_id = out.ts;
	MPI_Send(&out, 1, MPI_out_tuple, OUT_MANAGER, GENERIC_TAG, MPI_COMM_WORLD);
	MPI_Send(&out.ts, 1, MPI_LONG, MASTER, GENERIC_TAG, MPI_COMM_WORLD);
	out.ts++;
	out.score = 10;
	MPI_Send(&out, 1, MPI_out_tuple, OUT_MANAGER, GENERIC_TAG, MPI_COMM_WORLD);
	MPI_Send(&out.ts, 1, MPI_LONG, MASTER, GENERIC_TAG, MPI_COMM_WORLD);
	out.ts++;
	out.score = 1000;
	MPI_Send(&out, 1, MPI_out_tuple, OUT_MANAGER, GENERIC_TAG, MPI_COMM_WORLD);
	MPI_Send(&out.ts, 1, MPI_LONG, MASTER, GENERIC_TAG, MPI_COMM_WORLD);
	out.ts=0;
	out.score = 100;
	out.user_id = out.ts;
	MPI_Send(&out, 1, MPI_out_tuple, OUT_MANAGER, GENERIC_TAG, MPI_COMM_WORLD);
	MPI_Send(&out.ts, 1, MPI_LONG, MASTER, GENERIC_TAG, MPI_COMM_WORLD);
	out.ts=1000;
	out.score = 100;
	out.user_id = out.ts;
	MPI_Send(&out, 1, MPI_out_tuple, OUT_MANAGER, GENERIC_TAG, MPI_COMM_WORLD);

	timestamp = -1;
	MPI_Send(&timestamp, 1, MPI_LONG, MASTER, GENERIC_TAG, MPI_COMM_WORLD);
}
