import RPi.GPIO as gpio
from time import sleep

PIN = 2

def gpio_rise():
    print("Rise")

def gpio_fall():
    print("Fall")

gpio.setmode(gpio.BCM)
gpio.setup(PIN, gpio.IN)
gpio.add_event_detect(PIN, gpio.RISING, callback=gpio_rise)
#gpio.add_event_detect(PIN, gpio.FALLING, callback=gpio_fall)

while True:
    sleep(1)

