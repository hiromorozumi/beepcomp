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
	std::vector<float> nPinkTable;
	std::vector<float> pTable;

	float noiseLevel; // user can change noise element mix level ... default = 1.0f;
	float squareLevel; // user can change picthed element mix level ... default = 1.0f
	
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
	
	int noiseType[6];
	
	bool beefUp;
	float beefUpFactor;
	float beefUpFactorNoise;
	float compRatio;
	float compThreshold;

	double kickFreq;
	double snareFreq;
	double hihatFreq;
	int kickPeakTime;
	int kickDecayTime;
	int snarePeakTime;
	int snareDecayTime;
	int hihatPeakTime;
	int hihatDecayTime;

	float history[NOSC_HISTORY_SIZE];
	int historyWriteWait;
	int historyWriteIndex;
	
	void setTable();
	void advance();
	void advanceEnvelope();
	void setIncrement(int dType);
	void initializePhase();
	float getOutput();
	float compress(float in);
	float limit(float in);
	void enableBeefUp();
	void disableBeefUp();
	void setBeefUpFactor(float factor);
	
	void useWhiteNoise();
	void usePinkNoise();
	
	void setKickNoiseType(int type);
	void setSnareNoiseType(int type);
	void setHiHatNoiseType(int type);
	
	void setNoiseLevel(float nLevel);
	void setSquareLevel(float sqLevel);
	void setDrumTone(int dType, double nMilSecAttack, double nMilSecPeak, double nMilSecDecay, float peakVol,
		double freq, double nMilSecPTime, float pBeginningLevel, double pFallRatio);
	void resetDrumTones();
	void setKickLength(int lenMilSec);
	void setSnareLength(int lenMilSec);
	void setHiHatLength(int lenMilSec);
	void tuneKick(double freq);
	void tuneSnare(double freq);
	void tuneHiHat(double freq);
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