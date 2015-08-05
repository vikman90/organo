// 29 July 2015

#include <pthread.h>
#include <strings.h>
#include <string.h>
#include <limits.h>
#include <syslog.h>
#include "midi.h"
#include "player.h"
#include "output.h"

#define DEFAULT_TEMPO 500000	// usec / quarter = 120 bpm

static pthread_t thread;
static volatile enum player_state_t state = STOPPED;
static volatile int active = 0;
static volatile score_t *scores = NULL;
static volatile int nscores;
static volatile int loop;
static volatile int cur_idplaylist = 0;
static volatile int first_idscore = 0;
static volatile score_t *cur_score;
static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

// Play a file

static int playscore(midifile_t *file) {
	midievent_t *event;
	midievent_t *tracks[file->ntracks];
	int deltas[file->ntracks];
	int finished[file->ntracks];
	int active, min_delta, tempo = DEFAULT_TEMPO;
	struct timespec timereq;
	unsigned short i;

	for (i = 0; i < file->ntracks; i++) {
		tracks[i] = file->tracks[i];
		deltas[i] = tracks[i]->delta;
	}

	bzero(finished, file->ntracks * sizeof(int));
	output_panic();
	
	while (1) {
		active = 0;
		min_delta = INT_MAX;
		
		// Check whether another thread has changed the state

		if (state != PLAYING) {
			if (state == PAUSED) {
				pthread_mutex_lock(&mutex);
				pthread_mutex_unlock(&mutex);
			} else {
				// STOPPED
				output_panic();
				return 1;
			}
		}

		for (i = 0; i < file->ntracks; i++) {
			if (!finished[i]) {
				event = tracks[i];

				// 1 Execute events with delta = 0

				while (deltas[i] == 0) {
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
					deltas[i] = event->delta;
				}

				// 2 Find minimum delta and ending contition

				if (!finished[i]) {
					active = 1;

					if (deltas[i] < min_delta)
						min_delta = deltas[i];
				}

				tracks[i] = event;
			}
		}

		output_update();

		if (!active)
			break;

		// 3 Substract minimum delta to every pending event

		for (i = 0; i < file->ntracks; i++) {
			if (!finished[i])
				deltas[i] -= min_delta;
		}

		// 4 Wait

		min_delta = min_delta * tempo / file->timediv;
		timereq.tv_sec = min_delta / 1000000;
		timereq.tv_nsec = (min_delta % 1000000) * 1000;
		nanosleep(&timereq, NULL);
	}

	output_panic();
	return 0;
}

// Thread's main function

static void* player_run(void *arg) {
	int i, nerrors = 0;
	char error[nscores];

	arg = arg;

	if (loop) {
		bzero(error, nscores);

		// Find first score, if indicated

		if (first_idscore < 0)
			i = 0;
		else {

			for (i = 0; i < nscores; i++)
				if (scores[i].idscore == first_idscore)
					break;

			i = (i < nscores) ? i : 0;
		}

		// Play

		while (1) {
			if (scores[i].file) {
				cur_score = &scores[i];
				syslog(LOG_INFO, "Starting execution of %s", scores[i].path);
				int retval = playscore(scores[i].file);
				syslog(LOG_INFO, "Execution finished with code %d", retval);
				
				if (retval)
					break;
			} else {
				syslog(LOG_WARNING, "Score %s not loaded", scores[i].path);
				
				if (!error[i]) {
					error[i] = 1;

					if (++nerrors >= nscores)
						break;
				}
			}

			i = (i + 1) % nscores;
		}
	} else {
		// No loop
	
		for (i = 0; i < nscores; i++) {
			if (scores[i].file) {
				cur_score = &scores[i];
				syslog(LOG_INFO, "Starting execution of %s", scores[i].path);
				int retval = playscore(scores[i].file);
				syslog(LOG_INFO, "Execution finished with code %d", retval);

				if (retval)
					break;
			} else
				syslog(LOG_WARNING, "Score %s not loaded", scores[i].path);
		}
	}

	active = 0;
	return NULL;
}

// Play a playlist

int player_start(score_t *_scores, int n, int idplaylist, int idscore, int _loop) {
	if (state != STOPPED)
		player_stop();
	
	// Delete scores at this point to avoid race conditions
	
	if (scores)
		score_destroy((score_t *)scores, nscores);

	active = 1;
	scores = _scores;
	nscores = n;
	loop = _loop;
	cur_idplaylist = idplaylist;
	first_idscore = idscore;
	
	if (pthread_create(&thread, NULL, player_run, NULL) != 0) {
		active = 0;
		return -1;
	}
	
	state = PLAYING;
	return 0;
}

// Wait thread to end (only if loop = 0)

int player_wait() {
	return pthread_join(thread, NULL);
}

// Pause player, if running

int player_pause() {
	if (state != PLAYING)
		return -1;

	pthread_mutex_lock(&mutex);
	state = PAUSED;
	return 0;
}

// Resume player, if paused

int player_resume() {
	switch (state) {
	case PAUSED:
		state = PLAYING;
		pthread_mutex_unlock(&mutex);
	case PLAYING:
		return 0;
	default:
		return -1;
	}
}

// Stop player

int player_stop() {
	switch (state) {
	case PAUSED:
		pthread_mutex_unlock(&mutex);
	case PLAYING:
		state = STOPPED;
		pthread_join(thread, NULL);
	default:
		return 0;
	}
}

// Get state and current idplaylist and idscore

enum player_state_t player_state(int *idplaylist, score_t **score) {
	if ((state == PLAYING || state == PAUSED) && idplaylist && score) {
		*idplaylist = cur_idplaylist;
		*score = (score_t *)cur_score;
		return PLAYING;
	}

	return state;
}
