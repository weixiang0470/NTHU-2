CXX := g++
CXXFLAGS := -std=c++17 -Wall -Ofast



SRCDIR = .
BINDIR = ../bin

# Files
TARGET := $(BINDIR)/hw4
SRCS := hw4.cpp
OBJS := $(SRCS:.cpp=.o)

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

%.o: %.cpp inputparser.hpp
	$(CXX) $(CXXFLAGS) -c $< -o $@


grade:
	./grade.sh
	
clean:
	rm -rf $(OBJS) $(TARGET)
