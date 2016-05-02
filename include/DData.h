// DData.h ///////////////////////////////////////////////
// DData Class - Definition //////////////////////////////

#ifndef DDATA_H
#define DDATA_H

#include <vector>

class DData
{
	
static const int SAMPLE_RATE;

public:

	std::vector<int> drumNote;
	std::vector<int> len;
	std::vector<int> param;
	int sampleRate;
	int totalFrames;
	
	std::vector<int> eventType;
	std::vector<int> eventParam;
	std::vector<long> eventFrame;
	int nEvents;
	
	DData();
	DData(int sRate);
	~DData();
	void clear();
	int getSize();

private:

};

// DData should have 2 parallel arrays - drumNote[n] and len[n]
// for example, the 3rd drum note in the music score is a snare that lasts 22050 frames
// then ddata.drumNote[2] = 1 (snare), ddata.len[2] = 22050
// ddata.drumNote[n] = 65535 means REST
// ddata.drumNote[n] = -1 means end of DData

#endif