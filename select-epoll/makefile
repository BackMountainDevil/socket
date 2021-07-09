CC = g++
CFLAGS = -std=c++11
 
all: select.cpp client.cpp server.cpp
	$(CC) $(CFLAGS) select.cpp -o select
	$(CC) $(CFLAGS) server.cpp   -o server
	$(CC) $(CFLAGS) client.cpp  -o client

clean:
	rm  select server client