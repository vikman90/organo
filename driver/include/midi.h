/*
 * Midi file management
 * Victor Manuel Fernandez Castro
 * 29 July 2015
 */

#ifndef MIDI_H
#define MIDI_H

#define DEFAULT_TEMPO 500000	// usec / quarter = 120 bpm

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
	PITCH_BEND = 0xE0,
	METAEVENT = 0xFF
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
	char fps;
	char hour;
	char min;
	char sec;
	float fr;
} midioffset_t;

typedef struct miditime_t {
	char numerator;
	char denominator;
	char metronome;
	char quantization;
} miditime_t;

typedef struct midikey_t {
	char flats;
	char mode;
} midikey_t;

typedef struct metaevent_t {
	char type;
	unsigned int length;
	char *data;
} metaevent_t;

typedef struct midievent_t {
	int delta;
	char type;
	char channel;
	
	union {
		char param1;
		char note;			// NOTE_OFF, NOTE_ON, NOTE_AFTERTOUCH
		char controller;	// CONTROLLER
		char program;		// PROGRAM_CHANGE
	};
	
	union {
		char param2;
		char velocity;		// NOTE_OFF, NOTE_ON
		char value;		// CONTROLLER
	};
	
	struct metaevent_t *metaevent;
	struct midievent_t *next;
} midievent_t;

typedef struct midifile_t {
	short format;
	short division;
	short timediv;
	unsigned short ntracks;
	struct midievent_t **tracks;
} midifile_t;

// Creates a complete MIDI file from a file path
int midifile_init(midifile_t *score, const char *path);

// Deletes a file structure
void midifile_destroy(midifile_t *file);

// Get duration of a score in seconds
int midifile_duration(const midifile_t *file);

// Aftertouch value, for NOTE_AFTERTOUCH and CHANNEL_AFTERTOUCH
char midievent_aftertouch(const midievent_t *event);

// Pitch value, for PITCH_BEND
short midievent_pitch(const midievent_t *event);

// Sequence number
short metaevent_number(const metaevent_t *event);

// Text for TEXT_EVENT, COPYRIGHT_NOTICE, SEQUENCE_NAME, INSTRUMENT_NAME, LYRICS, MARKER, CUE_POINT, PROGRAM_NAME and DEVICE_NAME
const char* metaevent_text(const metaevent_t *event);

// Channel prefix
char metaevent_channel(const metaevent_t *event);

// Tempo in microseconds per quarter-note
int metaevent_tempo(const metaevent_t *event);

// SMPTE offset
void metaevent_offset(const metaevent_t *event, midioffset_t *offset);

// Time signature
void metaevent_time(const metaevent_t *event, miditime_t *time);

// Key signature
void metaevent_key(const metaevent_t *event, midikey_t *key);

#endif