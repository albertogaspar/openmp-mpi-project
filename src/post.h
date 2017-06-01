#ifndef POST_H
#define POST_H
#include "map.h"
#include "types.h"
#include <time.h>


// Create a new post
post* post_create(long ts, long post_id, long user_id, char* content, char* user);
// Delete a post
void post_delete(post* post);
// Print a post
void post_show(post* post);
// Update post score by a given amount and if is_daily_decrement is true also update num_of_dec
// Returns true if the post is still acrive, false otherwise
bool post_update_score(post* post, int delta, bool is_daily_decrement);
//add commenter (only if new) to commenters and update last_comment_ts
void post_add_comments_info(post* post, long user_id, time_t last_comment_ts);
//compare 2 posts by score, timestamp, timestamp of last comment
int post_compare(post first, post second);

#endif
