// 29 July 2015

#include <pthread.h>
#include <strings.h>
#include <string.h>
#include <limits.h>
#include <time.h>
#include "midi.h"
#include "player.h"
#include "output.h"

#define DEFAULT_TEMPO 500000	// usec / quarter = 120 bpm
#define BUFFER_MAX 256

static const char SCORE_HOME[] = "/home/pi/midi/";
static const struct timespec PAUSE_SLEEP = { 0, 100000000 };

static pthread_t thread;
static enum player_state_t state = STOPPED;
static score_t *scores;
static int nscores;
static int loop;
static int cur_idplaylist = 0;
static int cur_idscore = 0;

// Play a file

static int playscore(midifile_t *file) {
	midievent_t *event;
	midievent_t *tracks[file->ntracks];
	struct timespec timereq;
	int active, min_delta, tempo = DEFAULT_TEMPO;
	unsigned short i;
	char finished[file->ntracks];
	
	for (i = 0; i < file->ntracks; i++)
		tracks[i] = file->tracks[i];

	bzero(finished, file->ntracks);
	output_panic();

	while (1) {
		active = 0;
		min_delta = INT_MAX;

		if (state != PAUSED)
			nanosleep(&PAUSE_SLEEP, NULL);
		else if (state == STOPPED) {
			output_panic();
			return 1;
		}

		for (i = 0; i < file->ntracks; i++) {
			if (!finished[i]) {
				event = tracks[i];

				// 1 Los que no deban esperar, ejecutar

				while (event->delta == 0) {
					if (event->type == NOTE_OFF)
						output_noteoff(i, event->note);
					else if (event->type == NOTE_ON) {
						if (event->velocity > 0)
							output_noteon(i, event->note);
						else
							output_noteoff(i, event->note);
					} else if (event->type == METAEVENT) {
						if (event->metaevent->type == END_OF_TRACK) {
							finished[i] = 1;
							break;
						} else if (event->metaevent->type == SET_TEMPO)
							tempo = metaevent_tempo(event->metaevent);
					}

					event = event->next;

				}

				// 2 Buscar minimo delta y condicion de final

				if (!finished[i]) {
					active = 1;

					if (event->delta < min_delta) {
						min_delta = event->delta;
					}
				}

				tracks[i] = event;
			}
		}

		output_update();

		if (!active)
			break;

		// 3 Restar delta minimo a todos

		for (i = 0; i < file->ntracks; i++) {
			if (!finished[i])
				tracks[i]->delta -= min_delta;
		}

		// 4 Esperar

		min_delta = min_delta * tempo / file->timediv;
		timereq.tv_sec = min_delta / 1000000;
		timereq.tv_nsec = (min_delta % 1000000) * 1000;
		nanosleep(&timereq, NULL);
	}

	output_panic();
	return 0;
}

// Thread main

static void* player_run(void *arg) {
	int i, nerrors = 0;
	midifile_t file;
	char buffer[BUFFER_MAX];
	char error[nscores];
	
	arg = arg;
	
	if (loop) {
		bzero(error, nscores);

		// Find first score, if indicated
		
		if (cur_idscore < 0)
			i = 0;
		else {

			for (i = 0; i < nscores; i++)
				if (scores[i].idscore == cur_idscore)
					break;

			i = (i < nscores) ? nscores : 0;
		}

		while (1) {
			strcpy(buffer, SCORE_HOME);
			strcat(buffer, scores[i].source);
			
			if (midifile_init(&file, buffer) < 0) {
				midifile_destroy(&file);
				
				if (!error[i]) {
					error[i] = 1;
					
					if (++nerrors >= nscores)
						break;
				}
			} else {
				cur_idscore = scores[i].idscore;
				int retval = playscore(&file);
				midifile_destroy(&file);
				
				if (retval)
					break;
			}

			i = (i + 1) % nscores;
		}
	} else {
		for (i = 0; i < nscores; i++) {
			strcpy(buffer, SCORE_HOME);
			strcat(buffer, scores[i].source);
			
			if (midifile_init(&file, buffer) < 0) {
				midifile_destroy(&file);
			} else {
				cur_idscore = scores[i].idscore;
				int retval = playscore(&file);
				midifile_destroy(&file);
				
				if (retval)
					break;
			}
		}
	}

	return NULL;
}

// Play a playlist (idscore = -1 for playing the first score)

int player_start(score_t *_scores, int n, int idplaylist, int idscore, int _loop) {
	if (state != STOPPED)
		player_stop();

	scores = _scores;
	nscores = n;
	loop = _loop;
	cur_idplaylist = idplaylist;
	cur_idscore = idscore;

	return pthread_create(&thread, NULL, player_run, NULL);
}

// Pause player, if running

int player_pause() {
	if (state == STOPPED)
		return -1;
	
	state = PAUSED;
	return 0;
}

// Resume player, if paused

int player_resume() {
	if (state == STOPPED)
		return -1;
	
	state = PLAYING;
	return 0;
}

// Stop player

int player_stop() {
	if (state == STOPPED)
		return 0;
	
	state = STOPPED;
	pthread_join(thread, NULL);
	score_destroy(scores, nscores);
	
	return 0;
}

// Get state and current idplaylist and idscore

enum player_state_t player_state(int *idplaylist, int *idscore) {
	if (state == PLAYING && idplaylist && idscore) {
		*idplaylist = cur_idplaylist;
		*idscore = cur_idscore;
		return PLAYING;
	}

	return state;
}
