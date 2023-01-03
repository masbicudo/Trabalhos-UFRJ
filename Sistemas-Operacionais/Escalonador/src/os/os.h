#ifndef OS_H_
#define OS_H_

#include <stdbool.h>
#include "map.h"
#include "queue.h"

typedef struct os os;
typedef struct process process;
typedef struct process_queue process_queue;
typedef struct scheduler scheduler;
typedef struct device device;

// https://en.wikipedia.org/wiki/Process_management_(computing)
#define PROC_STATE_START 0
#define PROC_STATE_READY 1
#define PROC_STATE_BLOCKED 2
#define PROC_STATE_READY_SUSPEND 3
#define PROC_STATE_BLOCKED_SUSPEND 4
#define PROC_STATE_RUNNING 5
#define PROC_STATE_EXIT 6

typedef struct process
{
    int pid;
    bool blocked;
    int current_priority; // current priority level (greater means less priority)
    int ready_since;      // when this process had become ready for the last time
} process;

typedef struct scheduler
{
    process_queue *queues;    // pointer to a list of queues by priority
    int queue_count;          // number of queues
    process *current_process; // current process
    int time_slice_end;       // when the time slice of the current process will end
} scheduler;

typedef struct device
{
    int job_duration;             // (simulation) fixed amount of time needed to complete a job
    char *name;                   // name of the device
    process_queue *blocked_queue; // pointer to a queue of blocked processes waiting for this device
    process *current_process;     // current process using this device
    int current_job_end;          // (simulation) when the os will receive a signal indicating that the job is done
    int ret_queue;                // return queue for processes whose job on this device has finished
    bool is_connected;            // returns whether device is connected
} device;

typedef struct os
{
    map pid_map;          // map of PIDs to process pointers
    int next_pid;         // next pid number to assign
    scheduler *scheduler; // process scheduler that manages ready/running processes
    device *devices;      // list of devices connected and available to the operating system

    // global information
    int max_processes;
} os;

// Quando um processo quer usar um dispositivo, ele simplesmente pede ao
// sistema operacional dizendo o que quer fazer com o dispositivo.
// O sistema operacional irá colocar o processo em modo de espera e
// inserir o mesmo na fila de espera do dispositivo. Quando o
// dispositivo estiver livre, o sistema operacional recebe um aviso que
// o fará iniciar a próxima tarefa da fila.

// Quando um processo deseja

/**
 * Process queue
 */
typedef struct process_queue
{
    process **items;
    int current;
    int count;
    int capacity;
} process_queue;

static inline int pq_init(process_queue *pq, int capacity) { return queue_init((queue *)pq, capacity); }
static inline void pq_dispose(process_queue *pq) { queue_dispose((queue *)pq); }
static inline int pq_enqueue(process_queue *pq, process *item) { return queue_enqueue((queue *)pq, (void *)item); }
static inline int pq_dequeue(process_queue *pq, process **out) { return queue_dequeue((queue *)pq, (void **)out); }
static inline int pq_get(process_queue *pq, int index, process **out) { return queue_get((queue *)pq, index, (void **)out); }

int scheduler_init(scheduler *scheduler, int queue_capacity, int num_queues);
void scheduler_dispose(scheduler *scheduler);

int device_init(device *device, char *name, int job_duration, int ret_queue, int proc_queue_size);
void device_dispose(device *device);

int os_init(os *os, int max_devices, int max_processes, int max_priority_level);
void os_dispose(os *os);

int enqueue_on_device(int time, device *device, process *process);
int select_next_process(scheduler *scheduler, process **out);

int process_init(process *p, int pid);
void process_dispose(process *process);

#endif
