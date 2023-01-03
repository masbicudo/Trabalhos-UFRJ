#ifndef QUEUE_H_
#define QUEUE_H_

typedef struct _queue queue;

typedef struct _queue {
    void** items;
    int current;
    int count;
    int capacity;
} queue;

int queue_init(queue* pq, int capacity);
void queue_dispose(queue* pq);
int queue_enqueue(queue* pq, void* process);
int queue_dequeue(queue* pq, void** out);
int queue_get(queue* pq, int index, void** out);

#endif
