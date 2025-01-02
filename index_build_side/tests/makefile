# Compiler and flags
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -I/opt/homebrew/opt/openssl/include
LDFLAGS = -L/opt/homebrew/opt/openssl/lib -lssl -lcrypto

# Source files
TEST_DOWNLOADER_SRC = test_downloader.cpp ../downloader.cpp
TEST_PARSER_SRC = test_parser.cpp ../parser.cpp

# Object files
TEST_DOWNLOADER_OBJ = $(TEST_DOWNLOADER_SRC:.cpp=.o)
TEST_PARSER_OBJ = $(TEST_PARSER_SRC:.cpp=.o)

# Targets
TARGETS = test_downloader test_parser

# Default target: build all test executables
all: $(TARGETS)

# Build the test_downloader executable
test_downloader: $(TEST_DOWNLOADER_OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

# Build the test_parser executable
test_parser: $(TEST_PARSER_OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

# Compile individual object files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean up build files
clean:
	rm -f $(TEST_DOWNLOADER_OBJ) $(TEST_PARSER_OBJ) $(TARGETS)

# Run the tests
run: all
	@echo "Running test_downloader..."
	./test_downloader
	@echo "Running test_parser..."
	./test_parser
