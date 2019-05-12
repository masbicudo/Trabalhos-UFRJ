#include "hash.h"

#define OK 0
#define ERR_INVALID_KEY 100
#define ERR_DUPLICATE_KEY 101
#define ERR_KEY_NOT_FOUND 102

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
