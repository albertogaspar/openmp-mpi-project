#ifndef POST_H
#define POST_H
#include "map.h"
#include "utils.h"


typedef struct post{
    time_t ts; // the post's timestamp
    long post_id; // the unique id of the post
    long user_id; // the unique id of the user
    char* post; // a string containing the actual post content
    char* user; // a string containing the actual user name
    int score; // a post's score
    bool is_active; // a post is considered no longer active as soon as its
                    // total score reaches zero, even if it receives additional
                    // comments in the future.
	int num_of_dec; //number of times the score was decreased due to 24h elapsed
	map_t commenters;
}post;

// Create a new post
post* post_create(long ts, long post_id, long user_id, char* post, char* user);
// Delete a post
void post_delete(post* post,);
// Print a post
void post_show(post* post,);
// Update post score by a given amount
void post_update_score(post* post, int delta);
//add commenter (only if new) to commenters
void post_add_commenter(post* post, long user_id);

#endif
