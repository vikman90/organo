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
INCPATH		= -I$(INC) -I/usr/include/mysql
LIBS		= -L/usr/lib/arm-linux-gnueabihf
LDFLAGS		= -O1 -s -pthread

CP_FILE		= cp
RM_FILE		= rm -f
RM_DIR		= rm -rf

TARGET		= $(BIN)/midinfo $(BIN)/organd $(BIN)/test $(BIN)/login
INSTALLS	= $(INSTALL_SBIN)/organd $(INSTALL_INIT)/organ $(INSTALL_BIN)/organ-login

.PHONY: all install uninstall clean objclean

$(OBJ)/%.o: $(SRC)/%.c
	$(CC) -c $(CFLAGS) $(INCPATH) -o $@ $<

all: $(TARGET)

install: $(INSTALLS)
	update-rc.d organ defaults
	service organ start

$(INSTALL_SBIN)/organd: $(BIN)/organd
	service organ stop 2> /dev/null; true
	$(CP_FILE) $< $@
	
$(INSTALL_INIT)/organ: $(SCRIPTS)/organ
	$(CP_FILE) $< $@

$(INSTALL_BIN)/organ-login: $(BIN)/login
	$(CP_FILE) $< $@

uninstall:
	service organ stop 2> /dev/null; true
	update-rc.d organ remove
	$(RM_FILE) $(INSTALLS)

clean: objclean
	$(RM_FILE) $(TARGET)

objclean:
	$(RM_FILE) $(OBJ)/*.o

$(BIN)/login: $(OBJ)/login.o
	$(CC) $(LDFLAGS) -o $@ $^ $(LIBS) -lcrypt

$(BIN)/midinfo: $(OBJ)/midi.o $(OBJ)/midinfo.o
	$(CC) $(LDFLAGS) -o $@ $^ $(LIBS)

$(BIN)/organd: $(OBJ)/database.o $(OBJ)/gpio.o $(OBJ)/midi.o $(OBJ)/organd.o $(OBJ)/player.o
	$(CC) $(LDFLAGS) -o $@ $^ $(LIBS) -lmysqlclient

$(BIN)/test: $(OBJ)/database.o $(OBJ)/test.o $(OBJ)/midi.o $(OBJ)/monitor.o $(OBJ)/player.o
	$(CC) $(LDFLAGS) -o $@ $^ $(LIBS)

$(OBJ)/database.o: $(INC)/database.h
$(OBJ)/midi.o: $(INC)/midi.h
$(OBJ)/midinfo.o: $(INC)/midi.h
$(OBJ)/organd.o: $(INC)/database.h $(INC)/output.h $(INC)/player.h
$(OBJ)/player.o: $(INC)/midi.h $(INC)/output.h $(INC)/player.h
$(OBJ)/test.o: $(INC)/database.h $(INC)/output.h $(INC)/player.h
