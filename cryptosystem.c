#include <pbc.h>
#include <signal.h>
#include "cryptosystem.h"
#include "math.h"

void done() {
  fprintf(stderr, "%c[0;%d;m", 0x1b, 32);
  fprintf(stderr, " done");
  fprintf(stderr, "%c[0;;m", 0x1b);
  fprintf(stderr, "\n");
}

void sig_int() {
  fprintf(stderr, "%c[0;%d;m", 0x1b, 33);
  fprintf(stderr, " interactive signal");
  fprintf(stderr, "%c[0;;m", 0x1b);
  fprintf(stderr, "\n");
  exit(1);
}

void sig_segv() {
  fprintf(stderr, "%c[0;%d;m", 0x1b, 31);
  fprintf(stderr, " segmentation fault");
  fprintf(stderr, "%c[0;;m", 0x1b);
  fprintf(stderr, "\n");
  exit(1);
}

int cryptosystem_init(cryptosystem_t cs, size_t order, size_t dimensions) {

  int i;
  mpz_t p, q, r, s, n; // n == p * q * r * s
  mpz_t pq, rs;
  mpz_t qrs, pqs, prs, prq;
  element_t gamma, h, power;

  signal(SIGINT, sig_int);
  signal(SIGSEGV, sig_segv);

  fprintf(stderr, "Building a new cryptosystem of 4 x %d-bit primes, with %d dimensions.\n", order, dimensions);

  fprintf(stderr, "Reading random bytes from /dev/urandom.\n");
  pbc_random_set_file("/dev/urandom");

  fprintf(stderr, "- Initializing multiple precision objects:");
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
  done();

  cs[0].order = order;
  cs[0].dimensions = dimensions;
  //cs[0].tag_Gs = ...;

  fprintf(stderr, "- Picking random primes (this may take a while):");
  fprintf(stderr, "1 ");
  fflush(NULL);
  random_prime(p, cs[0].order);
  fprintf(stderr, "2 ");
  fflush(NULL);
  random_prime(q, cs[0].order);
  fprintf(stderr, "3 ");
  fflush(NULL);
  random_prime(r, cs[0].order);
  fprintf(stderr, "4 ");
  fflush(NULL);
  random_prime(s, cs[0].order);
  done();

  fprintf(stderr, "- Computing pairing function (this will take even longer):");
  fflush(NULL);
  mpz_mul(pq, p, q);
  mpz_mul(rs, r, s);
  mpz_mul(n, pq, rs);
  pbc_param_init_a1_gen(cs[0].param, n);
  pairing_init_pbc_param(cs[0].pairing, cs[0].param);
  done();

  fprintf(stderr, "- Picking prime subgroup generators:");
  fflush(NULL);
  element_init_G1(cs[0].g_Gp, cs[0].pairing);
  element_init_G1(cs[0].g_Gq, cs[0].pairing);
  element_init_G1(cs[0].g_Gr, cs[0].pairing);
  element_init_G1(cs[0].g_Gs, cs[0].pairing);
  element_random(cs[0].g_Gp);
  element_random(cs[0].g_Gq);
  element_random(cs[0].g_Gr);
  element_random(cs[0].g_Gs);
  mpz_mul(qrs, q, rs);
  mpz_mul(pqs, pq, s);
  mpz_mul(prs, p, rs);
  mpz_mul(prq, pq, r);
  element_pow_mpz(cs[0].g_Gp, cs[0].g_Gp, qrs);
  element_pow_mpz(cs[0].g_Gr, cs[0].g_Gr, pqs);
  element_pow_mpz(cs[0].g_Gq, cs[0].g_Gq, prs);
  element_pow_mpz(cs[0].g_Gs, cs[0].g_Gs, prq);
  done();

  cs[0].hs = malloc(cs[0].dimensions * 2 * sizeof(element_t));
  cs[0].us = malloc(cs[0].dimensions * 2 * sizeof(element_t));

  element_init_Zr(power, cs[0].pairing);

  fprintf(stderr, "- Picking per-dimension elements (this will take a very long time):\n");
  fflush(NULL);
  for (i = 0; i < cs[0].dimensions * 2; i += 2) {
    fprintf(stderr, ".");
    fflush(NULL);
    element_init_G1(cs[0].hs[i], cs[0].pairing);
    element_set(cs[0].hs[i], cs[0].g_Gp);
    element_random(power);
    element_pow_zn(cs[0].hs[i], cs[0].hs[i], power);

    element_init_G1(cs[0].hs[i+1], cs[0].pairing);
    element_set(cs[0].hs[i+1], cs[0].g_Gp);
    element_random(power);
    element_pow_zn(cs[0].hs[i+1], cs[0].hs[i+1], power);

    element_init_G1(cs[0].us[i], cs[0].pairing);
    element_set(cs[0].us[i], cs[0].g_Gp);
    element_random(power);
    element_pow_zn(cs[0].us[i], cs[0].us[i], power);

    element_init_G1(cs[0].us[i+1], cs[0].pairing);
    element_set(cs[0].us[i+1], cs[0].g_Gp);
    element_random(power);
    element_pow_zn(cs[0].us[i+1], cs[0].us[i+1], power);
  }
  done();

  fprintf(stderr, "- Picking gamma:");
  fflush(NULL);
  element_init_Zr(gamma, cs[0].pairing);
  element_set(gamma, cs[0].g_Gp);
  element_random(power);
  element_pow_zn(gamma, gamma, power);
  done();

  fprintf(stderr, "- Picking h:");
  fflush(NULL);
  element_init_G1(h, cs[0].pairing);
  element_set(h, cs[0].g_Gp);
  element_random(power);
  element_pow_zn(h, h, power);
  done();

  fprintf(stderr, "- Computing P:");
  fflush(NULL);
  element_init_GT(cs[0].P, cs[0].pairing);
  pairing_apply(cs[0].P, cs[0].g_Gp, h, cs[0].pairing);
  element_pow_zn(cs[0].P, cs[0].P, gamma);
  done();

  fprintf(stderr, "- Computing h^(-gamma):");
  fflush(NULL);
  element_init_G1(cs[0].h_neg_gamma, cs[0].pairing);
  element_neg(cs[0].h_neg_gamma, gamma);
  element_pow_zn(cs[0].h_neg_gamma, h, cs[0].h_neg_gamma);
  done();

  fprintf(stderr, "- Computing GT generator:");
  fflush(NULL);
  element_init_GT(cs[0].g_GT, cs[0].pairing);
  pairing_apply(cs[0].g_GT, cs[0].g_Gp, cs[0].g_Gp, cs[0].pairing);
  done();

  fprintf(stderr, "- Cleaning up:");
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
  element_clear(gamma);
  element_clear(h);
  done();

  signal(SIGINT, SIG_DFL);
  signal(SIGSEGV, SIG_DFL);

  return 0;
}

int cryptosystem_clear(cryptosystem_t cs) {
  int i;
  for (i = 0; i < cs[0].dimensions * 2; i += 2) {
    element_clear(cs[0].hs[i]);
    element_clear(cs[0].hs[i+1]);
    element_clear(cs[0].us[i]);
    element_clear(cs[0].us[i+1]);
  }
  free(cs[0].hs);
  free(cs[0].us);
  
  element_clear(cs[0].h_neg_gamma);
  element_clear(cs[0].P);

  element_clear(cs[0].g_Gp);
  element_clear(cs[0].g_Gq);
  element_clear(cs[0].g_Gr);
  element_clear(cs[0].g_Gs);

  element_clear(cs[0].g_GT);
  // g_GT

  pbc_param_clear(cs[0].param);
  pairing_clear(cs[0].pairing);

  return 0;
}
