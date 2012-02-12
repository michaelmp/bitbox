#include "cryptosystem.h"
#include "bitbox.h"
#include <string.h>

cryptosystem_t CS;
size_t ORDER = 64;
size_t DIMENSIONS = 10;

int setup_cryptosystem(int argc, char **argv) {
  cryptosystem_init(CS, ORDER, DIMENSIONS);
  cryptosystem_clear(CS);
  return 0;
}

int handle_init(int argc, char **argv) {
  setup_cryptosystem(argc, argv);
  return 0;
}

int handle_encrypt(int argc, char **argv) {
  char buffer[BLOCK_SIZE + 1];
  memset(buffer, 0, BLOCK_SIZE + 1);
  setup_cryptosystem(argc, argv);
  while (fgets(buffer, BLOCK_SIZE + 1, stdin)) {
    puts(buffer);
  }
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
  if (argc < 2) {
    // print usage
  } else {
    if (strstr(BB_ENCRYPT, argv[1]) == BB_ENCRYPT && strlen(argv[1])) {
      handle_encrypt(argc, argv);
    } else if (strstr(BB_INIT, argv[1]) == BB_INIT && strlen(argv[1])) {
      handle_init(argc, argv);
    }
  }

  return 0;
}
