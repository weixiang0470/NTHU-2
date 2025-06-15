CC = g++
CFLAGS = -Wall -std=c++17 -Ofast -pthread
SOURCES = ids.cpp hc.cpp ga.cpp ga2.cpp test.cpp
EXECUTABLE = $(SOURCES:%.cpp=%)

all: $(EXECUTABLE)

%: %.cpp
	$(CC) $(CFLAGS) -o $@ $<

clean:
	rm -f $(EXECUTABLE)