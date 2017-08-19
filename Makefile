ALLEGRO_INSTALL_PREFIX=/
LIBS=${ALLEGRO_INSTALL_PREFIX}/usr/local/lib/liballegro.so          \
     ${ALLEGRO_INSTALL_PREFIX}/usr/local/lib/liballegro_dialog.so   \
     ${ALLEGRO_INSTALL_PREFIX}/usr/local/lib/liballegro_font.so     \
     ${ALLEGRO_INSTALL_PREFIX}/usr/local/lib/liballegro_ttf.so      \
     ${ALLEGRO_INSTALL_PREFIX}/usr/local/lib/liballegro_primitives.so \
     ${ALLEGRO_INSTALL_PREFIX}/usr/local/lib/liballegro_image.so


PROJ=headEngine
SRC=src/engine.cpp     \
    src/characters.cpp \
    src/game_loop.cpp  \
    src/world.cpp
OBJ=engine.o characters.o game_loop.o world.o

CC=clang
CXX=clang++

all: ${OBJ} ${SRC}
	${CXX} --shared -I/usr/local/include -lpthread ${OBJ} -std=c++11 -o ${PROJ}.so ${LIBS}

${OBJ}: ${SRC}
	${CXX} -Wall -std=c++11 -fPIC -c -g ${SRC}

clean:
	rm -f *.o
	rm -f ${PROJ}.so
	rm -f *~
	rm -f src/*~
