#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/time.h>
#include "mmh.h"

static void test_min_max(int top) {
    // MinMaxHeap* mmh = mmh_create_capacity(top);
    MinMaxHeap* mmh = mmh_create();
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
        mmh_add(mmh, r);
    }

    // show("BEFORE heapify", h, top);
    mmh_heapify(mmh);
    // show("AFTER heapify", h, top);

    long long hmin = mmh_min(mmh);
    long long hmax = mmh_max(mmh);
    printf("min %2s %lld %lld\n", hmin == i ? "OK" : "XX", hmin, i);
    printf("max %2s %lld %lld\n", hmax == a ? "OK" : "XX", hmax, a);
    mmh_destroy(mmh);
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
    for (int j = 1; j <= 10; ++j) {
        test_min_max(10000);
    }

    return 0;
}
