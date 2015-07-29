/*
 * Midi file management
 * Victor Manuel Fernandez Castro
 * 29 July 2015
 */

#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "midi.h"

static unsigned int varlen(int fd);
static int parse(midievent_t *first, int fd);

inline static unsigned int swap_int(unsigned int value) {
	return (value >> 24) | (value << 24) | ((value & 0xFF0000) >> 8) | ((value & 0xFF00) << 8);
}

inline static unsigned short swap_short(unsigned short value) {
	return ((value & 0xFF) << 8) | (value >> 8);
}

// -------------------- Event functions ----------------------------------------

// Creates a complete MIDI file from a file path

int midifile_init(midifile_t *score, const char *path) {
	int fd = open(path, O_RDONLY);
	unsigned int length;
	unsigned short format, ntracks, division, i;
	char buffer[4];
	
	if (fd < 0)
		return -1;
	
	// Mthd

	read(fd, buffer, 4);
	
	if (memcmp(buffer, "MThd", 4)) {
		close(fd);
		return -1;
	}
	
	// Length of header data
	
	read(fd, &length, 4);
	
	if (swap_int(length) != 6) {
		close(fd);
		return -1;
	}

	// Format

	read(fd, &format, 2);
	score->format = swap_short(format);

	// Number of tracks
	
	read(fd, &ntracks, 2);
	score->ntracks = swap_short(ntracks);
	
	// Division type

	read(fd, &division, 2);
	division = swap_short(division);	
	score->division = (division & 0x8000) >> 15;
	score->time = (division & 0x7FFF);
	
	// Read tracks
	
	score->tracks = (midievent_t *)malloc(sizeof(midievent_t) * score->ntracks);

	for (i = 0; i < score->ntracks; i++) {
		if (parse(score->tracks + i, fd) < 0) {
			close(fd);
			return -1;
		}
	}

	close(fd);
	return 0;	
}

// Deletes a file structure

void midifile_destroy(midifile_t *file) {
	midievent_t *current, *next;
	unsigned short i;
	
	for (i = 0; i < file->ntracks; i++) {
		current = file->tracks + i;
		
		if (current->type == 0xFF) {
			free(current->metaevent->data);
			free(current->metaevent);
		}
		
		current = current->next;
		
		while (current) {
			if (current->type == 0xFF) {
				free(current->metaevent->data);
				free(current->metaevent);
			}
			
			next = current->next;
			free(current);
			current = next;
		}
	}
	
	free(file->tracks);
}

// Parse event for a track, until reaching END_OF_TRACK

static int parse(midievent_t *first, int fd) {
	char buffer[4];
	midievent_t *current = first;
	unsigned char value, status = 0;
	
	// Mtrk
	
	read(fd, buffer, 4);
	
	if (memcmp(buffer, "MTrk", 4))
		return -1;
	
	// Length of chunk (ignore)
	
	lseek(fd, 4, SEEK_CUR);
	
	while (1) {
		current->delta = varlen(fd);
		read(fd, &value, 1);
		
		if (value < 0xF0) {
			// Standard event
			
			if (value < 0x80) {
				// Keep running status
				
				current->type = status & 0xF0;
				current->channel = status & 0x0F;
				current->param1 = value;
			} else {
				current->type = value & 0xF0;
				current->channel = value & 0x0F;
				read(fd, &current->param1, 1);
				status = value;
			}
			
			if (!(current->type == PROGRAM_CHANGE || current->type == CHANNEL_AFTERTOUCH))
				read(fd, &current->param2, 1);
			
		} else if (value == 0xFF) {
			// Metaevent
			
			current->type = value;
			current->metaevent = (metaevent_t *)malloc(sizeof(metaevent_t));
			read(fd, &current->metaevent->type, 1);
			current->metaevent->length = varlen(fd);
			current->metaevent->data = (unsigned char *)malloc(current->metaevent->length);
			read(fd, current->metaevent->data, current->metaevent->length);
			
			if (current->metaevent->type == END_OF_TRACK) {
				current->next = NULL;
				break;
			}
		}
		
		else if (value == 0xF0 || value == 0xF7)
			; // System exclusive event, discard
		else
			return -1;
		
		current->next = (midievent_t *)malloc(sizeof(midievent_t));
		current = current->next;
	}
	
	return 0;
}

// Reads a variable-lenvth value from file

static unsigned int varlen(int fd) {
	unsigned char byte;
	unsigned int value;
	
	read(fd, &byte, 1);
	value = byte & 0x7F;
	
	while (byte & 0x80) {
		read(fd, &byte, 1);
		value = (value << 7) | (byte & 0x7F);
	}
	
	return value;
}

// Aftertouch value, for NOTE_AFTERTOUCH and CHANNEL_AFTERTOUCH

unsigned char midievent_aftertouch(const midievent_t *event) {
	return event->type == NOTE_AFTERTOUCH ? event->param2 : event->param1;
}

// Pitch value, for PITCH_BEND

unsigned short midievent_pitch(const midievent_t *event) {
	return event->param1 | (event->param2 << 7);
}

// -------------------- Metaevent functions ------------------------------------

// Sequence number

unsigned short metaevent_number(const metaevent_t *event) {
	return (event->data[0] << 8) | event->data[1];
}

// Text for TEXT_EVENT, COPYRIGHT_NOTICE, SEQUENCE_NAME, INSTRUMENT_NAME, LYRICS, MARKER, CUE_POINT, PROGRAM_NAME and DEVICE_NAME

char* metaevent_text(const metaevent_t *event) {
	char *data = (char *)malloc(event->length + 1);
	memcpy(data, event->data, event->length);
	data[event->length] = '\0';
	return data;
}

// Channel prefix

unsigned char metaevent_channel(const metaevent_t *event) {
	return event->data[0];
}

// Tempo in microseconds per quarter-note

unsigned char metaevent_tempo(const metaevent_t *event) {
	return (event->data[0] << 16) | (event->data[1] << 8) | event->data[2];
}

// SMPTE offset

void metaevent_offset(const metaevent_t *event, midioffset_t *offset) {
	unsigned char t = event->data[0] >> 6;
	
	switch (t) {
	case 0:
		offset->fps = 24;
		break;
	case 1:
		offset->fps = 25;
		break;
	default:
		offset->fps = 30;
	}
	
	offset->hour = event->data[0] & 0x3F;
	offset->min = event->data[1];
	offset->sec = event->data[2];
	offset->fr = event->data[3] + event->data[4] / 100.0;
}

// Time signature

void metaevent_time(const metaevent_t *event, miditime_t *time) {
	unsigned char exp = event->data[1];
	time->numerator = event->data[0];
	time->denominator = 1;
	
	while ((exp--) > 0)
		time->denominator *= 2;
	
	time->metronome = event->data[2];
	time->quantization = event->data[3];
}

// Key signature

void metaevent_key(const metaevent_t *event, midikey_t *key) {
	key->flats = event->data[0];
	key->mode = event->data[1];
}