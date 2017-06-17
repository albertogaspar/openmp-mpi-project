#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "post.h"
#include "comment.h"
#include "constants.h"

#define LIST_MAX 10
#define SPLITTER "|"

// res is the container for the date, array_size should be at least 32
void parser_ts2date(time_t t, char* res, int array_size)
{
        const char *format = default_format;
        struct tm lt;
        char ms[10];
        sprintf(ms, ".%ld+0000", t%1000);
        t = t/1000;
        localtime_r(&t, &lt);
        if (strftime(res, array_size, format, &lt) == 0) {
                (void) fprintf(stderr,  "strftime(3): cannot format supplied "
                                        "date/time into buffer of size %u "
                                        "using: '%s'\n",
                                        array_size, format);
                strcpy(res, "Error");
                return;
        }
        strcat(res, ms);

        printf("OUTPUT_PARSER:%u -> '%s'\n", (unsigned) t, res);
}

time_t parse_ts(char* date)
{
	struct tm mytm;
	time_t t;
	int millisec;
    printf("PARSER: ts read: %s\n", date);
    char* tokens = strsep(&date, ".");

    strptime(tokens,"%Y-%m-%dT%H:%M:%S",&mytm);
    mytm.tm_isdst = 0;

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
	printf("COMMENT_PARSER: line = %s\n", line);
    time_t ts = parse_ts(strsep(&line, SPLITTER));
    long comment_id = strtol_def(strsep(&line, SPLITTER), -1);
    long user_id = strtol_def(strsep(&line, SPLITTER), -1);

    char* com = strsep(&line, SPLITTER);
    char* content = (char*) malloc((strlen(com)+1)*sizeof(char));
    strcpy(content, com);

    char* us = strsep(&line, SPLITTER);
    char* user = (char*) malloc((strlen(us)+1)*sizeof(char));
    strcpy(user, us);

    long comment_replied = strtol_def(strsep(&line, SPLITTER), -1);
    printf("PARSER: Comment replied = %ld\n", comment_replied);
    long commented_post = strtol_def(strsep(&line, "\n"), -1);
    printf("PARSER: Commented post = %ld\n", commented_post);


    return comment_create(ts, comment_id, user_id, content, user, comment_replied, commented_post);
}

struct comment* parser_next_comment(FILE** file)
{
    char line[2000];
    // Allocation of space for new comment
    struct comment* new_comment;
    // Read one line of the file
    char* s = fgets(line, 2000, *file);
    if (s == NULL){
		printf("PARSER: read NULL\n");
		return NULL;
	}
    new_comment = parse_comment(line);
    return new_comment;
}

struct post* parse_post(char* line){
	printf("POST_PARSER: line = %s\n", line);
    time_t ts = parse_ts(strsep(&line, SPLITTER));

    long post_id = strtol_def(strsep(&line, SPLITTER),-1);

    long user_id = strtol_def(strsep(&line, SPLITTER),-1);

    char* p = strsep(&line, SPLITTER);
    char* content = (char*) malloc((strlen(p)+1)*sizeof(char));
    strcpy(content, p);
    printf("PARSER: content = %s\n", content);

    char* us = strsep(&line, "\n");
    char* user = (char*) malloc((strlen(us)+1)*sizeof(char));
    strcpy(user, us);
    printf("PARSER: user = %s\n", user);

    return post_create(ts, post_id, user_id, content, user);
}

struct post* parser_next_post(FILE **file)
{
    char line[500];
    // Allocation of space for new comment
    struct post* new_post;
    // Read one line of the file
    char* s = fgets(line, 500, *file);

	if (s == NULL){
		printf("PARSER: read NULL\n");
		return NULL;
	}

    new_post = parse_post(line);

    return new_post;
}
