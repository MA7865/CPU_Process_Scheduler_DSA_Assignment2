// Scheduler.h
#ifndef SCHEDULER_H
#define SCHEDULER_H
#include "MinHeap.h"

// Possible results when adding a process
enum class AddResult {
    OK,
    INVALID_PID,
    INVALID_PRIORITY,
    INVALID_BURST,
    DUPLICATE_PID,
    QUEUE_FULL
};

class Scheduler {
public:

    // Constructor
    Scheduler();

    // Add a new process after validation
    AddResult addProcess(int pid, int priority, int burstTime);

    // Remove and return the next process
    bool executeNext(Process& out);

    // View the next process without removing it
    bool viewNext(Process& out) const;

    // Simulate all waiting processes
    void runAll();

    // Give read-only access to the ready queue
    const MinHeap& getReadyQueue() const;

    // Scheduler status functions
    int waitingCount() const;
    bool isEmpty() const;
    bool isFull() const;

private:

    MinHeap readyQueue;

    // Tracks which PIDs are currently waiting
    bool active[MAX_PID + 1];

    // Assigns insertion order to processes
    int nextSequence;
};

#endif

