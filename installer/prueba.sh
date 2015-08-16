#!/bin/sh
################################################################################
# Installer for PHP client
# Requires organ driver to be installed
# Victor Manuel Fernandez Castro
# 10 August 2015
################################################################################

PACKAGES="apache2 php5 mysql-server php5-mysqlnd"
SUDOERS="/etc/sudoers"

ORGAN_GROUP="organ"

SITE_SOURCE="organo"
SITE_TARGET="001-organo"
SITE_NAME="organo.ddns.net"
SITE_ROOT="/home/pi/manager"
SITE_POOL="/home/pi/midis"

HTTP_USER="www-data"
HTTP_SUDO="/sbin/shutdown, /usr/bin/organ-login *"

DB_USER="root"
DB_SOURCE="organo.sql"

################################################################################


################################################################################

cp $SUDOERS $(dirname $0)/sudoers.bak

if [ -z "$(grep $HTTP_USER $SUDOERS)" ]; then
	echo "$HTTP_USER ALL=(ALL) NOPASSWD: $HTTP_SUDO" >> $SUDOERS
else
	HTTP_SUDO_SED=$(echo "$HTTP_SUDO" | sed 's/\//\\\//g')
	sed "s/$HTTP_USER.*/$HTTP_USER ALL=(ALL) NOPASSWD: $HTTP_SUDO_SED/g" $(dirname $0)/sudoers.bak > sudoers.fix
fi

################################################################################



################################################################################

