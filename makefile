all:
	gcc -Wall cryptosystem.c math.c bitbox.c -static -L/usr/local/lib -lpbc -lgmp -I/usr/local/include/pbc
