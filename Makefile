CC = gcc
SRC = $(wildcard *.c)
EXE = $(patsubst %.c, %, $(SRC))

all: ${EXE}

%: %.c
	$(CC) -o $@.out $@.c

clean:
	rm *.out