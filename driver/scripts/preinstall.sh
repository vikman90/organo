#!/bin/sh
# Preinstalling script
# Victor Manuel Fernandez Castro
# 4 August 2015

USER='organ'

mkdir -p obj bin
adduser --quiet --group --system --no-create-home $USER
adduser $USER organ
