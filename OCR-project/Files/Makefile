# Makefile

CPPFLAGS = -MMD
CC = gcc
CFLAGS= -Wall -Wextra -Werror -std=c99 -O2 `pkg-config --cflags sdl2` `pkg-config --cflags gtk+-3.0` #-g -fsanitize=address -fno-omit-frame-pointer

LDFLAGS = #-rdynamic -fsanitize=address
LDLIBS = -lm `pkg-config --libs sdl` `pkg-config --libs gtk+-3.0` `pkg-config gmodule-2.0 --libs` -lSDL -lSDL_image

# add files in SRC
SRC = main.c CLImain.c base_structures.c main_pre_processing.c pixel_operations.c pre_processing.c segmentation.c main_lib.c letter_extraction.c ML/network.c ML/neurons.c

OBJ = ${SRC:.c=.o}
DEP = ${SRC:.c=.d}

all: main

main: ${OBJ}

run: all
	./main

.PHONY: clean

clean:
	${RM} ${OBJ}   # remove object files
	${RM} ${DEP}   # remove dependency files
	${RM} main     # remove main program

-include ${DEP}

# END
