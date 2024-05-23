CC=gcc

all: PC1 PC2 PC3

PC1: Clean
	$(CC) -Wall -o $@ PC1.c Liste.c

PC2: Clean
	$(CC) -Wall -o $@ PC2.c Liste.c

PC3: Clean
	$(CC) -Wall -o $@ PC3.c Liste.c

Clean:
	rm -f PC1; rm -f PC2; rm -f PC3


