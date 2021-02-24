CXX = g++
CXXFLAGS = -Wall -Wextra -g -std=c++14

SRC = main.cpp

EXE = $(SRC:.cpp=.x)

.SUFFIXES:
SUFFIXES =

.SUFFIXES: .cpp .x

all: $(EXE)

.PHONY: all

%.x: %.cpp
	$(CXX) $< -o $@ $(CXXFLAGS)

clean:
	rm -f $(EXE) *~

.PHONY: clean