/*
 * Midi file management
 * Victor Manuel Fernandez Castro
 * 29 July 2015
 */

#ifndef MIDI_H
#define MIDI_H

enum format_t { SINGLE_TRACK, MULTIPLE_SIMULTANEOUS, MULTIPLE_INDEPENDENT };
enum division_t { TICKS_PER_BEAT, FRAMES_PER_SECOND };
enum midimode_t { MAJOR, MINOR };

enum midievent_type_t { 
	NOTE_OFF = 0x80,
	NOTE_ON = 0x90,
	NOTE_AFTERTOUCH = 0xA0,
	CONTROLLER = 0xB0,
	PROGRAM_CHANGE = 0xC0,
	CHANNEL_AFTERTOUCH = 0xD0,
	PITCH_BEND = 0xE0
};

enum metaevent_type_t {
	SEQUENCE_NUMBER = 0x0,
	TEXT_EVENT = 0x1,
	COPYRIGHT_NOTICE = 0x2,
	SEQUENCE_NAME = 0x3,
	INSTRUMENT_NAME = 0x4,
	LYRICS = 0x5,
	MARKER = 0x6,
	CUE_POINT = 0x7,
	PROGRAM_NAME = 0x08,
	DEVICE_NAME = 0x09,
	CHANNEL_PREFIX = 0x20,
	END_OF_TRACK = 0x2F,
	SET_TEMPO = 0x51,
	SMPTE_OFFSET = 0x54,
	TIME_SIGNATURE = 0x58,
	KEY_SIGNATURE = 0x59,
	SEQUENCER_SPECIFIC = 0x7F
};

typedef struct midioffset_t {
	unsigned char fps;
	unsigned char hour;
	unsigned char min;
	unsigned char sec;
	float fr;
} midioffset_t;

typedef struct miditime_t {
	unsigned char numerator;
	unsigned char denominator;
	unsigned char metronome;
	unsigned char quantization;
} miditime_t;

typedef struct midikey_t {
	unsigned char flats;
	unsigned char mode;
} midikey_t;

typedef struct metaevent_t {
	unsigned char type;
	unsigned int length;
	unsigned char *data;
} metaevent_t;

typedef struct midievent_t {
	unsigned int delta;
	unsigned char type;
	unsigned char channel;
	
	union {
		unsigned char param1;
		unsigned char note;			// NOTE_OFF, NOTE_ON, NOTE_AFTERTOUCH
		unsigned char controller;	// CONTROLLER
		unsigned char program;		// PROGRAM_CHANGE
	};
	
	union {
		unsigned char param2;
		unsigned char velocity;		// NOTE_OFF, NOTE_ON
		unsigned char value;		// CONTROLLER
	};
	
	struct metaevent_t *metaevent;
	struct midievent_t *next;
} midievent_t;

typedef struct midifile_t {
	unsigned short format;
	unsigned short division;
	unsigned short time;
	unsigned short ntracks;
	struct midievent_t *tracks;
} midifile_t;

// Creates a complete MIDI file from a file path
int midifile_init(midifile_t *score, const char *path);

// Deletes a file structure
void midifile_destroy(midifile_t *file);

// Aftertouch value, for NOTE_AFTERTOUCH and CHANNEL_AFTERTOUCH
unsigned char midievent_aftertouch(const midievent_t *event);

// Pitch value, for PITCH_BEND
unsigned short midievent_pitch(const midievent_t *event);

// Sequence number
unsigned short metaevent_number(const metaevent_t *event);

// Text for TEXT_EVENT, COPYRIGHT_NOTICE, SEQUENCE_NAME, INSTRUMENT_NAME, LYRICS, MARKER, CUE_POINT, PROGRAM_NAME and DEVICE_NAME
char* metaevent_text(const metaevent_t *event);

// Channel prefix
unsigned char metaevent_channel(const metaevent_t *event);

// Tempo in microseconds per quarter-note
unsigned char metaevent_tempo(const metaevent_t *event);

// SMPTE offset
void metaevent_offset(const metaevent_t *event, midioffset_t *offset);

// Time signature
void metaevent_time(const metaevent_t *event, miditime_t *time);

// Key signature
void metaevent_key(const metaevent_t *event, midikey_t *key);

#endif