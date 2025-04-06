#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <stdbool.h>

#define MAX_PROCESSES 20

typedef struct {
    char name[10];
    int arrival_time;
    int burst_time;
    int priority;
    int start_time;
    int completion_time;
    int turnaround_time;
    int waiting_time;
    int remaining_time;
} Process;

typedef struct {
    Process processes[MAX_PROCESSES];
    int count;
} ProcessList;

void load_dataset(ProcessList *pl, const char *filename);
void print_results(ProcessList *pl);
void calculate_metrics(ProcessList *pl);
void fcfs(ProcessList *pl);
void sjf(ProcessList *pl);
void srtf(ProcessList *pl);
void round_robin(ProcessList *pl, int quantum);
void priority_preemptive(ProcessList *pl);
void priority_non_preemptive(ProcessList *pl);
void print_gantt_chart(ProcessList *pl);
void sort_by_arrival(ProcessList *pl);
void sort_by_burst(ProcessList *pl);
void sort_by_priority(ProcessList *pl);

int main() {
    ProcessList pl = {0};
    int choice, quantum;
    char filename[100];
    
    printf("Energy-Sufficient CPU Scheduling Simulator\n");
    printf("=========================================\n\n");
    
    printf("Enter dataset filename: ");
    scanf("%s", filename);
    load_dataset(&pl, filename);
    
    printf("\nScheduling Algorithms:\n");
    printf("1. First Come First Serve (FCFS)\n");
    printf("2. Shortest Job First (SJF)\n");
    printf("3. Shortest Remaining Time First (SRTF)\n");
    printf("4. Round Robin (RR)\n");
    printf("5. Priority with Preemption\n");
    printf("6. Priority without Preemption\n");
    printf("Enter your choice: ");
    scanf("%d", &choice);
    
    switch(choice) {
        case 1:
            fcfs(&pl);
            break;
        case 2:
            sjf(&pl);
            break;
        case 3:
            srtf(&pl);
            break;
        case 4:
            printf("Enter time quantum: ");
            scanf("%d", &quantum);
            round_robin(&pl, quantum);
            break;
        case 5:
            priority_preemptive(&pl);
            break;
        case 6:
            priority_non_preemptive(&pl);
            break;
        default:
            printf("Invalid choice\n");
            return 1;
    }
    
    printf("\nScheduling Results:\n");
    print_results(&pl);
    calculate_metrics(&pl);
    printf("\nGantt Chart:\n");
    print_gantt_chart(&pl);
    
    return 0;
}

void load_dataset(ProcessList *pl, const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        printf("Error opening file\n");
        exit(1);
    }
    
    char line[100];
    fgets(line, sizeof(line), file); 
    
    while (fgets(line, sizeof(line), file) && pl->count < MAX_PROCESSES) {
        Process *p = &pl->processes[pl->count++];
        sscanf(line, "%s %d %d %d", p->name, &p->arrival_time, &p->burst_time, &p->priority);
        p->remaining_time = p->burst_time;
    }
    
    fclose(file);
}

void print_results(ProcessList *pl) {
    printf("Process | Arrival | Burst | Prio | Start | Compl | TAT | Wait\n");
    printf("--------|---------|-------|------|-------|-------|-----|-----\n");
    
    for (int i = 0; i < pl->count; i++) {
        Process *p = &pl->processes[i];
        printf("%7s | %7d | %5d | %4d | %5d | %5d | %3d | %4d\n",
               p->name, p->arrival_time, p->burst_time, p->priority,
               p->start_time, p->completion_time, p->turnaround_time, p->waiting_time);
    }
}

void calculate_metrics(ProcessList *pl) {
    int total_tat = 0, total_wt = 0;
    
    for (int i = 0; i < pl->count; i++) {
        Process *p = &pl->processes[i];
        total_tat += p->turnaround_time;
        total_wt += p->waiting_time;
    }
    
    printf("\nPerformance Metrics:\n");
    printf("Total Turnaround Time: %d\n", total_tat);
    printf("Average Turnaround Time: %.2f\n", (float)total_tat / pl->count);
    printf("Total Waiting Time: %d\n", total_wt);
    printf("Average Waiting Time: %.2f\n", (float)total_wt / pl->count);
}

void print_gantt_chart(ProcessList *pl) {
  
    int max_time = 0;
    for (int i = 0; i < pl->count; i++) {
        if (pl->processes[i].completion_time > max_time) {
            max_time = pl->processes[i].completion_time;
        }
    }
    
    
    printf("+");
    for (int i = 0; i <= max_time; i++) {
        printf("--");
    }
    printf("+\n");
    

    printf("|");
    for (int i = 0; i < pl->count; i++) {
        Process *p = &pl->processes[i];
        int duration = p->completion_time - p->start_time;
        for (int j = 0; j < duration; j++) {
            printf("%.2s", p->name);
        }
    }
    printf("|\n");
    
   
    printf("+");
    for (int i = 0; i <= max_time; i++) {
        printf("--");
    }
    printf("+\n");
    
    printf("0");
    for (int i = 1; i <= max_time; i++) {
        printf(" %2d", i);
    }
    printf("\n");
}

void sort_by_arrival(ProcessList *pl) {
    for (int i = 0; i < pl->count - 1; i++) {
        for (int j = 0; j < pl->count - i - 1; j++) {
            if (pl->processes[j].arrival_time > pl->processes[j+1].arrival_time) {
                Process temp = pl->processes[j];
                pl->processes[j] = pl->processes[j+1];
                pl->processes[j+1] = temp;
            }
        }
    }
}

void sort_by_burst(ProcessList *pl) {
    for (int i = 0; i < pl->count - 1; i++) {
        for (int j = 0; j < pl->count - i - 1; j++) {
            if (pl->processes[j].burst_time > pl->processes[j+1].burst_time) {
                Process temp = pl->processes[j];
                pl->processes[j] = pl->processes[j+1];
                pl->processes[j+1] = temp;
            }
        }
    }
}

void sort_by_priority(ProcessList *pl) {
    for (int i = 0; i < pl->count - 1; i++) {
        for (int j = 0; j < pl->count - i - 1; j++) {
            if (pl->processes[j].priority > pl->processes[j+1].priority) {
                Process temp = pl->processes[j];
                pl->processes[j] = pl->processes[j+1];
                pl->processes[j+1] = temp;
            }
        }
    }
}

void fcfs(ProcessList *pl) {
    sort_by_arrival(pl);
    
    int current_time = 0;
    for (int i = 0; i < pl->count; i++) {
        Process *p = &pl->processes[i];
        
        if (current_time < p->arrival_time) {
            current_time = p->arrival_time;
        }
        
        p->start_time = current_time;
        p->completion_time = current_time + p->burst_time;
        p->turnaround_time = p->completion_time - p->arrival_time;
        p->waiting_time = p->start_time - p->arrival_time;
        
        current_time = p->completion_time;
    }
}

void sjf(ProcessList *pl) {
    sort_by_arrival(pl);
    
    int current_time = 0;
    int completed = 0;
    bool is_completed[MAX_PROCESSES] = {false};
    
    while (completed != pl->count) {
        int idx = -1;
        int min_burst = INT_MAX;
        
        for (int i = 0; i < pl->count; i++) {
            if (!is_completed[i] && pl->processes[i].arrival_time <= current_time) {
                if (pl->processes[i].burst_time < min_burst) {
                    min_burst = pl->processes[i].burst_time;
                    idx = i;
                }
                if (pl->processes[i].burst_time == min_burst) {
                    if (pl->processes[i].arrival_time < pl->processes[idx].arrival_time) {
                        idx = i;
                    }
                }
            }
        }
        
        if (idx != -1) {
            Process *p = &pl->processes[idx];
            p->start_time = current_time;
            p->completion_time = current_time + p->burst_time;
            p->turnaround_time = p->completion_time - p->arrival_time;
            p->waiting_time = p->start_time - p->arrival_time;
            
            is_completed[idx] = true;
            completed++;
            current_time = p->completion_time;
        } else {
            current_time++;
        }
    }
}

void srtf(ProcessList *pl) {
    sort_by_arrival(pl);
    
    int current_time = 0;
    int completed = 0;
    int prev = -1;

    for (int i = 0; i < pl->count; i++) {
        pl->processes[i].remaining_time = pl->processes[i].burst_time;
    }
    
    while (completed != pl->count) {
        int idx = -1;
        int min_remaining = INT_MAX;
        
        for (int i = 0; i < pl->count; i++) {
            if (pl->processes[i].arrival_time <= current_time && 
                pl->processes[i].remaining_time > 0 && 
                pl->processes[i].remaining_time < min_remaining) {
                min_remaining = pl->processes[i].remaining_time;
                idx = i;
            }
        }
        
        if (idx != -1) {
            if (prev != idx) {
                if (prev != -1 && pl->processes[prev].remaining_time > 0) {
                    pl->processes[prev].completion_time = current_time;
                }
                pl->processes[idx].start_time = current_time;
                prev = idx;
            }
            
            pl->processes[idx].remaining_time--;
            current_time++;
            
            if (pl->processes[idx].remaining_time == 0) {
                completed++;
                pl->processes[idx].completion_time = current_time;
                pl->processes[idx].turnaround_time = 
                    pl->processes[idx].completion_time - pl->processes[idx].arrival_time;
                pl->processes[idx].waiting_time = 
                    pl->processes[idx].turnaround_time - pl->processes[idx].burst_time;
            }
        } else {
            current_time++;
        }
    }
}

void round_robin(ProcessList *pl, int quantum) {
    sort_by_arrival(pl);
    
    int current_time = 0;
    int completed = 0;
    int queue[MAX_PROCESSES];
    int front = 0, rear = -1;
    bool is_queued[MAX_PROCESSES] = {false};
   
    for (int i = 0; i < pl->count; i++) {
        pl->processes[i].remaining_time = pl->processes[i].burst_time;
    }
    
 
    for (int i = 0; i < pl->count; i++) {
        if (pl->processes[i].arrival_time <= current_time) {
            queue[++rear] = i;
            is_queued[i] = true;
        }
    }
    
    while (completed != pl->count) {
        if (front > rear) {
            current_time++;
           
            for (int i = 0; i < pl->count; i++) {
                if (!is_queued[i] && pl->processes[i].arrival_time <= current_time) {
                    queue[++rear] = i;
                    is_queued[i] = true;
                }
            }
            continue;
        }
        
        int idx = queue[front++];
        Process *p = &pl->processes[idx];
        
        if (p->remaining_time == p->burst_time) {
            p->start_time = current_time;
        }
        
        if (p->remaining_time > quantum) {
            current_time += quantum;
            p->remaining_time -= quantum;
            
        
            for (int i = 0; i < pl->count; i++) {
                if (!is_queued[i] && pl->processes[i].arrival_time <= current_time) {
                    queue[++rear] = i;
                    is_queued[i] = true;
                }
            }
            
            queue[++rear] = idx;
        } else {
            current_time += p->remaining_time;
            p->remaining_time = 0;
            p->completion_time = current_time;
            p->turnaround_time = p->completion_time - p->arrival_time;
            p->waiting_time = p->turnaround_time - p->burst_time;
            completed++;
            
         
            for (int i = 0; i < pl->count; i++) {
                if (!is_queued[i] && pl->processes[i].arrival_time <= current_time) {
                    queue[++rear] = i;
                    is_queued[i] = true;
                }
            }
        }
    }
}

void priority_preemptive(ProcessList *pl) {
    sort_by_arrival(pl);
    
    int current_time = 0;
    int completed = 0;
    int prev = -1;
    
 
    for (int i = 0; i < pl->count; i++) {
        pl->processes[i].remaining_time = pl->processes[i].burst_time;
    }
    
    while (completed != pl->count) {
        int idx = -1;
        int highest_priority = INT_MAX;
        
        for (int i = 0; i < pl->count; i++) {
            if (pl->processes[i].arrival_time <= current_time && 
                pl->processes[i].remaining_time > 0 && 
                pl->processes[i].priority < highest_priority) {
                highest_priority = pl->processes[i].priority;
                idx = i;
            }
        }
        
        if (idx != -1) {
            if (prev != idx) {
                if (prev != -1 && pl->processes[prev].remaining_time > 0) {
                    pl->processes[prev].completion_time = current_time;
                }
                pl->processes[idx].start_time = current_time;
                prev = idx;
            }
            
            pl->processes[idx].remaining_time--;
            current_time++;
            
            if (pl->processes[idx].remaining_time == 0) {
                completed++;
                pl->processes[idx].completion_time = current_time;
                pl->processes[idx].turnaround_time = 
                    pl->processes[idx].completion_time - pl->processes[idx].arrival_time;
                pl->processes[idx].waiting_time = 
                    pl->processes[idx].turnaround_time - pl->processes[idx].burst_time;
            }
        } else {
            current_time++;
        }
    }
}

void priority_non_preemptive(ProcessList *pl) {
    sort_by_arrival(pl);
    
    int current_time = 0;
    int completed = 0;
    bool is_completed[MAX_PROCESSES] = {false};
    
    while (completed != pl->count) {
        int idx = -1;
        int highest_priority = INT_MAX;
        
        for (int i = 0; i < pl->count; i++) {
            if (!is_completed[i] && pl->processes[i].arrival_time <= current_time) {
                if (pl->processes[i].priority < highest_priority) {
                    highest_priority = pl->processes[i].priority;
                    idx = i;
                }
                if (pl->processes[i].priority == highest_priority) {
                    if (pl->processes[i].arrival_time < pl->processes[idx].arrival_time) {
                        idx = i;
                    }
                }
            }
        }
        
        if (idx != -1) {
            Process *p = &pl->processes[idx];
            p->start_time = current_time;
            p->completion_time = current_time + p->burst_time;
            p->turnaround_time = p->completion_time - p->arrival_time;
            p->waiting_time = p->start_time - p->arrival_time;
            
            is_completed[idx] = true;
            completed++;
            current_time = p->completion_time;
        } else {
            current_time++;
        }
    }
}
