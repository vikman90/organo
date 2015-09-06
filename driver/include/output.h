/*
 * Interface for generic output
 * Victor Manuel Fernandez Castro
 * 29 July 2015
 */

#ifndef OUTPUT_H
#define OUTPUT_H

// Initialize output
int output_init();

// Release output
void output_destroy();

// Play note
void output_noteon(int track, int note);

// Stop note
void output_noteoff(int track, int note);

// Dump data into output
void output_update();

// Silence every note and reset device
void output_panic();

// Silence every note, keeping device's state
void output_silence();

// Metronome click
void output_metronome();

// Enable or disable metronome
void output_metronome_enable(int enabled);

// Query metronome state
int output_metronome_enabled();

#endif // ORGAN_H
