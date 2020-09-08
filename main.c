#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/time.h>
#include "mmh.h"

static void test_min_max(int top) {
    int* h = (int*) malloc(top*sizeof(int));
    int i = INT32_MAX;
    int a = INT32_MIN;
    for (int j = 0; j < top; ++j) {
        h[j] = rand();
        if (i > h[j]) {
            i = h[j];
        }
        if (a < h[j]) {
            a = h[j];
        }
    }
    // show("BEFORE heapify", h, top);
    mmh_heapify(h, top);
    // show("AFTER heapify", h, top);

    int hmin = mmh_min(h, top);
    int hmax = mmh_max(h, top);
    printf("min %2s %d %d\n", hmin == i ? "OK" : "XX", hmin, i);
    printf("max %2s %d %d\n", hmax == a ? "OK" : "XX", hmax, a);
    free((void*) h);
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
