SRC = poriball.c
CC = cc
CFLAGS = -g -Wall ${INCS}
INCS = -I/usr/include/SDL
LIBS = -lSDL -lSDL_image


all: poriball

poriball:
	@${CC} ${CFLAGS} ${SRC} ${LIBS}
