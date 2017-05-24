#ifndef UTILS_H
#define UTILS_H

#define TAG 1

typedef enum { false, true } bool;

typedef enum {
    MASTER, POST_MANAGER, COMMENT_MANAGER, OUT_MANAGER
} proc;

int utils_compare_int(int first, int second);

char* paxtok (char *str, char *seps);
#endif
