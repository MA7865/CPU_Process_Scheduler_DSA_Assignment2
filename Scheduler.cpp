
#include "Scheduler.h"

#include <iostream>
#include <iomanip>
#include <string>

using namespace std;


// =====================================================
// 1. CONSTRUCTOR
// =====================================================

Scheduler::Scheduler() {
    // Initially, no process IDs are active
    for (int i = 0; i <= MAX_PID; i++) {
        active[i] = false;
    }
    // First process gets sequence number 1
    nextSequence = 1;
}


// =====================================================s
// 2. ADD PROCESS
// =====================================================

AddResult Scheduler::addProcess(int pid, int priority, int burstTime) {
    // Check PID range
    if (pid < 1 || pid > MAX_PID) {
        return AddResult::INVALID_PID;
    }
    // Priority must be positive
    if (priority <= 0) {
        return AddResult::INVALID_PRIORITY;
    }
    // Burst time must be positive
    if (burstTime <= 0) {
        return AddResult::INVALID_BURST;
    }
    // Check whether this PID is already waiting
    if (active[pid]) {
        return AddResult::DUPLICATE_PID;
    }
    // Check whether the heap has reached capacity
    if (readyQueue.isFull()) {
        return AddResult::QUEUE_FULL;
    }
    // Create a new process
    Process p;

    p.pid = pid;
    p.priority = priority;
    p.burstTime = burstTime;
    p.sequence = nextSequence;

    // Insert the process into the MinHeap
    if (!readyQueue.insert(p)) {
        return AddResult::QUEUE_FULL;
    }
    // Mark this PID as currently waiting
    active[pid] = true;
    // Increment sequence only after successful insertion
    nextSequence++;
    return AddResult::OK;
}


// =====================================================
// 3. EXECUTE NEXT PROCESS
// =====================================================

bool Scheduler::executeNext(Process& out) {

    // Try to remove the highest-priority process
    if (!readyQueue.extractMin(out)) {
        return false;
    }
    // Process is no longer waiting
    active[out.pid] = false;
    return true;
}


// =====================================================
// 4. VIEW NEXT PROCESS
// =====================================================

bool Scheduler::viewNext(Process& out) const {
    // Peek returns the root without removing it
    return readyQueue.peek(out);
}


// =====================================================
// 5. RUN ALL PROCESSES
// =====================================================

void Scheduler::runAll() {
    // Handle empty queue
    if (readyQueue.isEmpty()) {
        cout << "\nNo processes waiting.\n";
        return;
    }
    // Simulation variables
    long long clock = 0;
    long long totalWait = 0;
    long long totalTurnaround = 0;
    int count = 0;
    Process p;
    cout << "\n========== CPU SCHEDULING SIMULATION ==========\n\n";
    cout << left
         << setw(10) << "PID"
         << setw(12) << "Priority"
         << setw(12) << "Burst"
         << setw(12) << "Start"
         << setw(12) << "Finish"
         << setw(12) << "Waiting"
         << setw(12) << "Turnaround"
         << "\n";
    cout << string(82, '-') << "\n";

    // Extract processes in scheduling order
    while (readyQueue.extractMin(p)) {
        // This process has left the ready queue
        active[p.pid] = false;
        // Calculate execution times
        long long start = clock;
        long long finish = start + p.burstTime;
        // All processes arrive at time 0
        long long waitingTime = start;
        long long turnaroundTime = finish;
        // Display current process results
        cout << left
             << setw(10) << p.pid
             << setw(12) << p.priority
             << setw(12) << p.burstTime
             << setw(12) << start
             << setw(12) << finish
             << setw(12) << waitingTime
             << setw(12) << turnaroundTime
             << "\n";
        // Update totals
        totalWait += waitingTime;
        totalTurnaround += turnaroundTime;
        count++;
        // CPU clock moves to the finish time
        clock = finish;
    }
    // Calculate averages
    double averageWaiting =
        static_cast<double>(totalWait) / count;
    double averageTurnaround =
        static_cast<double>(totalTurnaround) / count;

    cout << "\n==============================================\n";
    cout << fixed << setprecision(2);
    cout << "Average Waiting Time: "
         << averageWaiting << "\n";
    cout << "Average Turnaround Time: "
         << averageTurnaround << "\n";
    cout << "\nAll processes completed.\n";
}


// =====================================================
// 6. READ-ONLY ACCESS TO READY QUEUE
// =====================================================

const MinHeap& Scheduler::getReadyQueue() const {
    return readyQueue;
}


// =====================================================
// 7. WAITING COUNT
// =====================================================

int Scheduler::waitingCount() const {
    return readyQueue.size();
}


// =====================================================
// 8. CHECK EMPTY
// =====================================================

bool Scheduler::isEmpty() const {
    return readyQueue.isEmpty();
}


// =====================================================
// 9. CHECK FULL
// =====================================================

bool Scheduler::isFull() const {
    return readyQueue.isFull();
}
