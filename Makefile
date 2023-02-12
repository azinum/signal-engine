# Makefile

CC=clang

CFLAGS_COMMON=-Wall -O3

CFLAGS=-ffast-math -Iinclude -lm

PKG_LIBS=`pkg-config --libs sdl2`

PKG_CFLAGS=`pkg-config --cflags sdl2`

TARGETS=main png2c

%: src/%.c
	${CC} $< -o signal_engine ${CFLAGS} ${CFLAGS_COMMON} ${PKG_LIBS} ${PKG_CFLAGS}

%: tools/%.c
	${CC} $< -o $@ ${CFLAGS_COMMON} -Iinclude -lm

all: ${TARGETS}
#	${CC} src/main.c -o signals ${CFLAGS} ${PKG_LIBS} ${PKG_CFLAGS}

run:
	./signal_engine

rebuild_assets:
	./png2c.sh
