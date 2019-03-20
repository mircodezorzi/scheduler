#ifndef PROCESS_H
#define PROCESS_H

typedef struct Process Process;
struct Process {
    char name[128];
    int pid;

    Process* parent;

    int priority;
    enum { Launched, Acquiring,
           Ready,    Executing,
           Blocked,  Zombie,
           Terminated } status;

    int t_arrival;
    int t_duration;

    int t_ellapsed;
    int t_turnaround;
};

#endif
