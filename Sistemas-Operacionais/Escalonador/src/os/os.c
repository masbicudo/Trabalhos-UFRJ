#include <stdlib.h>
#include "os.h"
#include "return_codes.h"
#include "safe_alloc.h"

int scheduler_init(scheduler *scheduler, int queue_capacity, int queue_count)
{
    scheduler->current_process = 0;
    scheduler->queues = safe_malloc(queue_count * sizeof(queue), scheduler);
    scheduler->queue_count = queue_count;
    for (int it = 0; it < queue_count; it++)
        pq_init(scheduler->queues + it, queue_capacity);
    return OK;
}

void scheduler_dispose(scheduler *scheduler)
{
    for (int it = 0; it < scheduler->queue_count; it++)
        pq_dispose(scheduler->queues + it);
    safe_free(scheduler->queues, scheduler);
}

int device_init(device *device, char *name, int job_duration, int ret_queue, int proc_queue_size)
{
    device->name = name;
    device->job_duration = job_duration;
    device->current_process = 0;
    device->current_job_end = -1;
    device->ret_queue = ret_queue;
    device->is_connected = true;
    // Initializing device blocked queue
    device->blocked_queue = safe_malloc(sizeof(queue), device);
    pq_init(device->blocked_queue, proc_queue_size);
    return OK;
}

void device_dispose(device *device)
{
    pq_dispose(device->blocked_queue);
    safe_free(device->blocked_queue, device);
}

int os_init(os *os, int max_devices, int max_processes, int max_priority_level)
{
    os->next_pid = 1;
    os->devices = safe_malloc(max_devices * sizeof(device), os);
    os->max_processes = max_processes;
    map_init(&(os->pid_map), max_processes * 2, max_processes * 2 * 0.75, 2.0);
    os->scheduler = safe_malloc(sizeof(scheduler), os);
    scheduler_init(os->scheduler, max_processes, max_priority_level);
    return OK;
}

void os_dispose(os *os)
{
    scheduler_dispose(os->scheduler);
    safe_free(os->scheduler, os);
    map_dispose(&(os->pid_map));
    device_dispose(os->devices + 2);
    device_dispose(os->devices + 1);
    device_dispose(os->devices + 0);
    safe_free(os->devices, os);
}

int enqueue_on_device(int time, device *device, process *process)
{
    if ((device->blocked_queue)->count == 0 && device->current_process == NULL)
    {
        // There are no processes waiting to use the device and
        // no process is currently using the device, which means we can
        // give this process the control over this device.
        device->current_process = process;
        device->current_job_end = time + device->job_duration;
        return OK;
    }

    // Either there is a process currently using this device
    // or there are processes on the queue waiting to get control
    // over it. Either way, this process must be enqueued.
    int r = pq_enqueue(device->blocked_queue, process);
    return r;
}

int select_next_process(scheduler *scheduler, process **out)
{
    // iterating queues in order of priority
    for (int it = 0; it < scheduler->queue_count; it++)
    {
        process_queue *queue = scheduler->queues + it;
        if (queue->count > 0)
        {
            // get process out of the queue
            pq_dequeue(queue, out);
            return OK;
        }
    }
    // all queues are empty
    return ERR_QUEUE_EMPTY;
}

int process_init(process *p, int pid)
{
    p->pid = pid;

    p->blocked = 0; // not blocked

    p->current_priority = 0; // will start at priority queue 0 (greatest priority)
    p->ready_since = -1;     // never became ready (this is a new process)

    return OK;
}

void process_dispose(process *process)
{
}
