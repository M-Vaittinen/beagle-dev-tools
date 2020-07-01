FOO=$(CC)gcc
CFLAGS=-Wall -O2

all: tm1637

tm1637: tm1637.c tm1637.h main.c
	$(FOO) $(CFLAGS) tm1637.c main.c -o tm1637

clean:
	rm tm1637

