#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "mmh.h"

#define MMH_LFT(mmh, pos) (2 * (pos) + 1)
#define MMH_RGT(mmh, pos) (2 * (pos) + 2)
#define MMH_PAR(mmh, pos) (((pos) - 1) / 2)

#define MMH_SWP(mmh, p0, p1) \
    do { \
        mmh_t t = mmh->dat[p0]; \
        mmh->dat[p0] = mmh->dat[p1]; \
        mmh->dat[p1] = t; \
    } while (false)

#define MMH_CHECK_MIN(mmh, fnd, val, pos, min, par, pav) \
    do { \
        if (!fnd || val > mmh->dat[pos]) { \
            val = mmh->dat[pos]; \
            fnd = true; \
            min = pos; \
            par = pav; \
        } \
    } while (false)

#define MMH_CHECK_MAX(mmh, fnd, val, pos, max, par, pav) \
    do { \
        if (!fnd || val < mmh->dat[pos]) { \
            val = mmh->dat[pos]; \
            fnd = true; \
            max = pos; \
            par = pav; \
        } \
    } while (false)

#if 0
static void show(const char* msg, MinMaxHeap* mmh);
#endif

static bool mmh_is_min_level(MinMaxHeap* mmh, unsigned int pos);
static void mmh_push_down(MinMaxHeap* mmh, unsigned int pos);
static void mmh_push_down_min(MinMaxHeap* mmh, unsigned int pos);
static void mmh_push_down_max(MinMaxHeap* mmh, unsigned int pos);
static void mmh_grow(MinMaxHeap* mmh, unsigned int cap);

MinMaxHeap* mmh_create(void) {
    MinMaxHeap* mmh = (MinMaxHeap*) malloc(sizeof(MinMaxHeap));
    memset(mmh, 0, sizeof(MinMaxHeap));
    return mmh;
}

MinMaxHeap* mmh_create_capacity(unsigned int capacity) {
    MinMaxHeap* mmh = mmh_create();
    mmh_grow(mmh, capacity);
    return mmh;
}

void mmh_destroy(MinMaxHeap* mmh) {
    free((void*) mmh->dat);
    mmh->dat = 0;
    free((void*) mmh);
    mmh = 0;
}

unsigned int mmh_add(MinMaxHeap* mmh, mmh_t value) {
    if (mmh->pos >= mmh->cap) {
        mmh_grow(mmh, mmh->cap > 0 ? 2 * mmh->cap : 16);
    }
    mmh->dat[mmh->pos] = value;
    return ++mmh->pos;
}

void mmh_clear(MinMaxHeap* mmh) {
    mmh->pos = 0;
}

void mmh_heapify(MinMaxHeap* mmh) {
    if (mmh->pos <= 0) {
        return;
    }
    unsigned int mid = mmh->pos / 2;
    for (int j = mid - 1; j >= 0; --j) {
        mmh_push_down(mmh, j);
        // show("AFTER push_down", mmh);
    }
}

mmh_t mmh_min(MinMaxHeap* mmh) {
    if (mmh->pos <= 0) {
        return 0;
    }

    mmh_t ret = mmh->dat[0];
    return ret;
}

mmh_t mmh_max(MinMaxHeap* mmh) {
    if (mmh->pos <= 0) {
        return 0;
    }

    mmh_t ret = mmh->dat[0];
    if (mmh->pos > 1 && ret < mmh->dat[1]) {
        ret = mmh->dat[1];
    }
    if (mmh->pos > 2 && ret < mmh->dat[2]) {
        ret = mmh->dat[2];
    }
    return ret;
}

static bool mmh_is_min_level(MinMaxHeap* mmh, unsigned int pos) {
    (void) mmh;
    unsigned int chk = pos + 1;
    if (chk < 0x0001) return false;
    if (chk < 0x0002) return true;
    if (chk < 0x0004) return false;
    if (chk < 0x0008) return true;
    if (chk < 0x0010) return false;
    if (chk < 0x0020) return true;
    if (chk < 0x0040) return false;
    if (chk < 0x0080) return true;
    if (chk < 0x0100) return false;
    if (chk < 0x0200) return true;
    if (chk < 0x0400) return false;
    if (chk < 0x0800) return true;
    if (chk < 0x1000) return false;
    if (chk < 0x2000) return true;
    if (chk < 0x4000) return false;
    if (chk < 0x8000) return true;
    return false;
}

static void mmh_push_down(MinMaxHeap* mmh, unsigned int pos) {
    if (mmh_is_min_level(mmh, pos)) {
        mmh_push_down_min(mmh, pos);
    } else {
        mmh_push_down_max(mmh, pos);
    }
}

static void mmh_push_down_min(MinMaxHeap* mmh, unsigned int pos) {
    while (true) {
        unsigned int old = pos;
        unsigned int par = UINT32_MAX;
        bool found = false;
        mmh_t mval = 0;
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

static void mmh_grow(MinMaxHeap* mmh, unsigned int cap) {
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
