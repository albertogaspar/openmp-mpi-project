#include "mpi.h"
#include "utils.h"
#include "output.h"
#include <stdio.h>
#include <stdlib.h>
//#include "serialization.h"
#include "constants.h"
#include "types.h"
#include "post.h"
#include "parser.h"


/* output: <ts, top1_post_id, top1_post_user, top1_post_score, top1_post_commenters, top2_post_id,
 * top2_post_user, top2_post_score, top2_post_commenters, top3_post_id, top3_post_user,
 * top3_post_score, top3_post_commenters> */
void out_print_best(post* array[], time_t ts, FILE** file) {
	int i;
	char date[32];
	parser_ts2date(ts, date, sizeof(date));
	fprintf(*file, "%s", date);
	for(i=0; i<NUM_OF_BEST; i++) {
		if(array[i] == NULL) {
			fprintf(*file, " , - , - , - , -");
		}
		else {
			int num_commenters = map_size(array[i]->commenters);
			fprintf(*file, " , %ld , %ld , %d , %d", array[i]->post_id, array[i]->user_id, array[i]->score, num_commenters);
		}
	}
	fprintf(*file, "\n");
	printf("\t\tOUTPUT: BEST THREE CHANGED!\n");
}


bool out_compare_with_best(post *best_three[], post *p){

	int i, j;
	int compare;
	for(i = 0; i < NUM_OF_BEST; i++) {
		//if p is already in the best three skip the comparison
		if( best_three[i] != NULL && p->post_id==best_three[i]->post_id ){
			return false;
		}
		if( best_three[i]==NULL || (compare = post_compare(*p, *best_three[i])) > 0 ) {
			if(best_three[i]!=NULL)
				printf("OUTPUT: comparison between post %ld and %ld result %d\n", p->post_id, best_three[i]->post_id, compare);
			for(j = NUM_OF_BEST - 2 ; j >= i ; j--) {
				best_three[j+1] = best_three[j];
			}
			best_three[i] = p;
			return true;
		}
	}
	return false;
}


/*void copyTuple(out_tuple *dest, out_tuple *src){
	dest->num_commenters = src->num_commenters;
	dest->post_id = src->post_id;
	dest->score = src->score;
	dest->user_id = src->user_id;
	dest->ts = src->ts;
}

int sort_tuples(post* array, int n) {
   int i, k, changed;
   changed = 0;
   out_tuple tmp;

   for(k=0; k<n-1; k++) {
         for (i=0; i<n-1-k; i++) {
           if ( out_tuple_compare(array[i], array[i+1]) < 0 ) // "<" to have decreasing order
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
