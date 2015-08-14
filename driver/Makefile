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

INSTALL_BIN		= /usr/bin
INSTALL_SBIN	= /usr/sbin
INSTALL_INIT	= /etc/init.d

CC			= gcc
DEFINES		=
CFLAGS		= -pipe -O2 -Wall -Wextra -pthread $(DEFINES)
INCPATH		= -I$(INC)
LIBS		= 
LDFLAGS		= -O1 -s -pthread

CP_FILE		= cp
RM_FILE		= rm -f
RM_DIR		= rm -rf

TARGET		= $(BIN)/midinfo $(BIN)/organd $(BIN)/remoted $(BIN)/test $(BIN)/login $(BIN)/organ

INSTALLS	= $(INSTALL_INIT)/organ $(INSTALL_INIT)/remote \
				$(INSTALL_SBIN)/organd $(INSTALL_SBIN)/remoted \
				$(INSTALL_BIN)/organ-login $(INSTALL_BIN)/organ $(INSTALL_BIN)/organ-midinfo

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

$(INSTALL_SBIN)/remoted: $(BIN)/remoted
	service remote stop 2> /dev/null; true
	$(CP_FILE) $< $@
	insserv remote
	service remote start

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
	service remote stop 2> /dev/null; true
	insserv -r organ
	insserv -r remote
	$(RM_FILE) $(INSTALLS)

preinstall:
	$(SCRIPTS)/preinstall.sh

clean: objclean
	$(RM_FILE) $(TARGET)

objclean:
	$(RM_FILE) $(OBJ)/*.o

$(BIN)/login: $(OBJ)/login.o
	$(CC) $(LDFLAGS) -o $@ $^ $(LIBS) -lcrypt

$(BIN)/midinfo: $(OBJ)/midi.o $(OBJ)/midinfo.o
	$(CC) $(LDFLAGS) -o $@ $^ $(LIBS)

$(BIN)/organ: $(OBJ)/organ.o
	$(CC) $(LDFLAGS) -o $@ $^ $(LIBS)

$(BIN)/organd: $(OBJ)/gpio.o $(OBJ)/midi.o $(OBJ)/organd.o $(OBJ)/player.o $(OBJ)/socket.o
	$(CC) $(LDFLAGS) -o $@ $^ $(LIBS)

$(BIN)/remoted: $(OBJ)/remoted.o $(OBJ)/uart.o
	$(CC) $(LDFLAGS) -o $@ $^ $(LIBS)

$(BIN)/test: $(OBJ)/test.o $(OBJ)/midi.o $(OBJ)/monitor.o $(OBJ)/player.o
	$(CC) $(LDFLAGS) -o $@ $^ $(LIBS)

$(OBJ)/midi.o: $(INC)/midi.h
$(OBJ)/midinfo.o: $(INC)/midi.h
$(OBJ)/organd.o:  $(INC)/output.h $(INC)/player.h $(INC)/socket.h
$(OBJ)/player.o: $(INC)/midi.h $(INC)/output.h $(INC)/player.h
$(OBJ)/remoted.o: $(INC)/uart.h
$(OBJ)/socket.o: $(INC)/player.h
$(OBJ)/test.o: $(INC)/output.h $(INC)/player.h
