CC = g++
CFLAGS = -std=c++11

thread: 1thread.cpp
	$(CC) $(CFLAGS) -D_REENTRANT 1thread.cpp -o 1thread -lpthread

clean:
	rm  1thread