#include "hash.h"

#define OK 0
#define ERR_INVALID_KEY 100

typedef struct {
    int key;
    int value;
} keyvalue;

typedef struct {
    keyvalue* list; // list of key/values pairs
    int capacity; // total capacity of the list
    int count; // number of items in the map
    int growth_trigger; // count at which to grow the list
    float growth_rate; // how much to grow the list when growing the next time
} map;
