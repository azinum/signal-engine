# Makefile

CC=clang

CFLAGS=-Wall -O3 -ffast-math -Iinclude

PKG_LIBS=`pkg-config --libs sdl2`

PKG_CFLAGS=`pkg-config --cflags sdl2`

all:
	${CC} src/main.c -o signals ${CFLAGS} ${PKG_LIBS} ${PKG_CFLAGS}

run:
	./signals
