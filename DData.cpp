// DData.cpp ////////////////////////////////////////////
// DData Class - Implementation /////////////////////////

#include <vector>
#include <algorithm>
#include "DData.h"

using namespace std;

const int DData::SAMPLE_RATE = 44100;

DData::DData()
{
	sampleRate = SAMPLE_RATE;
	totalFrames = 0;
	nEvents = 0;
}

DData::DData(int sRate)
{
	sampleRate = sRate;
	totalFrames = 0;
	nEvents = 0;
}

DData::~DData()
{
}

// makes all data empty for this DData
void DData::clear()
{
	drumNote.clear();
	drumNote.resize(0);
	len.clear();
	len.resize(0);
	param.clear();
	param.resize(0);
	
	eventType.clear();
	eventType.resize(0);
	eventParam.clear();
	eventParam.resize(0);
	eventFrame.clear();
	eventFrame.resize(0);
	nEvents = 0;

	totalFrames = 0;
}

// returns the size of DData array
int DData::getSize()
{
	return min(   min( drumNote.size(), len.size() ),  param.size()  );
}
