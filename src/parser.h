#ifndef PARSER_H
#define PARSER_H
#include "post.h"
#include "comment.h"


//return new comment from file
struct comment* next_comment();

//return new post from file
struct comment* next_post();
