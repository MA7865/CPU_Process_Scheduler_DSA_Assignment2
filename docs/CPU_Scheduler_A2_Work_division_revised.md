# CPU Process Scheduler Using a Custom Binary Min-Heap — Assignment 2

## 1. Project Overview

| Detail               | Description                                          |
| -------------------- | ---------------------------------------------------- |
| Project Title        | CPU Process Scheduler Using a Custom Binary Min-Heap |
| Course               | Data Structures and Algorithms                       |
| Programming Language | C++                                                  |
| Group Size           | 4 Members *(confirm with instructor — the brief refers to "both members")* |
| Interface            | Console-based                                        |
| Main Data Structure  | Binary Min-Heap (custom, array-based)                |
| Scheduling Algorithm | Non-preemptive Priority Scheduling                   |

---

## 2. Problem Definition

### 2.1 Problem Statement

In an operating system, many processes can be ready to run at the same time, but a single CPU can run only one of them at a time. The **CPU scheduler** decides which ready process runs next. The decision directly affects how long every process waits.

A First-Come, First-Served (FCFS) queue runs processes in arrival order. However, some processes are more urgent than others (for example, system or interactive tasks versus background jobs) and should run first even if they arrived later.

Our project simulates a **priority-based CPU scheduler**. Waiting processes are stored in a custom Binary Min-Heap ordered by priority. Whenever the CPU becomes free, the scheduler selects the highest-priority process. The program also simulates running all waiting processes and reports each process's **waiting time** and **turnaround time**, so the effect of the scheduling decisions is visible.

### 2.2 Who / What Is Affected

- **The operating system's scheduler:** it must repeatedly and efficiently find the most urgent ready process.
- **Users and their programs:** high-priority work (e.g. interactive tasks) should not be delayed behind low-priority background work.

### 2.3 Inputs

| Input       | Description                                    | Constraint                                  |
| ----------- | ---------------------------------------------- | ------------------------------------------- |
| Process ID  | Unique identifier of the process               | Integer, 1 – 9999, unique among waiting processes |
| Priority    | Urgency of the process                         | Positive integer; smaller = more urgent     |
| Burst Time  | CPU time the process needs (in time units)     | Positive integer                            |
| Menu choice | Operation the user wants to perform            | Integer 1 – 7                               |

### 2.4 Outputs

- Confirmation or a clear error message for every operation.
- The process selected for execution (ID, priority, burst time).
- The full ready queue in scheduling order.
- For **Run All**: an execution timeline (start and finish time per process), each process's waiting and turnaround time, and the averages.

### 2.5 Assumptions

- All processes are ready at time 0 (no arrival-time simulation).
- Scheduling is **non-preemptive**: once a process starts, it runs until its burst time is complete.
- A smaller priority number means a higher priority (priority 1 runs before priority 2).
- Equal priorities are resolved by insertion order (earlier insertion runs first).
- Context-switch time is ignored.
- A process ID may be reused once its earlier process has executed.

### 2.6 Boundaries of the Solution

- This is a **simulation**. It does not interact with the real operating system or execute real programs.
- Maximum of **100** waiting processes at a time.
- No preemption, no time slicing, no starvation prevention (see Section 12).

### 2.7 Source of the Problem

The problem is adapted from the CPU scheduling chapter of a standard operating systems textbook:

> Silberschatz, A., Galvin, P. B., & Gagne, G. *Operating System Concepts* (10th ed.), Wiley — Chapter 5, "CPU Scheduling" (Priority Scheduling).

**How we adapted it:** We restricted the problem to non-preemptive priority scheduling with all processes ready at time 0, added FCFS tie-breaking via an insertion sequence number, added interactive operations (add / view / execute / display), and fixed the capacity at 100 so the focus stays on the data structure.

---

## 3. Main Operations

| # | Operation            | Description                                                                                  |
| - | -------------------- | -------------------------------------------------------------------------------------------- |
| 1 | Add Process          | Validate and insert a new process into the ready queue.                                      |
| 2 | Execute Next Process | Remove and "run" the highest-priority process; show its details.                            |
| 3 | View Next Process    | Show which process would run next, without removing it.                                      |
| 4 | Display Ready Queue  | Show all waiting processes in the order they would execute, without changing the queue.     |
| 5 | Run All (Simulate)   | Execute every waiting process in order; print the timeline, waiting/turnaround times and averages. |
| 6 | Scheduler Status     | Show the number of waiting processes and whether the queue is empty or full.                 |
| 7 | Exit                 | End the program.                                                                             |

**Run All** is the operation that ties the data structure to the scheduling outcome: it shows how the heap's ordering determines how long each process waits.

### Input Validation

- Reject process IDs outside 1 – 9999.
- Reject duplicate process IDs among currently waiting processes.
- Reject zero or negative priorities.
- Reject zero or negative burst times.
- Reject insertion when the queue is full (100 processes).
- Handle Execute / View / Run All when the queue is empty.
- Handle invalid menu choices and non-numeric input without crashing.

---

## 4. Data Structure Selection

### 4.1 Selected Data Structure: Binary Min-Heap

A Binary Min-Heap is a complete binary tree in which every node comes before (or equals) its children in scheduling order. The root is therefore always the process that should run next.

The heap is stored in a plain C++ array (no `std::priority_queue`).

**Index calculations (zero-based):** for a node at index `i`:

- Parent: `(i - 1) / 2`
- Left child: `2 * i + 1`
- Right child: `2 * i + 2`

### 4.2 Ordering Rule (`comesBefore`)

Process `a` comes before process `b` if:

1. `a.priority < b.priority`, **or**
2. `a.priority == b.priority` **and** `a.sequence < b.sequence`.

A heap on its own is **not stable**: equal keys can come out in any order. The `sequence` number (a counter incremented on every insertion) makes every key unique and guarantees FCFS order among equal priorities.

### 4.3 Comparison with Alternatives

Let `n` = number of waiting processes.

| Data Structure          | Insert   | Remove next | Benefit                                  | Limitation                                       |
| ----------------------- | -------- | ----------- | ---------------------------------------- | ------------------------------------------------ |
| FIFO Queue              | O(1)     | O(1)        | Very simple                              | Ignores priority entirely — wrong behaviour       |
| Unsorted Array / List   | O(1)     | O(n)        | Fast insertion                           | Must scan everything to find the next process     |
| Sorted Array / List     | O(n)     | O(1)        | Next process always at the front         | Every insertion must find its position and shift / walk |
| **Binary Min-Heap**     | **O(log n)** | **O(log n)** | **Both operations fast; simple array storage** | **Not fully sorted — displaying in order needs extra work** |

**Justification:** A scheduler both adds and removes processes constantly. The unsorted and sorted alternatives each make one of those operations O(n). The heap is the only option that keeps **both** at O(log n).

**Limitation we accept:** Because the heap is only partially ordered, displaying the full queue in order costs O(n log n) (Section 10).

### 4.4 Duplicate-ID Check

To keep Add Process at O(log n), the scheduler keeps a lookup array `bool active[MAX_PID + 1]` (`MAX_PID = 9999`):

- On insert: reject if `active[pid]` is true; otherwise set it to true.
- On execute: set `active[pid]` to false.

This makes the duplicate check O(1) instead of scanning the heap in O(n). It costs a fixed ~10 KB of memory, which is why PIDs are limited to 1 – 9999.

---

## 5. Algorithms (Pseudocode)

### 5.1 comesBefore(a, b)

```
if a.priority != b.priority:
    return a.priority < b.priority
return a.sequence < b.sequence
```

### 5.2 insert(p) — Sift Up

```
if heapSize == CAPACITY: return false
heap[heapSize] = p
i = heapSize
heapSize = heapSize + 1
while i > 0:
    parent = (i - 1) / 2
    if comesBefore(heap[i], heap[parent]):
        swap(heap[i], heap[parent])
        i = parent
    else:
        break
return true
```

### 5.3 extractMin(out) — Sift Down

```
if heapSize == 0: return false
out = heap[0]
heapSize = heapSize - 1
heap[0] = heap[heapSize]
i = 0
loop:
    left = 2*i + 1;  right = 2*i + 2;  best = i
    if left < heapSize and comesBefore(heap[left], heap[best]):  best = left
    if right < heapSize and comesBefore(heap[right], heap[best]): best = right
    if best == i: break
    swap(heap[i], heap[best])
    i = best
return true
```

### 5.4 addProcess(pid, priority, burst)

```
if pid < 1 or pid > MAX_PID:   return INVALID_PID
if priority <= 0:              return INVALID_PRIORITY
if burst <= 0:                 return INVALID_BURST
if active[pid]:                return DUPLICATE_PID
if readyQueue.isFull():        return QUEUE_FULL
p = Process(pid, priority, burst, nextSequence)
nextSequence = nextSequence + 1
readyQueue.insert(p)
active[pid] = true
return OK
```

### 5.5 runAll()

```
if readyQueue is empty: print "No processes waiting"; return
clock = 0; totalWait = 0; totalTurnaround = 0; count = 0
while readyQueue.extractMin(p):
    active[p.pid] = false
    start  = clock
    finish = clock + p.burstTime
    print p.pid, p.priority, p.burstTime, start, finish, waiting = start, turnaround = finish
    totalWait += start; totalTurnaround += finish; count += 1
    clock = finish
print averages: totalWait / count, totalTurnaround / count
```

*(Waiting time = start time and turnaround time = finish time because all processes arrive at time 0.)*

### 5.6 displayReadyQueue()

```
temp = copy of readyQueue
while temp.extractMin(p):
    print p
```

The original heap is never modified.

---

## 6. Division of Work — Four Members

Each member owns one component, **and every member must be able to explain and trace the complete program** (the viva is individual and can cover any part).

### Member 1 — Process Structure and Binary Min-Heap

**Primary responsibility:** The central data structure.

**Tasks**

1. Define `Process` (`pid`, `priority`, `burstTime`, `sequence`) in `Process.h`.
2. Implement the `MinHeap` class (fixed array of capacity 100, `heapSize`):
   - `comesBefore`, `siftUp`, `siftDown`, `swapAt`
   - `insert`, `peek`, `extractMin`
   - `isEmpty`, `isFull`, `size`
   - `isValidHeap()` — checks the heap property for every parent/child pair (used by Member 4's tests).
3. Test the heap independently: random insertion orders, equal priorities, insert-until-full, extract-until-empty.
4. Prepare the **dry trace** (Section 9) and the heap part of the complexity analysis.

**Deliverables:** `Process.h`, `MinHeap.h`, `MinHeap.cpp`, dry-trace slides, heap complexity analysis.

### Member 2 — Scheduler Logic and Simulation

**Primary responsibility:** Turning the heap into a scheduler and producing the scheduling results.

**Tasks**

1. Implement the `Scheduler` class using Member 1's `MinHeap`.
2. Implement `addProcess` with validation, automatic sequence numbers and the O(1) `active[]` duplicate check (Section 4.4).
3. Implement `executeNext` and `viewNext` (empty-queue handling included).
4. Implement **`runAll`**: the execution timeline, per-process waiting and turnaround time, and averages (Section 5.5).
5. Provide a read-only accessor for the queue so Member 3 can display it without modifying it.
6. Prepare the scheduling explanation (why priority scheduling, how ties are resolved, how the metrics are calculated).

**Deliverables:** `Scheduler.h`, `Scheduler.cpp`, simulation output for the example in Section 9, scheduling slides.

### Member 3 — Console Interface, Validation and Display

**Primary responsibility:** Everything the user sees and types.

**Tasks**

1. Implement the menu in `main.cpp` (options 1 – 7, Section 3); loop until Exit.
2. Implement safe numeric input: reject non-numeric input, clear `cin` errors, and re-prompt.
3. Translate the scheduler's result codes into clear messages (e.g. "Process ID 5 is already waiting").
4. Implement **Display Ready Queue** (copy the heap and extract from the copy) and **Scheduler Status**.
5. Format the Run All output as a readable table (and optionally a simple text Gantt chart, e.g. `| P2 | P4 | P3 | P1 |` with times underneath).
6. Write the user instructions section of the README.

**Deliverables:** `main.cpp`, input handling, display and status features, formatted output, user instructions.

### Member 4 — Automated Testing, Integration and Evaluation

**Primary responsibility:** Proving the program works, and the technical evaluation.

**Tasks**

1. Lead the interface agreement (Section 7) **before** coding starts.
2. Write **`tests.cpp`**: an automated test driver that runs every test in Section 8, compares expected and actual results, calls `isValidHeap()` after every operation, and prints PASS/FAIL.
3. Integrate all modules; make sure both `main.cpp` and `tests.cpp` compile and run.
4. Fill in the test table with the **actual** program output.
5. Write the overall complexity analysis (Section 10) and the evaluation (Section 12).
6. Write `README.md` (description, file structure, compile and run instructions).
7. Compile the contribution and sources statement, including the AI-use declaration (Section 14).
8. Coordinate the slides and run a mock viva for the whole group.

**Deliverables:** `tests.cpp`, test-results table, `README.md`, complexity and evaluation sections, contribution and sources statement.

---

## 7. Shared Coding Agreement

Agree on these **before** writing code.

| Specification        | Agreed Decision                                   |
| -------------------- | ------------------------------------------------- |
| Scheduling algorithm | Non-preemptive priority scheduling                |
| Main data structure  | Custom array-based Binary Min-Heap                |
| Priority convention  | Smaller number = higher priority                  |
| Equal priority       | Smaller `sequence` (earlier insertion) runs first |
| Process ID           | Integer 1 – 9999, unique among waiting processes  |
| Priority, burst time | Positive integers                                 |
| Capacity             | 100 processes (`MAX_CAPACITY`)                    |
| Empty queue          | Return `false`; UI shows a message                |
| Full queue           | Reject insertion; UI shows a message              |
| STL priority queue   | **Not used** anywhere for the assessed heap       |

### Agreed Signatures

```cpp
// Process.h
const int MAX_CAPACITY = 100;
const int MAX_PID = 9999;

struct Process {
    int pid;
    int priority;
    int burstTime;
    int sequence;
};

// MinHeap.h
class MinHeap {
public:
    MinHeap();
    bool insert(const Process& p);      // false if full
    bool peek(Process& out) const;      // false if empty
    bool extractMin(Process& out);      // false if empty
    bool isEmpty() const;
    bool isFull() const;
    int  size() const;
    bool isValidHeap() const;           // for testing
private:
    Process heap[MAX_CAPACITY];
    int heapSize;
    bool comesBefore(const Process& a, const Process& b) const;
    void siftUp(int index);
    void siftDown(int index);
    void swapAt(int i, int j);
};

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
```

---

## 8. Testing Plan

At least five purposeful tests are required; we use the following. Tests 1–5 run **in sequence** on the same scheduler; the others start from an empty scheduler.

| #  | Category   | Input / Operation                                                   | Expected Result                                                        | Actual Result |
| -- | ---------- | ------------------------------------------------------------------- | ---------------------------------------------------------------------- | ------------- |
| 1  | Normal     | Add P1 (pr 3, bt 5), P2 (pr 1, bt 4), P3 (pr 2, bt 7)               | All accepted; View Next shows P2                                       |               |
| 2  | Normal     | Execute Next                                                        | P2 removed and shown; size becomes 2                                   |               |
| 3  | Normal     | View Next, then Status                                              | P3 shown; size still 2 (peek does not remove)                          |               |
| 4  | Normal     | Display Ready Queue, then Status                                    | Shows P3, P1 in that order; size still 2                               |               |
| 5  | Normal     | Add P2 again (pr 1, bt 3)                                           | Accepted — P2 has already executed, so its ID is free                  |               |
| 6  | Tie-break  | Add P4 (pr 2), then P5 (pr 2); Execute twice                        | P4 executes before P5                                                  |               |
| 7  | Simulation | Add the Section 9 example; Run All                                  | Order P2, P4, P3, P1; avg waiting 5.75; avg turnaround 10.25; queue empty afterwards |               |
| 8  | Boundary   | Add one process; Execute Next                                       | Process returned; queue empty                                           |               |
| 9  | Boundary   | Add 100 processes                                                   | All 100 accepted; Status shows full                                     |               |
| 10 | Boundary   | Add a 101st process                                                 | Rejected: queue full                                                    |               |
| 11 | Invalid    | Execute Next / View Next / Run All on empty queue                   | Clear "queue is empty" message; no crash                                |               |
| 12 | Invalid    | Add a process whose ID is already waiting                           | Rejected: duplicate ID                                                  |               |
| 13 | Invalid    | PID 0, PID 10000, priority 0, burst time -2                         | Each rejected with the matching message                                 |               |
| 14 | Invalid    | Menu input `abc`, then `9`                                          | Error message; menu shown again; program keeps running                  |               |

`tests.cpp` also checks `isValidHeap()` after every insert and extract.

The final submission must contain the **actual** output from running the compiled program.

---

## 9. Dry Trace

Processes inserted in this order:

| Seq | Process | Priority | Burst Time |
| --- | ------- | -------- | ---------- |
| 1   | P1      | 3        | 5          |
| 2   | P2      | 1        | 4          |
| 3   | P3      | 2        | 7          |
| 4   | P4      | 1        | 2          |

### 9.1 Insertions (Sift Up)

| Step      | Heap array (index 0 → n-1) | Decisions                                                                                     |
| --------- | -------------------------- | --------------------------------------------------------------------------------------------- |
| Start     | [ ]                        | Empty heap                                                                                    |
| Insert P1 | [P1]                       | Placed at index 0; no parent                                                                  |
| Insert P2 | [P2, P1]                   | Placed at index 1; parent P1 (pr 3). P2 (pr 1) comes first → swap                            |
| Insert P3 | [P2, P1, P3]               | Placed at index 2; parent P2 (pr 1). P3 (pr 2) does not come first → stop                    |
| Insert P4 | [P2, P4, P3, P1]           | Placed at index 3; parent P1 (pr 3) → swap to index 1. New parent P2: **same priority 1**, P2 has smaller sequence (2 < 4) → stop |

Tree after all insertions:

```
          P2(1)
         /     \
     P4(1)     P3(2)
     /
  P1(3)
```

### 9.2 Extractions (Sift Down)

| Step         | Returned | Heap array    | Decisions                                                                                   |
| ------------ | -------- | ------------- | ------------------------------------------------------------------------------------------- |
| Extract #1   | P2       | [P4, P1, P3]  | Last element P1 moved to root. Children P4 (pr 1) and P3 (pr 2); P4 comes first and before P1 → swap. P1 at index 1 has no children → stop |
| Extract #2   | P4       | [P3, P1]      | P3 moved to root. Only child P1 (pr 3); P3 comes first → stop                               |
| Extract #3   | P3       | [P1]          | P1 moved to root; no children                                                               |
| Extract #4   | P1       | [ ]           | Heap empty                                                                                  |

### 9.3 Run All Result

| Order | Process | Burst | Start | Finish | Waiting | Turnaround |
| ----- | ------- | ----- | ----- | ------ | ------- | ---------- |
| 1     | P2      | 4     | 0     | 4      | 0       | 4          |
| 2     | P4      | 2     | 4     | 6      | 4       | 6          |
| 3     | P3      | 7     | 6     | 13     | 6       | 13         |
| 4     | P1      | 5     | 13    | 18     | 13      | 18         |
|       |         |       |       |        | **Avg 5.75** | **Avg 10.25** |

```
| P2 | P4 |    P3    |   P1   |
0    4    6          13       18
```

**For comparison:** under plain FCFS (P1, P2, P3, P4) the waiting times are 0, 5, 9, 16 (average 7.5). The two priority-1 processes wait 5 and 16 units under FCFS but only 0 and 4 under our scheduler. This is the behaviour the project is designed to produce.

---

## 10. Time and Space Complexity

Let **n** = the number of processes currently waiting (n ≤ 100).

| Operation            | Best Case | Worst Case  | Explanation                                                                        |
| -------------------- | --------- | ----------- | ---------------------------------------------------------------------------------- |
| Add Process          | O(1)      | O(log n)    | Validation and the `active[]` lookup are O(1); sift-up may stop immediately (best) or travel to the root (height ⌊log₂ n⌋) |
| Execute Next         | O(1)      | O(log n)    | Sift-down may stop at once or travel to a leaf                                     |
| View Next            | O(1)      | O(1)        | Reads the root                                                                     |
| Display Ready Queue  | O(n log n) | O(n log n) | O(n) to copy, then n extractions of O(log n) each                                  |
| Run All              | O(n log n) | O(n log n) | n extractions                                                                      |
| Scheduler Status     | O(1)      | O(1)        | Reads `heapSize` and capacity                                                      |
| `isValidHeap()` (tests) | O(n)   | O(n)        | Checks each parent/child pair once                                                 |

**Why O(log n)?** A heap is a complete binary tree, so with n nodes its height is ⌊log₂ n⌋. Sift-up and sift-down each do at most one comparison-and-swap per level.

### Space Complexity

- Heap array: O(n) used (fixed allocation of `MAX_CAPACITY`).
- `active[]` lookup: O(MAX_PID) — a constant 10,000 booleans, independent of n.
- Temporary copy for Display Ready Queue: O(n) extra.
- Run All: O(1) extra.

---

## 11. Suggested File Structure

```
CPU_Process_Scheduler/
├── Process.h        (Member 1)
├── MinHeap.h        (Member 1)
├── MinHeap.cpp      (Member 1)
├── Scheduler.h      (Member 2)
├── Scheduler.cpp    (Member 2)
├── main.cpp         (Member 3)
├── tests.cpp        (Member 4)
└── README.md        (Member 4, user instructions by Member 3)
```

**Compile and run:**

```
g++ -std=c++17 -Wall main.cpp Scheduler.cpp MinHeap.cpp -o scheduler
./scheduler

g++ -std=c++17 -Wall tests.cpp Scheduler.cpp MinHeap.cpp -o tests
./tests
```

---

## 12. Evaluation, Limitations and Improvement

### What the Solution Achieves

- Always selects the correct next process (highest priority, FCFS among equals) in O(log n).
- Keeps both insertion and removal efficient, unlike sorted or unsorted lists.
- Shows the effect of scheduling through waiting and turnaround times.
- Handles invalid, empty, full and duplicate cases safely.

### Limitations

- All processes arrive at time 0; no arrival-time simulation.
- Non-preemptive: a newly added urgent process cannot interrupt a running one.
- **Starvation:** if high-priority processes keep arriving, a low-priority process may never run.
- Fixed capacity of 100 and PIDs limited to 1 – 9999.
- Displaying the queue in order costs O(n log n) because the heap is not fully sorted.

### Proposed Improvement: Aging (Starvation Prevention)

After every execution, decrease the priority number of each waiting process by 1 (minimum 1) for every k executions it has waited. Changing many keys breaks the heap property, so rebuild the heap afterwards with bottom-up heapify (O(n)). Low-priority processes eventually reach the front, so none waits forever.

---

## 13. Viva Preparation

The viva is **individual**. Every member may be asked to explain any code, **trace an unfamiliar input**, justify the design and complexity, find a fault, or adapt the program. **All four members must practise all of the following** in addition to their own component.

### 13.1 Everyone: Tracing Practice

Each member should trace these on paper, writing the array after every step:

1. Insert (pr): 5, 3, 8, 1, 3, 2 → then extract three times.
2. Insert five processes that all have priority 2 → confirm they come out in insertion order.
3. Given the array [P_a(1), P_b(4), P_c(2), P_d(6), P_e(5)], extract once and show the sift-down.

### 13.2 Everyone: Model Answers for Likely Questions

| Question | Model Answer |
| --- | --- |
| Why a heap and not a sorted list? | A sorted list makes removal O(1) but insertion O(n). The heap makes both O(log n), and a scheduler does both constantly. |
| Why a min-heap? | Smaller number = higher priority, so the most urgent process must be the minimum, which a min-heap keeps at the root. |
| What if two processes have the same priority? | `comesBefore` compares `sequence`; the earlier insertion has the smaller sequence and comes first. |
| Why is insertion O(log n)? | The new element moves up at most one level per swap; a complete tree with n nodes has height ⌊log₂ n⌋. |
| Why is the duplicate check O(1)? | `active[pid]` is a direct array lookup; it is set on insert and cleared on execute. |
| **Adapt:** larger number = higher priority? | Change only `comesBefore`: use `a.priority > b.priority`. The heap logic is unchanged (it behaves as a max-heap on priority). |
| **Adapt:** change a waiting process's priority? | Find its index (scan O(n), or maintain a pid→index array updated on every swap for O(1)), update the priority, then sift up if it became more urgent or sift down if less urgent. |
| **Adapt:** add arrival times? | Sort processes by arrival time; advance a clock; insert every process whose arrival time ≤ clock into the heap; when the CPU is free, extract the min. If the heap is empty, jump the clock to the next arrival. |
| **Adapt:** Round Robin? | Replace the heap with a circular queue: dequeue, run for a time quantum, and enqueue again if burst time remains. |
| **Fault:** what if sift-down only compared with the left child? | The right child could come before the new parent, breaking the heap property; the wrong process could be executed next. |
| What happens on an empty or full heap? | `extractMin` / `peek` return false on empty; `insert` returns false on full; the UI prints a message and nothing changes. |

### 13.3 Component-Specific Focus

- **Member 1:** index formulas, sift-up / sift-down line by line, `isValidHeap()`.
- **Member 2:** `addProcess` validation order, the `active[]` array, `runAll` metric calculations.
- **Member 3:** `cin` error recovery, why Display copies the heap, how result codes become messages.
- **Member 4:** how each test was chosen, expected vs actual results, the overall complexity and evaluation.

---

## 14. Proposal and Contribution / Sources Statement

### 14.1 Proposal (submit for approval first)

Must contain:

- The problem and the process affected (Sections 2.1 – 2.2).
- Expected inputs, outputs and at least three meaningful operations (Sections 2.3, 2.4, 3).
- The proposed data structure and why it is suitable (Section 4).
- The source of the problem (Section 2.7).

### 14.2 Contribution and Sources Statement (template)

| Member   | Contribution |
| -------- | ------------ |
| Member 1 | `Process.h`, `MinHeap.h/.cpp`, dry trace |
| Member 2 | `Scheduler.h/.cpp`, Run All simulation, scheduling explanation |
| Member 3 | `main.cpp`, input handling, display/status, user instructions |
| Member 4 | `tests.cpp`, integration, test table, complexity, evaluation, README |

**References:** Silberschatz, Galvin & Gagne, *Operating System Concepts*, Ch. 5; course lecture notes on heaps and priority queues.

**AI assistance:** Declare which AI tools were used, which parts they affected (e.g. planning document structure, explanations, review of code), and how the group checked the results (e.g. hand-traced the heap operations, ran the automated tests, compared the output with the dry trace).

---

## 15. Implementation Order

| Stage | Activity                                                        | Members              |
| ----- | --------------------------------------------------------------- | -------------------- |
| 1     | Finalise problem definition and submit the proposal             | All                  |
| 2     | Agree on the structures and signatures (Section 7)              | All (led by Member 4) |
| 3     | Implement and unit-test the heap                                | Member 1             |
| 4     | Implement the scheduler and Run All                             | Member 2             |
| 5     | Implement the menu, input handling and display                  | Member 3             |
| 6     | Write `tests.cpp`, integrate and run all tests                  | Member 4 + owners    |
| 7     | Fix bugs; fill the test table with actual results               | All                  |
| 8     | Dry trace, complexity, evaluation, slides                       | All                  |
| 9     | Mock viva: each member traces a new input and answers Section 13 | All                 |

**Final objective:** A small, working C++ scheduler whose custom Binary Min-Heap visibly determines which process runs next and how long each process waits, and which every group member can explain, trace and adapt independently.
