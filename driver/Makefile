################################################################################
# Organ project
# Victor Manuel Fernandez Castro
# 29 July 2015
################################################################################
# Syntax: make [all|clean|objclean]
################################################################################
# all:      Compiles the project. [por defecto]
# clean:    Deletes binaries and objects.
# objclean: Deletes objects.
################################################################################

# Macros

BIN	= bin
OBJ	= obj
INC	= include
SRC	= src

CC			= gcc
DEFINES		=
CFLAGS		= -pipe -O2 -Wall -Wextra -pthread $(DEFINES)
INCPATH		= -I$(INC)
LIBS		= 
LDFLAGS		= -Wl,-O1 -Wl,-s -pthread

RM_FILE		= rm -f
RM_DIR		= rm -rf

TARGET		= $(BIN)/midinfo $(BIN)/test

.PHONY: all clean objclean

$(OBJ)/%.o: $(SRC)/%.c
	$(CC) -c $(CFLAGS) $(INCPATH) -o $@ $<

all: $(TARGET)

clean: objclean
	$(RM_FILE) $(TARGET)
	
objclean:
	$(RM_FILE) $(OBJ)/*.o

$(BIN)/midinfo: $(OBJ)/midi.o $(OBJ)/midinfo.o
	$(CC) $(LDFLAGS) -o $@ $^ $(LIBS)

$(BIN)/test: $(OBJ)/midi.o $(OBJ)/test.o $(OBJ)/player.o $(OBJ)/monitor.o
	$(CC) $(LDFLAGS) -o $@ $^ $(LIBS)

$(OBJ)/midi.o: $(INC)/midi.h
$(OBJ)/midinfo.o: $(INC)/midi.h
$(OBJ)/player.o: $(INC)/midi.h $(INC)/output.h
$(OBJ)/test.o: $(INC)/midi.h $(INC)/player.h $(INC)/output.h
