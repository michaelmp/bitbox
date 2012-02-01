#include <pbc.h>
#include "cryptosystem.h"
#include "math.h"

int init_cryptosystem(cryptosystem_t cs, size_t order, size_t dimensions) {

  int i;
  mpz_t p, q, r, s, n; // n == p * q * r * s
  mpz_t pq, rs;
  mpz_t qrs, pqs, prs, prq;
  element_t power;

  printf("Building a new cryptosystem of 4 primes, 4 x %d bits, %d dimensions.\n", order, dimensions);

  printf("- Initializing multiple precision objects... ");
  fflush(NULL);
  mpz_init(p);
  mpz_init(q);
  mpz_init(r);
  mpz_init(s);
  mpz_init(n);
  mpz_init(pq);
  mpz_init(rs);
  mpz_init(qrs);
  mpz_init(pqs);
  mpz_init(prs);
  mpz_init(prq);
  printf("done.\n");

  cs[0].order = order;
  cs[0].dimensions = dimensions;
  //cs[0].tags = ...;

  printf("- Finding random primes (this may take a while)... ");
  fflush(NULL);
  random_prime(p, cs[0].order);
  random_prime(q, cs[0].order);
  random_prime(r, cs[0].order);
  random_prime(s, cs[0].order);
  printf("done.\n");

  printf("- Generating pairing parameters (this may take a while)... ");
  fflush(NULL);
  mpz_mul(pq, p, q);
  mpz_mul(rs, r, s);
  mpz_mul(n, pq, rs);
  pbc_param_init_a1_gen(cs[0].param, n);
  pairing_init_pbc_param(cs[0].pairing, cs[0].param);
  printf("done.\n");

  printf("- Generating subgroup generators (this may take a while)... ");
  fflush(NULL);
  element_init_G1(cs[0].g_G_p, cs[0].pairing);
  element_init_G1(cs[0].g_G_q, cs[0].pairing);
  element_init_G1(cs[0].g_G_r, cs[0].pairing);
  element_init_G1(cs[0].g_G_s, cs[0].pairing);
  element_random(cs[0].g_G_p);
  element_random(cs[0].g_G_q);
  element_random(cs[0].g_G_r);
  element_random(cs[0].g_G_s);
  mpz_mul(qrs, q, rs);
  mpz_mul(pqs, pq, s);
  mpz_mul(prs, p, rs);
  mpz_mul(prq, pq, r);
  element_pow_mpz(cs[0].g_G_p, cs[0].g_G_p, qrs);
  element_pow_mpz(cs[0].g_G_r, cs[0].g_G_r, pqs);
  element_pow_mpz(cs[0].g_G_q, cs[0].g_G_q, prs);
  element_pow_mpz(cs[0].g_G_s, cs[0].g_G_s, prq);
  printf("done.\n");

  /*
  element_init_Zr(power, cs[0].pairing);
  for (i = 0; i < cs[0].order; i++) {
    element_set(cs[0].hs[i][0], cs[0].g_G_p);
    element_random(power);
    element_pow_zn(cs[0].hs[i][0], cs[0].hs[i][0], power);

    element_set(cs[0].hs[i][1], cs[0].g_G_p);
    element_random(power);
    element_pow_zn(cs[0].hs[i][1], cs[0].hs[i][1], power);

    element_set(cs[0].us[i][0], cs[0].g_G_p);
    element_random(power);
    element_pow_zn(cs[0].us[i][0], cs[0].us[i][0], power);

    element_set(cs[0].us[i][1], cs[0].g_G_p);
    element_random(power);
    element_pow_zn(cs[0].us[i][1], cs[0].us[i][1], power);
  }
  */

  /*
  gamma = element(cs[0].pairing, Zr, get_random(p));
  //
  //

  element_t h;

  element_set(h, cs[0].g_G_p);
  element_random(power);
  element_pow_zn(h, h, power);

  element_t P;

  P = cs[0].pairing.apply(g_G_p, h) ** gamma; //

  element_t h_gamma;

  h_gamma = h ** (-gamma); //

  cs[0].secret = SecretKey(g_G_p, g_G_q, g_G_r, g_G_s, hs, us, h_gamma, P); //
  cs[0].g_GT = cs[0].pairing.apply(g_G_p, g_G_p); //
  */

  printf("- Cleaning up... ");
  mpz_clear(p);
  mpz_clear(q);
  mpz_clear(r);
  mpz_clear(s);
  mpz_clear(n);
  mpz_clear(pq);
  mpz_clear(rs);
  mpz_clear(qrs);
  mpz_clear(pqs);
  mpz_clear(prs);
  mpz_clear(prq);
  element_clear(power);
  printf("done.\n");

  return 0;
}
