#include "cryptosystem.h"

cryptosystem_t CS;
size_t ORDER = 512;
size_t DIMENSIONS = 100;

int setup_cryptosystem(int argc, char **argv) {
  return 0;
}

int handle_init(int argc, char **argv) {
  return init_cryptosystem(CS, ORDER, DIMENSIONS);
}

int handle_encrypt() {
  return 0;
}

int handle_decrypt() {
  return 0;
}

int handle_generate() {
  return 0;
}

int handle_import() {
  return 0;
}

int handle_export() {
  return 0;
}

int main(int argc, char **argv) {
  // init
  handle_init(argc, argv);

  setup_cryptosystem(argc, argv);

  // enc
  handle_encrypt();

  // dec
  handle_decrypt();

  // gen
  handle_generate();

  // import
  handle_import();

  // export
  handle_export();

  return 0;
}
