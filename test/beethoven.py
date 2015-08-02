# Test for keys and registers
# Victor Manuel Fernandez Castro
# 27 July 2015
#
# Ludwig van Beethoven: Ode to Joy (9th Simphony in D minor, Op. 125, 4th mov.)

import RPi.GPIO as gpio
from time import sleep

# GPIO pins

ports = [2, 3, 4, 17]
rckl = 27
srckl = 22

# Board configuration

regsize = 7
pulsewidth = 0.000001

# Notes and registers

c1 = [1, 0, 0, 0, 0, 0, 0]
d1 = [0, 1, 0, 0, 0, 0, 0]
e1 = [1, 0, 1, 0, 0, 0, 0]
f1 = [1, 0, 0, 1, 0, 0, 0]
g1 = [1, 0, 0, 0, 1, 0, 0]
a1 = [1, 0, 0, 0, 0, 1, 0]
b1 = [1, 0, 0, 0, 0, 0, 1]
c2 = [1, 0, 0, 0, 0, 0, 0]
rest = [0 for i in range(regsize)]
regs1 = [1, 1, 0, 0, 0, 0, 0]
regs2 = [0, 0, 1, 1, 0, 0, 0]

# Duration

quarter = 1.0
eighth = 0.5

def dump(data):
    '''Dump data as list of bits onto ports'''

    for i in range(regsize):
        for j in range(len(ports)):
            gpio.output(ports[j], data[j][i])

        gpio.output(srckl, True)

        # Prueba a comentar esta linea
        sleep(pulsewidth)

        gpio.output(srckl, False)

    gpio.output(rckl, True)

    # Prueba a comentar esta linea
    sleep(pulsewidth)
    
    gpio.output(rckl, False)

def play(notes, registers, duration):
    '''Play a list of notes at channel 1, and registers at channel 4'''
    dump([notes, rest, rest, registers])
    sleep(duration * 3 / 4.0)
    dump([rest, rest, rest, registers])
    sleep(duration / 4.0)

def panic():
    '''Stop every note and close all registers'''
    dump([rest, rest, rest, rest])
    sleep(1)
   
# Setup

gpio.setmode(gpio.BCM)

for port in ports:
    gpio.setup(port, gpio.OUT)

gpio.setup(rckl, gpio.OUT)
gpio.setup(srckl, gpio.OUT)

panic()

while True:

    # First phrase

    play(e1, regs1, eighth)
    play(e1, regs1, eighth)
    play(f1, regs1, eighth)
    play(g1, regs1, eighth)
    play(g1, regs1, eighth)
    play(f1, regs1, eighth)
    play(e1, regs1, eighth)
    play(d1, regs1, eighth)
    play(c1, regs1, eighth)
    play(c1, regs1, eighth)
    play(d1, regs1, eighth)
    play(e1, regs1, eighth)
    play(e1, regs1, quarter)
    play(d1, regs1, quarter)

    # Second phrase

    play(e1, regs2, eighth)
    play(e1, regs2, eighth)
    play(f1, regs2, eighth)
    play(g1, regs2, eighth)
    play(g1, regs2, eighth)
    play(f1, regs2, eighth)
    play(e1, regs2, eighth)
    play(d1, regs2, eighth)
    play(c1, regs2, eighth)
    play(c1, regs2, eighth)
    play(d1, regs2, eighth)
    play(e1, regs2, eighth)
    play(d1, regs2, quarter)
    play(c1, regs2, quarter)
