#include "post.h"
#include "utils.h"
#include "map.h"

// Create a new post
post* post_create(long ts, long post_id, long user_id, char* post, char* user)
{
    post* new_post = (post*) malloc(sizeof(post));
    if (post==NULL){
        exit(1);
    }
    new_post->ts = ts;
    new_post->post_id = post_id;
    new_post->user_id = user_id;
    new_post->post = post;
    new_post->user = user;
    new_post->score = 10;
    new_post->is_active = true;
    new_post->num_of_dec = 0;
    map_init( new_post->commenters);

    // Spawn a thread that decreses the score every 24 hours
    pthread_t tid;
    pthread_create(&tid, NULL, &decrease_score, new_post);
}

// Delete a post
void post_delete(post* post)
{
    free(post);
}

// Print a post
void post_show(post* post)
{
    char* date[32];
    ts2date(post->ts, date, sizeof(date));
    printf("%s posted %s on %l \n", post->user, post->post, date);
}

void post_update_score(post* p, int delta, bool daily_decrement){
    p->score = p->score + delta;
    if (daily_decrement){
        p->num_of_dec = p->num_of_dec + delta;
    }
    if(p->score<=0) {
        p->is_active = false;
    }
}

void post_add_commenter(post* post, long user_id){
	map_put(post->commenters, user_id);
}
