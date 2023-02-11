# Makefile

CC=clang

CFLAGS_COMMON=-Wall -O3

CFLAGS=-ffast-math -Iinclude

PKG_LIBS=`pkg-config --libs sdl2`

PKG_CFLAGS=`pkg-config --cflags sdl2`

TARGETS=main png2c

%: src/%.c
	${CC} $< -o signals ${CFLAGS} ${CFLAGS_COMMON} ${PKG_LIBS} ${PKG_CFLAGS}

%: tools/%.c
	${CC} $< -o $@ ${CFLAGS_COMMON} ${shell head $< -n 1 | grep -oP '(?<=C_FLAGS: ).*'}

all: ${TARGETS}
#	${CC} src/main.c -o signals ${CFLAGS} ${PKG_LIBS} ${PKG_CFLAGS}

run:
	./signals

rebuild_assets:
	./png2c.sh
