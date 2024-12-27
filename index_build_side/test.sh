#!/bin/bash

# Test for the crawler.cpp program

# Compile the C++ file
echo "Compiling crawler.cpp..."
g++ -std=c++11 -o crawler crawler.cpp

# Test 1: Run the program with a valid URL
echo "Running test 1: Connecting to http://example.com"
./crawler http://example.com

# Test 2: Run the program with another valid URL
echo "Running test 2: Connecting to http://www.google.com"
./crawler http://www.google.com

# Test 3: Run the program with an invalid URL to see error handling
echo "Running test 3: Connecting to an invalid URL"
./crawler http://nonexistentwebsite.com
