CC=gcc
SOURCE=calc_coeff.c
CFLAGS=-Wall
TGT=calc_coeff

all: $(TGT)

$(TGT): $(SOURCE)
	$(CC) $(CFLAGS) $(SOURCE) -o $(TGT) -lm
clean:
	rm -rf $(TGT)
