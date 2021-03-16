CC=arm-linux-gnueabihf-gcc
CXX=arm-linux-gnueabihf-g++
SDL_CFLAGS=$(shell arm-linux-gnueabihf-pkg-config sdl2 --cflags)
SDL_LIBS=$(shell arm-linux-gnueabihf-pkg-config sdl2 --libs)

all: bcm_host.o yyg_fix.o empty.o
	$(CC) -fPIC -shared -Wl,-soname,libbcm_host.so.1 bcm_host.o yyg_fix.o $(SDL_LIBS) -o libbcm_host.so
	$(CC) -fPIC -shared -Wl,-soname,libcurl.so.4 empty.o -o libcurl.so.4

%.o: %.c
	$(CC) -c $< -o $@ -fPIC $(SDL_CFLAGS)

clean:
	rm -f libcurl.so.4 libbcm_host.so bcm_host.o yyg_fix.o empty.o