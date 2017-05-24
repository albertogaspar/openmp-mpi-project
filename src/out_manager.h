#ifndef OUT_MANAGER_H
#define OUT_MANAGER_H

#include <time.h>
#include "post.h"

typedef struct out_tuple{
	time_t ts; //ts of the post
	time_t comment_ts;  //ts of the last comment related to the post
	long post_id;
	long user_id;
	int num_commenters;
	int score;
}out_tuple;

void out_manager_run();

out_tuple out_create_tuple(post p);

#endif
