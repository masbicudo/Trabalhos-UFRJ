#include <stdlib.h>
#include <string.h>
#include "queue.h"
#include "return_codes.h"
#include "safe_alloc.h"

int queue_init(queue *pq, int capacity)
{
    pq->current = 0;
    pq->count = 0;
    pq->capacity = capacity;
    pq->items = safe_malloc(capacity * sizeof(void *), pq);
    return OK;
}

void queue_dispose(queue *pq)
{
    safe_free(pq->items, pq);
}

int queue_enqueue(queue *pq, void *item)
{
    if (pq->count == pq->capacity)
        return ERR_QUEUE_FULL;

    int index = (pq->current + pq->count) % pq->capacity;
    pq->items[index] = item;
    pq->count++;

    return OK;
}

int queue_dequeue(queue *pq, void **out)
{
    if (pq->count == 0)
        return ERR_QUEUE_EMPTY;

    *out = pq->items[pq->current];
    pq->count--;
    pq->current = (pq->current + 1) % pq->capacity;

    return OK;
}

int queue_get(queue *pq, int index, void **out)
{
    if (index < 0 || index >= pq->count)
        return ERR_OUT_OF_BOUNDS;
    *out = pq->items[(pq->current + index) % pq->capacity];
    return OK;
}
