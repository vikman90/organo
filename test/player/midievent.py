# 19 January 2015

import logging

#logging.basicConfig(level=logging.INFO)

# MIDI events

NOTE_OFF = 0x80
NOTE_ON = 0x90
NOTE_AFTERTOUCH = 0xA0
CONTROLLER = 0xB0
PROGRAM_CHANGE = 0xC0
CHANNEL_AFTERTOUCH = 0xD0
PITCH_BEND = 0xE0

event_types = {NOTE_OFF, NOTE_ON, NOTE_AFTERTOUCH, CONTROLLER, \
            PROGRAM_CHANGE, CHANNEL_AFTERTOUCH, PITCH_BEND}

# Meta-events

SEQUENCE_NUMBER = 0x0
TEXT_EVENT = 0x1
COPYRIGHT_NOTICE = 0x2
SEQUENCE_NAME = 0x3
INSTRUMENT_NAME = 0x4
LYRICS = 0x5
MARKER = 0x6
CUE_POINT = 0x7
PROGRAM_NAME = 0x08
DEVICE_NAME = 0x09
CHANNEL_PREFIX = 0x20
END_OF_TRACK = 0x2F
SET_TEMPO = 0x51
SMPTE_OFFSET = 0x54
TIME_SIGNATURE = 0x58
KEY_SIGNATURE = 0x59
SEQUENCER_SPECIFIC = 0x7F

metaevent_types = {SEQUENCE_NUMBER, TEXT_EVENT, COPYRIGHT_NOTICE, \
                   SEQUENCE_NAME, INSTRUMENT_NAME, LYRICS, MARKER, \
                   CUE_POINT, PROGRAM_NAME, DEVICE_NAME, CHANNEL_PREFIX,
                   END_OF_TRACK, SET_TEMPO, SMPTE_OFFSET, TIME_SIGNATURE, \
                   KEY_SIGNATURE, SEQUENCER_SPECIFIC}

def varlen(file):
    '''Reads a variable-length value from file'''
    byte = file.read(1)[0]
    value = byte & 0x7F
    
    while byte & 0x80:
        byte = file.read(1)[0]
        value = (value << 7) | (byte & 0x7F)

    return value

class MidiEvent:
    def __init__(self, delta, value, file):
        self.delta = delta
        self.type = value & 0xF0
        self.channel = value & 0x0F

        if self.type not in event_types:
            raise Exception('Invalid event type: 0x{0:02x}'.format(self.type))

        self.param1 = file.read(1)[0]

        if not (self.type == PROGRAM_CHANGE \
                or self.type == CHANNEL_AFTERTOUCH):
            self.param2 = file.read(1)[0]

    def __repr__(self):
        string = '{0}: event {1:02x}@{2:02x} ({3:02x}'.format(self.delta, \
                                                              self.type, \
                                                              self.channel, \
                                                              self.param1)

        if not (self.type == PROGRAM_CHANGE \
                or self.type == CHANNEL_AFTERTOUCH):
            return string + ', {0:02x})'.format(self.param2)
        else:
            return string + ')'

    def note(self):
        '''Note number, for NOTE_OFF, NOTE_ON and NOTE_AFTERTOUCH'''
        return self.param1

    def velocity(self):
        '''Velocity, for NOTE_OFF and NOTE_ON'''
        return self.param2

    def aftertouch(self):
        '''Aftertouch value, for NOTE_AFTERTOUCH and CHANNEL_AFTERTOUCH'''
        return self.param2 if self.type == NOTE_AFTERTOUCH else self.param1

    def controller():
        '''Controller number, for CONTROLLER'''
        return self.param1

    def value():
        '''Controller value, for CONTROLLER'''
        return self.param2

    def program():
        '''Program number, for PROGRAM_CHANGE'''
        return self.param1
    
    def pitch():
        '''Pitch value, for PITCH_BEND'''
        return self.param1 | (self.param2 << 7)

class MetaEvent(MidiEvent):
    def __init__(self, delta, evtype, data):
        if evtype not in metaevent_types:
            raise Exception('Invalid meta-event: 0x{0:02x}'.format(evtype))
        
        self.delta = delta
        self.type = evtype        
        self.data = data

    def __repr__(self):
        return '{0}: Meta-event {1:02x} ({2})'.format(self.delta, \
                                                      self.type, \
                                                      self.data)

    def number(self):
        '''Sequence number'''
        return (self.data[0] << 8) | self.data[1]

    def text(self):
        '''Text for TEXT_EVENT, COPYRIGHT_NOTICE, SEQUENCE_NAME,
INSTRUMENT_NAME, LYRICS, MARKER, CUE_POINT, PROGRAM_NAME and DEVICE_NAME'''
        return self.data

    def channel(self):
        '''Channel prefix'''
        return data[0]

    def tempo(self):
        '''Tempo in seconds per quarter-note'''
        return ((self.data[0] << 16) | (self.data[1] << 8) | self.data[2]) / 1000000

    def offset(self):
        '''SMPTE offset as tuple (fps, hour, min, sec, fr) '''
        fps = self.data[0] >> 6

        if fps == 0:
            fps = 24
        elif fps == 1:
            fps = 25
        else:
            fps = 30

        return fps, data[0] & 0x3F, data[1], data[2], data[3] + data[4] / 100

    def time(self):
        '''Time signature as tuple (numerator, denominator,
metronome, quantization'''
        return self.data[0], 2 ** self.data[1], self.data[2], self.data[3]

    def key(self):
        '''Key signature as tuple (flats, major|minor)'''
        return self.data[0], self.data[1]

def parseEvents(file):
    '''Parse and yield events from a track, until the END_OF_TRACK meta-event'''
    runningstatus = 0
    
    while True:
        delta = varlen(file)
        value = file.read(1)[0]
        
        if value < 0xF0:
            if value < 0x80:
                value = runningstatus
                file.seek(file.tell() - 1)
            try:
                event = MidiEvent(delta, value, file)
                runningstatus = value
                logging.info(event)
                yield event
                
            except Exception as e:
                logging.error(e)
                
        elif value == 0xFF:
            try:
                evtype = file.read(1)[0]
                data = file.read(varlen(file))
                event = MetaEvent(delta, evtype, data)
                logging.info(event)
                yield event

                if event.type == END_OF_TRACK:
                    break
                
            except Exception as e:
                logging.error(e)

        elif value == 0xF0 or value == 0xF7:
            logging.warning('SysEx message discarded')
        else:
            logging.error('Invalid event type: 0x{0:02x}'.format(value))

    raise StopIteration

            
