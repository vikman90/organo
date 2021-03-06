#!/bin/sh
################################################################################
# Installer for PHP client
# Requires organ driver to be installed
# Victor Manuel Fernandez Castro
# 10 August 2015
################################################################################

PACKAGES="apache2 php5 mysql-server php5-mysqlnd"

HOST_USER="pi"
ORGAN_GROUP="organ"

SITE_SOURCE="organo"
SITE_TARGET="001-organo"
SITE_NAME="localhost"
SITE_ROOT="/home/pi/manager"
SITE_POOL="/home/pi/midis"

HTTP_USER="www-data"

DB_USER="root"
DB_SOURCE="organo.sql"

################################################################################

apt-get -y install $PACKAGES
adduser $HTTP_USER $ORGAN_GROUP

################################################################################

SITE_ROOT_SED=$(echo $SITE_ROOT | sed 's/\//\\\//g')
SITE_POOL_SED=$(echo $SITE_POOL | sed 's/\//\\\//g')

sed -i "s/ServerName.*/ServerName $SITE_NAME/g" $(dirname $0)/$SITE_SOURCE
sed -i "s/DocumentRoot.*/DocumentRoot $SITE_ROOT_SED/g" $(dirname $0)/$SITE_SOURCE
sed -i "/<Directory \/>/b; s/<Directory.*/<Directory $SITE_ROOT_SED>/g" $(dirname $0)/$SITE_SOURCE
sed -i "s/Alias \/pool.*/Alias \/pool $SITE_POOL_SED/g" $(dirname $0)/$SITE_SOURCE

cp $(dirname $0)/$SITE_SOURCE /etc/apache2/sites-available
ln -fs /etc/apache2/sites-available/$SITE_SOURCE /etc/apache2/sites-enabled/$SITE_TARGET
service apache2 reload

################################################################################

mysql -u$DB_USER -p < $(dirname $0)/$DB_SOURCE
mkdir -p $SITE_ROOT $SITE_POOL
chown $HOST_USER:$HOST_USER $SITE_ROOT $SITE_POOL
chmod a+w $SITE_POOL
