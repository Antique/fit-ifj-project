PRJ=ifj09
#
PROGS=$main $(PRJ)comp
CC=gcc
CFLAGS=-Wall -std=c99 -pedantic -lm -O3


$(PRJ): main.c lex.c string.c hash.c stack.c parser.c interpret.c
	$(CC) $(CFLAGS) -o $@ main.c lex.c string.c hash.c stack.c parser.c interpret.c


clean:
	rm -f *.o *.out *.exe $(PROGS)
