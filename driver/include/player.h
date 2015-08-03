// 29 July 2015

#ifndef PLAYER_H
#define PLAYER_H

#include "database.h"

enum player_state_t { PLAYING, STOPPED, PAUSED };

// Play a playlist (idscore = -1 for playing the first score)
int player_start(score_t *scores, int n, int idplaylist, int idscore);

// Pause player, if running
int player_pause();

// Resume player, if paused
int player_resume();

// Stop player
int player_stop();

// Get state and current idplaylist and idscore
enum player_state_t player_state(int *idplaylist, int *idscore);

#endif // PLAYER_H
