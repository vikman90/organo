// 29 July 2015

#ifndef PLAYER_H
#define PLAYER_H

#include "database.h"

enum player_state_t { PLAYING, STOPPED, PAUSED };

int player_start(score_t *scores, int n, int idplaylist, int idscore);
int player_wait();
int player_pause();
int player_resume();
int player_stop();
enum player_state_t player_state(int *idplaylist, int *idscore);

#endif // PLAYER_H
