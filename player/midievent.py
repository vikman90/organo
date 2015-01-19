class MidiEvent:    
    def __init__(self):
        print('MidiEvent init')


class MetaEvent(MidiEvent):
    def __init__(self):
        print('MetaEvent init')
                 
def varlen(file):
    byte = file.read(1)[0]
    

    while byte & 0x80:
        byte = file.read(1)[0]
        value = (value << 7) | (byte & 0x7F)

    return value
