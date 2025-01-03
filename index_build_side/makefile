# Makefile for Crawler Project

# --------------------------------------------
# Compiler and Tools
# --------------------------------------------
CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++17

# --------------------------------------------
# Directories
# --------------------------------------------
OPENSSL_DIR = /opt/homebrew/opt/openssl@3
INCLUDE_DIR = include
SRC_DIR = src
TEST_DIR = tests

# --------------------------------------------
# Include and Library Paths
# --------------------------------------------
INCLUDE_PATHS = -I$(OPENSSL_DIR)/include -I$(INCLUDE_DIR)
LIBRARY_PATHS = -L$(OPENSSL_DIR)/lib

# --------------------------------------------
# Linker Flags
# --------------------------------------------
LDFLAGS = $(LIBRARY_PATHS) -lssl -lcrypto

# --------------------------------------------
# Source Files
# --------------------------------------------

# Main Application Sources (Including url.cpp)
MAIN_SRCS = $(SRC_DIR)/crawler.cpp \
            $(SRC_DIR)/downloader.cpp \
            $(SRC_DIR)/parser.cpp \
            $(SRC_DIR)/url.cpp

# Test Sources
TEST_DOWNLOADER_SRCS = $(TEST_DIR)/test_downloader.cpp
TEST_PARSER_SRCS = $(TEST_DIR)/test_parser.cpp
TEST_URL_SRCS = $(TEST_DIR)/test_url.cpp

# --------------------------------------------
# Object Files
# --------------------------------------------

# Main Application Objects
MAIN_OBJS = $(MAIN_SRCS:.cpp=.o)

# Test Objects
TEST_DOWNLOADER_OBJS = $(TEST_DOWNLOADER_SRCS:.cpp=.o)
TEST_PARSER_OBJS = $(TEST_PARSER_SRCS:.cpp=.o)
TEST_URL_OBJS = $(TEST_URL_SRCS:.cpp=.o)

# --------------------------------------------
# Targets
# --------------------------------------------

# Executable Names
MAIN_TARGET = crawler
TEST_DOWNLOADER_TARGET = test_downloader
TEST_PARSER_TARGET = test_parser
TEST_URL_TARGET = test_url

# All Targets
all: $(MAIN_TARGET) $(TEST_DOWNLOADER_TARGET) $(TEST_PARSER_TARGET) $(TEST_URL_TARGET)

# --------------------------------------------
# Build Rules
# --------------------------------------------

# Build Main Executable
$(MAIN_TARGET): $(MAIN_OBJS)
	$(CXX) $(CXXFLAGS) $(INCLUDE_PATHS) -o $@ $^ $(LDFLAGS)

# Build Test Downloader Executable
$(TEST_DOWNLOADER_TARGET): $(TEST_DOWNLOADER_OBJS) $(SRC_DIR)/downloader.o $(SRC_DIR)/url.o
	$(CXX) $(CXXFLAGS) $(INCLUDE_PATHS) -o $@ $^ $(LDFLAGS)

# Build Test Parser Executable
$(TEST_PARSER_TARGET): $(TEST_PARSER_OBJS) $(SRC_DIR)/parser.o $(SRC_DIR)/url.o
	$(CXX) $(CXXFLAGS) $(INCLUDE_PATHS) -o $@ $^ $(LDFLAGS)

# Build Test URL Executable
$(TEST_URL_TARGET): $(TEST_URL_OBJS) $(SRC_DIR)/url.o
	$(CXX) $(CXXFLAGS) $(INCLUDE_PATHS) -o $@ $^ $(LDFLAGS)

# --------------------------------------------
# Compilation Rules
# --------------------------------------------

# Pattern rule for compiling source files in src/
$(SRC_DIR)/%.o: $(SRC_DIR)/%.cpp $(INCLUDE_DIR)/*.h
	$(CXX) $(CXXFLAGS) $(INCLUDE_PATHS) -c $< -o $@

# Pattern rule for compiling test_downloader.cpp
$(TEST_DIR)/test_downloader.o: $(TEST_DIR)/test_downloader.cpp $(INCLUDE_DIR)/*.h
	$(CXX) $(CXXFLAGS) $(INCLUDE_PATHS) -c $< -o $@

# Pattern rule for compiling test_parser.cpp
$(TEST_DIR)/test_parser.o: $(TEST_DIR)/test_parser.cpp $(INCLUDE_DIR)/*.h
	$(CXX) $(CXXFLAGS) $(INCLUDE_PATHS) -c $< -o $@

# Pattern rule for compiling test_url.cpp
$(TEST_DIR)/test_url.o: $(TEST_DIR)/test_url.cpp $(INCLUDE_DIR)/*.h
	$(CXX) $(CXXFLAGS) $(INCLUDE_PATHS) -c $< -o $@

# --------------------------------------------
# Phony Targets
# --------------------------------------------
.PHONY: all clean run_tests run_test_downloader run_test_parser run_test_url

# Clean up build artifacts
clean:
	rm -f $(SRC_DIR)/*.o $(TEST_DIR)/*.o $(MAIN_TARGET) $(TEST_DOWNLOADER_TARGET) $(TEST_PARSER_TARGET) $(TEST_URL_TARGET)

# Run all tests
run_tests: all
	@echo "Running test_downloader..."
	./$(TEST_DOWNLOADER_TARGET)
	@echo "Running test_parser..."
	./$(TEST_PARSER_TARGET)
	@echo "Running test_url..."
	./$(TEST_URL_TARGET)

# Individual test targets
run_test_downloader: all
	@echo "Running test_downloader..."
	./$(TEST_DOWNLOADER_TARGET)

run_test_parser: all
	@echo "Running test_parser..."
	./$(TEST_PARSER_TARGET)

run_test_url: all
	@echo "Running test_url..."
	./$(TEST_URL_TARGET)
