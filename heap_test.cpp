// heap_test.cpp — standalone tests for the MinHeap (Member 1)
//
// Compile and run:
//   g++ -std=c++17 -Wall heap_test.cpp MinHeap.cpp -o heap_test
//   ./heap_test

#include <iostream>
#include <cstdlib>
#include "MinHeap.h"

using namespace std;

int passed = 0;
int failed = 0;

// Records one check and prints PASS / FAIL with a description
void check(bool condition, const string& description) {
    if (condition) {
        cout << "  [PASS] " << description << "\n";
        passed++;
    } else {
        cout << "  [FAIL] " << description << "\n";
        failed++;
    }
}

// Quick way to build a Process
Process makeProcess(int pid, int priority, int burst, int sequence) {
    Process p = {pid, priority, burst, sequence};
    return p;
}

void printProcess(const Process& p) {
    cout << "    P" << p.pid
         << " (priority " << p.priority
         << ", burst " << p.burstTime
         << ", seq " << p.sequence << ")\n";
}

// ---------------------------------------------------------------
// Test 1: the practice example — priorities 5, 3, 8, 1, 3, 2
// ---------------------------------------------------------------
void testPracticeExample() {
    cout << "\nTest 1: Insert priorities 5, 3, 8, 1, 3, 2 then extract all\n";
    MinHeap h;
    int priorities[] = {5, 3, 8, 1, 3, 2};

    bool allValid = true;
    for (int i = 0; i < 6; i++) {
        h.insert(makeProcess(i + 1, priorities[i], 10, i + 1));
        if (!h.isValidHeap()) allValid = false;
    }
    check(allValid, "heap property holds after every insert");
    check(h.size() == 6, "size is 6 after 6 inserts");

    // Expected: priority 1(P4), 2(P6), 3(P2), 3(P5), 5(P1), 8(P3)
    int expectedPids[] = {4, 6, 2, 5, 1, 3};
    bool orderCorrect = true;
    bool validAfterExtract = true;
    Process p;

    cout << "  Extraction order:\n";
    for (int i = 0; i < 6; i++) {
        h.extractMin(p);
        printProcess(p);
        if (p.pid != expectedPids[i]) orderCorrect = false;
        if (!h.isValidHeap()) validAfterExtract = false;
    }
    check(orderCorrect, "extract order is P4, P6, P2, P5, P1, P3");
    check(validAfterExtract, "heap property holds after every extract");
    check(h.isEmpty(), "heap is empty after extracting everything");
}

// ---------------------------------------------------------------
// Test 2: the project's dry-trace example (Section 9 of the .md)
// ---------------------------------------------------------------
void testDryTraceExample() {
    cout << "\nTest 2: Dry-trace example P1(3), P2(1), P3(2), P4(1)\n";
    MinHeap h;
    h.insert(makeProcess(1, 3, 5, 1));
    h.insert(makeProcess(2, 1, 4, 2));
    h.insert(makeProcess(3, 2, 7, 3));
    h.insert(makeProcess(4, 1, 2, 4));

    int expectedPids[] = {2, 4, 3, 1};
    bool orderCorrect = true;
    Process p;
    for (int i = 0; i < 4; i++) {
        h.extractMin(p);
        if (p.pid != expectedPids[i]) orderCorrect = false;
    }
    check(orderCorrect, "execution order is P2, P4, P3, P1");
}

// ---------------------------------------------------------------
// Test 3: equal priorities must come out in insertion order (FCFS)
// ---------------------------------------------------------------
void testEqualPriorities() {
    cout << "\nTest 3: Five processes, all priority 2\n";
    MinHeap h;
    for (int i = 1; i <= 5; i++) {
        h.insert(makeProcess(i, 2, 10, i));
    }

    bool fcfs = true;
    Process p;
    for (int i = 1; i <= 5; i++) {
        h.extractMin(p);
        if (p.pid != i) fcfs = false;
    }
    check(fcfs, "equal priorities come out in insertion order P1..P5");
}

// ---------------------------------------------------------------
// Test 4: empty heap — peek and extract must fail safely
// ---------------------------------------------------------------
void testEmptyHeap() {
    cout << "\nTest 4: Empty heap\n";
    MinHeap h;
    Process p;
    check(h.isEmpty(), "new heap is empty");
    check(h.size() == 0, "new heap has size 0");
    check(!h.peek(p), "peek on empty heap returns false");
    check(!h.extractMin(p), "extractMin on empty heap returns false");
    check(h.isValidHeap(), "empty heap counts as valid");
}

// ---------------------------------------------------------------
// Test 5: single element
// ---------------------------------------------------------------
void testSingleElement() {
    cout << "\nTest 5: Single element\n";
    MinHeap h;
    Process p;
    h.insert(makeProcess(7, 4, 3, 1));
    check(h.extractMin(p) && p.pid == 7, "the only process (P7) is extracted");
    check(h.isEmpty(), "heap is empty afterwards");
}

// ---------------------------------------------------------------
// Test 6: peek returns the root without removing it
// ---------------------------------------------------------------
void testPeek() {
    cout << "\nTest 6: Peek does not remove\n";
    MinHeap h;
    h.insert(makeProcess(1, 5, 10, 1));
    h.insert(makeProcess(2, 2, 10, 2));
    h.insert(makeProcess(3, 9, 10, 3));

    Process p;
    check(h.peek(p) && p.pid == 2, "peek returns P2 (priority 2)");
    check(h.size() == 3, "size is still 3 after peek");
    check(h.peek(p) && p.pid == 2, "peeking again gives the same process");
}

// ---------------------------------------------------------------
// Test 7: full heap — 100 inserts succeed, the 101st fails
// ---------------------------------------------------------------
void testFullHeap() {
    cout << "\nTest 7: Capacity limit (" << MAX_CAPACITY << ")\n";
    MinHeap h;
    bool allInserted = true;
    for (int i = 1; i <= MAX_CAPACITY; i++) {
        if (!h.insert(makeProcess(i, (i * 7) % 13 + 1, 5, i))) allInserted = false;
    }
    check(allInserted, "all 100 inserts succeed");
    check(h.isFull(), "heap reports full");
    check(h.isValidHeap(), "full heap is valid");
    check(!h.insert(makeProcess(101, 1, 5, 101)), "101st insert is rejected");
    check(h.size() == MAX_CAPACITY, "size stays 100 after the rejected insert");
}

// ---------------------------------------------------------------
// Test 8: random stress test with mixed inserts and extracts
// Checks validity after every operation, and that every extracted
// process is never "better" than the one extracted before it
// (unless new processes were inserted in between).
// ---------------------------------------------------------------
void testRandomStress() {
    cout << "\nTest 8: 5000 random inserts/extracts\n";
    srand(42);  // fixed seed so the test is repeatable
    MinHeap h;
    int seq = 1;
    bool alwaysValid = true;

    for (int step = 0; step < 5000; step++) {
        bool doInsert = (rand() % 3 != 0);   // ~2/3 inserts, ~1/3 extracts
        if (doInsert && !h.isFull()) {
            h.insert(makeProcess(seq, rand() % 10 + 1, 5, seq));
            seq++;
        } else {
            Process p;
            h.extractMin(p);
        }
        if (!h.isValidHeap()) alwaysValid = false;
    }
    check(alwaysValid, "heap property holds after all 5000 operations");

    // Drain what is left: priorities must never decrease,
    // and equal priorities must come out in sequence order
    bool sortedDrain = true;
    Process prev, cur;
    bool first = true;
    while (h.extractMin(cur)) {
        if (!first) {
            if (cur.priority < prev.priority) sortedDrain = false;
            if (cur.priority == prev.priority && cur.sequence < prev.sequence) sortedDrain = false;
        }
        prev = cur;
        first = false;
    }
    check(sortedDrain, "draining the heap gives correct (priority, sequence) order");
}

int main() {
    cout << "===== MinHeap Tests =====\n";

    testPracticeExample();
    testDryTraceExample();
    testEqualPriorities();
    testEmptyHeap();
    testSingleElement();
    testPeek();
    testFullHeap();
    testRandomStress();

    cout << "\n===== " << passed << " passed, " << failed << " failed =====\n";
    return failed == 0 ? 0 : 1;
}
