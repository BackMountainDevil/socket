CC = g++
CFLAGS = -std=c++11
C = gcc

all: zombie.c fork.c wait.c waitpid.c signal.c sigaction.c zombie-sig.c
	$(C) fork.c   -o fork
	$(C) zombie.c  -o zombie
	$(C) wait.c -o wait
	$(C) waitpid.c -o waitpid
	$(C) signal.c -o signal
	$(C) sigaction.c -o sigaction
	$(C) zombie-sig.c -o zombie-sig

clean:
	rm  fork zombie wait waitpid signal sigaction zombie-sig