#ifndef MMH_H_
#define MMH_H_

typedef struct MinMaxHeap {
    int cap;
    int pos;
    int* dat;
} MinMaxHeap;

MinMaxHeap* mmh_create(void);
MinMaxHeap* mmh_create_capacity(int capacity);
void mmh_destroy(MinMaxHeap* mmh);

int mmh_add(MinMaxHeap* mmh, int value);
void mmh_heapify(MinMaxHeap* mmh);

int mmh_min(MinMaxHeap* mmh);
int mmh_max(MinMaxHeap* mmh);

#endif
