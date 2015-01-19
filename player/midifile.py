# 18 Enero 2015

from enum import Enum
from struct import unpack
from miditrack import MidiTrack

FormatType = Enum('FormatType', ['SingleTrack', 'MultipleSimultaneous', \
                                     'MultipleIndependent'])
DivisionType = Enum('DivisionType', ['TicksPerBeat', 'FramesPerSecond'])

class MidiFile:
    def __init__(self, pathname):
        '''Constructor'''
        
        file = open(pathname, 'rb')
        chunk = unpack('>4sIHHH', file.read(14))

        if chunk[0] != b'MThd':
            raise Exception()

        if chunk[1] != 6:
            raise Exception()

        if chunk[2] == 0:
            self.formatType = FormatType.SingleTrack
        elif chunk[2] == 1:
            self.formatType = FormatType.MultipleSimultaneous
        elif chunk[2] == 0:
            self.formatType = FormatType.MultipleIndependent
        else:
            raise Exception()

        self.divisionType = DivisionType.TicksPerBeat \
                            if chunk[4] & 0x8000 == 0 \
                            else DivisionType.FramesPerSecond

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
        
    
    
    
