#include <stdlib.h>
#include <string.h>
#include "map.h"
#include "return_codes.h"
#include "safe_alloc.h"

// https://en.wikipedia.org/wiki/Hash_table

int map_init(map *map, int capacity, int growth_trigger, float growth_rate)
{
    int size = capacity * sizeof(map_entry);
    map_entry *list = safe_malloc(size, map);
    if (list == 0)
        return ERR_OUT_OF_MEMORY;
    memset(list, 0, size);
    map->list = list;
    map->capacity = capacity;
    map->count = 0;
    map->growth_trigger = growth_trigger;
    map->growth_rate = growth_rate;
    return OK;
}

int map_grow(map *map)
{
    int new_capacity = (int)(map->capacity * map->growth_rate);
    if (new_capacity <= map->capacity)
        new_capacity = map->capacity + 1;
    map_entry *new_list = safe_malloc(new_capacity * sizeof(map_entry), map);
    memset(new_list, 0, new_capacity);

    // reinserting items
    int it;
    for (it = 0; it < map->capacity; it++)
    {
        if (map->list[it].key != 0)
        {
            int new_index = triple32inc(map->list[it].key) % new_capacity;
            // if key is zero, position is free, just add item there
            // if key is not zero, go to first position
            // while next is >= 0, go to next position
            // iterate to find a value with key equal to zero
            // add item there and set pointer from previous item
            if (new_list[new_index].key != 0)
            {
                int *ptr = &(new_list[new_index].first);
                if (*ptr >= 0)
                {
                    while (1)
                    {
                        new_index = *ptr;
                        ptr = &(new_list[new_index].next);
                        if (*ptr < 0)
                            break;
                    }
                }
                while (1)
                {
                    new_index = (new_index + 1) % new_capacity;
                    if (new_list[new_index].key == 0)
                        break;
                }
                *ptr = new_index;

                new_list[new_index].key = map->list[it].key;
                new_list[new_index].value = map->list[it].value;
                new_list[new_index].first = -1;
                new_list[new_index].next = -1;
            }
            else
            {
                new_list[new_index].key = map->list[it].key;
                new_list[new_index].value = map->list[it].value;
                new_list[new_index].first = new_index;
                new_list[new_index].next = -1;
            }
        }
    }

    safe_free(map->list, map);
    map->list = new_list;
    int new_growth_trigger = (int)(map->growth_trigger * map->growth_rate);
    if (new_growth_trigger <= map->growth_trigger)
        new_growth_trigger = map->growth_trigger + 1;
    if (new_growth_trigger > map->capacity)
        new_growth_trigger = map->capacity;
    map->growth_trigger = new_growth_trigger;
    map->capacity = new_capacity;
}

int map_insert(map *map, int key, int value)
{
    if (key <= 0)
        return ERR_INVALID_KEY;

    // checking whether to grow the map or not
    if (map->count >= map->growth_trigger)
        map_grow(map);

    int new_index = triple32inc(key) % map->capacity;
    // if key is zero, position is free, just add item there
    // if key is not zero, go to first position
    // while next is >= 0, go to next position
    // iterate to find a value with key equal to zero
    // add item there and set pointer from previous item
    if (map->list[new_index].key != 0)
    {
        int *ptr = &(map->list[new_index].first);
        if (*ptr >= 0)
        {
            while (1)
            {
                new_index = *ptr;
                if (map->list[new_index].key == key)
                    return ERR_DUPLICATE_KEY;
                ptr = &(map->list[new_index].next);
                if (*ptr < 0)
                    break;
            }
        }
        while (1)
        {
            new_index = (new_index + 1) % map->capacity;
            if (map->list[new_index].key == 0)
                break;
        }
        *ptr = new_index;

        map->list[new_index].key = key;
        map->list[new_index].value = value;
        map->list[new_index].first = -1;
        map->list[new_index].next = -1;
    }
    else
    {
        map->list[new_index].key = key;
        map->list[new_index].value = value;
        map->list[new_index].first = new_index;
        map->list[new_index].next = -1;
    }

    return OK;
}

int map_get(map *map, int key, int *out)
{
    if (key <= 0)
        return ERR_INVALID_KEY;

    int index = triple32inc(key) % map->capacity;
    // if key is zero, position is empty -> not found
    // if key is not zero, go to first position and check
    // while next is >= 0, go to next position and check
    if (map->list[index].key != 0)
    {
        int *ptr = &(map->list[index].first);
        if (*ptr >= 0)
        {
            while (1)
            {
                index = *ptr;
                if (map->list[index].key == key)
                {
                    *out = map->list[index].value;
                    return OK;
                }
                ptr = &(map->list[index].next);
                if (*ptr < 0)
                    break;
            }
        }
    }

    return ERR_KEY_NOT_FOUND;
}

int map_delete(map *map, int key)
{
    if (key <= 0)
        return ERR_INVALID_KEY;

    int index = triple32inc(key) % map->capacity;
    // if key is zero, position is empty -> not found
    // if key is not zero, go to first position and check
    // while next is >= 0, go to next position and check
    // if found, set previous item pointer to next of deleted
    // clear item
    if (map->list[index].key != 0)
    {
        int *ptr = &(map->list[index].first);
        if (*ptr >= 0)
        {
            while (1)
            {
                index = *ptr;
                if (map->list[index].key == key)
                {
                    *ptr = map->list[index].next;
                    map->list[index].key = 0;
                    map->list[index].value = 0;
                    map->list[index].first = 0;
                    map->list[index].next = 0;
                    return OK;
                }
                ptr = &(map->list[index].next);
                if (*ptr < 0)
                    break;
            }
        }
    }

    return ERR_KEY_NOT_FOUND;
}

int snprintf(char *buf, size_t size, const char *fmt, ...);

int map_info(map *map, char *out, int length)
{
    return snprintf(out, length, "map(capacity: %d; count: %d; growth_rate: %f; growth_trigger: %d)",
                    map->capacity,
                    map->count,
                    map->growth_rate,
                    map->growth_trigger);
}

void map_dispose(map *map)
{
    safe_free(map->list, map);
}
