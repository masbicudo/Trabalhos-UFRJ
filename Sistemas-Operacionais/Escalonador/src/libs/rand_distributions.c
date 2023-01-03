#include "rand_distributions.h"

double rnd_poisson(pcg32_random_t* r, double avg_iterations) {
  // returns the number of processes that will appear in a unit of time
  // avg_iterations: the average number of iterations to get the result
  double p = 1.0 / avg_iterations;
  int x = 1;
  while (1) {
    double d = drand(r);
    if (d < p) return x / avg_iterations;
    x++;
  }
}
