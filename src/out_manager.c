#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>

#define NUM_OF_BEST 3

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


extern const int POST_MANAGER;
extern const int MASTER;
extern const int TAG;

/* output: <ts, top1_post_id, top1_post_user, top1_post_score, top1_post_commenters, top2_post_id,
 * top2_post_user, top2_post_score, top2_post_commenters, top3_post_id, top3_post_user,
 * top3_post_score, top3_post_commenters> */
void out_print_tuple(long ts, out_tuple* array ) {
	int i;
	printf("<%ld", ts);
	for(i=0; i<NUM_OF_BEST; i++) {
		if(array[i].post_id == -1) {
			printf(" , - , - , - , -");
		}
		else {
			printf(" , %ld , %ld , %d , %d", array[i].post_id, array[i].user_id, array[i].score, array[i].num_commenters);
		}
	}
	printf(">\n");
}

void copyTuple(out_tuple *dest, out_tuple *src){
	dest->num_commenters = src->num_commenters;
	dest->post_id = src->post_id;
	dest->score = src->score;
	dest->user_id = src->user_id;
}

int sort_tuples(out_tuple* array, int n) {
   int i, k, changed;
   changed = 0;
   out_tuple tmp;

   for(k=0; k<n-1; k++) {
         for (i=0; i<n-1-k; i++) {
           if (array[i].score < array[i+1].score) /* "<" to have decreasing order */
           {
             copyTuple(&tmp , &array[i]);
             copyTuple(&array[i], &array[i+1]);
             copyTuple(&array[i+1], &tmp);
             changed = 1;
           }
         }
     }
   return changed;
 }


void out_manager_run(){
	int i=0;
	out_tuple best_posts[NUM_OF_BEST + 1];
	for(i=0; i<NUM_OF_BEST + 1; i++) {
		best_posts[i].post_id = -1;
		best_posts[i].user_id = -1;
		best_posts[i].num_commenters = -1;
		best_posts[i].score = -1;
	}
	out_tuple temp;
	post_score first_msg;
	user_num second_msg;
	MPI_Status stat;
	long int current_ts;
	int changed;

	MPI_Recv(&current_ts, 1, MPI_LONG, MASTER, TAG, MPI_COMM_WORLD, &stat);

	//stop when receive current_ts=-1
	while(current_ts!=-1) {

		MPI_Recv(&first_msg, 1, MPI_LONG_INT, POST_MANAGER, TAG, MPI_COMM_WORLD, &stat);
		temp.post_id = first_msg.post_id;
		temp.score = first_msg.score;
		MPI_Recv(&second_msg, 1, MPI_LONG_INT, POST_MANAGER, TAG, MPI_COMM_WORLD, &stat);
		temp.user_id = second_msg.user_id;
		temp.num_commenters = second_msg.num_commenters;

		best_posts[NUM_OF_BEST] = temp;
		changed = sort_tuples(best_posts, NUM_OF_BEST + 1);

		//print output only if there is a change in the order
		if(changed) {
			out_print_tuple(current_ts, best_posts);
		}

		//receive next ts
		MPI_Recv(&current_ts, 1, MPI_LONG, MASTER, TAG, MPI_COMM_WORLD, &stat);

	}

}

int main(int argc, char *argv[]){
	int i=0;
	out_tuple best_posts[NUM_OF_BEST + 1];
	for(i=0; i<NUM_OF_BEST + 1; i++) {
		best_posts[i].post_id = 0;
		best_posts[i].user_id = 0;
		best_posts[i].num_commenters = 0;
		best_posts[i].score = i;
	}

	long int current_ts = 43;
	int changed;
	changed = sort_tuples(best_posts, NUM_OF_BEST + 1);
	//print output only if there is a change in the order
	if(changed) {
		out_print_tuple(current_ts, best_posts);
	}

}



