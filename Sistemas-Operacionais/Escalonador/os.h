#include <stdbool.h>
#include "map.h"

// https://en.wikipedia.org/wiki/Process_management_(computing)
#define PROC_STATE_START 0
#define PROC_STATE_READY 1
#define PROC_STATE_BLOCKED 2
#define PROC_STATE_READY_SUSPEND 3
#define PROC_STATE_BLOCKED_SUSPEND 4
#define PROC_STATE_RUNNING 5
#define PROC_STATE_EXIT 6

typedef struct {
    int pid;
    bool blocked;
} process;

typedef struct {
    process** items;
    int length;
} process_list;

typedef struct {
    process_list* queues; // pointer to a list of queues by priority
} scheduler;

typedef struct {
    process_list blocked; // pointer to a list of blocked queues by device
} device;

typedef struct {
    process_list processes; // list of all processes
    map pid_map; // map of PIDs to indexes in the process list
    scheduler scheduler; // process scheduler that manages ready/running processes
    device* devices; // list of devices connected and available to the operating system
} master;

// Quando um processo quer usar um dispositivo, ele simplesmente pede ao
// sistema operacional dizendo o que quer fazer com o dispositivo.
// O sistema operacional irá colocar o processo em modo de espera e
// inserir o mesmo na fila de espera do dispositivo. Quando o
// dispositivo estiver livre, o sistema operacional recebe um aviso que
// o fará iniciar a próxima tarefa da fila.

// Quando um processo deseja 
