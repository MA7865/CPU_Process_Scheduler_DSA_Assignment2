// Process.h
#ifndef PROCESS_H         // guard to prevent double declarations of struct process
#define PROCESS_H


// shared limits used by the heap and the scheduler
const int MAX_CAPACITY = 100;   //  max number of waiting processes
const int MAX_PID = 9999;   // valid PIDs are 1..9999


struct Process{
    int pid;    // unique process ID
    int priority;   // smaller number = more urgent
    int burstTime;  // CPU time needed
    int sequence;   // insertion order, used to break the tie (FCFS)

};

#endif


