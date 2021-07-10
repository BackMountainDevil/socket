CC = g++
CFLAGS = -std=c++11

cs: client.cpp server-thread.cpp
	$(CC) $(CFLAGS) -D_REENTRANT server-thread.cpp -o server-thread -lpthread
	$(CC) $(CFLAGS) -D_REENTRANT client.cpp -o client -lpthread

thread: 1thread.cpp 2thread-mutex.cpp
	$(CC) $(CFLAGS) -D_REENTRANT 1thread.cpp -o 1thread -lpthread
	$(CC) $(CFLAGS) -D_REENTRANT 2thread-mutex.cpp  -o 2thread-mutex  -lpthread

clean:
	rm  1thread 2thread-mutex client server-thread