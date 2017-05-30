#include "mpi.h"
#include "utils.h"
#include "out_manager.h"
#include <stdio.h>
#include <stdlib.h>
#include "serialization.h"
#include "constants.h"
#include "types.h"

#define NUM_OF_BEST 3

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

int out_tuple_compare(out_tuple first, out_tuple second){
	if(first.score == second.score && first.ts == second.ts)
		return (utils_compare_int(first.comment_ts, second.comment_ts));
	if(first.score == second.score)
		return (utils_compare_int(first.ts, second.ts));
	return (utils_compare_int(first.score, second.score));
}

void copyTuple(out_tuple *dest, out_tuple *src){
	dest->num_commenters = src->num_commenters;
	dest->post_id = src->post_id;
	dest->score = src->score;
	dest->user_id = src->user_id;
	dest->ts = src->ts;
}

int sort_tuples(out_tuple* array, int n) {
   int i, k, changed;
   changed = 0;
   out_tuple tmp;

   for(k=0; k<n-1; k++) {
         for (i=0; i<n-1-k; i++) {
           if ( out_tuple_compare(array[i], array[i+1]) < 0 ) /* "<" to have decreasing order */
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

void init_best_posts(out_tuple* best_posts) {
	int i;

	for(i=0; i<NUM_OF_BEST + 1; i++) {
			best_posts[i].ts = -1;
			best_posts[i].comment_ts = -1;
			best_posts[i].post_id = -1;
			best_posts[i].user_id = -1;
			best_posts[i].num_commenters = -1;
			best_posts[i].score = -1;
	}
}

void out_manager_run(){

	out_tuple best_posts[NUM_OF_BEST + 1];
	init_best_posts(best_posts);
	out_tuple temp;
	MPI_Status stat;
	long int current_ts;
	int changed;

	MPI_Recv(&current_ts, 1, MPI_LONG, MASTER, GENERIC_TAG, MPI_COMM_WORLD, &stat);
	printf("received current_ts: %ld\n", current_ts);

	//stop when receive current_ts=-1
	while(current_ts!=STOP) {
		//receiving out tuple from post manager
		MPI_Datatype MPI_out_tuple = serialize_out_tuple();
		MPI_Recv(&temp, 1, MPI_out_tuple, POST_MANAGER, GENERIC_TAG, MPI_COMM_WORLD, &stat);
   		printf("Received: user_id = %ld score = %d post_id = %ld num_commenters = %d \n",
			temp.user_id, temp.score, temp.post_id, temp.num_commenters);

		best_posts[NUM_OF_BEST] = temp;
		changed = sort_tuples(best_posts, NUM_OF_BEST + 1);

		//print output only if there is a change in the order
		if(changed) {
			out_print_tuple(current_ts, best_posts);
		}

		//receive next ts
		MPI_Recv(&current_ts, 1, MPI_LONG, MASTER, GENERIC_TAG, MPI_COMM_WORLD, &stat);
		printf("received current_ts: %ld\n", current_ts);

	}

}

void out_create_tuple(post *p, out_tuple* ot){
	ot->ts = p->ts;
	ot->comment_ts = p->last_comment_ts;
	ot->post_id = p-> post_id;
	ot->user_id = p-> user_id;
	ot->num_commenters = map_size(p->commenters);
	ot->score = p-> score;
}
/*
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

}*/
