all: bellsrv


CFLAGS=$(shell pkgconf --cflags sdl2 SDL2_mixer)
LDFLAGS=$(shell pkgconf --libs sdl2 SDL2_mixer)

bellsrv: main.c
	gcc $^ -o $@ $(CFLAGS) $(LDFLAGS)
