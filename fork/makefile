CC = g++
CFLAGS = -std=c++11
C = gcc

cs: client.cpp server.cpp mpclient.cpp
	$(CC) $(CFLAGS) server.cpp   -o server
	$(CC) $(CFLAGS) client.cpp  -o client
	$(CC) $(CFLAGS) mpclient.cpp  -o mpclient

all: zombie.c fork.c wait.c waitpid.c signal.c sigaction.c zombie-sig.c client.cpp server.cpp
	$(C) fork.c   -o fork
	$(C) zombie.c  -o zombie
	$(C) wait.c -o wait
	$(C) waitpid.c -o waitpid
	$(C) signal.c -o signal
	$(C) sigaction.c -o sigaction
	$(C) zombie-sig.c -o zombie-sig
	$(CC) $(CFLAGS) server.cpp   -o server
	$(CC) $(CFLAGS) client.cpp  -o client

clean:
	rm  fork zombie wait waitpid signal sigaction zombie-sig server client mpclient