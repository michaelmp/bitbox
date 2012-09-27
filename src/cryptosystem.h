#ifndef BB_CRYPTOSYSTEM_H_
#define BB_CRYPTOSYSTEM_H_

#include <pbc.h>

struct cryptosystem_s {
  size_t dimensions;
  size_t order;
  pairing_t pairing;
  pbc_param_t param;
  element_t g_GT;
  element_t g_Gp;
  element_t g_Gq;
  element_t g_Gr;
  element_t g_Gs;
  element_t h_neg_gamma;
  element_t P;
  element_t *hs;
  element_t *us;
};
typedef struct cryptosystem_s *cryptosystem_ptr;
typedef struct cryptosystem_s cryptosystem_t[1];

typedef element_t *acl_t;

int cryptosystem_init(cryptosystem_t, size_t order, size_t dimensions);
int cryptosystem_clear(cryptosystem_t);
int encrypt(char**, char*, cryptosystem_t);
//int decrypt(char**, char*, token_t, cryptosystem_t);
//int decrypt(cryptosystem_t*, char*, char**);
//int generate_token(cryptosystem_t*, element_t*);

#endif // BB_CRYPTOSYSTEM_H_
