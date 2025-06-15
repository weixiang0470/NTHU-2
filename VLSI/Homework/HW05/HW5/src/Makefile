CXX := g++
CXXFLAGS := -std=c++17 -Wall -Ofast

SRCDIR := .
BINDIR := ../bin
INCLUDES := -I.

# Files
SRCS := $(wildcard $(SRCDIR)/*.cpp)
OBJS := $(SRCS:.cpp=.o)

TARGET := $(BINDIR)/hw5
# SRCS := main.cpp
# OBJS := $(SRCS:.cpp=.o)

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

%.o: %.cpp parser.hpp row.hpp cell.hpp subrow.hpp cluster.hpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

grade:
	./grade.sh
	
clean:
	rm -rf $(OBJS) $(TARGET)