#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "os.h"
#include "ansi_console.h"
#include "ansi_colors.h"
#include "sim_plan_txt.h"
#include "uthash.h"
#include "safe_alloc.h"

#include "config.h"

#define LOG_PROC_NEW "proc-new"
#define LOG_PROC_END "proc-end"
#define LOG_PROC_OUT "proc-out"
#define LOG_PROC_IN "proc-in "

#define ACTION_NEW 1
#define ACTION_END 2
#define ACTION_IO 3

bool plan_txt_set_time(simulation_plan *plan, int time, os *os)
{
  txt_sim_data *data = (txt_sim_data *)plan->data;
  // printing the whole plan
  if (time < 0)
  {
    printf("[os]\n");
    printf("  time_slice %d\n", data->time_slice);
    device_entry **device = NULL;
    for (int index = 0; (device = (device_entry **)utarray_next(data->devices, device)); index++)
    {
      if (index == 0)
        printf("[devices]\n");
      printf("  %s %d %d\n", (*device)->name, (*device)->duration, (*device)->return_queue);
    }
    timeline_entry *entry = NULL;
    int proc_count = 0;
    for (int index = 0; (entry = (timeline_entry *)utarray_next(data->global_timeline, entry)); index++)
    {
      if (index == 0)
        printf("[timeline]\n");
      char *action_name;
      if (entry->action == ACTION_NEW)
      {
        action_name = "new";
        proc_count++;
      }
      else if (entry->action == ACTION_END)
        action_name = "end";
      else if (entry->action == ACTION_IO)
      {
        device_entry **device = (device_entry **)utarray_eltptr(data->devices, entry->device_id);
        action_name = (*device)->name;
      }
      printf("  %d %s %d\n", entry->time, action_name, entry->sim_pid);
    }
    for (int itP = 0; itP < proc_count; itP++)
    {
      timeline_entry *entry = NULL;
      int index = 0;
      while (entry = (timeline_entry *)utarray_next(data->proc_timeline, entry))
      {
        if (entry->sim_pid != itP)
          continue;
        if (index++ == 0)
          printf("[process %d]\n", itP);
        char *action_name;
        if (entry->action == ACTION_END)
          action_name = "end";
        else if (entry->action == ACTION_IO)
        {
          device_entry **device = (device_entry **)utarray_eltptr(data->devices, entry->device_id);
          action_name = (*device)->name;
        }
        printf("  %d %s\n", entry->time, action_name);
      }
    }

    return true;
  }

  // checking for plan errors

  if (time > 0)
  {
    // looking for actions planed for the previous time
    timeline_entry *entry = NULL;
    while ((entry = (timeline_entry *)utarray_next(data->global_timeline, entry)))
    {
      if (!entry->done && entry->time < time)
      {
        entry->done = true;
        char *action_name;
        if (entry->action == ACTION_NEW)
          action_name = "new";
        else if (entry->action == ACTION_END)
          action_name = "end";
        else if (entry->action == ACTION_IO)
        {
          device_entry **device = (device_entry **)utarray_eltptr(data->devices, entry->device_id);
          action_name = (*device)->name;
        }
        printf("Error in plan! Global timeline entry was skipped: time=%d, action='%s', spid=%d", entry->time, action_name, entry->sim_pid);
      }
    }
    while ((entry = (timeline_entry *)utarray_next(data->proc_timeline, entry)))
    {
      txt_sim_proc *sim_proc = data->sim_procs + entry->sim_pid;
      if (!entry->done && entry->time < sim_proc->proc_time)
      {
        entry->done = true;
        char *action_name;
        if (entry->action == ACTION_NEW)
          action_name = "new";
        else if (entry->action == ACTION_END)
          action_name = "end";
        else if (entry->action == ACTION_IO)
        {
          device_entry **device = (device_entry **)utarray_eltptr(data->devices, entry->device_id);
          action_name = (*device)->name;
        }
        printf("Error in plan! Process spid=%d timeline entry was skipped: time=%d, action='%s'", entry->sim_pid, entry->time, action_name);
      }
    }
  }

  // checking if simulation can continue at the current state and time
  // - no new processes will be created
  // - all created processes are dead
  timeline_entry *entry = NULL;
  while ((entry = (timeline_entry *)utarray_next(data->global_timeline, entry)))
    if (entry->action == ACTION_NEW && !entry->done)
      return true;
  for (int itP = 0; itP < data->sim_proc_count; itP++)
    if (!data->sim_procs[itP].dead)
      return true;
  return false;
}
int plan_txt_incoming_processes(simulation_plan *plan, int time)
{
  txt_sim_data *data = (txt_sim_data *)plan->data;
  int count = 0;

  // it is only possible to start new processes from the global timeline sections
  timeline_entry *entry = NULL;
  while ((entry = (timeline_entry *)utarray_next(data->global_timeline, entry)))
  {
    if (entry->time == time && !entry->done && entry->action == ACTION_NEW)
    {
      int sim_pid = data->sim_proc_count;
      if (entry->sim_pid == sim_pid)
      {
        // allocating next available sim_pid
        data->sim_proc_count++;
        txt_sim_proc *sim_proc = data->sim_procs + sim_pid;
        memset(sim_proc, 0, sizeof(txt_sim_proc));

        entry->done = true;

        // incrementing count
        count++;
      }
      else
      {
        printf("Error! process ids must be given in ones increment order, beginning with 1.");
        // Program exits if file pointer returns NULL.
        exit(1);
      }
    }
  }
  return count;
}
void plan_txt_create_process(simulation_plan *plan, int time, int pid)
{
  txt_sim_data *data = (txt_sim_data *)plan->data;

  // mapping pid to sim_pid
  int sim_pid = 0;
  for (; sim_pid < data->sim_proc_count; sim_pid++)
  {
    // searching for a sim_proc that is NOT initialized (pid == 0)
    txt_sim_proc *sim_proc = data->sim_procs + sim_pid;
    if (sim_proc->pid == 0)
      break;
  }
  map_insert(&data->pid_map, pid, sim_pid);

  // filling the new sim_proc data:
  // - pid
  // - process duration
  // - IO request probability for each device
  txt_sim_proc *sim_proc = data->sim_procs + sim_pid;
  sim_proc->pid = pid;
  sim_proc->proc_time = 0;
  sim_proc->dead = false;
  // TODO: printf("t=%4d %s  pid=%2d  duration=%2d  disk=%f  tape=%f  printer=%f\n", time, LOG_PROC_NEW, pid);
}
int plan_txt_get_sim_pid(simulation_plan *plan, int pid)
{
  txt_sim_data *data = (txt_sim_data *)plan->data;
  int sim_pid;
  map_get(&data->pid_map, pid, &sim_pid);
  return sim_pid;
}
timeline_entry *find_entry(simulation_plan *plan, int time, int pid, int action_type)
{
  txt_sim_data *data = (txt_sim_data *)plan->data;
  int sim_pid = plan_txt_get_sim_pid(plan, pid);
  txt_sim_proc *sim_proc = data->sim_procs + sim_pid;

  // no more entries shall be provided for dead processes
  if (sim_proc->dead)
    return 0;

  // looking for actions planed for the given process,
  // and searching for the desired action type
  timeline_entry *entry = NULL;
  while ((entry = (timeline_entry *)utarray_next(data->global_timeline, entry)))
  {
    if (entry->time == time && entry->action == action_type && entry->sim_pid == sim_pid && !entry->done)
      return entry;
  }
  while ((entry = (timeline_entry *)utarray_next(data->proc_timeline, entry)))
  {
    if (entry->time == sim_proc->proc_time && entry->action == action_type && entry->sim_pid == sim_pid && !entry->done)
      return entry;
  }

  // no actions found
  return 0;
}
int count_entries(simulation_plan *plan, int action_type)
{
  int result = 0;
  txt_sim_data *data = (txt_sim_data *)plan->data;
  timeline_entry *entry = NULL;

  while ((entry = (timeline_entry *)utarray_next(data->global_timeline, entry)))
    if (entry->action == action_type)
      result++;

  while ((entry = (timeline_entry *)utarray_next(data->proc_timeline, entry)))
    if (entry->action == action_type)
      result++;

  return result;
}
bool plan_txt_is_process_finished(simulation_plan *plan, int time, int pid)
{
  timeline_entry *entry = find_entry(plan, time, pid, ACTION_END);
  if (entry == NULL)
    return false; // ACTION_END not found at the given time

  // when a finished process is found, we need to indicate that in the
  // sim_proc structure too to avoid ERRORs from the txt plan,
  // e.g. a dead process requesting IO
  txt_sim_data *data = (txt_sim_data *)plan->data;
  int sim_pid = plan_txt_get_sim_pid(plan, pid);
  txt_sim_proc *sim_proc = data->sim_procs + sim_pid;
  sim_proc->dead = true;

  entry->done = true;
  return true;
}
void plan_txt_run_one_time_unit(simulation_plan *plan, int time, int pid)
{
  txt_sim_data *data = (txt_sim_data *)plan->data;
  int sim_pid = plan_txt_get_sim_pid(plan, pid);
  txt_sim_proc *sim_proc = data->sim_procs + sim_pid;
  sim_proc->proc_time++;
}
int plan_txt_request_io(simulation_plan *plan, int time, int pid)
{
  timeline_entry *entry = find_entry(plan, time, pid, ACTION_IO);
  if (entry == NULL)
    return -1;
  entry->done = true;
  return entry->device_id;
}
int match_spaces(char *code)
{
  char *ptr2 = code;
  while (*ptr2 == ' ' || *ptr2 == '\t')
    ptr2++;
  return ptr2 - code;
}
int match_non_spaces(char *code)
{
  char *ptr2 = code;
  while (*ptr2 != ' ' && *ptr2 != '\t' && *ptr2 != 0)
    ptr2++;
  return ptr2 - code;
}
int match_numbers(char *code, int *value)
{
  char *ptr2 = code;
  while (*ptr2 >= '0' && *ptr2 <= '9')
    ptr2++;
  int num_cnt = ptr2 - code;
  if (num_cnt && value != NULL)
    *value = atoi(code);
  return num_cnt;
}
bool read_char(char **code, char ch)
{
  if (**code != ch)
    return false;
  (*code)++;
  return true;
}
bool read_string(char **code, const char *str)
{
  int it = 0;
  for (; str[it] != 0; it++)
  {
    if (str[it] != (*code)[it])
      return false;
  }
  (*code) += it;
  return true;
}
bool match_head(char *code, char *name, int *value)
{
  int ret_value = 0;
  code += match_spaces(code);
  if (!read_char(&code, '['))
    return false;
  code += match_spaces(code);
  if (!read_string(&code, name))
    return false;
  code += match_spaces(code);
  code += match_numbers(code, &ret_value);
  code += match_spaces(code);
  if (!read_char(&code, ']'))
    return false;
  if (value)
    *value = ret_value;
  return true;
}
bool match_entry(char *code, int *num1, char **str2, int *str_len, int *num3, int *num4)
{
  int ret_num1 = 0;
  char *ret_str2 = 0;
  int ret_num3 = 0;
  int ret_num4 = 0;

  if (num1 != NULL)
  {
    code += match_spaces(code);

    int num_len = match_numbers(code, &ret_num1);
    if (num_len == 0)
      return false;
    code += num_len;
  }

  int ret_str_len = -1;
  if (str2 != NULL)
  {
    code += match_spaces(code);

    ret_str_len = match_non_spaces(code);
    ret_str2 = code;
    code += ret_str_len;
    if (ret_str_len <= 0)
      return false;
  }

  if (num3 != NULL)
  {
    code += match_spaces(code);

    int num_len = match_numbers(code, &ret_num3);
    if (num_len == 0)
      return false;
    code += num_len;
  }

  if (num4 != NULL)
  {
    code += match_spaces(code);

    int num_len = match_numbers(code, &ret_num4);
    if (num_len == 0)
      return false;
    code += num_len;
  }

  if (num1 != NULL)
    *num1 = ret_num1;
  if (str2 != NULL)
  {
    *str2 = ret_str2;
    if (str_len != NULL)
      *str_len = ret_str_len;
  }
  if (num3 != NULL)
    *num3 = ret_num3;
  if (num4 != NULL)
    *num4 = ret_num4;

  return true;
}
void plan_txt_dispose(simulation_plan *plan)
{
  txt_sim_data *data = (txt_sim_data *)plan->data;

  utarray_free(data->devices);
  utarray_free(data->global_timeline);
  utarray_free(data->proc_timeline);

  map_dispose(&data->pid_map);
  safe_free(data->sim_procs, data);
  safe_free(plan->data, plan);
}
char *trim(char *line)
{
  while (line[0] == ' ' || line[0] == '\t')
    line++;
  char *l2 = line;
  while (l2[0] != '#' && l2[0] != '\n' && l2[0] != '\0')
    l2++;
  while (l2 - 1 > line && (l2[-1] == ' ' || l2[-1] == '\t'))
    l2--;
  l2[0] = '\0';
  return line;
}
bool plan_txt_create_device(simulation_plan *plan, int device_index, sim_plan_device *out)
{
  txt_sim_data *data = (txt_sim_data *)plan->data;
  int len = utarray_len(data->devices);
  if (device_index >= 0 && device_index < len)
  {
    device_entry **entry = (device_entry **)utarray_eltptr(data->devices, device_index);
    out->name = (*entry)->name;
    out->job_duration = (*entry)->duration;
    out->ret_queue = (*entry)->return_queue;
    return true;
  }
  return false;
}
void plan_txt_get_os_settings(simulation_plan *plan, plan_os_settings *out)
{
  txt_sim_data *data = (txt_sim_data *)plan->data;
  out->time_slice = data->time_slice;
}
void plan_txt_print_time_final_state(simulation_plan *plan, int time, os *os)
{
  txt_sim_data *data = (txt_sim_data *)plan->data;

  // printing time
  if (time < 0)
    printf("time");
  else
    printf("%4d", time);
  printf(" ");

  // printing running process
  printf(" ");
  if (time < 0)
  {
    printf(" r");
  }
  else
  {
    if (os->scheduler->current_process != NULL)
    {
      int sim_pid;
      map_get(&data->pid_map, os->scheduler->current_process->pid, &sim_pid);
      if (!(0 <= sim_pid && sim_pid < 100))
      {
        printf($red "Error! sim_pid cannot be greater or equal to 100" $cdef);
        exit(1);
      }
      printf("%2d", sim_pid);
    }
    else
    {
      printf("  ");
    }
  }
  printf(" ");

  // printing processes
  int max_proc_count = count_entries(plan, ACTION_NEW);
  for (int itP = 0; itP < max_proc_count; itP++)
  {
    printf(" ");
    if (time < 0)
    {
      char pname[4];
      memset(pname, 0, 4);
      snprintf(pname, 3, "p%d", itP);
      printf("%3s", pname);
    }
    else
    {
      if (itP < data->sim_proc_count)
      {
        txt_sim_proc *sim_proc = data->sim_procs + itP;
        if (sim_proc->dead)
        {
          printf("  x");
        }
        else
        {
          printf("%3d", sim_proc->proc_time);
        }
      }
      else
      {
        printf("   ");
      }
    }
  }
  printf(" ");

  char *str;
  int cnt_chars = 0;
  for (int itC = 0; itC < max_proc_count; itC++)
  {
    // NOTE: this works given that there will be no more than 100 processes in total
    // - it takes into account the number of digits needed to print the spid
    // - it takes into account the '>' that separates each spid
    cnt_chars += (itC < 10 ? 1 : 2) + (itC ? 1 : 0);
  }

  // printing CPU queues
  int queue_count = os->scheduler->queue_count;
  str = safe_malloc(sizeof(char) * cnt_chars + 1, NULL);
  str[cnt_chars] = 0;
  for (int itQ = 0; itQ < queue_count; itQ++)
  {
    printf(" ");
    if (time < 0)
    {
      char qname[4];
      memset(qname, 0, 4);
      snprintf(qname, 3, "q%d", itQ);
      printf("%*s", cnt_chars, qname);
    }
    else
    {
      char *str2 = str;
      process_queue *queue = os->scheduler->queues + itQ;
      for (int index = 0; index < queue->count; index++)
      {
        int inv_idx = queue->count - index - 1;
        if (index > 0)
          (str2++)[0] = '>';
        process *proc;
        pq_get(queue, inv_idx, &proc);
        int sim_pid;
        map_get(&data->pid_map, proc->pid, &sim_pid);
        if (!(0 <= sim_pid && sim_pid < 100))
        {
          printf($red "Error! sim_pid cannot be greater or equal to 100" $cdef);
          exit(1);
        }
        int sz = snprintf(NULL, 0, "%d", sim_pid);
        snprintf(str2, sz, "%d", sim_pid);
        str2 += sz;
      }
      str2[0] = 0;
      printf("%*s", cnt_chars, str);
    }
  }
  safe_free(str, NULL);
  printf(" ");

  // printing device queues
  str = safe_malloc(sizeof(char) * cnt_chars + 1, NULL);
  str[cnt_chars] = 0;
  for (int itD = 0; itD < MAX_NUMBER_OF_DEVICES; itD++)
  {
    if (!os->devices[itD].is_connected)
      continue;

    printf(" ");
    if (time < 0)
    {
      printf("%*s", cnt_chars, os->devices[itD].name);
      printf(" ");
      printf("cp");
    }
    else
    {
      char *str2 = str;
      device *device = os->devices + itD;
      process_queue *queue = device->blocked_queue;
      for (int index = 0; index < queue->count; index++)
      {
        int inv_idx = queue->count - index - 1;
        if (index > 0)
          (str2++)[0] = '>';
        process *proc;
        pq_get(queue, inv_idx, &proc);
        int sim_pid;
        map_get(&data->pid_map, proc->pid, &sim_pid);
        if (!(0 <= sim_pid && sim_pid < 100))
        {
          printf($red "Error! sim_pid cannot be greater or equal to 100" $cdef);
          exit(1);
        }
        int sz = snprintf(NULL, 0, "%d", sim_pid);
        snprintf(str2, sz, "%d", sim_pid);
        str2 += sz;
      }
      str2[0] = 0;
      printf("%*s", cnt_chars, str);

      // printing processes currently using device
      printf(" ");
      if (device->current_process != NULL)
      {
        int sim_pid;
        map_get(&data->pid_map, device->current_process->pid, &sim_pid);
        printf("%2d", sim_pid);
      }
      else
      {
        printf("  ");
      }
    }
  }
  safe_free(str, NULL);

  // printing line end
  printf("\n");
}
UT_icd device_entry_ptr_icd = {sizeof(device_entry *), 0, 0, 0};
UT_icd timeline_entry_ptr_icd = {sizeof(timeline_entry), 0, 0, 0};
void plan_txt_init(simulation_plan *plan, char *filename, int max_sim_procs)
{
  txt_sim_data *data = safe_malloc(sizeof(txt_sim_data), plan);
  plan->data = (void *)data;
  data->sim_proc_capacity = max_sim_procs;
  data->sim_proc_count = 0;
  data->sim_procs = safe_malloc(max_sim_procs * sizeof(txt_sim_proc), data);
  data->time_slice = 4; // default time_slice is 4
  map_init(&data->pid_map, max_sim_procs, max_sim_procs * 3 / 4, 0.75f);

  // we are going to read all the txt file at
  // once and fill a struct with the data
  FILE *fptr = fopen(filename, "r");
  if (fptr == NULL)
  {
    printf("Error! opening file");
    exit(1);
  }

  utarray_new(data->devices, &device_entry_ptr_icd);
  utarray_new(data->global_timeline, &timeline_entry_ptr_icd);
  utarray_new(data->proc_timeline, &timeline_entry_ptr_icd);

  char line[1001];
  line[1000] = 0;
  int mode = 0;
  int current_sim_pid = 0;
  int num1;
  char *str2;
  int num3;
  int num4;

  // reading the file stream
  // - empty lines and comments after char '#' are ignored
  // - everything before the first section is ignored
  // - everything that does not match a rule is ignored
  // these are the relevant sections:
  // - devices: contains a list of the available devices
  // - timeline: contains the global timeline, processes
  //             must be started from this timeline
  // - process N: contains the process timeline
  while (fgets(line, 1000, fptr))
  {
    char *trimmed = trim(line);
    int str_len;
    if (mode == 1 && match_entry(line, 0, &str2, &str_len, &num3, &num4))
    {
      if (num3 >= 0 && num4 >= 0)
      {
        utarray_extend_back(data->devices);
        device_entry **entry = (device_entry **)utarray_back(data->devices);
        *entry = safe_malloc(sizeof(device_entry), data->devices);

        char *str_new = safe_malloc(str_len + 1, *entry);
        strncpy(str_new, str2, str_len);
        str_new[str_len] = 0;

        // creating the device entry, and then adding to the array
        (*entry)->name = str_new;
        (*entry)->duration = num3;
        (*entry)->return_queue = num4;
      }
    }
    else if (mode == 2 && match_entry(line, &num1, &str2, &str_len, &num3, 0))
    {
      // 1
      int time = num1;
      // 2
      char *str_new = safe_malloc(str_len + 1, NULL);
      strncpy(str_new, str2, str_len);
      str_new[str_len] = 0;
      // 3
      int sim_pid = num3;

      int action_id;
      int device_id = -1;
      if (strcmp("new", str_new) == 0)
      {
        action_id = ACTION_NEW;
        device_id = 0;
      }
      else if (strcmp("end", str_new) == 0)
      {
        action_id = ACTION_END;
        device_id = 0;
      }
      else
      {
        device_entry **p = NULL;
        while ((p = (device_entry **)utarray_next(data->devices, p)))
        {
          if (strcmp((*p)->name, str_new) == 0)
          {
            action_id = ACTION_IO;
            device_id = utarray_eltidx(data->devices, p);
            break;
          }
        }
      }
      safe_free(str_new, NULL);

      // creating the timeline entry, and then adding to the array
      if (time >= 0 && action_id >= 0 && device_id >= 0 && sim_pid >= 0)
      {
        timeline_entry entry;
        entry.time = time;
        entry.action = action_id;
        entry.device_id = device_id;
        entry.sim_pid = sim_pid;
        entry.done = false;
        utarray_push_back(data->global_timeline, &entry);
      }
    }
    else if (mode == 3 && match_entry(line, &num1, &str2, &str_len, 0, 0))
    {
      // 1
      int time = num1;
      // 2
      char *str_new = safe_malloc(str_len + 1, NULL);
      strncpy(str_new, str2, str_len);
      str_new[str_len] = 0;

      int action_id;
      int device_id = -1;
      if (strcmp("new", str_new) == 0)
      {
        action_id = ACTION_NEW;
        device_id = 0;
      }
      else if (strcmp("end", str_new) == 0)
      {
        action_id = ACTION_END;
        device_id = 0;
      }
      else
      {
        device_entry **p = NULL;
        while ((p = (device_entry **)utarray_next(data->devices, p)))
        {
          if (strcmp((*p)->name, str_new) == 0)
          {
            action_id = ACTION_IO;
            device_id = utarray_eltidx(data->devices, p);
            break;
          }
        }
      }
      safe_free(str_new, NULL);

      // creating the timeline entry, and then adding to the array
      if (time >= 0 && action_id >= 0 && device_id >= 0 && current_sim_pid >= 0)
      {
        timeline_entry entry;
        entry.time = time;
        entry.action = action_id;
        entry.device_id = device_id;
        entry.sim_pid = current_sim_pid;
        entry.done = false;
        utarray_push_back(data->proc_timeline, &entry);
      }
    }
    else if (mode == 10 && match_entry(line, 0, &str2, &str_len, &num3, 0))
    {
      char *str_new = safe_malloc(str_len + 1, NULL);
      strncpy(str_new, str2, str_len);
      str_new[str_len] = 0;

      if (strcmp("time_slice", str_new) == 0)
        data->time_slice = num3;

      safe_free(str_new, NULL);
    }
    else if (trimmed[0] == '\0')
    {
      // empty line
    }
    else if (match_head(trimmed, "devices", 0))
    {
      mode = 1;
    }
    else if (match_head(trimmed, "timeline", 0))
    {
      mode = 2;
    }
    else if (match_head(trimmed, "process", &current_sim_pid))
    {
      mode = 3;
    }
    else if (match_head(trimmed, "os", &current_sim_pid))
    {
      mode = 10;
    }
  }
  fclose(fptr);

  plan->print_time_final_state = &plan_txt_print_time_final_state;
  plan->get_os_settings = &plan_txt_get_os_settings;
  plan->set_time = &plan_txt_set_time;
  plan->incoming_processes = &plan_txt_incoming_processes;
  plan->create_process = &plan_txt_create_process;
  plan->is_process_finished = &plan_txt_is_process_finished;
  plan->run_one_time_unit = &plan_txt_run_one_time_unit;
  plan->requires_io = &plan_txt_request_io;
  plan->create_device = &plan_txt_create_device;
  plan->dispose = &plan_txt_dispose;
}