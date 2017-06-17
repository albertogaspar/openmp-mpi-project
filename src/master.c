#include <stdio.h>
#include <string.h>
#include "mpi.h"
#include "post_manager.h"
#include "comment_manager.h"
//#include "out_manager.h"
#include "constants.h"
#include "types.h"

void master_run(){
	MPI_Status stat;
	ts_rank current_tr, next_tr;
	time_t received_ts;
	int n_stops = 0;

	MPI_Recv(&received_ts, 1, MPI_LONG, POST_MANAGER, GENERIC_TAG, MPI_COMM_WORLD, &stat);
	current_tr.ts = received_ts;
	current_tr.rank = stat.MPI_SOURCE;
	MPI_Recv(&received_ts, 1, MPI_LONG, COMMENT_MANAGER, GENERIC_TAG, MPI_COMM_WORLD, &stat);
	if(received_ts > current_tr.ts) {
		next_tr.ts = received_ts;
		next_tr.rank = stat.MPI_SOURCE;
	}
	else {
		next_tr.ts = current_tr.ts;
		next_tr.rank = current_tr.rank;
		current_tr.ts = received_ts;
		current_tr.rank = stat.MPI_SOURCE;
	}
	MPI_Bcast(&current_tr, 1, MPI_LONG_INT, MASTER, MPI_COMM_WORLD);

	while(n_stops<2) {
		MPI_Recv(&received_ts, 1, MPI_LONG, MPI_ANY_SOURCE, GENERIC_TAG, MPI_COMM_WORLD, &stat);
		if(received_ts==STOP) {
			n_stops++;
			current_tr.ts = next_tr.ts;
			current_tr.rank = next_tr.rank;
			next_tr.ts = -1;
			MPI_Bcast(&current_tr, 1, MPI_LONG_INT, MASTER, MPI_COMM_WORLD);
		}
		else {
			if(received_ts >= current_tr.ts) {
				if(received_ts >= next_tr.ts && next_tr.ts != -1) {
					current_tr.ts = next_tr.ts;
					current_tr.rank = next_tr.rank;
					next_tr.ts = received_ts;
					next_tr.rank = stat.MPI_SOURCE;
				}
				else {
					current_tr.ts = received_ts;
					current_tr.rank = stat.MPI_SOURCE;
				}

			}
			else {
				if(next_tr.ts != -1){
					next_tr.ts = current_tr.ts;
					next_tr.rank = current_tr.rank;
				}
				current_tr.ts = received_ts;
				current_tr.rank = stat.MPI_SOURCE;
			}
			printf("\n\nMASTER: received = %ld , sending ts = %ld, next = %ld\n\n", received_ts, current_tr.ts, next_tr.ts);
			MPI_Bcast(&current_tr, 1, MPI_LONG_INT, MASTER, MPI_COMM_WORLD);

		}
	}
}

int main(int argc, char* argv[]){
    process_t rank;
    int size;
    char comment_file[100] = "", post_file[100] = "";

    if(argc>=2){
    	strcpy(post_file, argv[1]);
    	strcpy(comment_file, argv[2]);
    }
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, (int *)&rank);

    printf("Process %d started\n", rank);
    switch (rank)
    {
        case MASTER:
        	master_run();
            break;
        case POST_MANAGER:
            post_manager_run(post_file);
            break;
        case COMMENT_MANAGER:
        	comment_manager_run(comment_file);
            break;
        default:
            printf("Rank error! I'm the process with rank %d\n", rank);
            break;
    }

    MPI_Finalize();
}
