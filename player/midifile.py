# 18 Enero 2015

from struct import unpack
from midievent import readEvents

single_track = 1
multiple_simultaneous = 2
multiple_independent = 3

ticks_per_beat = 1
frames_per_second = 2

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
            self.formattype = single_track
        elif chunk[2] == 1:
            self.formattype = multiple_simultaneous
        elif chunk[2] == 0:
            self.formattype = multiple_independent
        else:
            raise Exception('Invalid format type')

        self.divisiontype = ticks_per_beat if chunk[4] & 0x8000 == 0 \
                            else frames_per_second

        self.timedivision = chunk[4] & 0x7FFF

        self.tracks = [MidiTrack(file) for i in range(chunk[3])]

    def __len__(self):
        '''Number of tracks'''
        return len(self.tracks)
    
    def __getitem__(self, key):
        '''Indexes a track'''
        return self.tracks[key]

if __name__ == '__main__':
    m = MidiFile('data/Preludio.mid')
        
    
    
    
