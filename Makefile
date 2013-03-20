CC = gcc
CFLAGS = -I. -Ofast -funroll-loops -flto -march=native -mtune=native -std=c99 -Wall -shared -fPIC
CFILES = lz4.c lz4hc.c sqlite3_lz4.c
BIN = sqlite3_lz4.so

all:
	$(CC) $(CFLAGS) $(CFILES) -o $(BIN)