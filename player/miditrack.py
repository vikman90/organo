# 19 Enero 2015

from enum import Enum
from struct import unpack
from MidiEvent import readevents

class MidiTrack:
    def __init__(self, file):
        chunk = unpack('>4sI', file.read(8))

        if chunk[0] != b'MTrk':
            raise Exception()

        self.size = chunk[1]
        self.events = [e for e in readevents(file)]
