// NOSC.h //////////////////////////////////////////
// NOSC class - Definition /////////////////////////

#ifndef NOSC_H
#define NOSC_H

#include <vector>

class NOSC
{
	
static const int NOSC_NTABLE_SIZE;
static const int NOSC_PTABLE_SIZE;
static const double NOSC_SAMPLE_RATE;	
static const int NOSC_HISTORY_SIZE = 64;

public:

	std::vector<float> nTable;
	std::vector<float> pTable;
	
	double phase;
	double increment;
	double pPhase;
	double pFrequency;
	double pIncrement;
	double pPitchFall;
	double pLevel;
	int drumType;
	float gain;
	bool resting;
	
	// each drum will have an independent envelope shape
	// kick, snare, hat (and quiet versions of them)
	int nEnvFrames[6];
	int nAttackFrames[6];
	int nPeakFrames[6];
	int nDecayFrames[6];
	float peakLevel[6];
	double frequency[6];
	double pDecayTime[6];
	double pitchFallDelta[6];
	double pitchFallLimit[6];
	float pStartLevel[6];
	float levelFallDelta[6];
	int envPos;
	bool envFinished;

	float history[NOSC_HISTORY_SIZE];
	int historyWriteWait;
	int historyWriteIndex;
	
	void setTable();
	void advance();
	void advanceEnvelope();
	void setIncrement(int dType);
	void initializePhase();
	float getOutput();
	void setDrumTone(int dType, double nMilSecAttack, double nMilSecPeak, double nMilSecDecay, float peakVol,
		double freq, double nMilSecPTime, float pBeginningLevel, double pFallRatio);
	void refreshEnvelope();
	float getEnvelopeOutput();
	void setGain(float g);
	float getGain();
	void rest();
	void unrest();
	void setNewDrum(int dType);
	float getPitchOutput();
	void pushHistory(float g);
	float getHistoricalAverage();
	void clearHistory();

	NOSC();
	~NOSC();
	
private:

};

#endif