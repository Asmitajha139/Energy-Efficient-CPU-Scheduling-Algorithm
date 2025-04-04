#include <stdio.h>
#include <stdlib.h>
#include <time.h>
typedef struct {
    int id;             
    float duration;     
    float energy_cost;  
} Task;

typedef struct {
    float frequency;   
    float power;        
} CPU;

Task create_task(int id, float duration) {
    Task t;
    t.id = id;
    t.duration = duration;
    t.energy_cost = 0.0; 
    return t;
}

Task* generate_workload(int num_tasks, int* seed) {
    Task* workload = (Task*)malloc(num_tasks * sizeof(Task));
    srand(*seed); 
    for (int i = 0; i < num_tasks; i++) {
        float duration = (rand() % 45) + 5;
        workload[i] = create_task(i + 1, duration);
    }
    *seed += 1; 
    return workload;
}

int main() {
    return 0; 
}
