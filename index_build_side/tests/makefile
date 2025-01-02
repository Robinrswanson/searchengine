# Compiler and flags
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -I/opt/homebrew/opt/openssl/include
LDFLAGS = -L/opt/homebrew/opt/openssl/lib -lssl -lcrypto

# Targets
TARGETS = test_downloader test_parser
TEST_DOWNLOADER_SRC = test_downloader.cpp ../downloader.cpp
TEST_PARSER_SRC = test_parser.cpp ../parser.cpp

# Default target: build all test executables
all: $(TARGETS)

# Build the test_downloader executable
test_downloader: $(TEST_DOWNLOADER_SRC)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

# Build the test_parser executable
test_parser: $(TEST_PARSER_SRC)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

# Clean up build files
clean:
	rm -f $(TARGETS)

# Run the tests
run: all
	@echo "Running test_downloader..."
	./test_downloader
	@echo "Running test_parser..."
	./test_parser
