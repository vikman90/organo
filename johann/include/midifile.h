// 16 Enero 2014

#ifndef MIDIFILE_H
#define MIDIFILE_H

#include "miditrack.h"

class MidiFile {
public:
	/// Describes the organization of the tracks
	enum FormatType {
		SingleTrack,                ///< A single track
		MultipleSimultaneous,       ///< Several tracks that sound simultaneously
		MultipleIndependent         ///< Several independent tracks
	};
	
	/// Describe how to interpret delta times
	enum DivisionType {
		TicksPerBeat,       ///< Ticks per beat (a beat is a quarter note)
		FramesPerSecond     ///< Frames per second
	};
	
	MidiFile();
	~MidiFile();
	
	inline FormatType getFormat() {
		return formatType;
	}
	
	inline unsigned short getTrackCount() {
		return nTracks;
	}
	
	inline DivisionType getDivisionType() {
		return divisionType;
	}
	
	inline unsigned short getDivision() {
		return timeDivision;
	}
	
	bool read(const char *path);
	
private:
	FormatType formatType;
	DivisionType divisionType;
	unsigned short nTracks;
	unsigned short timeDivision;
	MidiTrack *tracks;
};

#endif
