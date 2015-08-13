// 29 July 2015

#ifndef OUTPUT_H
#define OUTPUT_H

// Initialize output
int output_init();

// Release output
void output_destroy();

// Play note
void output_noteon(char track, char note);

// Stop note
void output_noteoff(char track, char note);

// Dump data into output
void output_update();

// Silence every note and reset device
void output_panic();

#endif // ORGAN_H
