#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "energy_scheduler.h"

#define ENERGY_ACTIVE_BASE 500    
#define ENERGY_IDLE 50           
#define STATE_TRANSITION_PENALTY 100  
#define LOG_FILE "scheduler_log.txt"
FILE *log_file;

void init_scheduler(Scheduler *sched) {
    sched->task_count = 0;
    sched->total_energy = 0;
    sched->current_time = 0;
    for (int i = 0; i < MAX_TASKS; i++) {
        sched->tasks[i].completed = 1;  
    }
}


int calculate_batch_energy(int exec_time, int active, int *new_active) {
    int energy = 0;
    if (!active) {
        energy += STATE_TRANSITION_PENALTY;  
        *new_active = 1;
    }
    
    int energy_active = (exec_time < 2) ? ENERGY_ACTIVE_BASE / 2 : ENERGY_ACTIVE_BASE;
    energy += exec_time * energy_active;
    return energy;
}

int add_task(Scheduler *sched, int id, int exec_time, int arrival_time, int priority) {
    if (sched->task_count >= MAX_TASKS) {
        fprintf(stderr, "Error: Task queue full\n");
        fprintf(log_file, "Error: Task queue full\n");
        return 0;
    }
    if (exec_time <= 0 || arrival_time < 0 || priority < 0) {
        fprintf(stderr, "Error: Invalid task parameters (id=%d, exec_time=%d, arrival=%d, priority=%d)\n",
                id, exec_time, arrival_time, priority);
        fprintf(log_file, "Error: Invalid task parameters (id=%d, exec_time=%d, arrival=%d, priority=%d)\n",
                id, exec_time, arrival_time, priority);
        return 0;
    }
    Task *t = &sched->tasks[sched->task_count++];
    t->task_id = id;
    t->exec_time = exec_time;
    t->arrival_time = arrival_time;
    t->priority = priority;
    t->completed = 0;
    printf("Added Task %d: Exec Time = %dms, Arrival = %dms, Priority = %d\n", 
           id, exec_time, arrival_time, priority);
    fprintf(log_file, "Added Task %d: Exec Time = %dms, Arrival = %dms, Priority = %d\n", 
            id, exec_time, arrival_time, priority);
    return 1;
}


int has_pending_tasks(Scheduler *sched) {
    for (int i = 0; i < sched->task_count; i++) {
        if (!sched->tasks[i].completed) return 1;
    }
    return 0;
}


void sort_tasks_by_exec_time(Scheduler *sched, int start, int end) {
    for (int i = start; i < end - 1; i++) {
        for (int j = i + 1; j < end; j++) {
            if (sched->tasks[i].exec_time > sched->tasks[j].exec_time && 
                sched->tasks[i].priority == sched->tasks[j].priority) {
                Task temp = sched->tasks[i];
                sched->tasks[i] = sched->tasks[j];
                sched->tasks[j] = temp;
            }
        }
    }
}


void schedule_tasks(Scheduler *sched) {
    int active = 0;
    printf("\nScheduling started at time %dms\n", sched->current_time);
    fprintf(log_file, "\nScheduling started at time %dms\n", sched->current_time);

    while (has_pending_tasks(sched)) {
        int batch_energy = 0;
        int batch_time = 0;
        int tasks_batched = 0;
        int max_priority = -1;
        int batch_start = -1;

     for (int i = 0; i < sched->task_count; i++) {
            Task *t = &sched->tasks[i];
            if (!t->completed && t->arrival_time <= sched->current_time) {
                if (t->priority > max_priority) {
                    max_priority = t->priority;
                    batch_energy = batch_time = tasks_batched = 0;
                    batch_start = i;
                    for (int j = 0; j <= i; j++) {
                        Task *tj = &sched->tasks[j];
                        if (!tj->completed && tj->arrival_time <= sched->current_time && tj->priority == max_priority) {
                            batch_time += tj->exec_time;
                            tj->completed = 1;
                            tasks_batched++;
                        }
                    }
                } else if (t->priority == max_priority) {
                    batch_time += t->exec_time;
                    t->completed = 1;
                    tasks_batched++;
                }
            }
        }

        if (tasks_batched > 0) {
            sort_tasks_by_exec_time(sched, batch_start, batch_start + tasks_batched);
            for (int i = batch_start; i < batch_start + tasks_batched; i++) {
                Task *t = &sched->tasks[i];
                printf("Time %dms: Batched Task %d (Priority %d), exec time %dms\n", 
                       sched->current_time + t->exec_time, t->task_id, t->priority, t->exec_time);
                fprintf(log_file, "Time %dms: Batched Task %d (Priority %d), exec time %dms\n", 
                        sched->current_time + t->exec_time, t->task_id, t->priority, t->exec_time);
                batch_energy += calculate_batch_energy(t->exec_time, active, &active); 
                sched->current_time += t->exec_time;
            }
            sched->total_energy += batch_energy;
            printf("Time %dms: Batch completed, energy +%d\n", sched->current_time, batch_energy);
            fprintf(log_file, "Time %dms: Batch completed, energy +%d\n", sched->current_time, batch_energy);
        } else {
            int next_arrival = -1;
            for (int i = 0; i < sched->task_count; i++) {
                if (!sched->tasks[i].completed && 
                    (next_arrival == -1 || sched->tasks[i].arrival_time < next_arrival)) {
                    next_arrival = sched->tasks[i].arrival_time;
                }
            }
            if (next_arrival > sched->current_time) {
                int idle_duration = next_arrival - sched->current_time;
                if (active) {
                    sched->total_energy += STATE_TRANSITION_PENALTY;
                    active = 0;
                    printf("Time %dms: CPU idle, energy +%d (transition)\n", 
                           sched->current_time, STATE_TRANSITION_PENALTY);
                    fprintf(log_file, "Time %dms: CPU idle, energy +%d (transition)\n", 
                            sched->current_time, STATE_TRANSITION_PENALTY);
                }
                sched->total_energy += idle_duration * ENERGY_IDLE;
                sched->current_time = next_arrival;
                printf("Time %dms: Fast-forwarded %dms, energy +%d (idle)\n", 
                       sched->current_time, idle_duration, idle_duration * ENERGY_IDLE);
                fprintf(log_file, "Time %dms: Fast-forwarded %dms, energy +%d (idle)\n", 
                        sched->current_time, idle_duration, idle_duration * ENERGY_IDLE);
            } else {
                if (active) {
                    sched->total_energy += STATE_TRANSITION_PENALTY;
                    active = 0;
                    printf("Time %dms: CPU idle, energy +%d (transition)\n", 
                           sched->current_time, STATE_TRANSITION_PENALTY);
                    fprintf(log_file, "Time %dms: CPU idle, energy +%d (transition)\n", 
                            sched->current_time, STATE_TRANSITION_PENALTY);
                }
                sched->total_energy += ENERGY_IDLE;
                sched->current_time++;
            }
        }
    }
    printf("Scheduling completed. Total energy: %d mJ\n", sched->total_energy);
    fprintf(log_file, "Scheduling completed. Total energy: %d mJ\n", sched->total_energy);
}
void print_schedule(Scheduler *sched) {
    printf("\nFinal Schedule:\n");
    printf("Task ID | Exec Time | Arrival Time | Priority | Completed\n");
    fprintf(log_file, "\nFinal Schedule:\n");
    fprintf(log_file, "Task ID | Exec Time | Arrival Time | Priority | Completed\n");
    for (int i = 0; i < sched->task_count; i++) {
        printf("%7d | %9d | %12d | %8d | %9d\n", 
               sched->tasks[i].task_id, sched->tasks[i].exec_time, 
               sched->tasks[i].arrival_time, sched->tasks[i].priority, 
               sched->tasks[i].completed);
        fprintf(log_file, "%7d | %9d | %12d | %8d | %9d\n", 
                sched->tasks[i].task_id, sched->tasks[i].exec_time, 
                sched->tasks[i].arrival_time, sched->tasks[i].priority, 
                sched->tasks[i].completed);
    }
    printf("Total Energy Consumed: %d mJ\n", sched->total_energy);
    fprintf(log_file, "Total Energy Consumed: %d mJ\n", sched->total_energy);
}
int main() {
    log_file = fopen(LOG_FILE, "w");
    if (!log_file) {
        fprintf(stderr, "Error: Could not open log file %s\n", LOG_FILE);
        return 1;
    }

    Scheduler sched;
    printf("Test Case 1: Overlapping arrivals\n");
    fprintf(log_file, "Test Case 1: Overlapping arrivals\n");
    init_scheduler(&sched);
    if (add_task(&sched, 1, 2, 0, 5) && 
        add_task(&sched, 2, 3, 1, 10) && 
        add_task(&sched, 3, 1, 2, 3)) {
        schedule_tasks(&sched);
        print_schedule(&sched);
    } else {
        printf("Test Case 1 failed due to invalid inputs\n");
        fprintf(log_file, "Test Case 1 failed due to invalid inputs\n");
    }
    printf("\nTest Case 2: Sparse arrivals\n");
    fprintf(log_file, "\nTest Case 2: Sparse arrivals\n");
    init_scheduler(&sched);
    if (add_task(&sched, 4, 2, 0, 8) && 
        add_task(&sched, 5, 1, 5, 4) && 
        add_task(&sched, 6, 3, 10, 6)) {
        schedule_tasks(&sched);
        print_schedule(&sched);
    } else {
        printf("Test Case 2 failed due to invalid inputs\n");
        fprintf(log_file, "Test Case 2 failed due to invalid inputs\n");
    }

    
    fclose(log_file);
    return 0;
}
