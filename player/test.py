# 26 January 2015

from midifile import MidiFile
from instrument import Instrument
midi = MidiFile('data/Preludio.mid')

# Test organ: 48 keys, starting at C2
i = Instrument(48, 36, midi, 0)   
i.play()
i.wait()
