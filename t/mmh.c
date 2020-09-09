#include <stdint.h>
#include <stdlib.h>
#include <sys/time.h>
#include <tap.h>
#include "mmh.h"

static void test_min_max(int top, int heapify, int grow) {
    MinMaxHeap* mmh = 0;
    if (grow) {
        // let heap grow dynamically
        mmh = mmh_create();
        ok(mmh != 0, "MinMaxHeap can be created with default size");
    } else {
        // preallocate heap to hold top elements
        mmh = mmh_create_capacity(top);
        ok(mmh != 0, "MinMaxHeap can be created with size %d", top);
    }

    // keep track of min and max while adding to heap
    long long i = INT32_MAX;
    long long a = INT32_MIN;
    for (int j = 0; j < top; ++j) {
        mmh_t r = rand();
        long long rr = (long long) r;
        if (i > rr) {
            i = rr;
        }
        if (a < rr) {
            a = rr;
        }
        if (heapify) {
            // add and lose the heap property
            mmh_add(mmh, r);
        } else {
            // add and keep the heap property
            mmh_insert(mmh, r);
        }
    }

    if (heapify) {
        // recover the heap property in one go
        mmh_heapify(mmh);
    }

    long long hmin = mmh_min(mmh);
    cmp_ok(hmin, "==", i, "MinMaxHeap min is %d for top %d, heapify %d, grow %d", i, top, heapify, grow);

    long long hmax = mmh_max(mmh);
    cmp_ok(hmax, "==", a, "MinMaxHeap max is %d for top %d, heapify %d, grow %d", a, top, heapify, grow);

    mmh_destroy(mmh);
}

static void test_heap_combinations(int top) {
    for (int t = 1; t <= top; t *= 10) {
        for (int h = 0; h < 2; ++h) {
            for (int g = 0; g < 2; ++g) {
                test_min_max(t, h, g);
            }
        }
    }
}

static void randomize(void) {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    unsigned long us = tv.tv_sec * 1000000 + tv.tv_usec;
    srand(us);
}

int main(int argc, char* argv[]) {
    (void) argc;
    (void) argv;

    randomize();

    test_heap_combinations(1000);

    done_testing();
}
