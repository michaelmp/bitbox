#ifndef BB_SECRET_H_
#define BB_SECRET_H_

typedef struct {
  element_t g_G_p;
  element_t g_G_q;
  element_t g_G_r;
  element_t g_G_s;
  [] hs;
  [] us;
  h_gamma;
  P;
} secret_t;

#endif // BB_SECRET_H_
