#!/bin/sh
# Preinstalling script
# Victor Manuel Fernandez Castro
# 4 August 2015

USER='organ'
PACKAGES='libmysqlclient-dev'

apt-get install $PACKAGES

mkdir -p $(dirname $0)/../obj $(dirname $0)/../bin
chown pi:pi $(dirname $0)/../obj $(dirname $0)/../bin
chmod a+x $(dirname $0)/organ

adduser --quiet --group --system --no-create-home $USER
adduser $USER $USER
