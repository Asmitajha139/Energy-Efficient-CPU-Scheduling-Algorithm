#include <stdio.h>
#include "energy_scheduler.h"


#define ENERGY_ACTIVE 500  
#define ENERGY_IDLE 50    
#define STATE_TRANSITION_PENALTY 100 

void init_scheduler(Scheduler *sched) {
    sched->task_count = 0;
    sched->total_energy = 0;
    sched->current_time = 0;
    for (int i = 0; i < MAX_TASKS; i++) {
        sched->tasks[i].completed = 1;  
    }
}
void add_task(Scheduler *sched, int id, int exec_time, int arrival_time) {
    if (sched->task_count < MAX_TASKS) {
        Task *t = &sched->tasks[sched->task_count];
        t->task_id = id;
        t->exec_time = exec_time;
        t->arrival_time = arrival_time;
        t->completed = 0; 
        sched->task_count++;
        printf("Added Task %d: Exec Time = %dms, Arrival = %dms\n", id, exec_time, arrival_time);
    } else {
        printf("Task queue full!\n");
    }
}
void schedule_tasks(Scheduler *sched) {
    int active = 0;
    printf("\nScheduling started at time %dms\n", sched->current_time);

    while (1) {
        int tasks_pending = 0;
       
        for (int i = 0; i < sched->task_count; i++) {
            if (!sched->tasks[i].completed) {
                tasks_pending = 1;
                break;
            }
        }
        if (!tasks_pending) break;  

        int batch_energy = 0;
        int batch_time = 0;
        int tasks_batched = 0;

        for (int i = 0; i < sched->task_count; i++) {
            if (!sched->tasks[i].completed && sched->tasks[i].arrival_time <= sched->current_time) {
                Task *t = &sched->tasks[i];
                batch_energy += t->exec_time * ENERGY_ACTIVE;
                batch_time += t->exec_time;
                t->completed = 1;
                tasks_batched++;
                printf("Time %dms: Batched Task %d, exec time %dms\n", 
                       sched->current_time + batch_time, t->task_id, t->exec_time);
            }
        }

        if (tasks_batched > 0) {
          
            if (!active) {
              
                sched->total_energy += STATE_TRANSITION_PENALTY;
                active = 1;
                printf("Time %dms: CPU active, energy +%d (transition)\n", 
                       sched->current_time, STATE_TRANSITION_PENALTY);
            }
            sched->total_energy += batch_energy;
            sched->current_time += batch_time;
            printf("Time %dms: Batch completed, energy +%d\n", 
                   sched->current_time, batch_energy);
        } else {
          
            if (active) {
              
                sched->total_energy += STATE_TRANSITION_PENALTY;
                active = 0;
                printf("Time %dms: CPU idle, energy +%d (transition)\n", 
                       sched->current_time, STATE_TRANSITION_PENALTY);
            }
            sched->total_energy += ENERGY_IDLE;
            sched->current_time++;
        }
    }
    printf("Scheduling completed. Total energy: %d mJ\n", sched->total_energy);
}


void print_schedule(Scheduler *sched) {
    printf("\nFinal Schedule:\n");
    printf("Task ID | Exec Time | Arrival Time | Completed\n");
    for (int i = 0; i < sched->task_count; i++) {
        printf("%7d | %9d | %12d | %9d\n", 
               sched->tasks[i].task_id, 
               sched->tasks[i].exec_time, 
               sched->tasks[i].arrival_time, 
               sched->tasks[i].completed);
    }
    printf("Total Energy Consumed: %d mJ\n", sched->total_energy);
}


int main() {
    Scheduler sched;
    init_scheduler(&sched);


    add_task(&sched, 1, 2, 0);  
    add_task(&sched, 2, 3, 1);  
    add_task(&sched, 3, 1, 5);  

    schedule_tasks(&sched);
    print_schedule(&sched);

    return 0;
}
