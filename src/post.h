#ifndef POST_H
#define POST_H
#include "map.h"


typedef struct post{
    long ts; // the post's timestamp
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
post* create_post(long ts, long post_id, long user_id, char* post, char* user);
// Delete a post
void del_post(long post_id);
// Print a post
void show_post(long post_id);
//
void update_score(int delta);
//add commenter (only if new) to commenters
void add_commenter_to_post(post* post, long user_id);

#endif
