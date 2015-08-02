#!/usr/bin/python3

# Generar señales en GPIO
# Victor Manuel Fernandez Castro
# 28 de Enero de 2015
# 
# Uso: generator.py F W
#  F: frecuencia en Hz
#  W: Ancho de pulso en seg
#
# Salida:
#  F Hz por el puerto GPIO2 (pin 3)

import RPi.GPIO as GPIO
from time import sleep
from sys import argv, exit

PIN = 2

GPIO.setmode(GPIO.BCM)
GPIO.setup(PIN, GPIO.OUT)

if len(argv) < 3:
    print('Uso:', argv[0], 'F(Hz) W(s)')
    exit(1)

frequency = float(argv[1])
period = 1.0 / frequency
pulsewidth = float(argv[2])

while True:
    GPIO.output(PIN, True)
    sleep(pulsewidth)
    GPIO.output(PIN, False)
    sleep(period - pulsewidth)
