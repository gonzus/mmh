#ifndef MMH_H_
#define MMH_H_

typedef struct MinMaxHeap {
    int cap;
    int pos;
    int* dat;
} MinMaxHeap;

MinMaxHeap* mmh_create(int capacity);
void mmh_destroy(MinMaxHeap* mmh);

void mmh_heapify(MinMaxHeap* mmh);
int mmh_min(MinMaxHeap* mmh);
int mmh_max(MinMaxHeap* mmh);

#endif
