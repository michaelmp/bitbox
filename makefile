SRC = src/*.c
TARGET = bitbox

all:
	gcc -Wall $(SRC) -static -L/usr/local/lib -lpbc -lgmp -I/usr/local/include/pbc -o $(TARGET)

clean:
	-rm $(TARGET)
