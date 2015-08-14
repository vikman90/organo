#!/bin/sh
# Preinstalling script
# Victor Manuel Fernandez Castro
# 4 August 2015

USER='organ'
CONFIG_DIR='/etc/organ'
CONFIG_FILE='remote.conf'

mkdir -p obj bin
chmod a+x organ remote

adduser --quiet --group --system --no-create-home $USER
adduser $USER $USER

mkdir -p $CONFIG_DIR
touch $CONFIG_DIR/$CONFIG_FILE
chmod 664 $CONFIG_DIR/$CONFIG_FILE
chown $USER:$USER $CONFIG_DIR/$CONFIG_FILE
