#include "mpi.h"
#include <stdio.h>
#include "map.h"

typedef struct out_tuple{
	long post_id;
	long user_id;
	int num_commenters;
	int score;
}out_tuple;

typedef struct post_score{
	long post_id;
	int score;
}post_score;

typedef struct user_num{
	long user_id;
	int num_commenters;
}user_num;

typedef struct out_tuple{
	long post_id;
	long user_id;
	int num_commenters;
	int score;
}out_tuple;

extern const int POST_MANAGER;
extern const int STOP;

//insert in order and discard the fourth element
void out_insert_tuple(map_t list, out_tuple* tuple){

}

void out_print_tuple(long ts, map_t list ) {
	printf("");
}




void out_manager_run(){
	map_t out_list = map_init();
	out_tuple tuple;
	post_score first_msg;
	user_num second_msg;
	MPI_Status stat;

	MPI_Recv(first_msg, 1, MPI_LONG_INT, POST_MANAGER, MPI_ANY_TAG, MPI_COMM_WORLD, &stat);
	//stop when receive a msg with tag = STOP
	while(stat.MPI_TAG!=STOP) {
		MPI_Recv(second_msg, 1, MPI_LONG_INT, POST_MANAGER, MPI_ANY_TAG, MPI_COMM_WORLD, &stat);
		tuple.post_id = first_msg.post_id;
		tuple.score = first_msg.score;
		tuple.user_id = second_msg.user_id;
		tuple.num_commenters = second_msg.num_commenters;

		out_insert_tuple(out_list, tuple);


	}

}



