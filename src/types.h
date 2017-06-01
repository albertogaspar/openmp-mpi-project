#ifndef TYPES_H
#define TYPES_H

#include <time.h>

typedef enum { false, true } bool;

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

typedef struct post{
    time_t ts; // the post's timestamp
    long post_id; // the unique id of the post
    long user_id; // the unique id of the user
    char* content; // a string containing the actual post content
    char* user; // a string containing the actual user name
    int score; // a post's score
	int num_of_dec; //number of times the score was decreased due to 24h elapsed
	time_t last_comment_ts; //timestamp of the last comment related to the post
    map_t commenters;
}post;

typedef struct comment
{
    time_t ts; // the comment's timestamp
    long comment_id; // the unique id of the comment
    long user_id; // the unique id of the user
    char* content; // a string containing the actual comment
    char* user; // a string containing the actual user name
    long comment_replied; // the id of the comment being replied to (-1 if the tuple is a reply to a post)
    long commented_post; // the id of the post being commented (-1 if the tuple is a reply to a comment)
    int score; // a comment's score
    int num_of_dec;
}comment;


#endif
