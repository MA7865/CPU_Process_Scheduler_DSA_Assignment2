// Scheduler.h
enum class AddResult { OK, INVALID_PID, INVALID_PRIORITY, INVALID_BURST,
                       DUPLICATE_PID, QUEUE_FULL };

class Scheduler {
public:
    Scheduler();
    AddResult addProcess(int pid, int priority, int burstTime);
    bool executeNext(Process& out);     // false if empty
    bool viewNext(Process& out) const;  // false if empty
    void runAll();                      // prints timeline + metrics
    const MinHeap& getReadyQueue() const;
    int  waitingCount() const;
    bool isEmpty() const;
    bool isFull() const;
private:
    MinHeap readyQueue;
    bool active[MAX_PID + 1];
    int nextSequence;
};
