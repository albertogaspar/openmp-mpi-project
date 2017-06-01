#include <stdio.h>
#include "mpi.h"
#include "../../src/post_manager.h"
#include "../../src/comment_manager.h"
#include "../../src/constants.h"
#include "../../src/types.h"

void comment_manager_run(){
	time_t ts = STOP;
	ts_rank current_tr;

	MPI_Send(&ts, 1, MPI_LONG, MASTER, GENERIC_TAG, MPI_COMM_WORLD);
	printf("COMMENT MANAGER: STOP sent\n");

}
