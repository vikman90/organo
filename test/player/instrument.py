# 26 January 2015

import midifile
import midievent
import logging
from threading import Thread
from time import sleep

logging.basicConfig(level=logging.INFO)

note_names = ['C', 'C#', 'D', 'D#', 'E', 'F', 'F#', 'G', 'G#', 'A', 'A#', 'B']

def note_string(note):
    '''Converts a note number into a string'''
    return note_names[note % 12] + str((note - 12) // 12)
        
class Instrument:
    '''Instrument with note-mapping, that can play a midi track'''
    
    state = 0
    
    def __init__(self, length, offset, midi, track):
        '''Creates an instrument with a given length and note-offset,
        and sets a MIDI file and a track number.'''

        if midi.divisiontype == midifile.FRAMES_PER_SECOND:
            raise Exception('FRAMES_PER_SECOND not yet implemented')

        self.length = length
        self.offset = offset
        self.thread = Thread(target=self._play, args=(midi, track))
        self.field = (1 << length) - 1
        self.fmt = '{0:0' + str(length) + 'b}'

    def __str__(self):
        '''Returns the state as a string'''
        return self.fmt.format(self.state)[::-1]
        
    def _play(self, midi, track):
        '''Background working'''
        
        for event in midi[track]:
            if event.delta > 0:
                print(self)
                sleep(event.delta * midi.tempo / midi.timedivision / 1000000)

            if isinstance(event, midievent.MidiEvent):
                if event.type == midievent.NOTE_ON:
                    if event.velocity() > 0:
                        self._note_on(event.note())
                    else:
                        self._note_off(event.note())
                elif event.type == midievent.NOTE_OFF:
                    self._note_off(event.note())

            elif event.type == midievent.SET_TEMPO:
                midi.tempo = event.tempo()
                
        print(self)
        
    def _note_on(self, note):
        '''NOTE_ON event'''
        logging.info(note_string(note) + ' on')
        note -= self.offset

        if note >= 0 and note < self.length:
            self.state |= (1 << note)
        else:
            logging.warning('Note ignored')

    def _note_off(self, note):
        '''NOTE_OFF event'''
        logging.info(note_string(note) + ' off')
        note -= self.offset

        if note >= 0 and note < self.length:
            self.state &= self.field - (1 << note)
        else:
            logging.warning('Note ignored')    
    def play(self):
        '''Play selected track on the instrument'''
        self.thread.start()

    def wait(self):
        self.thread.join()
        
    
