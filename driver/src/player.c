// 29 July 2015

#include <pthread.h>
#include <string.h>
#include <limits.h>
#include <time.h>
#include "midi.h"
#include "output.h"

#define DEFAULT_TEMPO 500000;	// usec / quarter = 120 bpm

static pthread_t thread;

static void* player_run(void *arg) {
	midifile_t *score = arg;
	midievent_t *event;
	struct timespec timereq;
	int min_delta, tempo = DEFAULT_TEMPO;
	unsigned short i;
	char finished[score->ntracks];
	char active = 1;

	memset(finished, 0, score->ntracks);
	output_panic();
	
	while (1) {
		active = 0;
		min_delta = INT_MAX;
		
		for (i = 0; i < score->ntracks; i++) {
			if (!finished[i]) {
				event = score->tracks[i];
				
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
				
				score->tracks[i] = event;
			}
		}
		
		output_update();
		
		if (!active)
			break;
				
		// 3 Restar delta minimo a todos
	
		for (i = 0; i < score->ntracks; i++) {
			if (!finished[i]) 
				score->tracks[i]->delta -= min_delta;
		}
		
		// 4 Esperar
		
		min_delta = min_delta * tempo / score->timediv;
		timereq.tv_sec = min_delta / 1000000;
		timereq.tv_nsec = (min_delta % 1000000) * 1000;
		nanosleep(&timereq, NULL);
	}
	
	output_panic();
	return NULL;
}

int player_start(midifile_t *score) {
	return pthread_create(&thread, NULL, player_run, score);
}

int player_wait() {
	return pthread_join(thread, NULL);
}
