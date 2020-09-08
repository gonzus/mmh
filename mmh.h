#ifndef MMH_H_
#define MMH_H_

/*
 * A simple Min-Max Heap that stores values of type mmh_t.
 * The heap grows automatically as needed when inserting elements.
 * After heapifying, it is asking for min() and max() is O(1).
 */

typedef int mmh_t;

typedef struct MinMaxHeap {
    unsigned int cap;   // total heap capacity
    unsigned int pos;   // actual used size in heap
    mmh_t* dat;         // array with heap data
} MinMaxHeap;

MinMaxHeap* mmh_create(void);
MinMaxHeap* mmh_create_capacity(unsigned int capacity);
void mmh_destroy(MinMaxHeap* mmh);

// Add a value to the heap.
unsigned int mmh_add(MinMaxHeap* mmh, mmh_t value);

// Clear the heap.  Do NOT recover memory.
void mmh_clear(MinMaxHeap* mmh);

// Make sure the heap has the min-max heap property.
void mmh_heapify(MinMaxHeap* mmh);

// Get the minimum value stored in the heap.
mmh_t mmh_min(MinMaxHeap* mmh);

// Get the maximum value stored in the heap.
mmh_t mmh_max(MinMaxHeap* mmh);

#endif
