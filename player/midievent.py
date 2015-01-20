# 19 Enero 2015

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

note_off = 0x80
note_on = 0x90
note_aftertouch = 0xA0
controller = 0xB0
program_change = 0xC0
channel_aftertouch = 0xD0
pitch_bend = 0xE0

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
key_signature = 0x59
sequencer_specific = 0x7F

class MidiEvent:
    def __init__(self, delta, value, file):
        self.delta = delta
        self.eventtype = value & 0xF0
        self.channel = value & 0x0F

        if self.eventtype == note_off or self.eventtype == note_on:
            self.note = file.read(1)[0]
            self.velocity = file.read(1)[0]
        elif self.eventtype == note_aftertouch:
            self.note = file.read(1)[0]
            self.aftertouch = file.read(1)[0]
        elif self.eventtype == controller:
            self.controller = file.read(1)[0]
            self.value = file.read(1)[0]
        elif self.eventtype == program_change:
            self.program = file.read(1)[0]
        elif self.eventtype == channel_aftertouch:
            self.aftertouch = file.read(1)[0]
        else:
            value = file.read(2)
            self.pitch = value[0] | (value[1] << 7)         

    def __repr__(self):
        return str(self.delta) + ': ' + str(self.eventtype) + '@' + \
               str(self.channel)

class MetaEvent(MidiEvent):
    def __init__(self, delta, evtype, data):
        self.delta = delta
        self.eventtype = evtype
        self.data = data

    def __repr__(self):
        return str(self.delta) + ': ' + str(self.eventtype) + ' ' + \
               str(self.data)

def readEvents(file):
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
                
            except ValueError:
                logging.warning('Invalid event type: 0x{0:02x}'.format(value & 0xF0))
                
        elif value == 0xFF:
            try:
                evtype = file.read(1)[0]
                data = file.read(varlen(file))
                event = MetaEvent(delta, evtype, data)
                logging.info(event)
                yield event

                if event.eventtype == end_of_track:
                    break
                
            except ValueError:
                logging.warning('Invalid meta-event: 0x{0:02x}'.format(evtype))

        elif value == 0xF0 or value == 0xF7:
            logging.warning('SysEx message discarded')
        else:
            raise Exception('Invalid event type: 0x{0:02x}'.format(value))

    raise StopIteration

            
