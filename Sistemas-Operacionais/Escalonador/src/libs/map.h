#ifndef MAP_H_
#define MAP_H_

#include "hash.h"

typedef struct {
    int key; // key of the entry
    int value; // value of the entry
    int first; // number of positions to skip to get to the first entry
    int next; // number of positions to skip to get to the next entry
} map_entry;

typedef struct {
    map_entry* list; // list of entries
    int capacity; // total capacity of the list
    int count; // number of items in the map
    int growth_trigger; // count at which to grow the list
    float growth_rate; // how much to grow the list when growing the next time
} map;

int map_init(map* map, int capacity, int growth_trigger, float growth_rate);
int map_grow(map* map);
int map_insert(map* map, int key, int value);
int map_get(map* map, int key, int* out);
int map_delete(map* map, int key);
int map_info(map* map, char* out, int length);
void map_dispose(map* map);

#endif