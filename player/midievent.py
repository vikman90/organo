# 19 Enero 2015

from enum import Enum
from struct import unpack
import logging

logging.basicConfig(level=logging.INFO)

def varlen(file):
    byte = file.read(1)[0]
    value = byte & 0x7F
    
    while byte & 0x80:
        byte = file.read(1)[0]
        value = (value << 7) | (byte & 0x7F)

    return value

class EventType(Enum):
    note_off = 0x80
    note_on = 0x90
    note_aftertouch = 0xA0
    controller = 0xB0
    program_change = 0xC0
    channel_aftertouch = 0xD0
    pitch_bend = 0xE0

class MetaEventType(Enum):
    sequence_number = 0x0
    text_event = 0x1
    copyright_notice = 0x2
    sequence_name = 0x3
    instrument_name = 0x4
    lyrics = 0x5
    marker = 0x6
    cue_point = 0x7
    program_name = 0x08
    device_name = 0x09
    channel_prefix = 0x20
    end_of_track = 0x2F
    set_tempo = 0x51
    smpte_offset = 0x54
    time_signature = 0x58
    key_signature = 0x58
    sequencer_specific = 0x7F

class MidiEvent:
    def __init__(self, delta, value, parameters):
        self.delta = delta
        self.eventtype = EventType(value & 0xF0)
        self.channel = value & 0x0F
        self.param1 = parameters[0]
        self.param2 = parameters[1]

    def __repr__(self):
        return str(self.delta) + ': ' + self.eventtype.name + '@' + \
               str(self.channel) + ' ' + str(self.param1) + \
               ' ' + str(self.param2)

class MetaEvent(MidiEvent):
    def __init__(self, delta, evtype, data):
        self.delta = delta
        self.eventtype = MetaEventType(evtype)

        i = 0
        length = data[i] & 0x7F

        while data[i] & 0x80:
            i += 1
            length = (length << 7) | (data[i] & 0x7F)

        i += 1
        self.data = data[i:]

    def __repr__(self):
        return str(self.delta) + ': ' + self.eventtype.name + str(self.data)

def readEvents(file):
    while True:
        delta = varlen(file)
        value = file.read(1)[0]
        
        if value < 0xF0:
            try:
                event = MidiEvent(delta, value, file.read(2))
                logging.info(event)
                yield event
                
            except ValueError:
                logging.warning('Invalid event type: ' + str(value & 0x80))
                
        elif value == 0xFF:
            try:
                evtype = file.read(1)
                data = file.read(varlen(file))
                event = MetaEvent(delta, evtype, data)
                logging.info(event)
                yield event

                if event.eventtype == MetaEventType.end_of_track:
                    break
                
            except ValueError:
                logging.warning('Invalid meta-event: ' + str(data[0]))

        elif value == 0xF0 or value == 0xF7:
            logging.warning('SysEx message discarded')
        else:
            raise Exception('Invalid event type: ' + str(value))

    raise StopIteration

            
