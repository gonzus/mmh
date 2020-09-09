#include <stdlib.h>
#include <sys/time.h>
#include "random.h"

void randomize(void) {
    static int done;
    if (done) {
        return;
    }

    done = 1;
    struct timeval tv;
    gettimeofday(&tv, NULL);
    unsigned long us = tv.tv_sec * 1000000 + tv.tv_usec;
    srand(us);
}

unsigned int random_in_range(unsigned int min, unsigned int max) {
   return min + rand() / (RAND_MAX / (max - min + 1) + 1);
}
