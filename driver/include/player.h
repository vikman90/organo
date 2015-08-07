// 29 July 2015

#ifndef PLAYER_H
#define PLAYER_H

#include "midi.h"

enum player_state_t { PAUSED, PLAYING, STOPPED };

/*
 * Play a playlist
 * scores: Pointer to array of strings (absolute path of MIDI files). It's always destroyed (although there is an error).
 * n: Length of array of scores.
 * Loop: nonzero to play in loop.
 * Returns 0 on success and -1 on error.
 */
int player_start(char **playlist, int n, int loop);

// Wait thread to end (only if loop = 0)
int player_wait();

// Pause player, if running
int player_pause();

// Resume player, if paused
int player_resume();

// Stop player
int player_stop();

// Get state and the path of the currently playing file
enum player_state_t player_state(const char **file);

#endif // PLAYER_H
