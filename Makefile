CC = gcc
SRC = $(wildcard *.c)
EXE = $(patsubst %.c, %, $(SRC))

all: ${EXE}

%: %.c
	$(CC) -o $@.out $@.c -lm

clean:
	rm *.out