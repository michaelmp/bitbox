#ifndef BB_CRYPTOSYSTEM_H
#define BB_CRYPTOSYSTEM_H

#include <pbc.h>

typedef struct {
  size_t dimensions;
  size_t order;
  pairing_t pairing;
  pbc_param_t param;
  //secret;
  element_t g_GT;
  element_t g_G_p;
  element_t g_G_q;
  element_t g_G_r;
  element_t g_G_s;
  //dlog;
  //tags;
} cryptosystem_t[1];

typedef element_t *acl_t;

int init_cryptosystem(cryptosystem_t, size_t order, size_t dimensions);
//int encrypt(cryptosystem_t*, char*, char**);
//int decrypt(cryptosystem_t*, char*, char**);
//int generate_token(cryptosystem_t*, element_t*);

#endif // BB_CRYPTOSYSTEM_H
