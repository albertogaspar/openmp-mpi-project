#include "../../src/map.h"
#include "../../src/map_iterator.h"
#include <stdio.h>

int main(int argc, char* argv[]){
    int i, *v;
    int values[10] = {0,1,2,3,4,5,6,7,8,9};
    void* iterator;
    map_t map = map_init();

    for(i = 0; i < 10; i++){
        map = map_put(map,9-i,values + i);
    }
    iterator = map_it_init(map);
    while(map_it_hasnext(map, iterator)){
        long k;
        k = map_it_next(map, &iterator);
        v = map_get(map, k);
        printf("%d\n",*v);
    }
    printf("for\n");
    for(i = 0; i<10;i++){
        v = map_get(map,i);
        if(v != NULL)
            printf("%d\n",*v);
    }

    map_remove(map, 4);
    printf("for after remove\n");
    for(i = 0; i<10;i++){
        v = map_get(map,i);
        if(v != NULL)
            printf("%d\n",*v);
    }
}
