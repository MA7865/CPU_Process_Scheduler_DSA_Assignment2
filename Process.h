// Process.h
const int MAX_CAPACITY = 100;
const int MAX_PID = 9999;

struct Process {
    int pid;
    int priority;
    int burstTime;
    int sequence;
};
