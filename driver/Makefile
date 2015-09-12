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
# preinstall:	Prepares the system for installing the project.
# clean:    	Deletes binaries and objects.
# objclean: 	Deletes objects.
################################################################################

# Macros

BIN		= bin
OBJ		= obj
INC		= include
SRC		= src
SCRIPTS = scripts

INSTALL_BIN		= /usr/local/bin
INSTALL_SBIN	= /usr/local/sbin
INSTALL_INIT	= /etc/init.d

CC			= gcc
DEFINES		=
CFLAGS		= -pipe -O2 -Wall -Wextra -pthread $(DEFINES)
INCPATH		= -I$(INC)
LIBS		= -lmysqlclient -lwiringPi -lwiringPiDev -lrt
LDFLAGS		= -O1 -s -pthread

CP_FILE		= cp
RM_FILE		= rm -f
RM_DIR		= rm -rf

TARGET		= $(BIN)/midinfo $(BIN)/organd $(BIN)/test $(BIN)/login $(BIN)/organ

INSTALLS	= $(INSTALL_INIT)/organ $(INSTALL_SBIN)/organd $(INSTALL_BIN)/organ-login $(INSTALL_BIN)/organ $(INSTALL_BIN)/organ-midinfo

.PHONY: all install uninstall preinstall clean objclean

$(OBJ)/%.o: $(SRC)/%.c
	$(CC) -c $(CFLAGS) $(INCPATH) -o $@ $<

all: $(TARGET)

install: $(INSTALLS)

$(INSTALL_SBIN)/organd: $(BIN)/organd
	service organ stop 2> /dev/null; true
	$(CP_FILE) $< $@
	insserv organ
	service organ start

$(INSTALL_INIT)/organ: $(SCRIPTS)/organ
	$(CP_FILE) $< $@

$(INSTALL_INIT)/remote: $(SCRIPTS)/remote
	$(CP_FILE) $< $@

$(INSTALL_BIN)/organ-login: $(BIN)/login
	$(CP_FILE) $< $@

$(INSTALL_BIN)/organ: $(BIN)/organ
	$(CP_FILE) $< $@

$(INSTALL_BIN)/organ-midinfo: $(BIN)/midinfo
	$(CP_FILE) $< $@

uninstall:
	service organ stop 2> /dev/null; true
	insserv -r organ
	$(RM_FILE) $(INSTALLS)

preinstall:
	chmod a+x $(SCRIPTS)/preinstall.sh
	$(SCRIPTS)/preinstall.sh

clean: objclean
	$(RM_FILE) $(TARGET)

objclean:
	$(RM_FILE) $(OBJ)/*.o

$(BIN)/login: $(OBJ)/login.o
	$(CC) $(LDFLAGS) -o $@ $^ -lcrypt

$(BIN)/midinfo: $(OBJ)/midi.o $(OBJ)/midinfo.o
	$(CC) $(LDFLAGS) -o $@ $^ -lm

$(BIN)/organ: $(OBJ)/organ.o
	$(CC) $(LDFLAGS) -o $@ $^

$(BIN)/organd: $(OBJ)/database.o $(OBJ)/gpio.o $(OBJ)/midi.o $(OBJ)/organd.o $(OBJ)/peripherals.o $(OBJ)/player.o $(OBJ)/socket.o $(OBJ)/uart.o
	$(CC) $(LDFLAGS) -o $@ $^ $(LIBS)

$(BIN)/test: $(OBJ)/test.o $(OBJ)/midi.o $(OBJ)/monitor.o $(OBJ)/player.o
	$(CC) $(LDFLAGS) -o $@ $^

$(OBJ)/database.o: $(INC)/values.h
$(OBJ)/gpio.o: $(INC)/values.h
$(OBJ)/midi.o: $(INC)/midi.h $(INC)/values.h
$(OBJ)/midinfo.o: $(INC)/midi.h
$(OBJ)/organd.o:  $(INC)/output.h $(INC)/peripherals.h $(INC)/player.h $(INC)/socket.h $(INC)/uart.h $(INC)/values.h
$(OBJ)/peripherals.o: $(INC)/output.h $(INC)/player.h $(INC)/values.h
$(OBJ)/player.o: $(INC)/midi.h $(INC)/output.h $(INC)/player.h
$(OBJ)/socket.o: $(INC)/player.h $(INC)/values.h
$(OBJ)/test.o: $(INC)/output.h $(INC)/player.h
$(OBJ)/uart.o: $(INC)/database.h $(INC)/player.h $(INC)/values.h
