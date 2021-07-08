CC = g++
CFLAGS = -std=c++11
C = gcc

all: zombie.c fork.c wait.c waitpid.c signal.c
	$(C) fork.c   -o fork
	$(C) zombie.c  -o zombie
	$(C) wait.c -o wait
	$(C) waitpid.c -o waitpid
	$(C) signal.c -o signal

clean:
	rm  fork zombie wait waitpid signal