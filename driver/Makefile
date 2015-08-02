################################################################################
# Organ project
# Victor Manuel Fernandez Castro
# 29 July 2015
################################################################################
# Syntax: make [all|install|uninstall|clean|objclean]
################################################################################
# all:      	Compiles the project. [default]
# install:		Installs the project on linux binaries folders.
# uninstall:	Uninstalls the project.
# clean:    	Deletes binaries and objects.
# objclean: 	Deletes objects.
################################################################################

# Macros

BIN		= bin
OBJ		= obj
INC		= include
SRC		= src
SCRIPTS = scripts

INSTALL_BIN		= /usr/bin
INSTALL_SBIN	= /usr/sbin
INSTALL_INIT	= /etc/init.d

CC			= gcc
DEFINES		=
CFLAGS		= -pipe -O2 -Wall -Wextra -pthread $(DEFINES)
INCPATH		= -I$(INC)
LIBS		= 
LDFLAGS		= -Wl,-O1 -Wl,-s -pthread

CP_FILE		= cp
RM_FILE		= rm -f
RM_DIR		= rm -rf


TARGET		= $(BIN)/midinfo $(BIN)/miditest $(BIN)/organd

.PHONY: all install uninstall clean objclean

$(OBJ)/%.o: $(SRC)/%.c
	$(CC) -c $(CFLAGS) $(INCPATH) -o $@ $<

all: $(TARGET)

install: all
	service organ stop 2> /dev/null; true
	$(CP_FILE) $(BIN)/organd $(INSTALL_SBIN)
	$(CP_FILE) $(SCRIPTS)/organ $(INSTALL_INIT)
	update-rc.d organ defaults
	service organ start
	
uninstall:
	service organ stop 2> /dev/null; true
	update-rc.d organ remove
	$(RM_FILE) $(INSTALL_SBIN)/organd
	$(RM_FILE) $(INSTALL_SBIN)/organ
	

clean: objclean
	$(RM_FILE) $(TARGET)
	
objclean:
	$(RM_FILE) $(OBJ)/*.o

$(BIN)/midinfo: $(OBJ)/midi.o $(OBJ)/midinfo.o
	$(CC) $(LDFLAGS) -o $@ $^ $(LIBS)

$(BIN)/miditest: $(OBJ)/midi.o $(OBJ)/test.o $(OBJ)/player.o $(OBJ)/monitor.o
	$(CC) $(LDFLAGS) -o $@ $^ $(LIBS)
	
$(BIN)/organd: $(OBJ)/daemon.o
	$(CC) $(LDFLAGS) -o $@ $^ $(LIBS)

$(OBJ)/midi.o: $(INC)/midi.h
$(OBJ)/midinfo.o: $(INC)/midi.h
$(OBJ)/player.o: $(INC)/midi.h $(INC)/output.h
$(OBJ)/test.o: $(INC)/midi.h $(INC)/player.h $(INC)/output.h
