#!/bin/sh
################################################################################
# Installer for PHP client
# Requires organ driver to be installed
# Victor Manuel Fernandez Castro
# 10 August 2015
################################################################################

PACKAGES="apache2 php5 mysql-server php5-mysqlnd"
SUDOERS="/etc/sudoers"

HOST_USER="pi"
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

apt-get -y install $PACKAGES
adduser $HTTP_USER $ORGAN_GROUP

################################################################################

SITE_ROOT_SED=$(echo $SITE_ROOT | sed 's/\//\\\//g')
SITE_POOL_SED=$(echo $SITE_POOL | sed 's/\//\\\//g')

sed -i "s/ServerName.*/ServerName $SITE_NAME/g" $(dirname $0)/$SITE_SOURCE
sed -i "s/Define root.*/Define root $SITE_ROOT_SED/g" $(dirname $0)/$SITE_SOURCE
sed -i "s/Alias \/pool.*/Alias pool $SITE_POOL_SED/g" $(dirname $0)/$SITE_SOURCE

cp $(dirname $0)/$SITE_SOURCE /etc/apache2/sites-available
ln -s /etc/apache2/sites-available/$SITE_SOURCE /etc/apache2/sites-enabled/$SITE_TARGET
service apache2 reload

################################################################################

cp $SUDOERS $(dirname $0)/sudoers.bak

if [ -z "$(grep $HTTP_USER $SUDOERS)" ]; then
	echo "$HTTP_USER ALL=(ALL) NOPASSWD: $HTTP_SUDO" >> $SUDOERS
else
	HTTP_SUDO_SED=$(echo "$HTTP_SUDO" | sed 's/\//\\\//g')
	sed "s/$HTTP_USER.*/$HTTP_USER ALL=(ALL) NOPASSWD: $HTTP_SUDO_SED/g" $(dirname $0)/sudoers.bak > $SUDOERS
fi

################################################################################

mysql -u$DB_USER -p < $(dirname $0)/$DB_SOURCE
mkdir -p $SITE_ROOT $SITE_POOL
chown $HOST_USER:$HOST_USER $SITE_ROOT $SITE_POOL