#!/bin/sh
# Preinstalling script
# Victor Manuel Fernandez Castro
# 4 August 2015

USER='organ'
PACKAGES='git libmysqlclient-dev'

SUDOERS="/etc/sudoers"
USER_SUDO="/sbin/shutdown, /usr/local/bin/organ-login"

HOST_USER="pi"

BIN=$(dirname $0)/../bin
OBJ=$(dirname $0)/../obj

SCRIPT=$(dirname $0)/organ
WIRINGPI=$(dirname $0)/wiringPi
WIRINGPI_GIT="git://git.drogon.net/wiringPi"

################################################################################

apt-get -y install $PACKAGES

################################################################################

mkdir -p $BIN $OBJ
chown $HOST_USER:$HOST_USER $BIN $OBJ
chmod a+x $SCRIPT

################################################################################

adduser --quiet --group --system --no-create-home $USER
adduser $USER $USER
adduser $USER dialout

################################################################################

cp $SUDOERS $(dirname $0)/sudoers.bak

if [ -z "$(grep ^%$USER $SUDOERS)" ]; then
	echo "%$USER ALL=(ALL) NOPASSWD: $USER_SUDO" >> $SUDOERS
else
	USER_SUDO_SED=$(echo "$USER_SUDO" | sed 's/\//\\\//g')
	sed "s/^%$USER.*/%$USER ALL=(ALL) NOPASSWD: $USER_SUDO_SED/g" $(dirname $0)/sudoers.bak > $SUDOERS
fi

################################################################################

if [ ! -d $WIRINGPI ]; then
	git clone $WIRINGPI_GIT $WIRINGPI
	chown -R $HOST_USER:$HOST_USER $WIRINGPI
fi

cd $WIRINGPI
./build
