#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "mmh.h"

// left, right and parent indexes
#define MMH_LFT(mmh, pos) (2 * (pos) + 1)
#define MMH_RGT(mmh, pos) (2 * (pos) + 2)
#define MMH_PAR(mmh, pos) (((pos) - 1) / 2)

// swap the values stored in two indexes
#define MMH_SWP(mmh, p0, p1) \
    do { \
        mmh_t t = mmh->dat[p0]; \
        mmh->dat[p0] = mmh->dat[p1]; \
        mmh->dat[p1] = t; \
    } while (false)

// helper to see if we found a new minimum
#define MMH_CHECK_MIN(mmh, fnd, val, pos, min, par, pav) \
    do { \
        if ((!fnd) || (val > mmh->dat[pos])) { \
            val = mmh->dat[pos]; \
            fnd = true; \
            min = pos; \
            par = pav; \
        } \
    } while (false)

// helper to see if we found a new maximum
#define MMH_CHECK_MAX(mmh, fnd, val, pos, max, par, pav) \
    do { \
        if ((!fnd) || (val < mmh->dat[pos])) { \
            val = mmh->dat[pos]; \
            fnd = true; \
            max = pos; \
            par = pav; \
        } \
    } while (false)

#if 0
static void show(const char* msg, MinMaxHeap* mmh);
#endif

static unsigned int mmh_find_max_pos(MinMaxHeap* mmh);
static unsigned int mmh_remove_pos(MinMaxHeap* mmh, unsigned int pos);

static bool mmh_is_min_level(MinMaxHeap* mmh, unsigned int pos);
static void mmh_grow(MinMaxHeap* mmh, unsigned int cap);

static void mmh_push_down(MinMaxHeap* mmh, unsigned int pos);
static void mmh_push_down_min(MinMaxHeap* mmh, unsigned int pos);
static void mmh_push_down_max(MinMaxHeap* mmh, unsigned int pos);

static void mmh_push_up(MinMaxHeap* mmh, unsigned int pos);
static void mmh_push_up_min(MinMaxHeap* mmh, unsigned int pos);
static void mmh_push_up_max(MinMaxHeap* mmh, unsigned int pos);

MinMaxHeap* mmh_create(void) {
    MinMaxHeap* mmh = (MinMaxHeap*) malloc(sizeof(MinMaxHeap));
    memset(mmh, 0, sizeof(MinMaxHeap));
    assert(mmh);
    return mmh;
}

MinMaxHeap* mmh_create_capacity(unsigned int capacity) {
    MinMaxHeap* mmh = mmh_create();
    mmh_grow(mmh, capacity);
    return mmh;
}

void mmh_destroy(MinMaxHeap* mmh) {
    assert(mmh);
    free((void*) mmh->dat);
    mmh->dat = 0;
    assert(!mmh->dat);
    free((void*) mmh);
    mmh = 0;
    assert(!mmh);
}

unsigned int mmh_size(MinMaxHeap* mmh) {
    assert(mmh);
    return mmh->pos;
}

unsigned int mmh_capacity(MinMaxHeap* mmh) {
    assert(mmh);
    return mmh->cap;
}

bool mmh_is_empty(MinMaxHeap* mmh) {
    assert(mmh);
    return mmh->pos <= 0;
}

unsigned int mmh_add(MinMaxHeap* mmh, mmh_t value) {
    assert(mmh);
    if (mmh->pos >= mmh->cap) {
        mmh_grow(mmh, mmh->cap > 0 ? 2 * mmh->cap : 16);
    }
    mmh->dat[mmh->pos++] = value;
    return mmh->pos;
}

unsigned int mmh_insert(MinMaxHeap* mmh, mmh_t value) {
    assert(mmh);
    mmh_add(mmh, value);
    mmh_push_up(mmh, mmh->pos - 1);
    return mmh->pos;
}

unsigned int mmh_remove_min(MinMaxHeap* mmh) {
    assert(mmh);
    if (mmh_is_empty(mmh)) {
        return mmh->pos;
    }
    unsigned int pos = 0;
    return mmh_remove_pos(mmh, pos);
}

unsigned int mmh_remove_max(MinMaxHeap* mmh) {
    assert(mmh);
    if (mmh_is_empty(mmh)) {
        return mmh->pos;
    }
    unsigned int pos = mmh_find_max_pos(mmh);
    return mmh_remove_pos(mmh, pos);
}

void mmh_clear(MinMaxHeap* mmh) {
    assert(mmh);
    mmh->pos = 0;
}

void mmh_heapify(MinMaxHeap* mmh) {
    assert(mmh);
    if (mmh_is_empty(mmh)) {
        return;
    }
    int mid = mmh->pos / 2;
    for (int j = mid - 1; j >= 0; --j) {
        mmh_push_down(mmh, j);
        // show("AFTER push_down", mmh);
    }
}

mmh_t mmh_min(MinMaxHeap* mmh) {
    assert(mmh);
    if (mmh_is_empty(mmh)) {
        return 0;
    }

    unsigned int pos = 0;
    return mmh->dat[pos];
}

mmh_t mmh_max(MinMaxHeap* mmh) {
    assert(mmh);
    if (mmh_is_empty(mmh)) {
        return 0;
    }

    unsigned int pos = mmh_find_max_pos(mmh);
    return mmh->dat[pos];
}

static unsigned int mmh_find_max_pos(MinMaxHeap* mmh) {
    assert(mmh);
    assert(mmh->pos > 0);
    unsigned int pos = 0;
    mmh_t max = mmh->dat[pos];
    if (mmh->pos > 1 && max < mmh->dat[1]) {
        pos = 1;
        max = mmh->dat[pos];
    }
    if (mmh->pos > 2 && max < mmh->dat[2]) {
        pos = 2;
        max = mmh->dat[pos];
    }
    return pos;
}

static unsigned int mmh_remove_pos(MinMaxHeap* mmh, unsigned int pos) {
    assert(mmh);
    assert(mmh->pos > 0);
    mmh->dat[pos] = mmh->dat[--mmh->pos];
    mmh_push_down(mmh, pos);
    return mmh->pos;
}

static bool mmh_is_min_level(MinMaxHeap* mmh, unsigned int pos) {
    (void) mmh;
    // we basically need to know the MSB in pos+1
    pos += 1;
    unsigned int c = 0;
    while (pos > 0) {
        ++c;
        pos >>= 1;
    }
    return ((c&1) != 0);
}

static void mmh_push_down(MinMaxHeap* mmh, unsigned int pos) {
    assert(mmh);
    if (mmh_is_min_level(mmh, pos)) {
        mmh_push_down_min(mmh, pos);
    } else {
        mmh_push_down_max(mmh, pos);
    }
}

static void mmh_push_down_min(MinMaxHeap* mmh, unsigned int pos) {
    assert(mmh);
    while (true) {
        unsigned int old = pos;
        unsigned int par = UINT32_MAX;
        bool found = false;
        mmh_t mval = INT32_MAX;
        unsigned int l = MMH_LFT(mmh, old);
        if (l < mmh->pos) {
            MMH_CHECK_MIN(mmh, found, mval, l, pos, par, UINT32_MAX);
            unsigned int ll = MMH_LFT(mmh, l);
            if (ll < mmh->pos) {
                MMH_CHECK_MIN(mmh, found, mval, ll, pos, par, l);
            }
            unsigned int lr = MMH_RGT(mmh, l);
            if (lr < mmh->pos) {
                MMH_CHECK_MIN(mmh, found, mval, lr, pos, par, l);
            }
        }
        unsigned int r = MMH_RGT(mmh, old);
        if (r < mmh->pos) {
            MMH_CHECK_MIN(mmh, found, mval, r, pos, par, UINT32_MAX);
            unsigned int rl = MMH_LFT(mmh, r);
            if (rl < mmh->pos) {
                MMH_CHECK_MIN(mmh, found, mval, rl, pos, par, r);
            }
            unsigned int rr = MMH_RGT(mmh, r);
            if (rr < mmh->pos) {
                MMH_CHECK_MIN(mmh, found, mval, rr, pos, par, r);
            }
        }
        if (!found) {
            break;
        }
        if (par != UINT32_MAX) {
            if (mmh->dat[pos] < mmh->dat[old]) {
                MMH_SWP(mmh, pos, old);
                if (mmh->dat[pos] > mmh->dat[par]) {
                    MMH_SWP(mmh, pos, par);
                }
            }
        } else if (mmh->dat[pos] < mmh->dat[old]) {
            MMH_SWP(mmh, pos, old);
        } else {
            break;
        }
    }
}

static void mmh_push_down_max(MinMaxHeap* mmh, unsigned int pos) {
    assert(mmh);
    while (true) {
        unsigned int old = pos;
        unsigned int par = UINT32_MAX;
        bool found = false;
        mmh_t mval = INT32_MIN;
        unsigned int l = MMH_LFT(mmh, old);
        if (l < mmh->pos) {
            MMH_CHECK_MAX(mmh, found, mval, l, pos, par, UINT32_MAX);
            unsigned int ll = MMH_LFT(mmh, l);
            if (ll < mmh->pos) {
                MMH_CHECK_MAX(mmh, found, mval, ll, pos, par, l);
            }
            unsigned int lr = MMH_RGT(mmh, l);
            if (lr < mmh->pos) {
                MMH_CHECK_MAX(mmh, found, mval, lr, pos, par, l);
            }
        }
        unsigned int r = MMH_RGT(mmh, old);
        if (r < mmh->pos) {
            MMH_CHECK_MAX(mmh, found, mval, r, pos, par, UINT32_MAX);
            unsigned int rl = MMH_LFT(mmh, r);
            if (rl < mmh->pos) {
                MMH_CHECK_MAX(mmh, found, mval, rl, pos, par, r);
            }
            unsigned int rr = MMH_RGT(mmh, r);
            if (rr < mmh->pos) {
                MMH_CHECK_MAX(mmh, found, mval, rr, pos, par, r);
            }
        }
        if (!found) {
            break;
        }
        if (par != UINT32_MAX) {
            if (mmh->dat[pos] > mmh->dat[old]) {
                MMH_SWP(mmh, pos, old);
                if (mmh->dat[pos] < mmh->dat[par]) {
                    MMH_SWP(mmh, pos, par);
                }
            }
        } else if (mmh->dat[pos] > mmh->dat[old]) {
            MMH_SWP(mmh, pos, old);
        } else {
            break;
        }
    }
}

static void mmh_push_up(MinMaxHeap* mmh, unsigned int pos) {
    assert(mmh);
    if (pos == 0) {
        return;
    }
    unsigned int par = MMH_PAR(mmh, pos);
    if (mmh_is_min_level(mmh, pos)) {
        if (mmh->dat[pos] > mmh->dat[par]) {
            MMH_SWP(mmh, pos, par);
            mmh_push_up_max(mmh, par);
        } else {
            mmh_push_up_min(mmh, pos);
        }
    } else {
        if (mmh->dat[pos] < mmh->dat[par]) {
            MMH_SWP(mmh, pos, par);
            mmh_push_up_min(mmh, par);
        } else {
            mmh_push_up_max(mmh, pos);
        }
    }
}

static void mmh_push_up_min(MinMaxHeap* mmh, unsigned int pos) {
    assert(mmh);
    while (true) {
        if (pos == 0) {
            break;
        }
        unsigned int par = MMH_PAR(mmh, pos);
        if (par == 0) {
            break;
        }
        unsigned int gpr = MMH_PAR(mmh, par);
        if (mmh->dat[pos] >= mmh->dat[gpr]) {
            break;
        }
        MMH_SWP(mmh, pos, gpr);
        pos = gpr;
    }
}

static void mmh_push_up_max(MinMaxHeap* mmh, unsigned int pos) {
    assert(mmh);
    while (true) {
        if (pos == 0) {
            break;
        }
        unsigned int par = MMH_PAR(mmh, pos);
        if (par == 0) {
            break;
        }
        unsigned int gpr = MMH_PAR(mmh, par);
        if (mmh->dat[pos] <= mmh->dat[gpr]) {
            break;
        }
        MMH_SWP(mmh, pos, gpr);
        pos = gpr;
    }
}

static void mmh_grow(MinMaxHeap* mmh, unsigned int cap) {
    assert(mmh);
    // printf("GROW %p %u to %u\n", (void*) mmh->dat, mmh->cap, cap);
    mmh_t* dat = realloc(mmh->dat, cap * sizeof(mmh_t));
    mmh->dat = dat;
    mmh->cap = cap;
}

#if 0
static void show(const char* msg, MinMaxHeap* mmh) {
    if (msg && msg[0]) {
        printf("%s\n", msg);
    }
    for (unsigned int j = 0; j < mmh->pos; ++j) {
        printf("h[%2u] = %d\n", j, mmh->dat[j]);
    }
    printf("==========\n");
}
#endif
