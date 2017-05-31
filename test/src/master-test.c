#include <stdio.h>
#include "mpi.h"
#include "../../src/post_manager.h"
#include "../../src/comment_manager.h"
#include "../../src/out_manager.h"
#include "../../src/constants.h"
#include "../../src/types.h"

void post_manager_run(){
	time_t ts = 1;
	ts_rank current_tr;

	MPI_Send(&ts, 1, MPI_LONG, MASTER, GENERIC_TAG, MPI_COMM_WORLD);
	// Receive current timestamp from master
	MPI_Bcast(&current_tr, 1, MPI_LONG_INT, MASTER, MPI_COMM_WORLD);
	printf("POST_MANAGER: Current ts is: %ld, from process %d\n", current_tr.ts, current_tr.rank);
	while (ts > current_tr.ts)
	{
		//read next timestamp
		MPI_Bcast(&current_tr, 1, MPI_LONG_INT, MASTER, MPI_COMM_WORLD);

	}

	ts = 4;
	MPI_Send(&ts, 1, MPI_LONG, MASTER, GENERIC_TAG, MPI_COMM_WORLD);
	// Receive current timestamp from master
	MPI_Bcast(&current_tr, 1, MPI_LONG_INT, MASTER, MPI_COMM_WORLD);
	printf("POST_MANAGER: Current ts is: %ld, from process %d\n", current_tr.ts, current_tr.rank);
	while (ts > current_tr.ts)
	{
		//read next timestamp
		MPI_Bcast(&current_tr, 1, MPI_LONG_INT, MASTER, MPI_COMM_WORLD);
		printf("POST_MANAGER: Current ts is: %ld, from process %d\n", current_tr.ts, current_tr.rank);

	}

	ts = 5;
	MPI_Send(&ts, 1, MPI_LONG, MASTER, GENERIC_TAG, MPI_COMM_WORLD);
	// Receive current timestamp from master
	MPI_Bcast(&current_tr, 1, MPI_LONG_INT, MASTER, MPI_COMM_WORLD);
	printf("POST_MANAGER: Current ts is: %ld, from process %d\n", current_tr.ts, current_tr.rank);
	while (ts > current_tr.ts)
	{
		//read next timestamp
		MPI_Bcast(&current_tr, 1, MPI_LONG_INT, MASTER, MPI_COMM_WORLD);
		printf("POST_MANAGER: Current ts is: %ld, from process %d\n", current_tr.ts, current_tr.rank);

	}


	ts = 8;
	MPI_Send(&ts, 1, MPI_LONG, MASTER, GENERIC_TAG, MPI_COMM_WORLD);
	// Receive current timestamp from master
	MPI_Bcast(&current_tr, 1, MPI_LONG_INT, MASTER, MPI_COMM_WORLD);
	printf("POST_MANAGER: Current ts is: %ld, from process %d\n", current_tr.ts, current_tr.rank);
	while (ts > current_tr.ts)
	{
		//read next timestamp
		MPI_Bcast(&current_tr, 1, MPI_LONG_INT, MASTER, MPI_COMM_WORLD);
		printf("POST_MANAGER: Current ts is: %ld, from process %d\n", current_tr.ts, current_tr.rank);

	}

	ts = 10;
	MPI_Send(&ts, 1, MPI_LONG, MASTER, GENERIC_TAG, MPI_COMM_WORLD);
	// Receive current timestamp from master
	MPI_Bcast(&current_tr, 1, MPI_LONG_INT, MASTER, MPI_COMM_WORLD);
	printf("POST_MANAGER(after send): Current ts is: %ld, from process %d\n", current_tr.ts, current_tr.rank);
	while (ts > current_tr.ts)
	{
		//read next timestamp
		MPI_Bcast(&current_tr, 1, MPI_LONG_INT, MASTER, MPI_COMM_WORLD);
		printf("POST_MANAGER(my ts: %ld> received): Current ts is: %ld, from process %d\n", ts, current_tr.ts, current_tr.rank);

	}

	ts = STOP;
	printf("POST_MANAGER: sending stop...\n");
	MPI_Send(&ts, 1, MPI_LONG, MASTER, GENERIC_TAG, MPI_COMM_WORLD);

	printf("POST_MANAGER: stop sent\n");
}

void comment_manager_run(){
	time_t ts = 2;
	ts_rank current_tr;

	MPI_Send(&ts, 1, MPI_LONG, MASTER, GENERIC_TAG, MPI_COMM_WORLD);
	// Receive current timestamp from master
	MPI_Bcast(&current_tr, 1, MPI_LONG_INT, MASTER, MPI_COMM_WORLD);
	printf("COMMENT_MANAGER: Current ts is: %ld, from process %d\n", current_tr.ts, current_tr.rank);
	while (ts >= current_tr.ts && current_tr.rank != COMMENT_MANAGER )
	{
		//read next timestamp
		MPI_Bcast(&current_tr, 1, MPI_LONG_INT, MASTER, MPI_COMM_WORLD);
		printf("COMMENT_MANAGER: Current ts is: %ld, from process %d\n", current_tr.ts, current_tr.rank);
	}

	ts = 3;
	MPI_Send(&ts, 1, MPI_LONG, MASTER, GENERIC_TAG, MPI_COMM_WORLD);
	// Receive current timestamp from master
	MPI_Bcast(&current_tr, 1, MPI_LONG_INT, MASTER, MPI_COMM_WORLD);
	printf("COMMENT_MANAGER: Current ts is: %ld, from process %d\n", current_tr.ts, current_tr.rank);
	while (ts >= current_tr.ts && current_tr.rank != COMMENT_MANAGER ){
		//read next timestamp
		MPI_Bcast(&current_tr, 1, MPI_LONG_INT, MASTER, MPI_COMM_WORLD);
		printf("COMMENT_MANAGER: Current ts is: %ld, from process %d\n", current_tr.ts, current_tr.rank);
	}

	ts = 5;
	MPI_Send(&ts, 1, MPI_LONG, MASTER, GENERIC_TAG, MPI_COMM_WORLD);
	// Receive current timestamp from master
	MPI_Bcast(&current_tr, 1, MPI_LONG_INT, MASTER, MPI_COMM_WORLD);
	printf("COMMENT_MANAGER: Current ts is: %ld, from process %d\n", current_tr.ts, current_tr.rank);
	while (ts >= current_tr.ts && current_tr.rank != COMMENT_MANAGER ){
		//read next timestamp
		MPI_Bcast(&current_tr, 1, MPI_LONG_INT, MASTER, MPI_COMM_WORLD);
		printf("COMMENT_MANAGER: Current ts is: %ld, from process %d\n", current_tr.ts, current_tr.rank);
	}

	ts = 7;
	MPI_Send(&ts, 1, MPI_LONG, MASTER, GENERIC_TAG, MPI_COMM_WORLD);
	// Receive current timestamp from master
	MPI_Bcast(&current_tr, 1, MPI_LONG_INT, MASTER, MPI_COMM_WORLD);
	printf("COMMENT_MANAGER: Current ts is: %ld, from process %d\n", current_tr.ts, current_tr.rank);
	while (ts >= current_tr.ts && current_tr.rank != COMMENT_MANAGER ){
		//read next timestamp
		MPI_Bcast(&current_tr, 1, MPI_LONG_INT, MASTER, MPI_COMM_WORLD);
		printf("COMMENT_MANAGER: Current ts is: %ld, from process %d\n", current_tr.ts, current_tr.rank);
	}

	ts = 8;
	MPI_Send(&ts, 1, MPI_LONG, MASTER, GENERIC_TAG, MPI_COMM_WORLD);
	// Receive current timestamp from master
	MPI_Bcast(&current_tr, 1, MPI_LONG_INT, MASTER, MPI_COMM_WORLD);
	printf("COMMENT_MANAGER: Current ts is: %ld, from process %d\n", current_tr.ts, current_tr.rank);
	while (ts >= current_tr.ts && current_tr.rank != COMMENT_MANAGER ){
		//read next timestamp
		MPI_Bcast(&current_tr, 1, MPI_LONG_INT, MASTER, MPI_COMM_WORLD);
		printf("COMMENT_MANAGER: Current ts is: %ld, from process %d\n", current_tr.ts, current_tr.rank);
	}

	ts = STOP;
	printf("COMMENT_MANAGER: sending stop...\n");
	MPI_Send(&ts, 1, MPI_LONG, MASTER, GENERIC_TAG, MPI_COMM_WORLD);

	printf("COMMENT_MANAGER: STOP sent\n");
}

void out_manager_run(){

}
