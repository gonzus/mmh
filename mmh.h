#ifndef MMH_H_
#define MMH_H_

#include <stdbool.h>

/*
 * A simple Min-Max Heap that stores values of type mmh_t.
 * The heap grows automatically as needed when inserting elements.
 * After heapifying, asking for min() and max() is O(1).
 */

typedef int mmh_t;

typedef struct MinMaxHeap {
    unsigned int cap;   // total heap capacity
    unsigned int pos;   // actual used size in heap
    mmh_t* dat;         // array with heap data
} MinMaxHeap;

// Return a newly created heap with initial capacity 0.
MinMaxHeap* mmh_create(void);

// Return a newly created heap with initial capacity as requested.
MinMaxHeap* mmh_create_capacity(unsigned int capacity);

// Destroy a heap.
void mmh_destroy(MinMaxHeap* mmh);

// Return the size of the heap.
unsigned int mmh_size(MinMaxHeap* mmh);

// Return the capacity of the heap.
unsigned int mmh_capacity(MinMaxHeap* mmh);

// Return true if heap is empty, false otherwise.
bool mmh_is_empty(MinMaxHeap* mmh);

// Add a value to the heap.
// This will BREAK the min-max heap property; you have to make sure you call
// mmh_heapify() after a series of calls to add.
// Return the size of the heap after adding.
unsigned int mmh_add(MinMaxHeap* mmh, mmh_t value);

// Insert a value into the heap.
// This will KEEP the min-max heap property.
// Return the size of the heap after inserting.
unsigned int mmh_insert(MinMaxHeap* mmh, mmh_t value);

// Remove the minimum element.
// This will KEEP the min-max heap property.
// Return the size of the heap after removing.
unsigned int mmh_remove_min(MinMaxHeap* mmh);

// Remove the maximum element.
// This will KEEP the min-max heap property.
// Return the size of the heap after removing.
unsigned int mmh_remove_max(MinMaxHeap* mmh);

// Clear the heap.  Do NOT recover memory.
void mmh_clear(MinMaxHeap* mmh);

// Make sure the heap has the min-max heap property.
void mmh_heapify(MinMaxHeap* mmh);

// Get the minimum value stored in the heap.
mmh_t mmh_min(MinMaxHeap* mmh);

// Get the maximum value stored in the heap.
mmh_t mmh_max(MinMaxHeap* mmh);

#endif
