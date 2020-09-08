#ifndef MMH_H_
#define MMH_H_

typedef int mmh_t;

typedef struct MinMaxHeap {
    unsigned int cap;
    unsigned int pos;
    mmh_t* dat;
} MinMaxHeap;

MinMaxHeap* mmh_create(void);
MinMaxHeap* mmh_create_capacity(unsigned int capacity);
void mmh_destroy(MinMaxHeap* mmh);

unsigned int mmh_add(MinMaxHeap* mmh, mmh_t value);
void mmh_heapify(MinMaxHeap* mmh);

mmh_t mmh_min(MinMaxHeap* mmh);
mmh_t mmh_max(MinMaxHeap* mmh);

#endif
