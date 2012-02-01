#include <pbc.h>
#include "math.h"

// From Geremy Condra's pypbc.c
int random_prime(mpz_t i, size_t bits) {
  pbc_mpz_randomb(i, bits);
  mpz_nextprime(i, i);

  return 0;
}
