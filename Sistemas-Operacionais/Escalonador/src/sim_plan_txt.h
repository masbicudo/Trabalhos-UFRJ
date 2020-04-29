#ifndef SIM_PLAN_MANUAL_H_
#define SIM_PLAN_MANUAL_H_

#include "utarray.h"
#include "sim_plan.h"
#include "map.h"
#include "rand.h"

typedef struct txt_sim_data txt_sim_data;
typedef struct timeline_entry timeline_entry;
typedef struct device_entry device_entry;
typedef struct txt_sim_proc txt_sim_proc;

struct timeline_entry
{
  int time;
  int action;
  int device_id;
  int sim_pid;

  bool done; // indicates that this action was already requested by the simulation
};
struct device_entry
{
  char *name;
  int duration;
  int return_queue;
};

struct txt_sim_data
{
  int sim_proc_count;
  int sim_proc_capacity;
  txt_sim_proc *sim_procs;

  map pid_map;

  UT_array *global_timeline;
  UT_array *proc_timeline;
  UT_array *devices;

  int time_slice;
};

struct txt_sim_proc
{
  int pid;
  int proc_time;
  bool dead;
};

void plan_txt_dispose(simulation_plan *plan);
void plan_txt_init(simulation_plan *plan, char *filename, int max_sim_procs);

#endif