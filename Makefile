CC = gcc
CFLAGS = -I. -std=c99 -Wall -W -Wundef -Wno-implicit-function-declaration -shared -fPIC
CFILES = lz4.c lz4hc.c sqlite3_lz4.c
BIN = sqlite3_lz4.so

all:
	$(CC) $(CFLAGS) $(CFILES) -o $(BIN)