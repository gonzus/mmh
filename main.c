#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/time.h>
#include "mmh.h"

static void test_min_max(int top) {
    MinMaxHeap* mmh = mmh_create(top);
    int i = INT32_MAX;
    int a = INT32_MIN;
    for (int j = 0; j < top; ++j) {
        int r = rand();
        if (i > r) {
            i = r;
        }
        if (a < r) {
            a = r;
        }
        mmh->dat[j] = r;
    }
    mmh->pos = top;

    // show("BEFORE heapify", h, top);
    mmh_heapify(mmh);
    // show("AFTER heapify", h, top);

    int hmin = mmh_min(mmh);
    int hmax = mmh_max(mmh);
    printf("min %2s %d %d\n", hmin == i ? "OK" : "XX", hmin, i);
    printf("max %2s %d %d\n", hmax == a ? "OK" : "XX", hmax, a);
    mmh_destroy(mmh);
}


int main(int argc, char* argv[]) {
    (void) argc;
    (void) argv;
    struct timeval tv;
    gettimeofday(&tv, NULL);
    unsigned long us = tv.tv_sec * 1000000 + tv.tv_usec;
    srand (us);
    for (int j = 1; j <= 10; ++j) {
        test_min_max(1000);
    }
    return 0;
}
