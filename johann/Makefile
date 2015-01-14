# 14 Enero 2015

BIN = bin
OBJ = obj
SRC = src

CXX = g++
CXXFLAGS = -pipe -O2 -Wall -Wextra -I
LFLAGS = -O2 -s

TARGET = johann

OBJECTS = $(OBJ)/main.o

.PHONY: all

$(OBJ)/%.o: $(SRC)/%.cpp
	$(CXX) $(CXXFLAGS) -o $@ $<

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) $(LFLAGS) -o $@ $^

