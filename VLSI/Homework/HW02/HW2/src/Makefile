CC = g++
CFLAGS = -Wall -w -std=c++17 -Ofast -fopenmp
BIN_DIR = ../bin
SOURCES = hw2_parallel.cpp hw2.cpp hw2_map.cpp hw2_map_parallel.cpp 
EXECUTABLE = $(SOURCES:%.cpp=$(BIN_DIR)/%)

THREADS ?= 8

all: $(EXECUTABLE)

$(BIN_DIR)/%: %.cpp
	$(CC) $(CFLAGS) -o $@ $<

clean:
	rm -rf $(EXECUTABLE)

