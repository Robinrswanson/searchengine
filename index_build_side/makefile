# Compiler
CC = g++

# OpenSSL Directory
OPENSSL_DIR = /opt/homebrew/opt/openssl@3

# Compiler Flags
CXXFLAGS = -Wall -std=c++17 -I$(OPENSSL_DIR)/include

# Linker Flags
LDFLAGS = -L$(OPENSSL_DIR)/lib -lssl -lcrypto

# Source Files
SRCS = crawler.cpp downloader.cpp

# Object Files
OBJS = $(SRCS:.cpp=.o)

# Target Executable
TARGET = crawler

# Default Target
all: $(TARGET)

# Build the target executable by linking all object files
$(TARGET): $(OBJS)
	$(CC) $(CXXFLAGS) -o $(TARGET) $(OBJS) $(LDFLAGS)

# Compile .cpp files to .o files
%.o: %.cpp downloader.h
	$(CC) $(CXXFLAGS) -c $< -o $@

# Clean up build artifacts
clean:
	rm -f $(OBJS) $(TARGET)
