CFLAGS=-Wall -ggdb -static-libgcc -I/usr/local/include -I/usr/include/ -I/home/mustafa/third-party/libbcrypt
LIBS=-L/usr/local/lib -lraylib -L/usr/local/lib/ -l:bcrypt.a -lm
CREATE_PASSWORD_CFLAGS=`pkg-config --cflags gtk4`
CREATE_PASSWORD_LIBS=-Bstatic `pkg-config --libs gtk4`

korunak: korunak.c kilitler.c kilitler.h
	gcc $(CFLAGS) -o korunak korunak.c $(LIBS)

create_password: create_password.c
	gcc $(CFLAGS) $(CREATE_PASSWORD_CFLAGS) -o create_password create_password.c $(LIBS) $(CREATE_PASSWORD_LIBS)

usb_detector: usb_detector.c test_main.c
	gcc -ggdb -o usb_detector usb_detector.c
	gcc -ggdb -o test_main test_main.c

