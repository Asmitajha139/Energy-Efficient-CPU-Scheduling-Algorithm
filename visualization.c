#include <stdio.h>
#include <stdlib.h>
#include "visualization.h"

void load_results(ProcessData processes[], int *num_processes) {
  
    *num_processes = 5;
    processes[0] = (ProcessData){1, 10.5, 5.0, 2.0};
    processes[1] = (ProcessData){2, 15.0, 7.0, 3.5};
    processes[2] = (ProcessData){3, 8.0, 4.0, 1.5};
    processes[3] = (ProcessData){4, 12.0, 6.0, 2.8};
    processes[4] = (ProcessData){5, 9.5, 5.5, 2.2};
}

void display_bar_chart(ProcessData processes[], int num_processes) {
    printf("\n=== Energy Consumption Bar Chart ===\n");
    for (int i = 0; i < num_processes; i++) {
        printf("P%d |", processes[i].pid);
        int bars = (int)(processes[i].energy / 2); // Scale for display
        for (int j = 0; j < bars; j++) printf("█");
        printf(" %.1fJ\n", processes[i].energy);
    }
}

void display_summary(ProcessData processes[], int num_processes) {
    float total_energy = 0, total_exec = 0, total_wait = 0;
    for (int i = 0; i < num_processes; i++) {
        total_energy += processes[i].energy;
        total_exec += processes[i].exec_time;
        total_wait += processes[i].wait_time;
    }
    printf("\n=== Performance Summary ===\n");
    printf("Total Energy: %.1f Joules\n", total_energy);
    printf("Avg Execution Time: %.1f ms\n", total_exec / num_processes);
    printf("Avg Waiting Time: %.1f ms\n", total_wait / num_processes);
}


void save_to_file(ProcessData processes[], int num_processes) {
    FILE *fp = fopen("results.txt", "w");
    if (fp == NULL) {
        printf("Error opening file!\n");
        return;
    }
    fprintf(fp, "PID,Energy(J),ExecTime(ms),WaitTime(ms)\n");
    for (int i = 0; i < num_processes; i++) {
        fprintf(fp, "%d,%.1f,%.1f,%.1f\n", 
                processes[i].pid, processes[i].energy, 
                processes[i].exec_time, processes[i].wait_time);
    }
    fclose(fp);
    printf("Results saved to results.txt\n");
}

int main() {
    ProcessData processes[MAX_PROCESSES];
    int num_processes;

load_results(processes, &num_processes);

    display_bar_chart(processes, num_processes);
    display_summary(processes, num_processes);

    save_to_file(processes, num_processes);

    return 0;
}
