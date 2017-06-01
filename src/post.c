#include "post.h"
#include "utils.h"
#include "map.h"
#include "parser.h"
#include "constants.h"

// Create a new post
post* post_create(long ts, long post_id, long user_id, char* content, char* user)
{
    post* new_post = (post*) malloc(sizeof(post));
    if (new_post==NULL){
        return NULL;
    }
    new_post->ts = ts;
    new_post->post_id = post_id;
    new_post->user_id = user_id;
    new_post->content = content;
    new_post->user = user;
    new_post->score = STARTING_SCORE;
    new_post->num_of_dec = 0;
    new_post->last_comment_ts = -1;
    map_init( new_post->commenters);

    return new_post;
}

// Delete a post
void post_delete(post* post)
{
    free(post->content);
    free(post->user);
    post->commenters = map_empty(post->commenters);
    free(post);
}

// Print a post
/*void post_show(post* post)
{
    char date[32];
    parser_ts2date(post->ts, date, sizeof(date));
    printf("%s posted %s on %l \n", post->user, post->content, date);
}*/

bool post_update_score(post* p, int delta, bool is_daily_decrement){
    p->score = p->score + delta;
    if (is_daily_decrement){
        p->num_of_dec = p->num_of_dec - delta;
    }
    if(p->score<=0) {
        return false;
    }
    return true;
}

void post_add_comments_info(post* post, long user_id, time_t last_comment_ts){
	map_put(post->commenters, user_id, last_comment_ts);
    post->last_comment_ts = last_comment_ts;
}

int post_compare(post first, post second){
	if(first.score == second.score && first.ts == second.ts)
		return (utils_compare_int(first.last_comment_ts, second.last_comment_ts));
	if(first.score == second.score)
		return (utils_compare_int(first.ts, second.ts));
	return (utils_compare_int(first.score, second.score));
}
