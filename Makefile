SRC = poriball.c vector.c physics.c io.c config.c
CC = cc
CFLAGS = -g -Wall ${INCS}
INCS = -I/usr/include/SDL
LIBS = -lSDL -lSDL_image -lSDL_ttf -lSDL_gfx


all: poriball

poriball:
	@echo "compiling..."
	@${CC} ${CFLAGS} ${SRC} ${LIBS} -o $@

clean:
	@echo "cleaning..."
	rm -f poriball

again: clean poriball
