// OSC.h /////////////////////////////////////////////
// OSC class - definition ////////////////////////////

#ifndef OSC_H
#define OSC_H

class Astro;
class LFO;

#include <vector>
#include "Astro.h"
#include "LFO.h"

using namespace std;

class OSC
{

static const int OSC_TABLE_SIZE;
static const int ENV_TABLE_SIZE;
static const double OSC_SAMPLE_RATE;
static const float TWO_PI;
static const int OSC_HISTORY_SIZE = 64;

public:
	
	vector<float> table;
	
	double phase;
	double dblPhaseIntPart;
	int intPhase;
	int intPhaseNext;
	float deviation;
	double increment;
	double freq;
	double adjustedFreq;
	double detune;
	float gain;
	
	bool resting;
	
	int nEnvFrames;
	int nAttackFrames;
	int nPeakFrames;
	int nDecayFrames;
	int decayStartPos;
	float peakLevel;
	float decayAmount;
	float sustainLevel;
	int envPos;
	bool envADfinished;
	
	int nReleaseFrames;
	int releasePos;
	bool envRfinished;
	
	Astro astro;
	bool astroEnabled;
	
	LFO lfo;
	bool lfoEnabled;
	
	float history[OSC_HISTORY_SIZE];
	int historyWriteWait;
	int historyWriteIndex;
	
	OSC();
	~OSC();
	
	void setTable(int type);	
	void advance();
	void setToRest();
	void setNewNote(double newFreq);
	void setFrequency(double noteFreq);
	void setIncrement(double noteFreq);
	void initializePhase();
	float getOutput();
	void advanceEnvelope();
	void refreshEnvelope();
	float getEnvelopeOutput();
	void setGain(float g);
	float getGain();
	void enableAstro();
	void disableAstro();
	void setAstroSpeed(int nCyclesPerSecond);
	void enableLFO();
	void disableLFO();
	void setLFOwaitTime(int milliseconds);
	void setLFOrange(int cents);
	void setLFOspeed(double cyclePerSeconds);
	void setAttackTime(int attackTimeMS);
	void setPeakTime(int peakTimeMS);
	void setDecayTime(int decayTimeMS);
	void setReleaseTime(int releaseTimeMS);
	void setPeakLevel(float peakLV);
	void setSustainLevel(float sustainLV);
	void setEnvelope(int attackTimeMS, int peakTimeMS, int decayTimeMS, int releaseTimeMS,
						float peakLV, float sustainLV);
	void readjustEnvParams();
	void pushHistory(float g);
	float getHistoricalAverage();
	void clearHistory();
};

#endif