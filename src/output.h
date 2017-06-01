#ifndef OUTPUT_H
#define OUTPUT_H

#include <time.h>
#include "post.h"
#include "types.h"


void out_print_best(post* array[], time_t ts);

bool out_compare_with_best(post *best_three[], post *p);

#endif
