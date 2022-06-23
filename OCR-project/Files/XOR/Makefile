# Simple Makefile
 
# Compilers vars
CC=gcc
CPPFLAGS= -MMD
CFLAGS= -Wall -Wextra -Werror -std=c99 -O -lm -o 
LDFLAGS=
LDLIBS=
 
SRC= xor.c
DEP= ${SRC:.c=.d}
PRG= ${SRC:.c=}
 
all: ${PRG}
 
-include ${DEP}
 
clean:
	rm -f *.o
	rm -f ${DEP}
	rm -f ${PRG}
 
# END Makefile
