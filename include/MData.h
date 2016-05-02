// MData.h ///////////////////////////////////////////////
// MData Class - Definition //////////////////////////////

#ifndef MDATA_H
#define MDATA_H

#include <vector>

class MData
{
	
static const int SAMPLE_RATE;

public:

	std::vector<double> freqNote;
	std::vector<int> len;
	std::vector<int> param;
	
	std::vector<int> eventType;
	std::vector<int> eventParam;
	std::vector<long> eventFrame;
	int nEvents;
	
	long totalFrames;
	int sampleRate;
	
	MData();
	MData(int sRate);
	~MData();
	void clear();
	int getSize();

private:

};

#endif