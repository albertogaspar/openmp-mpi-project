#ifndef COMMENT_H
#define COOMENT_H

#include <time.h>
#include "types.h"

// Create a new comment
comment* comment_create(time_t ts, long comment_id, long user_id, char* content, char* user, long comment_replied, long commented_post);

// Delete a comment
void comment_delete(comment* comment);

// Update comment score by a given amount and if is_daily_decrement is true also update num_of_dec
bool comment_update_score(comment* c, int delta, bool is_daily_decrement);

// Show comment
void comment_show(comment* comment);

#endif
