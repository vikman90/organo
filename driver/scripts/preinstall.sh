#!/bin/sh
# Preinstalling script
# Victor Manuel Fernandez Castro
# 4 August 2015

USER='organ'

adduser --quiet --group --system --no-create-home $USER
adduser $USER gpio
