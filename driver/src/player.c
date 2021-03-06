// 29 July 2015

#include <pthread.h>
#include <semaphore.h>
#include <strings.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>
#include <syslog.h>
#include "player.h"
#include "values.h"
#include "output.h"
#include "midi.h"

static pthread_t thread;
static volatile enum player_state_t state = STOPPED;
static volatile int active = 0;
static char **playlist = NULL;
static int nfiles;
static int loop;
static volatile int cur_ifile = 0;
static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
static sem_t playback;

// Play a file
static int playscore(midifile_t *file);

// Thread's main function
void* player_run(void *arg);

// Initializes the player

int player_init() {
	return sem_init(&playback, 0, 1);
}

// Destroys the player

void player_destroy() {
	player_stop();
	sem_destroy(&playback);
}

// Play a playlist

int player_start(char **_playlist, int n, int _loop) {
	int retval = 0;

	pthread_mutex_lock(&mutex);

	if (state < STOPPED && !active)
		state = STOPPED;

	switch (state) {
	case PAUSED:
		sem_post(&playback);

	case PLAYING:
		state = STOPPED;
		pthread_join(thread, NULL);

	case STOPPED:

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
			retval = -1;
		} else
			state = PLAYING;

		break;

	case ENGINEER:
		retval = -1;
	}

	pthread_mutex_unlock(&mutex);
	return retval;
}

// Wait thread to end (only if loop = 0)

int player_wait() {
	return pthread_join(thread, NULL);
}

// Pause player, if running

int player_pause() {
	int retval = 0;

	pthread_mutex_lock(&mutex);

	if (state < STOPPED && !active)
		state = STOPPED;

	if (state != PLAYING)
		retval = -1;
	else {
		sem_wait(&playback);
		state = PAUSED;
	}

	pthread_mutex_unlock(&mutex);
	return retval;
}

// Resume player, if paused

int player_resume() {
	int retval = 0;

	pthread_mutex_lock(&mutex);

	if (state < STOPPED && !active)
		state = STOPPED;

	switch (state) {
	case PAUSED:
		state = PLAYING;
		sem_post(&playback);
	case PLAYING:
		break;
	default:
		retval = -1;
	}

	pthread_mutex_unlock(&mutex);
	return retval;
}

// Stop player

int player_stop() {
	int retval = 0;

	pthread_mutex_lock(&mutex);

	if (state < STOPPED && !active)
		state = STOPPED;

	switch (state) {
	case PAUSED:
		state = PLAYING;
		sem_post(&playback);
	case PLAYING:
		state = STOPPED;
		pthread_join(thread, NULL);
	case STOPPED:
		break;
	case ENGINEER:
		retval = -1;
	}

	pthread_mutex_unlock(&mutex);
	return retval;
}

// Get state and copies the path of the currently playing file

player_state_t player_state(char *file) {
	pthread_mutex_lock(&mutex);
	player_state_t _state = state;

	if (state < STOPPED && !active)
		state = STOPPED;

	if ((state == PLAYING || state == PAUSED) && file)
		strcpy(file, playlist[cur_ifile]);

	pthread_mutex_unlock(&mutex);
	return _state;
}

// Enter into Engineering Mode

int player_engineer_enter() {
	pthread_mutex_lock(&mutex);

	if (state < STOPPED && !active)
		state = STOPPED;

	switch (state) {
	case PAUSED:
		state = PLAYING;
		sem_post(&playback);
	case PLAYING:
		state = STOPPED;
		pthread_join(thread, NULL);
	case STOPPED:
		state = ENGINEER;
	case ENGINEER:
		;
	}

	pthread_mutex_unlock(&mutex);
	return 0;
}

// Exit from Engineering Mode

int player_engineer_exit() {
	int retval = 0;

	pthread_mutex_lock(&mutex);

	if (state < STOPPED && !active)
		state = STOPPED;

	if (state == ENGINEER)
		state = STOPPED;
	else
		retval = -1;

	pthread_mutex_unlock(&mutex);
	return retval;
}

// Thread's main function

void* player_run(void __attribute__((unused)) *arg) {
	int i, nerrors = 0;
	char error[nfiles];
	midifile_t file;

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
	}

	active = 0;
	return NULL;
}

// Play a file

int playscore(midifile_t *file) {
	midievent_t *event;
	midievent_t *tracks[file->ntracks];
	miditime_t miditime;
	struct timespec timereq;
	int active, min_delta;
	int tempo = MIDI_DEFAULT_TEMPO;
	int metro_clock = file->timediv; // timediv * metro / metro
	int metro_cur = metro_clock;
	unsigned short i;
	char finished[file->ntracks];

	for (i = 0; i < file->ntracks; i++)
		tracks[i] = file->tracks[i];

	bzero(finished, file->ntracks);
	output_panic();

	while (1) {
		active = 0;
		min_delta = INT_MAX;

		if (state != PLAYING) {
			if (state == PAUSED) {
				output_silence();
				sem_wait(&playback);
				sem_post(&playback);
			} else {
				// STOPPED or ENGINEER
				output_panic();
				return 1;
			}
		}

		for (i = 0; i < file->ntracks; i++) {
			if (!finished[i]) {
				event = tracks[i];

				// 1 Execute events with delta = 0

				while (event->delta == 0) {
					switch (event->type) {
					case NOTE_OFF:
						output_noteoff(i, event->note);
						break;

					case NOTE_ON:
						if (event->velocity > 0)
							output_noteon(i, event->note);
						else
							output_noteoff(i, event->note);
						break;

					case METAEVENT:
						switch (event->metaevent->type) {
						case END_OF_TRACK:
							finished[i] = 1;
							break;

						case SET_TEMPO:
							tempo = metaevent_tempo(event->metaevent);
							break;

						case TIME_SIGNATURE:
							metaevent_time(event->metaevent, &miditime);
							metro_clock = metro_cur = file->timediv * miditime.metronome / MIDI_DEFAULT_METRO;
							break;
						}
					}

					if (finished[i])
						break;
					else
						event = event->next;
				}

				// 2 Find minimum delta and ending contition

				if (!finished[i]) {
					active = 1;

					if (event->delta < min_delta)
						min_delta = event->delta;
				}

				tracks[i] = event;
			}
		}

		if (metro_cur == 0) {
			output_metronome();
			metro_cur = metro_clock;
		}

		if (metro_cur < min_delta)
			min_delta = metro_cur;

		output_update();

		if (!active)
			break;

		// 3 Substract minimum delta to every pending event

		for (i = 0; i < file->ntracks; i++) {
			if (!finished[i])
				tracks[i]->delta -= min_delta;
		}

		metro_cur -= min_delta;

		// 4 Wait

		min_delta = (int)((double)min_delta * tempo / file->timediv);
		timereq.tv_sec = min_delta / 1000000;
		timereq.tv_nsec = (min_delta % 1000000) * 1000;
		nanosleep(&timereq, NULL);
	}

	output_panic();
	return 0;
}
