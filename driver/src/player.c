// 29 July 2015

#include <pthread.h>
#include <strings.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>
#include <syslog.h>
#include "player.h"
#include "output.h"

#define DEFAULT_TEMPO 500000	// usec / quarter = 120 bpm

static pthread_t thread;
static volatile enum player_state_t state = STOPPED;
static volatile int active = 0;
static char **playlist = NULL;
static int nfiles;
static int loop;
static volatile int cur_ifile = 0;
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
	char error[nfiles];
	midifile_t file;

	arg = arg;

	if (loop) {
		bzero(error, nfiles);

		// Play

		for (i = 0; 1; i = (i + 1) % nfiles) {
			if (error[i])
				continue;
			
			if (midifile_init(&file, playlist[i]) < 0) {
				syslog(LOG_WARNING, "Score %s not loaded", playlist[i]);
				midifile_destroy(&file);
				error[i] = 1;

				if (++nerrors >= nfiles)
					break;
			} else {
				cur_ifile = i;
				syslog(LOG_INFO, "Starting execution of %s", playlist[i]);
				int retval = playscore(&file);
				syslog(LOG_INFO, "Execution finished with code %d", retval);
				midifile_destroy(&file);
				
				if (retval)
					break;
			}
		}
	} else {
		// No loop
	
		for (i = 0; i < nfiles; i++) {
			if (midifile_init(&file, playlist[i]) < 0) {
				syslog(LOG_WARNING, "Score %s not loaded", playlist[i]);
				midifile_destroy(&file);
			} else
				cur_ifile = i;
				syslog(LOG_INFO, "Starting execution of %s", playlist[i]);
				int retval = playscore(&file);
				syslog(LOG_INFO, "Execution finished with code %d", retval);
				midifile_destroy(&file);
				
				if (retval)
					break;
		}
	}

	active = 0;
	return NULL;
}

// Play a playlist

int player_start(char **_playlist, int n, int _loop) {
	if (state != STOPPED)
		player_stop();
	
	// Delete scores at this point to avoid race conditions
	
	if (playlist) {
		int i;
		
		for (i = 0; i < nfiles; i++)
			free(playlist[i]);
		
		free(playlist);
	}

	active = 1;
	playlist = _playlist;
	nfiles = n;
	loop = _loop;
	cur_ifile = 0;
	
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
	if (!active)
		state = STOPPED;

	if (state != PLAYING)
		return -1;

	pthread_mutex_lock(&mutex);
	state = PAUSED;
	return 0;
}

// Resume player, if paused

int player_resume() {
	if (!active)
		state = STOPPED;

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
	if (!active)
		state = STOPPED;
	
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

enum player_state_t player_state(const char **file) {
	if (!active)
		state = STOPPED;
	else if ((state == PLAYING || state == PAUSED) && file) {
		*file = playlist[cur_ifile];
		return state;
	}

	return state;
}
