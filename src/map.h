#ifndef MAP_H
#define MAP_H

typedef struct node_t
{
    void * value;
    long key;
    struct node_t *next;
}node_t;

typedef node_t* map_t;

// Inizialize a new map;
map_t map_init();

// Insert a value in the map
map_t map_put(map_t map, long key, void *value);

// remove the value having key as key from the map
map_t map_remove(map_t map, long key);

// remove all the elements in the map and return an empty map
map_t map_empty(map_t map);

// return the value of having key as key
void * map_get(map_t map, long key);

//return 1 if empty 0 otherwise
int map_is_empty(map_t map);

//return the size of the map
int map_size(map_t map);

#endif
