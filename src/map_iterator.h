#ifndef MAP_ITERATOR_H
#define MAP_ITERATOR_H

#include "map.h"

void* map_it_init(map_t map);

int map_it_hasnext(map_t map, void* state);

void* map_it_next(map_t map, void** state);

#endif