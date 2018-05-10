CC=gcc
OPTIONS=-O2

all: grnn gerador

grnn: grnn.c
	$(CC) grnn.c -lm $(OPTIONS) -o $@

gerador: gerador.c
	$(CC) gerador.c -lm -fopenmp $(OPTIONS) -o $@

clean:
	rm grnn gerador
