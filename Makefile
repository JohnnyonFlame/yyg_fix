CC = arm-linux-gnueabihf-gcc
CXX = arm-linux-gnueabihf-g++
SDL_CFLAGS = $(shell arm-linux-gnueabihf-pkg-config sdl2 --cflags)
SDL_LIBS = $(shell arm-linux-gnueabihf-pkg-config sdl2 --libs)

PRELDFLAGS = -fPIC -shared -Wl,-soname,libbcm_host.so.1
CFLAGS = -fPIC

ifeq ($(LEGACY_RPI), 1)
	BACKEND = bcm_host
	PRELDFLAGS += -u vc_gpuserv_init
	CFLAGS += -Wno-multichar -Wall -Werror -Wno-unused-but-set-variable -I/opt/vc/include/ -I/opt/vc/include/interface/vmcs_host/ -I/opt/vc/include/interface/vchiq_arm/ -I/opt/vc/include/interface/vcos/pthreads/
	LDFLAGS = -L/opt/vc/lib -ldl -lvcos -lvchostif
else
	BACKEND = sdl2_host
	CFLAGS += $(SDL_CFLAGS)
	LDFLAGS = $(SDL_LIBS)
endif

all: src/backends/$(BACKEND).o src/yyg_fix.o
	$(CC) $(PRELDFLAGS) src/backends/$(BACKEND).o src/yyg_fix.o $(LDFLAGS) -o lib/libbcm_host.so

%.o: %.c
	$(CC) -c $< -o $@ -fPIC $(CFLAGS)

clean:
	rm -f lib/libbcm_host.so src/*.o src/backends/*.o