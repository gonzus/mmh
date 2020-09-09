#include <tap.h>
#include "random.h"
#include "mmh.h"

static int cmp(const void* vl, const void* vr) {
    const mmh_t* l = (const mmh_t*) vl;
    const mmh_t* r = (const mmh_t*) vr;
    if (*l < *r) return -1;
    if (*l > *r) return +1;
    return 0;
}

static void test_min_max(int top, int heapify, int grow, int erase) {
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

    mmh_t* data = (mmh_t*) calloc(top, sizeof(mmh_t));
    int b = 0;
    int t = top - 1;

    for (int j = 0; j < top; ++j) {
        mmh_t r = random_in_range(0, top-1);
        data[j] = r;
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
    cmp_ok(mmh_size(mmh), "==", top, "MinMaxHeap ends up with %d elements", top);

    qsort(data, top, sizeof(mmh_t), cmp);

    for (int j = 0; j < erase; ++j) {
        int from_top = random_in_range(0, 1);
        if (from_top) {
            mmh_remove_max(mmh);
            --t;
        } else  {
            mmh_remove_min(mmh);
            ++b;
        }
        if (b >= t) {
            break;
        }
    }
    cmp_ok(mmh_size(mmh), "==", top - erase, "MinMaxHeap ends up with %d elements after removing %d elements", top - erase, erase);

    mmh_t hmin = mmh_min(mmh);
    cmp_ok(hmin, "==", data[b], "MinMaxHeap min is %d for top %d, heapify %d, grow %d", data[b], top, heapify, grow);

    mmh_t hmax = mmh_max(mmh);
    cmp_ok(hmax, "==", data[t], "MinMaxHeap max is %d for top %d, heapify %d, grow %d", data[t], top, heapify, grow);

    free((void*) data);
    mmh_destroy(mmh);
}

static void test_heap_combinations(int top) {
    for (int t = 1; t <= top; t *= 10) {
        for (int h = 0; h < 2; ++h) {
            for (int g = 0; g < 2; ++g) {
                for (int e = 0; e < t; e = e * 10 + 1) {
                    test_min_max(t, h, g, e);
                }
            }
        }
    }
}

int main(int argc, char* argv[]) {
    (void) argc;
    (void) argv;

    randomize();

    test_heap_combinations(100000);

    done_testing();
}
