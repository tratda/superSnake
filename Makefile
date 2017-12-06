P=superSnake
OBJECTS=
CFLAGS = -std=gnu99 -g -Wall -Og
LDLIBS= -lncurses
CC=gcc

all: superSnake


$(P): $(OBJECTS)

static:
	gcc -std=gnu99 -g -Wall -Og  superSnake.c -o superSnake.static -lncurses -ltinfo

