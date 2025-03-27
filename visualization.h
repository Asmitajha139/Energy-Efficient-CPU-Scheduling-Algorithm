#ifndef VISUALIZATION_H
#define VISUALIZATION_H

#define MAX_PROCESSES 100

typedef struct {
    int pid;              
    float energy;        
    float exec_time;      
    float wait_time;      
} ProcessData;

void load_results(ProcessData processes[], int *num_processes);
void display_bar_chart(ProcessData processes[], int num_processes);
void display_summary(ProcessData processes[], int num_processes);
void save_to_file(ProcessData processes[], int num_processes);

#endif
