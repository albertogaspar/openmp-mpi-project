#ifndef PARSER_H
#define PARSER_H
#include "post.h"
#include "comment.h"
#include <stdio.h>


// From (long) timestamp to (char) date formatted as MMM dd YYYY
void parser_ts2date(long t, char* res, int array_size);

time_t parser_parse_ts(char* date);

//return new comment from file
struct comment* parser_next_comment(FILE **file);

//return new post from file
struct post* parser_next_post(FILE **file);

#endif
