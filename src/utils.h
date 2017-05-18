#ifndef UTILS_H
#define UTILS_H

typedef enum { false, true } bool;

/*
The total score of an active post P is computed as the sum of its own score plus
the score of all its related comments. Active posts having the same total score
should be ranked based on their timestamps (in descending order),
and if their timestamps are also the same, they should be ranked based on the
timestamps of their last received related comments (in descending order).
A comment C is related to a post P if it is a direct reply to P or if the chain
of C's preceding messages links back to P. Each new post has an initial own
score of 10 which decreases by 1 each time another 24 hours elapse since the
post's creation. Each new comment's score is also initially set to 10 and
decreases by 1 in the same way (every 24 hours since the comment's creation).
Both post and comment scores are non-negative numbers, that is, they cannot drop
below zero. A post is considered no longer active (that is, no longer part of
the present and future analysis) as soon as its total score reaches zero,
even if it receives additional comments in the future.
*/
char* paxtok (char *str, char *seps);
#endif
