#include "MinHeap.h"

MinHeap::MinHeap(){
    heapSize = 0;
}

bool MinHeap::isEmpty() const{
    
    return heapSize == 0;
}

bool MinHeap::isFull() const{
    
    return heapSize >= MAX_CAPACITY;
}

int MinHeap::size() const{
    return heapSize;
}

void MinHeap::swapAt(int i, int j){

    Process temp = heap[i];
    heap[i] = heap[j];
    heap[j] = temp;
    return;
}

bool MinHeap::comesBefore(const Process& a, const Process& b) const{
    
    if (a.priority < b.priority){
        return true;
    }
    else if (a.priority > b.priority){
        return false;
    }
    else if(a.sequence < b.sequence){
        return true;
    }
    else{
        return false;
    }
}

bool MinHeap::insert(const Process& p){

    if (isFull()){
        return false;
    }

    heap[heapSize] = p;
    heapSize+=1;
    siftUp(heapSize-1);
    return true;

}


void MinHeap::siftUp(int index){

    int parent;

    while (index > 0){
        parent = (index - 1) / 2;

        // checking if swapping needs to be done
        if (comesBefore(heap[index], heap[parent])){
            swapAt(index, parent);
            index = parent;
        }
        else{
            break;
        }
    }
}

bool MinHeap::peek(Process& out) const{
    
    if (isEmpty()){
        return false;
    }
    out = heap[0];
    return true;
}


bool MinHeap::extractMin(Process& out){

    if (isEmpty()){
        return false;
    }

    out = heap[0];
    heapSize-=1;
    heap[0] = heap[heapSize];
    siftDown(0);
    return true;

}

void MinHeap::siftDown(int index){
    int left, right, best;

    while (true){
        best = index;
        left = (2 * index) + 1;
        right = (2 * index) + 2;

        if (left < heapSize && comesBefore(heap[left], heap[best])){
            best = left;
        }
        if (right < heapSize && comesBefore(heap[right], heap[best])){
            best = right;
        }
        if ( best == index){
            return;
        }
        swapAt(index, best);
        index = best;
    }
}


bool MinHeap::isValidHeap() const{

    int parent;

    for (int i = 1; i < heapSize; i++){
        parent = (i - 1)/2;
        if (comesBefore(heap[i], heap[parent])){
            return false;
        }
    }
    return true;
}
