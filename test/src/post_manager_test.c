#include "../../src/post.h"
#include "../../src/post_manager.h"
#include "../../src/map.h"
#include "../../src/map_iterator.h"
#include "../../src/utils.h"
#include <omp.h>

//period of decrementation of the score in milliseconds
#define PERIOD_OF_DECR 24 * 3600 * 1000

void map_print(map_t map)
{
    int *v;
    void* iterator;
    iterator = map_it_init(map);
    while(map_it_hasnext(map, iterator)){
        long * k;
        k = map_it_next(map, &iterator);
        v = map_get(map, *k);
        printf("%d\n",*v);
    }
}

int main(int argc, char *argv[]){
    int i=0;
    int count;
    long int current_ts;
    map_t posts = map_init();
    struct post* p = NULL;

    while(p = parser_next_post() || i < 10) {
        map_put(posts, p->post_id, p);
        i++;
    }
    current_ts = p->ts;
    current_ts += 2*PERIOD_OF_DECR;

    printf("Elements inside list\n");
    map_print(posts);

    printf("Elements inside list AFTER two daily decrement\n");
    daily_decrement(posts, current_ts);
    map_print(posts);
}
