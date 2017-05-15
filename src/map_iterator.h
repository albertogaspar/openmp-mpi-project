#ifndef MAP_ITERATOR_H
#define MAP_ITERATOR_H

#include "map.h"

//inizialize an iterator for the map
void* map_it_init(map_t map);

//return 1 if there are more elements on which we can iterate
int map_it_hasnext(map_t map, void* state);

//return the key of the next element.
//Requires a pointer to the iterator state
void* map_it_next(map_t map, void** state);

#endif