# 18 January 2015

from struct import unpack
import logging
import midievent

SINGLE_TRACK = 1
MULTIPLE_SIMULTANEOUS = 2
MULTIPLE_INDEPENDENT = 3

TICKS_PER_BEAT = 1
FRAMES_PER_SECOND = 2

class MidiTrack:
    def __init__(self, file):
        '''Creates a track from a file'''
        
        chunk = unpack('>4sI', file.read(8))

        if chunk[0] != b'MTrk':
            raise Exception()

        self.size = chunk[1]
        self.events = [e for e in midievent.parseEvents(file)]

    def __iter__(self):
        '''Iterate over events'''
        return self.events.__iter__()

class MidiFile:
    '''Class representing a MIDI file as a list of MIDItracks'''

    # Default tempo: 1 / 120000000
    tempo = 500000
    
    def __init__(self, pathname):
        '''Creates a complete MIDI file from a file (path)'''
        
        file = open(pathname, 'rb')
        chunk = unpack('>4sIHHH', file.read(14))

        if chunk[0] != b'MThd':
            raise Exception('Invalid MIDI header')

        if chunk[1] != 6:
            raise Exception('Invalid MIDI header size')

        if chunk[2] == 0:
            self.formattype = SINGLE_TRACK
        elif chunk[2] == 1:
            self.formattype = MULTIPLE_SIMULTANEOUS
        elif chunk[2] == 2:
            self.formattype = MULTIPLE_INDEPENDENT
        else:
            raise Exception('Invalid format type')

        self.divisiontype = TICKS_PER_BEAT if chunk[4] & 0x8000 == 0 \
                            else FRAMES_PER_SECOND

        self.timedivision = chunk[4] & 0x7FFF
        self.tracks = [MidiTrack(file) for i in range(chunk[3])]
        
    def __len__(self):
        '''Number of tracks'''
        return len(self.tracks)
    
    def __getitem__(self, key):
        '''Indexes a track'''
        return self.tracks[key]    
