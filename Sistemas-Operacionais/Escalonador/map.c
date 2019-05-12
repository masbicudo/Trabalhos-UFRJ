#include <stdlib.h>
#include <string.h>
#include "map.h"

void map_init(map* map, int capacity, int growth_trigger, float growth_rate) {
    int size = sizeof(keyvalue[capacity]);
    keyvalue* list = malloc(size);
    memset(list, 0, size);
    map->list = list;
    map->capacity = capacity;
    map->count = 0;
    map->growth_trigger = growth_trigger;
    map->growth_rate = growth_rate;
}

int map_grow(map* map) {
    int new_capacity = (int)(map->capacity * map->growth_rate);
    keyvalue* new_list = malloc(sizeof(keyvalue[new_capacity]));
    memset(new_list, 0, new_capacity);

    // reinserting items
    int it;
    for (it = 0; it < map->capacity; it++)
        if (map->list[it].key != 0) {
            int new_index = triple32inc(map->list[it].key) % new_capacity;
            while (new_list[new_index].key != 0)
                new_index++;
            new_list[new_index] = map->list[it];
        }

    free(map->list);
    map->list = new_list;
    map->growth_trigger = (int)(map->growth_trigger * map->growth_rate);
    map->capacity = new_capacity;
}

int map_insert(map* map, int key, int value) {
    if (key <= 0) return ERR_INVALID_KEY;

    // checking whether to grow the map or not
    if (map->count >= map->growth_trigger)
        map_grow(map);

    return OK;
}
