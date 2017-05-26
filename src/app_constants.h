#ifndef APP_CONSTANTS_H
#define APP_CONSTANTS_H

#define DAILY_DECREMENT -1
#define STARTING_SCORE 10
#define STOP -1

//period of decrementation of the score in milliseconds
#define PERIOD_OF_DECR 24 * 3600 * 1000

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

#endif
