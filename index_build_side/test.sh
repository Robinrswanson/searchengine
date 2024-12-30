#!/bin/bash

# Test for the crawler.cpp program

# Compile the C++ file

# Test 1: Run the program with a valid URL
echo "Running test 1: Connecting to https://example.com"
./crawler https://example.com

# Test 2: Run the program with an invalid URL to see error handling
 echo "Running test 2: Connecting to an invalid URL"
 ./crawler https://google.com
