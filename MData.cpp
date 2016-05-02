// MData.cpp ////////////////////////////////////////////
// MData Class - Implementation /////////////////////////

#include <vector>
#include <algorithm>
#include "MData.h"

using namespace std;

const int MData::SAMPLE_RATE = 44100;

MData::MData()
{
	sampleRate = SAMPLE_RATE;
	totalFrames = 0;
	nEvents = 0;
}

MData::MData(int sRate)
{
	sampleRate = sRate;
	totalFrames = 0;
	nEvents = 0;
}

MData::~MData()
{
}

// makes all data empty for this MData
void MData::clear()
{
	freqNote.clear();
	freqNote.resize(0);
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

// returns the size of MData array
int MData::getSize()
{
	return min( min( freqNote.size(), len.size() ), param.size() );
}