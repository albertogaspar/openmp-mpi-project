#include <time.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include "comment.h"
#include "utils.h"

void* decrease_score(void* arg){
    struct comment *comment = arg;
    while(comment->score > 0){
        sleep(60*60*24); // sleep for 24 hours
        comment->score -= 1;
    }
    pthread_exit(NULL);
}

// Create a new post
comment* create_comment(time_t ts, long comment_id, long user_id, char* comment,
    char* user, long comment_replied, long post_commented)
{
    struct comment* new_comment;
    new_comment = (struct comment*) malloc(sizeof(struct comment));
    if (comment==NULL){
        return NULL;
    }
    new_comment->ts = ts;
    new_comment->comment_id = comment_id;
    new_comment->user_id = user_id;
    new_comment->comment = comment;
    new_comment->user = user;
    new_comment->comment_replied = comment_replied;
    new_comment->post_commented = post_commented;
    new_comment->score = 10;

    // Spawn a thread that decreses the score every 24 hours
    //pthread_t tid;
    //pthread_create(&tid, NULL, &decrease_score, new_comment);
}

// Delete a post
void del_comment(struct comment* comment)
{
    free(comment);
}

// Print a post
void show_comment(struct comment* comment)
{
    char date[32];
    ts2date(comment->ts/1000, date, sizeof(date));
    printf(" %s commented %s on %s \n", comment->user, comment->comment, date);
}
