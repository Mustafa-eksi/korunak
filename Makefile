CFLAGS=-Wall -I/usr/include/
LIBS=-L/usr/local/lib/raylib/ -lraylib

korunak: korunak.c kilitler.c kilitler.h
	gcc $(CFLAGS) -o korunak korunak.c $(LIBS)

