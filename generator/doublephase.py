#!/usr/bin/python3

# Generar dos señales en fase en GPIO
# Victor Manuel Fernandez Castro
# 28 de Enero de 2015
# 
# Uso: generator.py F W
#  F: frecuencia en Hz
#  W: Ancho de pulso en seg
#
# Salida:
#  F Hz por el puerto GPIO2 (pin 3)
#  F/2 Hz por el puerto GPIO3 (pin 5)

import RPi.GPIO as GPIO
from time import sleep
from sys import argv, exit
from threading import Thread

PIN1 = 2
PIN2 = 3

GPIO.setmode(GPIO.BCM)
GPIO.setup(PIN1, GPIO.OUT)
GPIO.setup(PIN2, GPIO.OUT)

if len(argv) < 3:
    print('Uso:', argv[0], 'F(Hz) W(s)')
    exit(1)

frequency = float(argv[1])
period = 1.0 / frequency
pulsewidth = float(argv[2])

i = 0
while True:
    if i == 0:
        GPIO.output(PIN1, True)
        GPIO.output(PIN2, True)
        sleep(pulsewidth)
        GPIO.output(PIN1, False)
        GPIO.output(PIN2, False)
        i = 1
    else:
        GPIO.output(PIN1, True)
        sleep(pulsewidth)
        GPIO.output(PIN1, False)
        sleep(period - pulsewidth)
        i = 0

