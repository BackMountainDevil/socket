CC = g++
CFLAGS = -std=c++11
 
all: select.cpp client.cpp server-select.cpp server-epoll.cpp server-epollet.cpp
	$(CC) $(CFLAGS) select.cpp -o select
	$(CC) $(CFLAGS) server-select.cpp   -o server-select
	$(CC) $(CFLAGS) server-epoll.cpp   -o server-epoll
	$(CC) $(CFLAGS) client.cpp  -o client
	$(CC) $(CFLAGS) server-epollet.cpp -o server-epollet

clean:
	rm  select server-select client server-epoll server-epollet