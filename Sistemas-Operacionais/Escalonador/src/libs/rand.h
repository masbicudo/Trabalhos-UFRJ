#ifndef RAND_H_
#define RAND_H_

#include <stdint.h>

typedef struct _pcg32_random_t { uint64_t state;  uint64_t inc; } pcg32_random_t;

uint32_t pcg32_random_r(pcg32_random_t* rng);
void pcg32_srandom_r(pcg32_random_t* rng, uint64_t initstate, uint64_t initseq);
double drand(pcg32_random_t* rng);

#endif