#!/bin/sh

# Installer for PHP client
# Requires organ driver to be installed
# Victor Manuel Fernandez Castro
# 10 August 2015

PACKAGES="apache2 php5 mysql-server php5-mysqlnd"
SUDOERS="/etc/sudoers"

ORGAN_GROUP="organ"

SITE_SOURCE="organo"
SITE_TARGET="001-organo"
SITE_ROOT="/home/pi/manager"
SITE_POOL="/home/pi/midis"

HTTP_USER="www-data"
HTTP_SUDO="/sbin/shutdown, /usr/bin/organ-login *"

DB_USER="root"
DB_SOURCE="organo.sql"

apt-get install $PACKAGES
adduser $HTTP_USER $ORGAN_GROUP

cp $SITE_SOURCE /etc/apache2/sites-available
ln -s /etc/apache2/sites-available/$SITE_SOURCE /etc/apache2/sites-enabled/$SITE_TARGET
service apache2 reload


if [ -z "$(grep $HTTP_USER $SUDOERS)" ]; then
	cp $SUDOERS $SUDOERS.bak
	echo "$HTTP_USER ALL=(ALL) NOPASSWD: $HTTP_SUDO" >> $SUDOERS
fi

mysql -u$DB_USER -p < $DB_SOURCE

mkdir -p $SITE_ROOT $SITE_ROOT
