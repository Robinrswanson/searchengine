# Compiler
CC = g++

# OpenSSL Directory
OPENSSL_DIR = /opt/homebrew/opt/openssl@3

# Compiler Flags
CXXFLAGS = -Wall -std=c++17 -I$(OPENSSL_DIR)/include

# Linker Flags
LDFLAGS = -L$(OPENSSL_DIR)/lib -lssl -lcrypto

# Source Files for Main Application
SRCS = crawler.cpp downloader.cpp parser.cpp

# Object Files for Main Application
OBJS = $(SRCS:.cpp=.o)

# Source Files for Tests
TEST_SRCS = tests/test_parser.cpp parser.cpp

# Object Files for Tests
TEST_OBJS = $(TEST_SRCS:.cpp=.o)

# Target Executables
TARGET = crawler
TEST_TARGET = test_parser

# Default Target: Build both main and test executables
all: $(TARGET) $(TEST_TARGET)

# Build Main Executable
$(TARGET): $(OBJS)
	$(CC) $(CXXFLAGS) -o $(TARGET) $(OBJS) $(LDFLAGS)

# Build Test Executable
$(TEST_TARGET): $(TEST_OBJS)
	$(CC) $(CXXFLAGS) -o $(TEST_TARGET) $(TEST_OBJS) $(LDFLAGS)

# Compile .cpp files to .o files for Main Application
%.o: %.cpp downloader.h parser.h
	$(CC) $(CXXFLAGS) -c $< -o $@

# Compile .cpp files to .o files for Tests
tests/%.o: tests/%.cpp parser.h
	$(CC) $(CXXFLAGS) -c $< -o $@

# Clean up build artifacts
clean:
	rm -f $(OBJS) $(TEST_OBJS) $(TARGET) $(TEST_TARGET)
