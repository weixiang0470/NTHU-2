CXX = g++
CXXFLAGS = -O2 -Wall -Wextra -std=c++17

TARGET ?= project1  # 允许外部传入可执行文件名
SRCS = main.cpp

$(TARGET): $(SRCS)
	$(CXX) $(CXXFLAGS) -o $@ $^

clean:
	rm -f $(TARGET)
