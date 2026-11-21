#ifndef MINHEAP_H
#define MINHEAP_H

#include "Process.h"

class MinHeap{
    public:

        MinHeap();  // constructor: starts empty

        bool insert(const Process& p);  // false if full
        bool peek(Process& out) const;  // false if empty
        bool extractMin(Process& out);  // false if empty

        bool isEmpty() const;   //
        bool isFull() const;
        int size() const;

        bool isValidHeap() const;   // checks heap property. (for testing)

    private:
        Process heap[MAX_CAPACITY]; // the array that stores the tree
        int heapSize;   // how many slots are in use

        bool comesBefore(const Process& a, const Process& b) const; // true if a comes before b. higher priority of a or to break the tie, if a was added first
        void siftUp(int index);
        void siftDown(int index);
        void swapAt(int i, int j);
};


#endif