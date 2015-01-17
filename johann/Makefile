# 14 Enero 2015

BIN = bin
OBJ = obj
SRC = src
INC = include

CXX = g++
CXXFLAGS = -pipe -O2 -Wall -Wextra -I$(INC)
LFLAGS = -O2 -s

TARGET = johann

OBJECTS = $(OBJ)/main.o $(OBJ)/midifile.o

.PHONY: all

$(OBJ)/%.o: $(SRC)/%.cpp
	$(CXX) -c $(CXXFLAGS) -o $@ $<

all: $(BIN)/$(TARGET)

$(BIN)/$(TARGET): $(OBJECTS)
	$(CXX) $(LFLAGS) -o $@ $^

$(SRC)/main.cpp: $(INC)/midifile.h
$(SRC)/midifile.cpp: $(INC)/midifile.h
