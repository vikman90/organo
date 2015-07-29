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
CFLAGS		= -pipe -O2 -Wall -Wextra $(DEFINES)
INCPATH		= -I$(INC)
LIBS		= 
LDFLAGS		= -Wl,-O1 -Wl,-s

RM_FILE		= rm -f
RM_DIR		= rm -rf

TARGET		= $(BIN)/player
OBJECTS		= $(OBJ)/midi.o $(OBJ)/player.o

.PHONY: all clean objclean

$(OBJ)/%.o: $(SRC)/%.c
	$(CC) -c $(CFLAGS) $(INCPATH) -o $@ $<

all: $(TARGET)

clean: objclean
	$(RM_FILE) $(TARGET)
	
objclean:
	$(RM_FILE) $(OBJECTS)

$(TARGET): $(OBJECTS)
	$(CC) $(LDFLAGS) -o $@ $^ $(LIBS)

$(OBJ)/midi.o: $(INC)/midi.h
$(OBJ)/player.o: $(INC)/midi.h
