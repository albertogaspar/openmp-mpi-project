#ifndef POST_H
#define POST_H


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
}post;

// Create a new post
post* create_post(long ts, long post_id, long user_id, char* post, char* user);
// Delete a post
void del_post(long post_id);
// Print a post
void show_post(long post_id);

#endif
