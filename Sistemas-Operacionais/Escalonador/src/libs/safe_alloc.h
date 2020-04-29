#ifndef _SAFE_ALLOC_H
#define _SAFE_ALLOC_H

static void* safe_malloc(int size, void* parent);
static void safe_free(void* ptr, void* parent);

#include <stdlib.h>
#include <stdio.h>

#define _DEBUG 1

#if _DEBUG == 1
static void *safe_malloc(int size, void *parent)
{
    int total_size = sizeof(void *) + size;
    void **base_ptr = (void **)malloc(total_size);
    *base_ptr = parent;
    void *ptr = (void *)(base_ptr + 1);
    return ptr;
}
#else
#define safe_malloc(size, parent) (malloc(size))
#endif

#if _DEBUG == 1
static void safe_free(void *ptr, void *parent)
{
    void **base_ptr = (void **)ptr;
    base_ptr--;
    if (*base_ptr != parent)
    {
        printf("Error! Allocation pointers do not match.");
        exit(1);
    }
    free(base_ptr);
}
#else
#define safe_free(size, parent) (free(size))
#endif

#endif