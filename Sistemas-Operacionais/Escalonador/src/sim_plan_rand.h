#ifndef SIM_PLAN_RAND_H_
#define SIM_PLAN_RAND_H_

#include "sim_plan.h"
#include "map.h"
#include "rand.h"

typedef struct rand_sim_data rand_sim_data;
typedef struct rand_sim_proc rand_sim_proc;

struct rand_sim_data
{
  pcg32_random_t *rng;

  int sim_proc_count;
  int sim_proc_capacity;
  rand_sim_proc *sim_procs;

  map pid_map;

  float prob_new_process;
  float avg_proc_duration;
  float prob_new_proc_cpu_bound;
};

void plan_rand_dispose(simulation_plan *plan);

void plan_rand_init(
    simulation_plan *plan,
    uint64_t initstate,
    uint64_t initseq,
    int max_sim_procs,
    float prob_new_process,
    float avg_proc_duration,
    float prob_new_proc_cpu_bound);

struct rand_sim_proc
{
  int pid;
  int remaining_duration;

  bool requires_io;
  float avg_disk_use;    // avg disk usage per unit of time
  float avg_tape_use;    // avg tape usage per unit of time
  float avg_printer_use; // avg printer usage per unit of time
};

#endif