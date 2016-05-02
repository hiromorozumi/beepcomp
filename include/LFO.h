// lFO.h //////////////////////////////////////////
// LFO Class - Definition /////////////////////////

#ifndef LFO_H
#define LFO_H

#include <vector>

class LFO
{
	static const double LFO_SAMPLE_RATE;
	static const int LFO_TABLE_SIZE;
	static const double LFO_TWO_PI;
	std::vector<float> table;
	
public:

	int tableType;
	int waitFrames;
	int waitTimeMSec;
	int waitPos;
	double rangeCents;
	double cyclesPerSec;
	double increment;
	double phase;

	LFO();
	~LFO();
	
	void initialize();
	void setTable(int type);
	void setWaitTime(int milliseconds);
	void setRange(int cents);
	void setSpeed(double cyclesPerSeconds);
	void refresh();
	double process(double originalFreq);
	
private:	
	
};


#endif