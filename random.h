#ifndef RANDOM_H_
#define RANDOM_H_

/*
 * Encapsulation of standard C random number generator.
 * We provide a way to randomize the state of the RNG.
 */

// Randomize the C RNG with a time-related seed.
// Can be called multiple times, it will only randomize on the first call.
void randomize(void);

// Return a random unsigned integer in the closed-closed interval [min, max].
unsigned int random_in_range(unsigned int min, unsigned int max);

#endif
