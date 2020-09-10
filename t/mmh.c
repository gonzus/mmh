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

    // We will fill the heap with data, and also save it in an array.
    // We remember Bottom (0) and Top (top-1) because later we will remove min
    // and max elements from the heap (and later add them back in a different
    // sequence), and we can easily keep track of that with the sorted data in
    // the array.
    mmh_t* data = (mmh_t*) calloc(top, sizeof(mmh_t));

    for (int j = 0; j < top; ++j) {
        mmh_t r = random_in_range(0, top-1);
        data[j] = r;
        if (heapify) {
            mmh_add(mmh, r);
        } else {
            mmh_insert(mmh, r);
        }
    }

    if (heapify) {
        mmh_heapify(mmh);
    }
    cmp_ok(mmh_size(mmh), "==", top, "MinMaxHeap ends up with %d elements", top);

    qsort(data, top, sizeof(mmh_t), cmp);
    int b = 0;
    int t = top - 1;
    int count = 0;
    mmh_t hmin = 0;
    mmh_t hmax = 0;

    // Now erase a number of elements (which could be zero).
    // Choose randomly whether to erase the min or max element.
    count = 0;
    for (int j = 0; j < erase; ++j) {
        int from_top = random_in_range(0, 1);
        if (from_top) {
            mmh_remove_max(mmh);
            --t;
            ++count;
        } else  {
            mmh_remove_min(mmh);
            ++b;
            ++count;
        }
        if (b >= t) {
            // maybe we ran out of elements? if yes, stop
            break;
        }
    }
    cmp_ok(count, "==", erase, "deleted %d elements from MinMaxHeap", erase);
    cmp_ok(mmh_size(mmh), "==", top - erase, "MinMaxHeap ends up with %d elements", top - erase);

    hmin = mmh_min(mmh);
    cmp_ok(hmin, "==", data[b], "MinMaxHeap min is %d for top %d, heapify %d, grow %d", data[b], top, heapify, grow);

    hmax = mmh_max(mmh);
    cmp_ok(hmax, "==", data[t], "MinMaxHeap max is %d for top %d, heapify %d, grow %d", data[t], top, heapify, grow);

    // Now add those elements back, also in random order.
    count = 0;
    while(true) {
        if (b == 0 && t == top - 1) {
            break;
        }
        int from_top = random_in_range(0, 1);
        if (from_top) {
            if (t >= top - 1) {
                continue;
            }
            ++count;
            ++t;
            if (heapify) {
                mmh_add(mmh, data[t]);
            } else {
                mmh_insert(mmh, data[t]);
            }
        } else {
            if (b <= 0) {
                continue;
            }
            ++count;
            --b;
            if (heapify) {
                mmh_add(mmh, data[b]);
            } else {
                mmh_insert(mmh, data[b]);
            }
        }
    }
    if (heapify) {
        mmh_heapify(mmh);
    }

    cmp_ok(count, "==", erase, "readded %d elements to MinMaxHeap", erase);
    cmp_ok(b, "==", 0, "b is back to %d", 0);
    cmp_ok(t, "==", top-1, "t is back to %d", top-1);
    cmp_ok(mmh_size(mmh), "==", top, "MinMaxHeap is back to %d elements", top);

    hmin = mmh_min(mmh);
    cmp_ok(hmin, "==", data[b], "MinMaxHeap min is %d for top %d, heapify %d, grow %d", data[b], top, heapify, grow);

    hmax = mmh_max(mmh);
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

    if (argc <= 1) {
        test_heap_combinations(100000);
    } else {
        for (int j = 1; j < argc; ++j) {
            int top = atoi(argv[j]);
            test_heap_combinations(top);
        }
    }

    done_testing();
}
