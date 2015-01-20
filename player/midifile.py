# 18 Enero 2015

from enum import Enum
from struct import unpack
from midievent import readEvents

FormatType = Enum('FormatType', ['single_track', 'multiple_simultaneous', \
                                     'multiple_independent'])
DivisionType = Enum('DivisionType', ['ticks_per_beat', 'frames_per_second'])

class MidiTrack:
    def __init__(self, file):
        chunk = unpack('>4sI', file.read(8))

        if chunk[0] != b'MTrk':
            raise Exception()

        self.size = chunk[1]
        self.events = [e for e in readEvents(file)]

class MidiFile:
    def __init__(self, pathname):
        '''Constructor'''
        
        file = open(pathname, 'rb')
        chunk = unpack('>4sIHHH', file.read(14))

        if chunk[0] != b'MThd':
            raise Exception('Invalid MIDI header')

        if chunk[1] != 6:
            raise Exception('Invalid MIDI header size')

        if chunk[2] == 0:
            self.formatType = FormatType.single_track
        elif chunk[2] == 1:
            self.formatType = FormatType.multiple_simultaneous
        elif chunk[2] == 0:
            self.formatType = FormatType.multiple_independent
        else:
            raise Exception('Invalid format type')

        self.divisionType = DivisionType.ticks_per_beat \
                            if chunk[4] & 0x8000 == 0 \
                            else DivisionType.frames_per_second

        self.timeDivision = chunk[4] & 0x7FFF

        self.tracks = [MidiTrack(file) for i in range(chunk[3])]

    def __len__(self):
        '''Number of tracks'''
        return len(self.tracks)
    
    def __getitem__(self, key):
        '''Indexes a track'''
        return self.tracks[key]

if __name__ == '__main__':
    m = MidiFile('data/Preludio.mid')
        
    
    
    
