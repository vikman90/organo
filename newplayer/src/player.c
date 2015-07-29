// 29 July 2015

#include <stdio.h>
#include "midi.h"

int main() {
	midifile_t midi;
	int r = midifile_init(&midi, "data/Preludio.mid");
	printf("%d\n", r);
	return 0;
}
