CXX := g++
CXXFLAGS := -std=c++17 -Wall -Ofast



SRCDIR = .
OBJDIR = build
BINDIR = ../bin

# Files
TARGET_HW3 = $(BINDIR)/hw3
TARGET_GA = $(BINDIR)/ga
SRC_HW3 = $(SRCDIR)/hw3.cpp
SRC_GA = $(SRCDIR)/ga.cpp
SRC_DATA = $(SRCDIR)/datastr.cpp
OBJ_HW3 = $(OBJDIR)/hw3.o
OBJ_GA = $(OBJDIR)/ga.o
OBJ_DATA = $(OBJDIR)/data.o

# Create directories if they don't exist
$(shell mkdir -p $(OBJDIR) $(BINDIR))

# Default target
all: $(TARGET_HW3) #$(TARGET_GA)

# Link the hw3 executable
$(TARGET_HW3): $(OBJ_HW3) $(OBJ_DATA)
	$(CXX) $(OBJ_HW3) $(OBJ_DATA) -o $(TARGET_HW3)

# Link the ga executable
$(TARGET_GA): $(OBJ_GA) $(OBJ_DATA)
	$(CXX) $(OBJ_GA) $(OBJ_DATA) -o $(TARGET_GA)

# Compile hw3.cpp
$(OBJDIR)/hw3.o: $(SRCDIR)/hw3.cpp $(SRCDIR)/datastr.hpp
	$(CXX) $(CXXFLAGS) -c $(SRCDIR)/hw3.cpp -o $(OBJDIR)/hw3.o

# Compile ga.cpp
$(OBJDIR)/ga.o: $(SRCDIR)/ga.cpp $(SRCDIR)/datastr.hpp
	$(CXX) $(CXXFLAGS) -c $(SRCDIR)/ga.cpp -o $(OBJDIR)/ga.o

# Compile datastr.cpp
$(OBJDIR)/data.o: $(SRCDIR)/datastr.cpp $(SRCDIR)/datastr.hpp
	$(CXX) $(CXXFLAGS) -c $(SRCDIR)/datastr.cpp -o $(OBJDIR)/data.o

# Clean up
clean:
	rm -rf $(OBJDIR) $(TARGET_HW3)
# $(TARGET_GA)

grade:
	./grade.sh
 
grade2:
	./grade2.sh

# Debug: print current paths
debug:
	@echo "Source directory: $(SRCDIR)"
	@echo "Object directory: $(OBJDIR)"
	@echo "Binary directory: $(BINDIR)"
	@echo "Source files for hw3: $(SRC_HW3)"
	@echo "Source files for ga: $(SRC_GA)"
	@echo "Object files: $(OBJ_HW3) $(OBJ_GA) $(OBJ_DATA)"

