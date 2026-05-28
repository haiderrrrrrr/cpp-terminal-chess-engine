CXX ?= g++
CXXFLAGS ?= -std=c++17 -Wall -Wextra -O2
TARGET := build/terminal-chess-engine
SOURCE := main.cpp

.PHONY: all run clean

all: $(TARGET)

$(TARGET): $(SOURCE)
	mkdir -p build
	$(CXX) $(CXXFLAGS) $(SOURCE) -o $(TARGET)

run: all
	./$(TARGET)

clean:
	rm -rf build
