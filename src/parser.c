#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "post.h"
#include "comment.h"
#include "constants.h"

#define LIST_MAX 10
#define SPLITTER "|"
#define _XOPEN_SOURCE

// res is the container for the date, array_size should be at least 32
/*void parser_ts2date(long t, char* res, int array_size)
{
        const char *format = default_format;
        struct tm lt;
        localtime_r(&t, &lt);
        if (strftime(res, array_size, format, &lt) == 0) {
                (void) fprintf(stderr,  "strftime(3): cannot format supplied "
                                        "date/time into buffer of size %u "
                                        "using: '%s'\n",
                                        array_size, format);
                return NULL;
        }
        printf("%u -> '%s'\n", (unsigned) t, res);
} */

time_t parse_ts(char* date)
{
	struct tm mytm;
	time_t t;
	int millisec;
    printf("PARSER: ts read: %s\n", date);
    char* tokens = strsep(&date, ".");

    strptime(tokens,"%Y-%m-%dT%H:%M:%S",&mytm);
    tokens = strsep(&date, "+");

    t = mktime(&mytm);
    millisec = atoi(tokens);

    t = t*1000 + millisec;
    printf("PARSER: t=%ld\n",t);
    return t;
}


long int strtol_def(char* string, long int def)
{
    if (strcmp(string, "") == 0 || strcmp(string, "\n") == 0)
        return def;
    return strtol(string, NULL, 10);
}

struct comment* parse_comment(char* line){
    time_t ts = parse_ts(strsep(&line, SPLITTER));
    long comment_id = strtol_def(strsep(&line, SPLITTER), -1);
    long user_id = strtol_def(strsep(&line, SPLITTER), -1);

    char* com = strsep(&line, SPLITTER);
    char* content = (char*) malloc(strlen(com)*sizeof(char));
    strcpy(content, com);

    char* us = strsep(&line, SPLITTER);
    char* user = (char*) malloc(strlen(us)*sizeof(char));
    strcpy(user, us);

    long comment_replied = strtol_def(strsep(&line, SPLITTER), -1);
    long commented_post = strtol_def(strsep(&line, SPLITTER), -1);

    return comment_create(ts, comment_id, user_id, content, user, comment_replied, commented_post);
}

struct comment* parser_next_comment(FILE** file)
{
    char line[500];
    // Allocation of space for new comment
    struct comment* new_comment;
    // Read one line of the file
    fgets(line, 500, *file);
    if (line == NULL || new_comment==NULL)
        return NULL;
    new_comment = parse_comment(line);
    return new_comment;
}

struct post* parse_post(char* line){
	printf("PARSER: line = %s\n", line);
    time_t ts = parse_ts(strsep(&line, SPLITTER));

    long post_id = strtol_def(strsep(&line, SPLITTER),-1);

    long user_id = strtol_def(strsep(&line, SPLITTER),-1);

    char* p = strsep(&line, SPLITTER);
    char* content = (char*) malloc(strlen(p)*sizeof(char));
    strcpy(content, p);

    char* us = strsep(&line, SPLITTER);
    char* user = (char*) malloc(strlen(us)*sizeof(char));
    strcpy(user, us);

    return post_create(ts, post_id, user_id, content, user);
}

struct post* parser_next_post(FILE **file)
{
    char line[500];
    // Allocation of space for new comment
    struct post* new_post;
    // Read one line of the file
    fgets(line, 500, *file);
    printf("PARSER: read line from file= %s\n", line);
    if (line == NULL ){
    	printf("PARSER: read NULL");
    	return NULL;
    }

    new_post = parse_post(line);

    return new_post;
}
//
// int main(int argc, char *argv[])
// {
//     struct comment* comments[LIST_MAX];
//     int count = 0;
//     int i = 0;
//
//     FILE *file;
//     file = fopen(COMMENT_FILE,"r");
//     if(!file)
//     {
//         perror("Error opening file");
//         return -1;
//     }
//
//     //Initialize the array
//     char str[500];
//     //memset(comments, 0, sizeof(comments));
//     while(fgets(str, 500, (FILE*) file)              /* Check for the end of file*/
//         &&(count < LIST_MAX)) /* To avoid memory corruption */
//     {
//         printf("%d\n", count);
//         printf("%s\n", str);
//
//         struct comment* new_comment;
//         new_comment = (struct comment*) malloc(sizeof(struct comment));
//         if (new_comment==NULL){
//             return NULL;
//         }
//         parse_comment(new_comment, str);
//         //init_comment(new_comment);
//         comments[count] = new_comment;
//         printf("TS:%ld , CID: %ld, UID: %ld, C: %s, U: %s, CR: %ld, PC: %ld, S: %d \n\n", comments[count]->ts, comments[count]->comment_id,
//         comments[count]->user_id,comments[count]->comment,comments[count]->user,comments[count]->comment_replied,
//         comments[count]->post_commented,comments[count]->score);
//         if (comments[count] == NULL)
//         {
//                 printf("Comment not correctly allocated\n" );
//                 return 0;
//         }
//         count++;
//     }
//     i=0;
//     printf("TS:%ld , CID: %ld, UID: %ld, C: %s, U: %s, CR: %ld, PC: %ld, S: %d \n\n", comments[i]->ts, comments[i]->comment_id,
//     comments[i]->user_id,comments[i]->comment,comments[i]->user,comments[i]->comment_replied,
//     comments[i]->post_commented,comments[i]->score);
//
//     /* Print the list */
//     printf("comments count: %d\n", count);
//     for(i = 0; i < count; i++)
//     {
//         printf("TS:%ld , CID: %ld, UID: %ld, C: %s, U: %s, CR: %ld, PC: %ld, S: %d \n\n", comments[i]->ts, comments[i]->comment_id,
//         comments[i]->user_id,comments[i]->comment,comments[i]->user,comments[i]->comment_replied,
//         comments[i]->post_commented,comments[i]->score);
//         show_comment(comments[i]);
//     }
//     fclose(file);
//     return 0;
// }
