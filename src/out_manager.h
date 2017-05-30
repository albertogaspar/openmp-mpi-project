#ifndef OUT_MANAGER_H
#define OUT_MANAGER_H

#include <time.h>
#include "post.h"
#include "types.h"

void out_manager_run();

out_tuple out_create_tuple(post *p);

#endif
