#include <time.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include "comment.h"
#include "utils.h"
//#include "parser.h"

comment* comment_create(time_t ts, long comment_id, long user_id, char* content, char* user,
		long comment_replied, long commented_post)
{
    struct comment* new_comment;
    new_comment = (struct comment*) malloc(sizeof(struct comment));
    if (new_comment == NULL){
        return NULL;
    }
    new_comment->ts = ts;
    new_comment->comment_id = comment_id;
    new_comment->user_id = user_id;
    new_comment->content = content;
    new_comment->user = user;
    new_comment->comment_replied = comment_replied;
    new_comment->commented_post = commented_post;
    new_comment->score = 10;
	new_comment->num_of_dec = 0;

    return new_comment;
}

// Delete a post
void comment_delete(comment* comment)
{
    free(comment);
}

bool comment_update_score(comment* c, int delta, bool is_daily_decrement){
    c->score = c->score + delta;
    if (is_daily_decrement){
        c->num_of_dec = c->num_of_dec - delta;
    }
	return true;
}

// Print a post
/*void comment_show(comment* comment)
{
    char date[32];
    //parser_ts2date(comment->ts/1000, date, sizeof(date));
    //printf(" %s commented %s on %s \n", comment->user, comment->content, date);
}*/
