// 16 Enero 2014

class MidiEvent {
};

class MidiTrack {
};

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
	void read(const char *path);
	
private:
	FormatType formatType;
	DivisionType divisionType;
	unsigned short nTracks;
	unsigned short timeDivision;
	MidiTrack *tracks;
};
