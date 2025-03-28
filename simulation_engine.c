#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "simulation_engine.h"


void init_simulation(SimState *state) {
    state->task_count = 0;
    state->total_energy = 0.0;
    state->current_time = 0;
    state->cpu_freq = 1.5;
    srand(time(NULL));    
}


void generate_tasks(SimState *state, int num_tasks) {
    if (num_tasks > MAX_TASKS) {
        printf("Error: Too many tasks requested!\n");
        return;
    }
    for (int i = 0; i < num_tasks; i++) {
        Task *t = &state->tasks[i];
        t->id = i + 1;
        t->arrival_time = rand() % 100;       
        t->burst_time = (rand() % 20) + 1;     
        t->deadline = t->arrival_time + (rand() % 50) + 10; 
        t->completed = 0;
        t->energy = 0.0;
        state->task_count++;
    }
    printf("Generated %d tasks.\n", num_tasks);
}


float calculate_energy(int burst_time, float cpu_freq) {
    float k = 0.1; 
    float power = k * cpu_freq * cpu_freq; 
    return power * (burst_time / 1000.0); 
}


void run_simulation(SimState *state) {
    printf("Starting simulation...\n");
    while (state->current_time < MAX_TIME) {
        for (int i = 0; i < state->task_count; i++) {
            Task *t = &state->tasks[i];
            if (!t->completed && t->arrival_time <= state->current_time) {
                
                t->energy = calculate_energy(t->burst_time, state->cpu_freq);
                state->total_energy += t->energy;
                t->completed = 1;
                printf("Task %d completed at %dms. Energy: %.2fJ\n", 
                       t->id, state->current_time + t->burst_time, t->energy);
                state->current_time += t->burst_time; 
            }
        }
        if (state->current_time < MAX_TIME) state->current_time++; 
    }
}


void print_results(SimState *state) {
    printf("\nSimulation Results:\n");
    printf("Total Energy Consumed: %.2f Joules\n", state->total_energy);
    printf("Total Tasks Completed: %d\n", state->task_count);
    printf("Final Time: %dms\n", state->current_time);
}

int main() {
    SimState state;
    init_simulation(&state);
    generate_tasks(&state, 10);
    run_simulation(&state);
    print_results(&state);
    return 0;
}
