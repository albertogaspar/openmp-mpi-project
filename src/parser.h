#ifndef PARSER_H
#define PARSER_H
#include "post.h"
#include "comment.h"


// From (long) timestamp to (char) date formatted as MMM dd YYYY
char* ts2date(long t, char* res, int array_size);

time_t parse_ts(char* date);

//return new comment from file
struct comment* next_comment();

//return new post from file
struct comment* next_post();
