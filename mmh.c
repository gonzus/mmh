#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdint.h>
#include "mmh.h"

#define MMH_MIN_LEVEL(h, l) \
    ((((int) log2(l+1)) & 1) == 0)

#define MMH_MAX_LEVEL(h, l) \
    ((((int) log2(l+1)) & 1) != 0)

#define MMH_ROOT(h) \
    (0)

#define MMH_LEFT(h, p) \
    (2 * (p) + 1)

#define MMH_RIGHT(h, p) \
    (2 * (p) + 2)

#define MMH_PARENT(h, p) \
    (((p) - 1) / 2)

#define MMH_SWAP(h, a, b) \
    do { \
        int t = h[a]; \
        h[a] = h[b]; \
        h[b] = t; \
    } while (0)

static void mmh_push_down(int h[], int s, int m);
static void mmh_push_down_min(int h[], int s, int m);
static void mmh_push_down_max(int h[], int s, int m);

#if 0
static void show(const char* msg, int h[], int s) {
    if (msg && msg[0]) {
        printf("%s\n", msg);
    }
    for (int j = 0; j < s; ++j) {
        printf("h[%2d] = %d\n", j, h[j]);
    }
    printf("==========\n");
}
#endif

void mmh_heapify(int h[], int s) {
    assert(s > 0);
    int m = s / 2;
    for (int j = m - 1; j >= 0; --j) {
        mmh_push_down(h, s, j);
        // show("AFTER push_down", h, s);
    }
}

int mmh_min(int h[], int s) {
    assert(s > 0);
    int m = h[0];
    return m;
}

int mmh_max(int h[], int s) {
    assert(s > 0);
    int m = h[0];
    if (s > 1 && m < h[1]) {
        m = h[1];
    }
    if (s > 2 && m < h[2]) {
        m = h[2];
    }
    return m;
}

static void mmh_push_down(int h[], int s, int i) {
    if (MMH_MIN_LEVEL(h, i)) {
        mmh_push_down_min(h, s, i);
    } else {
        mmh_push_down_max(h, s, i);
    }
}

static void mmh_push_down_min(int h[], int s, int m) {
    while (1) {
        int i = m;
        int g = 0;
        int mval = INT32_MAX;
        m = -1;
        int l = MMH_LEFT(h, i);
        if (l >= 0 && l < s) {
            if (mval > h[l]) {
                mval = h[l];
                m = l;
                g = 0;
            }
            int ll = MMH_LEFT(h, l);
            if (ll >= 0 && ll < s) {
                if (mval > h[ll]) {
                    mval = h[ll];
                    m = ll;
                    g = 1;
                }
            }
            int lr = MMH_RIGHT(h, l);
            if (lr >= 0 && lr < s) {
                if (mval > h[lr]) {
                    mval = h[lr];
                    m = lr;
                    g = 1;
                }
            }
        }
        int r = MMH_RIGHT(h, i);
        if (r >= 0 && r < s) {
            if (mval > h[r]) {
                mval = h[r];
                m = r;
                g = 0;
            }
            int rl = MMH_LEFT(h, r);
            if (rl >= 0 && rl < s) {
                if (mval > h[rl]) {
                    mval = h[rl];
                    m = rl;
                    g = 1;
                }
            }
            int rr = MMH_RIGHT(h, r);
            if (rr >= 0 && rr < s) {
                if (mval > h[rr]) {
                    mval = h[rr];
                    m = rr;
                    g = 1;
                }
            }
        }
        if (m < 0) {
            break;
        }
        if (g) {
            // printf("pushdown_min node %d=%d min %d=%d (grandchild)\n", i, h[i], m, h[m]);
            if (h[m] < h[i]) {
                MMH_SWAP(h, m, i);
                // printf("  swap %d %d\n", m, i);
                int p = MMH_PARENT(h, m);
                if (h[m] > h[p]) {
                    MMH_SWAP(h, m, p);
                    // printf("  swap parent %d %d\n", m, p);
                }
            }
        } else if (h[m] < h[i]) {
            // printf("pushdown_min node %d=%d min %d=%d\n", i, h[i], m, h[m]);
            MMH_SWAP(h, m, i);
            // printf("  swap %d %d\n", m, i);
        } else {
            break;
        }
    }
}

static void mmh_push_down_max(int h[], int s, int m) {
    while (1) {
        int i = m;
        int g = 0;
        int mval = INT32_MIN;
        m = -1;
        int l = MMH_LEFT(h, i);
        if (l >= 0 && l < s) {
            if (mval < h[l]) {
                mval = h[l];
                m = l;
                g = 0;
            }
            int ll = MMH_LEFT(h, l);
            if (ll >= 0 && ll < s) {
                if (mval < h[ll]) {
                    mval = h[ll];
                    m = ll;
                    g = 1;
                }
            }
            int lr = MMH_RIGHT(h, l);
            if (lr >= 0 && lr < s) {
                if (mval < h[lr]) {
                    mval = h[lr];
                    m = lr;
                    g = 1;
                }
            }
        }
        int r = MMH_RIGHT(h, i);
        if (r >= 0 && r < s) {
            if (mval < h[r]) {
                mval = h[r];
                m = r;
                g = 0;
            }
            int rl = MMH_LEFT(h, r);
            if (rl >= 0 && rl < s) {
                if (mval < h[rl]) {
                    mval = h[rl];
                    m = rl;
                    g = 1;
                }
            }
            int rr = MMH_RIGHT(h, r);
            if (rr >= 0 && rr < s) {
                if (mval < h[rr]) {
                    mval = h[rr];
                    m = rr;
                    g = 1;
                }
            }
        }
        if (m < 0) {
            break;
        }
        if (g) {
            // printf("pushdown_max node %d=%d max %d=%d (grandchild)\n", i, h[i], m, h[m]);
            if (h[m] > h[i]) {
                MMH_SWAP(h, m, i);
                // printf("  swap %d %d\n", m, i);
                int p = MMH_PARENT(h, m);
                if (h[m] < h[p]) {
                    MMH_SWAP(h, m, p);
                    // printf("  swap parent %d %d\n", m, p);
                }
            }
        } else if (h[m] > h[i]) {
            // printf("pushdown_max node %d=%d max %d=%d\n", i, h[i], m, h[m]);
            MMH_SWAP(h, m, i);
            // printf("  swap %d %d\n", m, i);
        } else {
            break;
        }
    }
}
