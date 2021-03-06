#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <time.h>

#define DAILY_DECREMENT -1
#define STARTING_SCORE 10
#define STOP -1
#define NUM_OF_BEST 3
#define NUM_OF_THREADS 4

//period of decrement of the score in milliseconds
#define PERIOD_OF_DECR 24 * 3600 * 1000

#define COMMENTS_FILE "resources/comments.dat"
#define POSTS_FILE "resources/posts-tiny.dat"
#define OUTPUT_FILE "/home/hduser/Scrivania/output.dat"

static const time_t default_time = 1230728833;
static const char default_format[] = "%Y-%m-%dT%H:%M:%S";

#endif
