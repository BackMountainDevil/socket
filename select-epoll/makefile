CC = g++
CFLAGS = -std=c++11
 
all: select.cpp client.cpp server-select.cpp
	$(CC) $(CFLAGS) select.cpp -o select
	$(CC) $(CFLAGS) server-select.cpp   -o server-select
	$(CC) $(CFLAGS) client.cpp  -o client

clean:
	rm  select server-select client