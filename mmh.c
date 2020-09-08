#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "mmh.h"

#define MMH_MIN_LEVEL(mmh, l) \
    ((((int) log2(l+1)) & 1) == 0)

#define MMH_MAX_LEVEL(mmh, l) \
    ((((int) log2(l+1)) & 1) != 0)

#define MMH_ROOT(mmh) \
    (0)

#define MMH_LEFT(mmh, p) \
    (2 * (p) + 1)

#define MMH_RIGHT(mmh, p) \
    (2 * (p) + 2)

#define MMH_PARENT(mmh, p) \
    (((p) - 1) / 2)

#define MMH_SWAP(mmh, a, b) \
    do { \
        int t = mmh->dat[a]; \
        mmh->dat[a] = mmh->dat[b]; \
        mmh->dat[b] = t; \
    } while (0)

static void mmh_push_down(MinMaxHeap* mmh, int i);
static void mmh_push_down_min(MinMaxHeap* mmh, int m);
static void mmh_push_down_max(MinMaxHeap* mmh, int m);

#if 0
static void show(const char* msg, MinMaxHeap* mmh) {
    if (msg && msg[0]) {
        printf("%s\n", msg);
    }
    for (int j = 0; j < mmh->pos; ++j) {
        printf("h[%2d] = %d\n", j, mmh->dat[j]);
    }
    printf("==========\n");
}
#endif

static void mmh_grow(MinMaxHeap* mmh, int cap) {
    // printf("GROW %p %d to %d\n", (void*) mmh->dat, mmh->cap, cap);
    int* dat = realloc(mmh->dat, cap * sizeof(int));
    mmh->dat = dat;
    mmh->cap = cap;
}

MinMaxHeap* mmh_create(void) {
    MinMaxHeap* mmh = (MinMaxHeap*) malloc(sizeof(MinMaxHeap));
    memset(mmh, 0, sizeof(MinMaxHeap));
    return mmh;
}

MinMaxHeap* mmh_create_capacity(int capacity) {
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

int mmh_add(MinMaxHeap* mmh, int value) {
    if (mmh->pos >= mmh->cap) {
        mmh_grow(mmh, mmh->cap > 0 ? 2 * mmh->cap : 16);
    }
    mmh->dat[mmh->pos] = value;
    return ++mmh->pos;
}

void mmh_heapify(MinMaxHeap* mmh) {
    if (mmh->pos <= 0) {
        return;
    }
    int m = mmh->pos / 2;
    for (int j = m - 1; j >= 0; --j) {
        mmh_push_down(mmh, j);
        // show("AFTER push_down", mmh);
    }
}

int mmh_min(MinMaxHeap* mmh) {
    if (mmh->pos <= 0) {
        return 0;
    }

    int m = mmh->dat[0];
    return m;
}

int mmh_max(MinMaxHeap* mmh) {
    if (mmh->pos <= 0) {
        return 0;
    }

    int m = mmh->dat[0];
    if (mmh->pos > 1 && m < mmh->dat[1]) {
        m = mmh->dat[1];
    }
    if (mmh->pos > 2 && m < mmh->dat[2]) {
        m = mmh->dat[2];
    }
    return m;
}

static void mmh_push_down(MinMaxHeap* mmh, int i) {
    if (MMH_MIN_LEVEL(mmh, i)) {
        mmh_push_down_min(mmh, i);
    } else {
        mmh_push_down_max(mmh, i);
    }
}

static void mmh_push_down_min(MinMaxHeap* mmh, int m) {
    while (1) {
        int i = m;
        int p = -1;
        int mval = INT32_MAX;
        m = -1;
        int l = MMH_LEFT(mmh, i);
        if (l >= 0 && l < mmh->pos) {
            if (mval > mmh->dat[l]) {
                mval = mmh->dat[l];
                m = l;
                p = -1;
            }
            int ll = MMH_LEFT(mmh, l);
            if (ll >= 0 && ll < mmh->pos) {
                if (mval > mmh->dat[ll]) {
                    mval = mmh->dat[ll];
                    m = ll;
                    p = l;
                }
            }
            int lr = MMH_RIGHT(mmh, l);
            if (lr >= 0 && lr < mmh->pos) {
                if (mval > mmh->dat[lr]) {
                    mval = mmh->dat[lr];
                    m = lr;
                    p = l;
                }
            }
        }
        int r = MMH_RIGHT(mmh, i);
        if (r >= 0 && r < mmh->pos) {
            if (mval > mmh->dat[r]) {
                mval = mmh->dat[r];
                m = r;
                p = -1;
            }
            int rl = MMH_LEFT(mmh, r);
            if (rl >= 0 && rl < mmh->pos) {
                if (mval > mmh->dat[rl]) {
                    mval = mmh->dat[rl];
                    m = rl;
                    p = r;
                }
            }
            int rr = MMH_RIGHT(mmh, r);
            if (rr >= 0 && rr < mmh->pos) {
                if (mval > mmh->dat[rr]) {
                    mval = mmh->dat[rr];
                    m = rr;
                    p = r;
                }
            }
        }
        if (m < 0) {
            break;
        }
        if (p >= 0) {
            if (mmh->dat[m] < mmh->dat[i]) {
                MMH_SWAP(mmh, m, i);
                if (mmh->dat[m] > mmh->dat[p]) {
                    MMH_SWAP(mmh, m, p);
                }
            }
        } else if (mmh->dat[m] < mmh->dat[i]) {
            MMH_SWAP(mmh, m, i);
        } else {
            break;
        }
    }
}

static void mmh_push_down_max(MinMaxHeap* mmh, int m) {
    while (1) {
        int i = m;
        int p = -1;
        int mval = INT32_MIN;
        m = -1;
        int l = MMH_LEFT(mmh, i);
        if (l >= 0 && l < mmh->pos) {
            if (mval < mmh->dat[l]) {
                mval = mmh->dat[l];
                m = l;
                p = -1;
            }
            int ll = MMH_LEFT(mmh, l);
            if (ll >= 0 && ll < mmh->pos) {
                if (mval < mmh->dat[ll]) {
                    mval = mmh->dat[ll];
                    m = ll;
                    p = l;
                }
            }
            int lr = MMH_RIGHT(mmh, l);
            if (lr >= 0 && lr < mmh->pos) {
                if (mval < mmh->dat[lr]) {
                    mval = mmh->dat[lr];
                    m = lr;
                    p = l;
                }
            }
        }
        int r = MMH_RIGHT(mmh, i);
        if (r >= 0 && r < mmh->pos) {
            if (mval < mmh->dat[r]) {
                mval = mmh->dat[r];
                m = r;
                p = -1;
            }
            int rl = MMH_LEFT(mmh, r);
            if (rl >= 0 && rl < mmh->pos) {
                if (mval < mmh->dat[rl]) {
                    mval = mmh->dat[rl];
                    m = rl;
                    p = r;
                }
            }
            int rr = MMH_RIGHT(mmh, r);
            if (rr >= 0 && rr < mmh->pos) {
                if (mval < mmh->dat[rr]) {
                    mval = mmh->dat[rr];
                    m = rr;
                    p = r;
                }
            }
        }
        if (m < 0) {
            break;
        }
        if (p >= 0) {
            if (mmh->dat[m] > mmh->dat[i]) {
                MMH_SWAP(mmh, m, i);
                if (mmh->dat[m] < mmh->dat[p]) {
                    MMH_SWAP(mmh, m, p);
                }
            }
        } else if (mmh->dat[m] > mmh->dat[i]) {
            MMH_SWAP(mmh, m, i);
        } else {
            break;
        }
    }
}
