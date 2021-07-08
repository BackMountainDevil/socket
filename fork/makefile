CC = g++
CFLAGS = -std=c++11
C = gcc

all: zombie.c fork.c wait.c
	$(C) fork.c   -o fork
	$(C) zombie.c  -o zombie
	$(C) wait.c -o wait

clean:
	rm  fork zombie wait