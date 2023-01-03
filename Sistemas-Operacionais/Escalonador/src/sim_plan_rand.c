#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "os.h"
#include "sim_plan_rand.h"
#include "rand_distributions.h"
#include "safe_alloc.h"

#define LOG_PROC_NEW "proc-new"
#define LOG_PROC_END "proc-end"
#define LOG_PROC_OUT "proc-out"
#define LOG_PROC_IN "proc-in "

int plan_rand_incoming_processes(simulation_plan *plan, int time)
{
  rand_sim_data *data = (rand_sim_data *)plan->data;
  int count = drand(data->rng) < data->prob_new_process ? rnd_poisson(data->rng, 10) : 0;
  return count;
}
void plan_rand_create_process(simulation_plan *plan, int time, int pid)
{
  rand_sim_data *data = (rand_sim_data *)plan->data;

  // mapping pid to sim_pid
  int sim_pid = data->sim_proc_count;
  map_insert(&data->pid_map, pid, sim_pid);
  data->sim_proc_count++;
  rand_sim_proc *sim_proc = data->sim_procs + sim_pid;
  memset(sim_proc, 0, sizeof(struct rand_sim_proc));

  // filling the new sim_proc data:
  // - process duration
  // - IO request probability for each device
  double prob = drand(data->rng);
  int duration = 1 + (data->avg_proc_duration - 1) * rnd_poisson(data->rng, 4);
  sim_proc->remaining_duration = duration;
  if (prob < data->prob_new_proc_cpu_bound)
  {
    sim_proc->requires_io = false;
    sim_proc->avg_disk_use = 0;
    sim_proc->avg_tape_use = 0;
    sim_proc->avg_printer_use = 0;

    printf("t=%4d %s  pid=%2d  duration=%2d  disk=%f  tape=%f  printer=%f\n", time, LOG_PROC_NEW, pid, duration, 0.0, 0.0, 0.0);
  }
  else
  {
    prob = (prob - data->prob_new_proc_cpu_bound) / (1 - data->prob_new_proc_cpu_bound);

    float avg_disk_use = prob < 0.33 ? 0.5 : 0.0;                 // avg number of disk calls per time unit
    float avg_tape_use = prob >= 0.33 && prob < 0.67 ? 0.3 : 0.0; // avg number of tape calls per time unit
    float avg_printer_use = prob >= 0.67 ? 0.1 : 0.0;             // avg number of printer calls per time unit

    sim_proc->requires_io = (avg_disk_use != 0.0) || (avg_tape_use != 0.0) || (avg_printer_use != 0.0);
    sim_proc->avg_disk_use = avg_disk_use;
    sim_proc->avg_tape_use = avg_tape_use;
    sim_proc->avg_printer_use = avg_printer_use;

    printf("t=%4d %s  pid=%2d  duration=%2d  disk=%f  tape=%f  printer=%f\n", time, LOG_PROC_NEW, pid, duration, avg_disk_use, avg_tape_use, avg_printer_use);
  }
}
rand_sim_proc *plan_rand_get_sim_proc(simulation_plan *plan, int pid)
{
  rand_sim_data *data = (rand_sim_data *)plan->data;
  int sim_pid;
  map_get(&data->pid_map, pid, &sim_pid);
  rand_sim_proc *sim_proc = &data->sim_procs[sim_pid];
  return sim_proc;
}
bool plan_rand_is_process_finished(simulation_plan *plan, int time, int pid)
{
  rand_sim_proc *sim_proc = plan_rand_get_sim_proc(plan, pid);
  return sim_proc->remaining_duration == 0;
}
void plan_rand_run_one_time_unit(simulation_plan *plan, int time, int pid)
{
  rand_sim_proc *sim_proc = plan_rand_get_sim_proc(plan, pid);
  sim_proc->remaining_duration--;
}
int plan_rand_request_io(simulation_plan *plan, int time, int pid)
{
  rand_sim_data *data = (rand_sim_data *)plan->data;
  rand_sim_proc *sim_proc = plan_rand_get_sim_proc(plan, pid);
  if (drand(data->rng) <= sim_proc->avg_disk_use)
    return 0;
  if (drand(data->rng) <= sim_proc->avg_tape_use)
    return 1;
  else if (drand(data->rng) <= sim_proc->avg_printer_use)
    return 2;
  return -1;
}
bool plan_rand_create_device(simulation_plan *plan, int device_index, sim_plan_device *out)
{
  if (device_index == 0)
  {
    out->name = "disk";
    out->job_duration = 3;
    out->ret_queue = 1;
  }
  if (device_index == 1)
  {
    out->name = "tape";
    out->job_duration = 8;
    out->ret_queue = 0;
  }
  if (device_index == 2)
  {
    out->name = "print";
    out->job_duration = 15;
    out->ret_queue = 0;
  }
  return device_index >= 0 && device_index < 3;
}
void plan_rand_get_os_settings(simulation_plan *plan, plan_os_settings *out)
{
  out->time_slice = 4;
}
void plan_rand_print_time_final_state(simulation_plan *plan, int time, os *os)
{
}
void plan_rand_dispose(simulation_plan *plan)
{
  rand_sim_data *data = (rand_sim_data *)plan->data;
  map_dispose(&data->pid_map);
  safe_free(data->rng, data);
  safe_free(data->sim_procs, data);
  safe_free(plan->data, plan);
}
void plan_rand_init(
    simulation_plan *plan,
    uint64_t initstate,
    uint64_t initseq,
    int max_sim_procs,
    float prob_new_process,
    float avg_proc_duration,
    float prob_new_proc_cpu_bound)
{
  // the random simulation requires some data to run
  // - a pointer to the rng
  // - a list of simulated processes, each containing:
  //    - the pid of the process in the OS struct
  //    - the duration of the process, tells when it will terminate
  //        (TODO: this should be determined at the moment, not in advance)
  //    - probability of IO requests for each device
  rand_sim_data *data = safe_malloc(sizeof(rand_sim_data), plan);

  // seeding the random number generator
  pcg32_random_t *r = safe_malloc(sizeof(pcg32_random_t), data);
  pcg32_srandom_r(r, initstate, initseq); // 2 very large primes

  plan->data = (void *)data;
  data->rng = r;
  data->sim_proc_capacity = max_sim_procs;
  data->sim_proc_count = 0;
  data->sim_procs = safe_malloc(max_sim_procs * sizeof(rand_sim_proc), data);
  map_init(&data->pid_map, max_sim_procs, max_sim_procs * 3 / 4, 0.75f);

  plan->print_time_final_state = &plan_rand_print_time_final_state;
  plan->get_os_settings = &plan_rand_get_os_settings;
  plan->set_time = NULL;
  plan->incoming_processes = &plan_rand_incoming_processes;
  plan->create_process = &plan_rand_create_process;
  plan->is_process_finished = &plan_rand_is_process_finished;
  plan->run_one_time_unit = &plan_rand_run_one_time_unit;
  plan->requires_io = &plan_rand_request_io;
  plan->dispose = &plan_rand_dispose;
}