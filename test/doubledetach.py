#!/usr/bin/python3

# Generar señales en GPIO
# Victor Manuel Fernandez Castro
# 28 de Enero de 2015
# 
# Uso: generator.py F1 F2 W
#  F: frecuencia en Hz
#  W: Ancho de pulso en seg
#
# Salida:
#  F1 Hz por el puerto GPIO2 (pin 3)
#  F2 Hz por el puerto GPIO3 (pin 5)

import RPi.GPIO as GPIO
from time import sleep
from sys import argv, exit
from threading import Thread

PIN1 = 2
PIN2 = 3

GPIO.setmode(GPIO.BCM)
GPIO.setup(PIN1, GPIO.OUT)
GPIO.setup(PIN2, GPIO.OUT)

if len(argv) < 4:
    print('Uso:', argv[0], 'F(Hz) W(s)')
    exit(1)

freq1 = float(argv[1])
period1 = 1.0 / freq1
freq2 = float(argv[2])
period2 = 1.0 / freq2
pulsewidth = float(argv[3])

def signal(pin, period, pulsewidth):
    while True:
        GPIO.output(pin, True)
        sleep(pulsewidth)
        GPIO.output(pin, False)
        sleep(period - pulsewidth)

Thread(target=signal, args=(PIN1, period1, pulsewidth)).start()
Thread(target=signal, args=(PIN2, period2, pulsewidth)).start()
