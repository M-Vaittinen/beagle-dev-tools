# bbb-MVaLePyPin top-level Makefile

CC      ?= gcc
CROSS_COMPILE ?=
CC      := $(CROSS_COMPILE)$(CC)

# No external library dependencies — only POSIX (termios, unistd, etc.)
CFLAGS  := -Wall -Wextra -Wpedantic -std=c11 -D_GNU_SOURCE -O2
LDFLAGS :=

SRCDIR  := src
SRCS    := $(SRCDIR)/main.c \
           $(SRCDIR)/pindata.c \
           $(SRCDIR)/pinmux.c \
           $(SRCDIR)/gpio.c \
           $(SRCDIR)/detect.c \
           $(SRCDIR)/hwaccess.c \
           $(SRCDIR)/miniline.c

OBJS    := $(SRCS:.c=.o)
TARGET  := bbb-MVaLePyPin

.PHONY: all clean kmodule kmodule-clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

$(SRCDIR)/%.o: $(SRCDIR)/%.c
	$(CC) $(CFLAGS) -I$(SRCDIR) -c -o $@ $<

clean:
	rm -f $(OBJS) $(TARGET)

kmodule:
	$(MAKE) -C kmodule

kmodule-clean:
	$(MAKE) -C kmodule clean

# Cross-compile example:
#   make CROSS_COMPILE=arm-linux-gnueabihf-
#
# For BBB (native build), just run:
#   make
# then copy bbb-MVaLePyPin to the board.
