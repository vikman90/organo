#!/bin/sh
# Preinstalling script
# Victor Manuel Fernandez Castro
# 4 August 2015

INSTALL_BIN='apt-get'
SUDOERS_FILE='/etc/sudoers'

PACKAGES='libmysqlclient-dev'
GRANTS="www-data ALL=(ALL) NOPASSWD: /sbin/shutdown, /usr/bin/organ-login *"

$INSTALL_BIN install $PACKAGES
cp $SUDOERS_FILE $SUDOERS_FILE.bak

line=$(grep -n "^www-data" $SUDOERS_FILE | cut -d':' -f1)

if [ "$line" != '' ]; then
	sed -i $line'd' $SUDOERS_FILE
fi

echo "$GRANTS" >> $SUDOERS_FILE
