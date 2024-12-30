CC = g++
LDFLAGS += -L/opt/homebrew/opt/openssl/lib -lssl -lcrypto
CXXFLAGS += -I/opt/homebrew/opt/openssl/include

# Target executable
all: crawler

# Build the crawler executable
crawler: crawler.o
	$(CC) $(LDFLAGS) -o crawler crawler.o

# Compile the object file, explicitly dependent on the header file
crawler.o: crawler.cpp downloader.h
	$(CC) $(CXXFLAGS) -c crawler.cpp -o crawler.o

# Clean up build artifacts
clean:
	rm -f *.o crawler
