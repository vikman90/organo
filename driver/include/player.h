// 29 July 2015

#ifndef PLAYER_H
#define PLAYER_H

#include "database.h"
#include "midi.h"

enum player_state_t { PAUSED, PLAYING, STOPPED };

// Play a playlist
int player_start(score_t *scores, int n, int idplaylist, int idscore, int loop);

// Wait thread to end (only if loop = 0)
int player_wait();

// Pause player, if running
int player_pause();

// Resume player, if paused
int player_resume();

// Stop player
int player_stop();

// Get state and current idplaylist and idscore
enum player_state_t player_state(int *idplaylist, score_t **score);

#endif // PLAYER_H
