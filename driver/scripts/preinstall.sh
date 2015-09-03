#!/bin/sh
# Preinstalling script
# Victor Manuel Fernandez Castro
# 4 August 2015

USER='organ'
PACKAGES='libmysqlclient-dev'

HOST_USER="pi"

BIN=$(dirname $0)/../bin
OBJ=$(dirname $0)/../obj

SCRIPT=$(dirname $0)/organ
WIRINGPI=$(dirname $0)/wiringPi
WIRINGPI_TAR=$(dirname $0)/wiringPi.tar.gz

apt-get -y install $PACKAGES

mkdir -p $BIN $OBJ
chown $HOST_USER:$HOST_USER $BIN $OBJ
chmod a+x $SCRIPT

adduser --quiet --group --system --no-create-home $USER
adduser $USER $USER

tar -xf $WIRINGPI_TAR -C $(dirname $0)
chown -R $HOST_USER:$HOST_USER $WIRINGPI
cd $WIRINGPI
./build
