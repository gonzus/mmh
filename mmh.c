#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "mmh.h"

#define MMH_LFT(mmh, p) (2 * (p) + 1)
#define MMH_RGT(mmh, p) (2 * (p) + 2)
#define MMH_PAR(mmh, p) (((p) - 1) / 2)

#define MMH_SWP(mmh, a, b) \
    do { \
        mmh_t t = mmh->dat[a]; \
        mmh->dat[a] = mmh->dat[b]; \
        mmh->dat[b] = t; \
    } while (0)

#if 0
static void show(const char* msg, MinMaxHeap* mmh);
#endif

static bool mmh_is_min_level(MinMaxHeap* mmh, unsigned int p);
static void mmh_push_down(MinMaxHeap* mmh, unsigned int i);
static void mmh_push_down_min(MinMaxHeap* mmh, unsigned int m);
static void mmh_push_down_max(MinMaxHeap* mmh, unsigned int m);
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
    unsigned int m = mmh->pos / 2;
    for (int j = m - 1; j >= 0; --j) {
        mmh_push_down(mmh, j);
        // show("AFTER push_down", mmh);
    }
}

mmh_t mmh_min(MinMaxHeap* mmh) {
    if (mmh->pos <= 0) {
        return 0;
    }

    mmh_t m = mmh->dat[0];
    return m;
}

mmh_t mmh_max(MinMaxHeap* mmh) {
    if (mmh->pos <= 0) {
        return 0;
    }

    mmh_t m = mmh->dat[0];
    if (mmh->pos > 1 && m < mmh->dat[1]) {
        m = mmh->dat[1];
    }
    if (mmh->pos > 2 && m < mmh->dat[2]) {
        m = mmh->dat[2];
    }
    return m;
}

static bool mmh_is_min_level(MinMaxHeap* mmh, unsigned int p) {
    (void) mmh;
    unsigned int l = p + 1;
    if (l < 0x0001) return false;
    if (l < 0x0002) return true;
    if (l < 0x0004) return false;
    if (l < 0x0008) return true;
    if (l < 0x0010) return false;
    if (l < 0x0020) return true;
    if (l < 0x0040) return false;
    if (l < 0x0080) return true;
    if (l < 0x0100) return false;
    if (l < 0x0200) return true;
    if (l < 0x0400) return false;
    if (l < 0x0800) return true;
    if (l < 0x1000) return false;
    if (l < 0x2000) return true;
    if (l < 0x4000) return false;
    if (l < 0x8000) return true;
    return false;
}

static void mmh_push_down(MinMaxHeap* mmh, unsigned int i) {
    if (mmh_is_min_level(mmh, i)) {
        mmh_push_down_min(mmh, i);
    } else {
        mmh_push_down_max(mmh, i);
    }
}

static void mmh_push_down_min(MinMaxHeap* mmh, unsigned int m) {
    while (1) {
        unsigned int i = m;
        unsigned int p = UINT32_MAX;
        bool found = false;
        mmh_t mval = 0;
        unsigned int l = MMH_LFT(mmh, i);
        if (l < mmh->pos) {
            if (!found || mval > mmh->dat[l]) {
                mval = mmh->dat[l];
                found = true;
                m = l;
                p = UINT32_MAX;
            }
            unsigned int ll = MMH_LFT(mmh, l);
            if (ll < mmh->pos) {
                if (!found || mval > mmh->dat[ll]) {
                    mval = mmh->dat[ll];
                    found = true;
                    m = ll;
                    p = l;
                }
            }
            unsigned int lr = MMH_RGT(mmh, l);
            if (lr < mmh->pos) {
                if (!found || mval > mmh->dat[lr]) {
                    mval = mmh->dat[lr];
                    found = true;
                    m = lr;
                    p = l;
                }
            }
        }
        unsigned int r = MMH_RGT(mmh, i);
        if (r < mmh->pos) {
            if (!found || mval > mmh->dat[r]) {
                mval = mmh->dat[r];
                found = true;
                m = r;
                p = UINT32_MAX;
            }
            unsigned int rl = MMH_LFT(mmh, r);
            if (rl < mmh->pos) {
                if (!found || mval > mmh->dat[rl]) {
                    mval = mmh->dat[rl];
                    found = true;
                    m = rl;
                    p = r;
                }
            }
            unsigned int rr = MMH_RGT(mmh, r);
            if (rr < mmh->pos) {
                if (!found || mval > mmh->dat[rr]) {
                    mval = mmh->dat[rr];
                    found = true;
                    m = rr;
                    p = r;
                }
            }
        }
        if (!found) {
            break;
        }
        if (p != UINT32_MAX) {
            if (mmh->dat[m] < mmh->dat[i]) {
                MMH_SWP(mmh, m, i);
                if (mmh->dat[m] > mmh->dat[p]) {
                    MMH_SWP(mmh, m, p);
                }
            }
        } else if (mmh->dat[m] < mmh->dat[i]) {
            MMH_SWP(mmh, m, i);
        } else {
            break;
        }
    }
}

static void mmh_push_down_max(MinMaxHeap* mmh, unsigned int m) {
    while (1) {
        unsigned int i = m;
        unsigned int p = UINT32_MAX;
        bool found = false;
        mmh_t mval = INT32_MIN;
        unsigned int l = MMH_LFT(mmh, i);
        if (l < mmh->pos) {
            if (!found || mval < mmh->dat[l]) {
                mval = mmh->dat[l];
                found = true;
                m = l;
                p = -1;
            }
            unsigned int ll = MMH_LFT(mmh, l);
            if (ll < mmh->pos) {
                if (!found || mval < mmh->dat[ll]) {
                    mval = mmh->dat[ll];
                found = true;
                    m = ll;
                    p = l;
                }
            }
            unsigned int lr = MMH_RGT(mmh, l);
            if (lr < mmh->pos) {
                if (!found || mval < mmh->dat[lr]) {
                    mval = mmh->dat[lr];
                found = true;
                    m = lr;
                    p = l;
                }
            }
        }
        unsigned int r = MMH_RGT(mmh, i);
        if (r < mmh->pos) {
            if (!found || mval < mmh->dat[r]) {
                mval = mmh->dat[r];
                found = true;
                m = r;
                p = -1;
            }
            unsigned int rl = MMH_LFT(mmh, r);
            if (rl < mmh->pos) {
                if (!found || mval < mmh->dat[rl]) {
                    mval = mmh->dat[rl];
                found = true;
                    m = rl;
                    p = r;
                }
            }
            unsigned int rr = MMH_RGT(mmh, r);
            if (rr < mmh->pos) {
                if (!found || mval < mmh->dat[rr]) {
                    mval = mmh->dat[rr];
                found = true;
                    m = rr;
                    p = r;
                }
            }
        }
        if (!found) {
            break;
        }
        if (p != UINT32_MAX) {
            if (mmh->dat[m] > mmh->dat[i]) {
                MMH_SWP(mmh, m, i);
                if (mmh->dat[m] < mmh->dat[p]) {
                    MMH_SWP(mmh, m, p);
                }
            }
        } else if (mmh->dat[m] > mmh->dat[i]) {
            MMH_SWP(mmh, m, i);
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
