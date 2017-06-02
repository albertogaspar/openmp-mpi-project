#include <stdlib.h>
#include <stdio.h>
#include "map.h"
#include "map_iterator.h"

map_t map_init(){
    return NULL;
}

map_t map_put(map_t map, long key, void *value){
	if( map_get(map, key)!=NULL ){
		return map;
	}
    node_t *new = (node_t*) malloc(sizeof(node_t));
    new->key = key;
    new->value = value;
    new->next = map;
    return new;
}

map_t map_remove(map_t map, long key){
    if (map_is_empty(map))
        return NULL;
    if(map->key == key){
        map_t temp = map->next;
        if (map == NULL)
            	printf("MAP: trying to remove item with key %ld, but it is NULL\n", key);
        free(map);
        return temp;
    }
    map->next = map_remove(map->next, key);
    return map;

}

map_t map_empty(map_t map){
    if(map == NULL)
    	return map;
    if(map->next != NULL){
    	map->next = map_empty(map->next);
    }
    if (map == NULL)
    	printf("MAP: trying to empty map, but it is NULL\n");
    free(map);
    return NULL;
}


void* map_get(map_t map, long key){
    while(map != NULL){
        if(map->key == key)
            return map->value;
        map = map->next;
    }
    return NULL;
}

int map_is_empty(map_t map){
    if (map == NULL)
        return 1;
    return 0;
}

int map_size(map_t map){
	int size = 0;
    while(map != NULL){
    	size++;
        map = map->next;
    }
    return size;
}


void* map_it_init(map_t map){
    void* state = map;
    return state;
}

int map_it_hasnext(map_t map, void* state){
    if(state == NULL){
        return 0;
    }
    return 1;
}

long map_it_next(map_t map, void** state_pointer){
    node_t* state = (node_t*) *state_pointer;
    long key = state->key;
    *state_pointer = state->next;
    return key;
}
