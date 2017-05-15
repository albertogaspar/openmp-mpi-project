#include "post.h"
#include "utils.h"

// Create a new post
post* create_post(long ts, long post_id, long user_id, char* post, char* user)
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

    // Spawn a thread that decreses the score every 24 hours
    pthread_t tid;
    pthread_create(&tid, NULL, &decrease_score, new_post);
}

// Delete a post
void del_post(post* post)
{
    free(post);
}

// Print a post
void show_post(post* post)
{
    char* date[32];
    ts2date(post->ts, date, sizeof(date));
    printf("%s posted %s on %l \n", post->user, post->post, date);
}

void* decrease_score(post* post){
    while(post->score > 0){
        sleep(60*60*24); // sleep for 24 hours
        post->score -= 1;
    }
    post->is_active = false;
    pthread_exit(NULL);
}
