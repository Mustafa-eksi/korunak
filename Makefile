CFLAGS=-Wall -ggdb -static -static-libgcc -static-libstdc++ -I/usr/include/ -I/home/mustafa/third-party/libbcrypt
LIBS=-L/usr/local/lib/raylib/ -l:libraylib.a -L/usr/local/lib/ -l:bcrypt.a

korunak: korunak.c kilitler.c kilitler.h
	gcc $(CFLAGS) -o korunak korunak.c $(LIBS)

create_password: create_password.c
	gcc $(CFLAGS) -o create_password create_password.c $(LIBS)
