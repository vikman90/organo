import RPi.GPIO as gpio
from time import sleep

pin = 5

gpio.setmode(gpio.BCM)
gpio.setup(pin, gpio.OUT)

while True:
    gpio.output(pin, True)
    gpio.output(pin, False)
    sleep(1 / 9600)
