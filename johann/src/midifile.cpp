// 16 Enero 2015

#include "midifile.h"
#include <iostream>
#include <fstream>
#include <cstring>

#define HEADER_ID "MThd"
#define HEADER_SIZE 6

#ifndef BYTE_ORDER
#error Byte order undefined
#elif BYTE_ORDER == LITTLE_ENDIAN
#define BIGENDIAN32(x) (((x & 0xFF) << 24) | ((x & 0xFF00) << 8) | ((x & 0xFF0000) >> 8) | ((x & 0xFF000000) >> 24))
#define BIGENDIAN16(x) (((x & 0xFF) << 8) | ((x & 0xFF00) >> 8))
#else
#define BIGENDIAN32(x) x
#define BIGENDIAN16(x) x
#endif

using namespace std;

MidiFile::MidiFile() {
	formatType = SingleTrack;
	divisionType = TicksPerBeat;
	nTracks = 0;
	timeDivision = 0;
	tracks = 0;
}

MidiFile::~MidiFile() {
	delete [] tracks;
}

bool MidiFile::read(const char *path) {
	char id[4];
	unsigned int size;
	unsigned short word;
	ifstream file(path, ios_base::in | ios_base::binary);
	
	if (!file)
		return false;
	
	file.read(id, 4);
	
	if (memcmp(id, HEADER_ID, 4)) {
		cerr << "Invalid header ID\n";
		return false;
	}

	file.read((char*)&size, 4);
	
	if (BIGENDIAN32(size) != HEADER_SIZE) {
		cerr << "Invalid header size\n";
		return false;
	}
	
	cerr << "y\n"; // la linea siguiente provoca violacion del segmento
	file.read((char*)word, 2);
	cerr << "d\n";
	
	switch (BIGENDIAN16(word)) {
	case 0:
		formatType = SingleTrack;
		break;
	case 1:
		formatType = MultipleSimultaneous;
		break;
	case 2:
		formatType = MultipleIndependent;
		break;
	default:
		cerr << "Invalid format type\n";
		return false;
	}

	file.read((char*)word, 2);

	nTracks = BIGENDIAN16(word);

	file.read((char*)word, 2);

	timeDivision = BIGENDIAN16(word);

	divisionType = (timeDivision & 0x8000 == 0) ? TicksPerBeat : FramesPerSecond;

	timeDivision &= ~0x8000;
	
	return true;	
}
