#ifndef SIMULATION_ENGINE_H
#define SIMULATION_ENGINE_H

#define MAX_TASKS 100
#define MAX_TIME 1000 // Simulation time in milliseconds


typedef struct {
    int id;           
    int arrival_time; 
    int burst_time;   
    int deadline;     
    int completed;    
    float energy;     
} Task;


typedef struct {
    Task tasks[MAX_TASKS];
    int task_count;
    float total_energy; 
    int current_time;   
    float cpu_freq;     
} SimState;


void init_simulation(SimState *state);
void generate_tasks(SimState *state, int num_tasks);
void run_simulation(SimState *state);
float calculate_energy(int burst_time, float cpu_freq);
void print_results(SimState *state);

#endif
