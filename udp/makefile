CC = g++
CFLAGS = -std=c++11
 
all: client.cpp server.cpp
	$(CC) $(CFLAGS) server.cpp   -o server
	$(CC) $(CFLAGS) client.cpp  -o client

clean:
	rm  server client