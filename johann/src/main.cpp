// 14 Enero 2015

#include <iostream>
#include <cstdlib>
#include "midifile.h"

using namespace std;

int main(int argc, char **argv) {
	if (argc < 2) {
		cerr << "Syntax: " << argv[0] << " <midi>\n";
		return EXIT_FAILURE;
	}
	
	MidiFile midi;
	
	if (!midi.read(argv[1]))
		return EXIT_FAILURE;
	else
		cout << "Pistas: " << midi.getTrackCount();
	
	return EXIT_SUCCESS;	
}