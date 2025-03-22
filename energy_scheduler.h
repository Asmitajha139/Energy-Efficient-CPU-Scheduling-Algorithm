#ifndef ENERGY_SCHEDULER_H
#define ENERGY_SCHEDULER_H
#define MAX_TASKS 100 
typedef struct {
    int task_id;         
    int exec_time;       
    int arrival_time;   
    int completed;       

typedef struct {
    Task tasks[MAX_TASKS];  
    int task_count;        
    int total_energy;       
    int current_time;       
} Scheduler;

void init_scheduler(Scheduler *sched);              
void add_task(Scheduler *sched, int id, int exec_time, int arrival_time);  
void schedule_tasks(Scheduler *sched);              
void print_schedule(Scheduler *sched);             
#endif
