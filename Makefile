# Makefile for Advanced Student Management System
# Supports Linux/macOS and Windows (MinGW)

CXX      = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -Wpedantic -O2
TARGET   = student_mgmt
SRCS     = main.cpp student.cpp ui.cpp
OBJS     = $(SRCS:.cpp=.o)

# Default build
all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Run target
run: all
	./$(TARGET)

# Clean build artifacts
clean:
	rm -f $(OBJS) $(TARGET)

# Clean including data file (WARNING: deletes saved records)
clean-all: clean
	rm -f students.csv

# Rebuild from scratch
rebuild: clean all

.PHONY: all run clean clean-all rebuild
