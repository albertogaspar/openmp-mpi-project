#ifndef TYPES_H
#define TYPES_H

#include <time.h>

typedef enum { GENERIC_TAG, NEW_COMMENT_UPDATE, DECREMENT_UPDATE} tag_t;

typedef enum {
    MASTER, POST_MANAGER, COMMENT_MANAGER, OUT_MANAGER
} process_t;

typedef struct post_increment{
	long post_id;
	int increment;
}post_increment;

typedef struct ts_rank{
	time_t ts;
	int rank;
}ts_rank;

typedef struct out_tuple{
	time_t ts; //ts of the post
	time_t comment_ts;  //ts of the last comment related to the post
	long post_id;
	long user_id;
	int num_commenters;
	int score;
}out_tuple;

#endif
