#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "post.h"
#include "comment.h"
//#include "utils.h"

#define LIST_MAX 10
#define COMMENT_FILE "comments.dat"
#define POSTS_FILE "posts.dat"
#define SPLITTER "|"


FILE* file;

long int strtol_def(char* string, long int def)
{
    if (strcmp(string, "") == 0 || strcmp(string, "\n") == 0)
        return def;
    return strtol(string, NULL, 10);
}

int parse_comment(struct comment* comment, char* line){
    comment->ts = parse_ts(strsep(&line, SPLITTER));
    comment->comment_id = strtol_def(strsep(&line, SPLITTER), -1);
    comment->user_id = strtol_def(strsep(&line, SPLITTER), -1);
    char* com = strsep(&line, SPLITTER);
    comment->comment = (char*) malloc(strlen(com)*sizeof(char));
    strcpy(comment->comment, com);
    char* us = strsep(&line, SPLITTER);
    comment->user = (char*) malloc(strlen(us)*sizeof(char));
    strcpy(comment->user, us);
    comment->comment_replied = strtol_def(strsep(&line, SPLITTER), -1);
    comment->post_commented = strtol_def(strsep(&line, SPLITTER), -1);
    comment->score = 10;
    printf("TS:%ld , CID: %ld, UID: %ld, C: %s, U: %s, CR: %ld, PC: %ld, S: %d \n\n", comment->ts, comment->comment_id,
    comment->user_id,comment->comment,comment->user,comment->comment_replied,comment->post_commented,comment->score);
    return 0;
}

struct comment* next_comment()
{
    char line[500];
    // Allocation of space for new comment
    struct comment* new_comment;
    new_comment = (struct comment*) malloc(sizeof(struct comment));
    // Read one line of the file
    fgets(line, 500, (FILE*) file);
    if (line == NULL || new_comment==NULL)
        return NULL;
    parse_comment(new_comment, line);
    return new_comment;
}

int parse_post(struct post* post, char* line){
    post->ts = parse_ts(strsep(&line, SPLITTER));
    post->post_id = strtol_def(strsep(&line, SPLITTER),-1);
    post->user_id = strtol_def(strsep(&line, SPLITTER),-1);
    char* p = strsep(&line, SPLITTER);
    post->post = (char*) malloc(strlen(p)*sizeof(char));
    strcpy(post->post, p);
    char* us = strsep(&line, SPLITTER);
    post->user = (char*) malloc(strlen(us)*sizeof(char));
    strcpy(post->user, us);
    post->score = 10;
    post->is_active = true;
    post->num_of_dec = 0;
    map_init( post->commenters);
    return 0;
}

struct comment* next_post()
{
    char line[500];
    // Allocation of space for new comment
    struct post* new_post;
    new_post = (struct post*) malloc(sizeof(struct post));
    // Read one line of the file
    fgets(line, 500, (FILE*) file);
    if (line == NULL || new_post==NULL)
        return NULL;
    parse_post(new_post, line);
    return new_post;
}

int main(int argc, char *argv[])
{
    struct comment* comments[LIST_MAX];
    int count = 0;
    int i = 0;

    FILE *file;
    file = fopen(COMMENT_FILE,"r");
    if(!file)
    {
        perror("Error opening file");
        return -1;
    }

    //Initialize the array
    char str[500];
    //memset(comments, 0, sizeof(comments));
    while(fgets(str, 500, (FILE*) file)              /* Check for the end of file*/
        &&(count < LIST_MAX)) /* To avoid memory corruption */
    {
        printf("%d\n", count);
        printf("%s\n", str);

        struct comment* new_comment;
        new_comment = (struct comment*) malloc(sizeof(struct comment));
        if (new_comment==NULL){
            return NULL;
        }
        parse_comment(new_comment, str);
        //init_comment(new_comment);
        comments[count] = new_comment;
        printf("TS:%ld , CID: %ld, UID: %ld, C: %s, U: %s, CR: %ld, PC: %ld, S: %d \n\n", comments[count]->ts, comments[count]->comment_id,
        comments[count]->user_id,comments[count]->comment,comments[count]->user,comments[count]->comment_replied,
        comments[count]->post_commented,comments[count]->score);
        if (comments[count] == NULL)
        {
                printf("Comment not correctly allocated\n" );
                return 0;
        }
        count++;
    }
    i=0;
    printf("TS:%ld , CID: %ld, UID: %ld, C: %s, U: %s, CR: %ld, PC: %ld, S: %d \n\n", comments[i]->ts, comments[i]->comment_id,
    comments[i]->user_id,comments[i]->comment,comments[i]->user,comments[i]->comment_replied,
    comments[i]->post_commented,comments[i]->score);

    /* Print the list */
    printf("comments count: %d\n", count);
    for(i = 0; i < count; i++)
    {
        printf("TS:%ld , CID: %ld, UID: %ld, C: %s, U: %s, CR: %ld, PC: %ld, S: %d \n\n", comments[i]->ts, comments[i]->comment_id,
        comments[i]->user_id,comments[i]->comment,comments[i]->user,comments[i]->comment_replied,
        comments[i]->post_commented,comments[i]->score);
        show_comment(comments[i]);
    }
    fclose(file);
    return 0;
}
