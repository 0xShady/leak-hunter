CC      ?= cc
CFLAGS  ?= -Wall -Wextra -Werror -Wstrict-prototypes

example: test.c leak_hunter.c leak_hunter.h
	$(CC) $(CFLAGS) test.c leak_hunter.c -o example

test:
	CC="$(CC)" CFLAGS="$(CFLAGS)" sh tests/run.sh

clean:
	rm -f example leak.log

.PHONY: test clean
