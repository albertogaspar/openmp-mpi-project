#include <stdio.h>
#include <limits.h>
#include "mpi.h"
#include "../../src/post_manager.h"
#include "../../src/comment_manager.h"
#include "../../src/constants.h"
#include "../../src/types.h"

void comment_manager_run(){
	time_t ts = LONG_MAX;
	ts_rank current_tr;

	MPI_Send(&ts, 1, MPI_LONG, MASTER, GENERIC_TAG, MPI_COMM_WORLD);
	while(1)
		MPI_Bcast(&current_tr, 1, MPI_LONG_INT, MASTER, MPI_COMM_WORLD);


	printf("COMMENT MANAGER: STOP sent\n");

}
