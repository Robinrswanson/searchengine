CC = g++
CFLAGS = -Wall -Iinclude
LDFLAGS = -Llib -lssl -lcrypto

all: crawler

crawler: crawler.o
	$(CC) -o crawler crawler.o

%.o: %.cpp
	$(CC) -c $< -o $@

clean:
	rm -f *.o crawler
