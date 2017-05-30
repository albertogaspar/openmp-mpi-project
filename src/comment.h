#ifndef COMMENT_H
#define COOMENT_H

#include <time.h>
#include "types.h"

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

// Create a new comment
comment* comment_create(time_t ts, long comment_id, long user_id, char* content, char* user, long comment_replied, long commented_post);

// Delete a comment
void comment_delete(comment* comment);

// Update comment score by a given amount and if is_daily_decrement is true also update num_of_dec
bool comment_update_score(comment* c, int delta, bool is_daily_decrement);

// Show comment
void comment_show(comment* comment);

#endif
