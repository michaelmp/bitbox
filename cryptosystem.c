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

  cs->order = order;
  cs->dimensions = dimensions;
  //cs->tag_Gs = ...;

  fprintf(stderr, "- Picking random primes (this may take a while):");
  fprintf(stderr, "1 ");
  fflush(NULL);
  random_prime(p, cs->order);
  fprintf(stderr, "2 ");
  fflush(NULL);
  random_prime(q, cs->order);
  fprintf(stderr, "3 ");
  fflush(NULL);
  random_prime(r, cs->order);
  fprintf(stderr, "4 ");
  fflush(NULL);
  random_prime(s, cs->order);
  done();

  fprintf(stderr, "- Computing pairing function (this will take even longer):");
  fflush(NULL);
  mpz_mul(pq, p, q);
  mpz_mul(rs, r, s);
  mpz_mul(n, pq, rs);
  pbc_param_init_a1_gen(cs->param, n);
  pairing_init_pbc_param(cs->pairing, cs->param);
  done();

  fprintf(stderr, "- Picking prime subgroup generators:");
  fflush(NULL);
  element_init_G1(cs->g_Gp, cs->pairing);
  element_init_G1(cs->g_Gq, cs->pairing);
  element_init_G1(cs->g_Gr, cs->pairing);
  element_init_G1(cs->g_Gs, cs->pairing);
  element_random(cs->g_Gp);
  element_random(cs->g_Gq);
  element_random(cs->g_Gr);
  element_random(cs->g_Gs);
  mpz_mul(qrs, q, rs);
  mpz_mul(pqs, pq, s);
  mpz_mul(prs, p, rs);
  mpz_mul(prq, pq, r);
  element_pow_mpz(cs->g_Gp, cs->g_Gp, qrs);
  element_pow_mpz(cs->g_Gr, cs->g_Gr, pqs);
  element_pow_mpz(cs->g_Gq, cs->g_Gq, prs);
  element_pow_mpz(cs->g_Gs, cs->g_Gs, prq);
  done();

  cs->hs = malloc(cs->dimensions * 2 * sizeof(element_t));
  cs->us = malloc(cs->dimensions * 2 * sizeof(element_t));

  element_init_Zr(power, cs->pairing);

  fprintf(stderr, "- Picking per-dimension elements (this will take a very long time):\n");
  fflush(NULL);
  for (i = 0; i < cs->dimensions * 2; i += 2) {
    fprintf(stderr, ".");
    fflush(NULL);
    element_init_G1(cs->hs[i], cs->pairing);
    element_set(cs->hs[i], cs->g_Gp);
    element_random(power);
    element_pow_zn(cs->hs[i], cs->hs[i], power);

    element_init_G1(cs->hs[i+1], cs->pairing);
    element_set(cs->hs[i+1], cs->g_Gp);
    element_random(power);
    element_pow_zn(cs->hs[i+1], cs->hs[i+1], power);

    element_init_G1(cs->us[i], cs->pairing);
    element_set(cs->us[i], cs->g_Gp);
    element_random(power);
    element_pow_zn(cs->us[i], cs->us[i], power);

    element_init_G1(cs->us[i+1], cs->pairing);
    element_set(cs->us[i+1], cs->g_Gp);
    element_random(power);
    element_pow_zn(cs->us[i+1], cs->us[i+1], power);
  }
  done();

  fprintf(stderr, "- Picking gamma:");
  fflush(NULL);
  element_init_Zr(gamma, cs->pairing);
  element_set(gamma, cs->g_Gp);
  element_random(power);
  element_pow_zn(gamma, gamma, power);
  done();

  fprintf(stderr, "- Picking h:");
  fflush(NULL);
  element_init_G1(h, cs->pairing);
  element_set(h, cs->g_Gp);
  element_random(power);
  element_pow_zn(h, h, power);
  done();

  fprintf(stderr, "- Computing P:");
  fflush(NULL);
  element_init_GT(cs->P, cs->pairing);
  pairing_apply(cs->P, cs->g_Gp, h, cs->pairing);
  element_pow_zn(cs->P, cs->P, gamma);
  done();

  fprintf(stderr, "- Computing h^(-gamma):");
  fflush(NULL);
  element_init_G1(cs->h_neg_gamma, cs->pairing);
  element_neg(cs->h_neg_gamma, gamma);
  element_pow_zn(cs->h_neg_gamma, h, cs->h_neg_gamma);
  done();

  fprintf(stderr, "- Computing GT generator:");
  fflush(NULL);
  element_init_GT(cs->g_GT, cs->pairing);
  pairing_apply(cs->g_GT, cs->g_Gp, cs->g_Gp, cs->pairing);
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

  for (i = 0; i < cs->dimensions * 2; i += 2) {
    element_clear(cs->hs[i]);
    element_clear(cs->hs[i+1]);
    element_clear(cs->us[i]);
    element_clear(cs->us[i+1]);
  }
  free(cs->hs);
  free(cs->us);
  
  element_clear(cs->h_neg_gamma);
  element_clear(cs->P);

  element_clear(cs->g_Gp);
  element_clear(cs->g_Gq);
  element_clear(cs->g_Gr);
  element_clear(cs->g_Gs);

  element_clear(cs->g_GT);

  pbc_param_clear(cs->param);
  pairing_clear(cs->pairing);

  return 0;
}
