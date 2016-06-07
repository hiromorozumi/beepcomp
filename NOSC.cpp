// NOSC.cpp ////////////////////////////////////////////
// NOSC class - Implementation /////////////////////////

#include <algorithm>
#include <iostream>
#include <cstdlib>
#include <windows.h>
#include <vector>
#include "NOSC.h"

using namespace std;

const int NOSC::NOSC_NTABLE_SIZE = 9999;
const int NOSC::NOSC_PTABLE_SIZE = 4096;
const double NOSC::NOSC_SAMPLE_RATE = 44100.0;

NOSC::NOSC()
{	
	// set up a noise wave nTable
	nTable.resize(NOSC_NTABLE_SIZE);
	nPinkTable.resize(NOSC_NTABLE_SIZE);
	pTable.resize(NOSC_PTABLE_SIZE);
	setTable();

	for(int i=0; i<6; i++)
		noiseType[i] = 0; // set all to use white noise to begin
	
	squareLevel = 1.0f;
	noiseLevel = 1.0f;
	
	// initialize NOSC variables
	phase = 0.0;
	pPhase = 0.0;
	increment = 0.0;
	gain = 0.5f; // default gain
	resting = true;
	drumType = 0;
	pLevel = 1.0f;
	pPitchFall = 0;
	
	// set default envelope for each drum
	setDrumTone(0, 1, 25,  15,  0.8, 200.0,  50  , 0.9, 2.0); // kick
	setDrumTone(1, 1, 20, 120, 0.95, 720.0,  100 , 0.85, 1.2); // snare
	setDrumTone(2, 1, 17,  3,  0.5, 2400.0, 1   , 0.2, 2.0); // hat
	
	setDrumTone(3, 1, 25,  15,  0.4, 200.0,  50  , 0.5, 2.0); // kick - quiet
	setDrumTone(4, 1, 20, 120, 0.5, 720.0,  100 , 0.45, 1.2); // snare - quiet
	setDrumTone(5, 1, 17,  3,  0.2, 2400.0, 1   , 0.1, 2.0); // hat - quiet
	
	kickFreq = 200.0;
	snareFreq = 720.0;
	hihatFreq = 2400.0;
	
	kickPeakTime = 25; kickDecayTime = 15;
	snarePeakTime = 20; snareDecayTime = 120;
	hihatPeakTime = 17; hihatDecayTime = 3;

	envPos = 0;	
	
	envFinished = false;

	beefUp = false;
	beefUpFactor = 1.0f;
	beefUpFactorNoise = 1.0f;
	compRatio = 4.0f;
	compThreshold = 0.90f;
	
	// initialize history table
	clearHistory();
}

NOSC::~NOSC()
{}

void NOSC::setTable()
{
	// fill nTable with random numbers ranging from -1 to 1
	for(int i=0; i<NOSC_NTABLE_SIZE; i++)
		nTable[i] = ( (float) rand() / RAND_MAX ) * 2.0f - 1.0f;
	
	int midTablePoint = NOSC_PTABLE_SIZE / 2;
	
	// create pTable (pitched element)
	for(int i=0; i<midTablePoint; i++)
		pTable[i] = 0.95f;
	for(int i=midTablePoint; i<NOSC_PTABLE_SIZE; i++)
		pTable[i] = -0.95f;
	
	float b0, b1, b2;
	
	// create pink noise table from white noise
	// using paul Kellet's economy method (http://www.firstpr.com.au/dsp/pink-noise/#uses)
	for(int i=0; i<NOSC_NTABLE_SIZE; i++)
	{
		b0 = 0.99765f * b0 + nTable[i] * 0.0990460f; 
		b1 = 0.96300f * b1 + nTable[i] * 0.2965164f; 
		b2 = 0.57000f * b2 + nTable[i] * 1.0526913f; 
		nPinkTable[i] = (b0 + b1 + b2 + nTable[i] * 0.1848f) * 0.32f;	
	}
}

void NOSC::setGain(float g)
	{ gain = g; }

float NOSC::getGain()
	{ return gain; }

void NOSC::rest()
	{ resting = true; }
	
void NOSC::unrest()
	{ resting = false; }
	
// set envelope for a drum
//
//	param:	drum type
//			attack time, peak time, decay time (in seconds, double)
//			peak volume (float)
//			frequency (double)
void NOSC::setDrumTone(int dType, double nMilSecAttack, double nMilSecPeak, double nMilSecDecay, float peakVol, 
	double freq, double nMilSecPTime, float pBeginningLevel, double pFallRatio)
{
	nAttackFrames[dType] = static_cast<int> (NOSC_SAMPLE_RATE * nMilSecAttack / 1000.0);
	nPeakFrames[dType] = static_cast<int> (NOSC_SAMPLE_RATE * nMilSecPeak / 1000.0);
	nDecayFrames[dType] = static_cast<int> (NOSC_SAMPLE_RATE * nMilSecDecay / 1000.0);
	nEnvFrames[dType] = nAttackFrames[dType] + nPeakFrames[dType] + nDecayFrames[dType];
	peakLevel[dType] = peakVol;
	frequency[dType] = freq;
	pitchFallDelta[dType] = (freq / pFallRatio) / (NOSC_SAMPLE_RATE * nMilSecPTime / 1000.0);
	pitchFallLimit[dType] = freq / pFallRatio;
	pStartLevel[dType] = pBeginningLevel;
	levelFallDelta[dType] = peakLevel[dType] / static_cast<float>(NOSC_SAMPLE_RATE * nMilSecPTime/1000.0);
	
	// DEBUG
	// cout << "attack=" << nAttackFrames[dType] << " peakTime=" << nPeakFrames[dType] << " decayTime" << nDecayFrames[dType] << " envFrames=" << nEnvFrames[dType] << endl;
}

// reset all drum settings to default
void NOSC::resetDrumTones()
{
	squareLevel = 1.0f;
	noiseLevel = 1.0f;
	
	useWhiteNoise();
	
	// set default envelope for each drum
	setDrumTone(0, 1, 25,  15,  0.8, 200.0,  50  , 0.9, 2.0); // kick
	setDrumTone(1, 1, 20, 120, 0.95, 720.0,  100 , 0.85, 1.2); // snare
	setDrumTone(2, 1, 17,  3,  0.5, 2400.0, 1   , 0.2, 2.0); // hat
	
	setDrumTone(3, 1, 25,  15,  0.4, 200.0,  50  , 0.5, 2.0); // kick - quiet
	setDrumTone(4, 1, 20, 120, 0.5, 720.0,  100 , 0.45, 1.2); // snare - quiet
	setDrumTone(5, 1, 17,  3,  0.2, 2400.0, 1   , 0.1, 2.0); // hat - quiet
	
	kickFreq = 200.0;
	snareFreq = 720.0;
	hihatFreq = 2400.0;
	
	kickPeakTime = 25; kickDecayTime = 15;
	snarePeakTime = 20; snareDecayTime = 120;
	hihatPeakTime = 17; hihatDecayTime = 3;
}

// sets kick's length - takes a millisecond value
void NOSC::setKickLength(int lenMilSec)
{
	int peakTime, decayTime;
	if(lenMilSec < 0) lenMilSec = 0;
	else if(lenMilSec > 400) lenMilSec = 400;
	
	if(lenMilSec >= 25)
	{
		peakTime = 25;
		decayTime = lenMilSec - peakTime;
	}
	else // if smaller than 30 milsec, you just get peak stage w/ no decay
	{
		peakTime = lenMilSec;
		decayTime = 0;
	}

	setDrumTone(0, 1, peakTime,  decayTime,  0.8, kickFreq,  50  , 0.9, 2.0); // kick	
	setDrumTone(3, 1, peakTime,  decayTime,  0.4, kickFreq,  50  , 0.5, 2.0); // kick - quiet
	kickPeakTime = peakTime; kickDecayTime = decayTime;
}

// sets snare's length - takes a millisecond value
void NOSC::setSnareLength(int lenMilSec)
{
	int peakTime, decayTime;
	if(lenMilSec < 0) lenMilSec = 0;
	else if(lenMilSec > 1000) lenMilSec = 1000;
	
	if(lenMilSec >= 20)
	{
		peakTime = 20;
		decayTime = lenMilSec - peakTime;
	}
	else // if smaller than 20 milsec, you just get peak stage w/ no decay
	{
		peakTime = lenMilSec;
		decayTime = 0;
	}
	
	setDrumTone(1, 1, peakTime,  decayTime, 0.95, snareFreq,  100 , 0.85, 1.2); // snare
	setDrumTone(4, 1, peakTime,  decayTime, 0.5 , snareFreq,  100 , 0.45, 1.2); // snare - quiet
	snarePeakTime = peakTime; snareDecayTime = decayTime;
}

// sets hihat's length - takes a millisecond value
void NOSC::setHiHatLength(int lenMilSec)
{
	int peakTime, decayTime;
	if(lenMilSec < 0) lenMilSec = 0;
	else if(lenMilSec > 1000) lenMilSec = 1000;

	if(lenMilSec >= 17)
	{
		peakTime = 17;
		decayTime = lenMilSec - peakTime;
	}
	else // if smaller than 17 milsec, you just get peak stage w/ no decay
	{
		peakTime = lenMilSec;
		decayTime = 0;
	}

	setDrumTone(2, 1, peakTime, decayTime,  0.5, hihatFreq, 1   , 0.2, 2.0); // hat
	setDrumTone(5, 1, peakTime, decayTime,  0.2, hihatFreq, 1   , 0.1, 2.0); // hat - quiet
	hihatPeakTime = peakTime; hihatDecayTime = decayTime;
}

// change kick's tuning frequency
// takes a double variable from 50 to 350 (default=200.0, at 50%)
void NOSC::tuneKick(double freq)
{
	if(freq < 50.0) freq = 50.0;
	else if(freq > 350.0) freq = 350.0;
	setDrumTone(0, 1, kickPeakTime, kickDecayTime,  0.8, freq,  50  , 0.9, 2.0); // kick
	setDrumTone(3, 1, kickPeakTime, kickDecayTime,  0.4, freq,  50  , 0.5, 2.0); // kick - quiet
	kickFreq = freq;
}

// change snare's tuning frequency
// takes a double variable from 200 to 1240 (default=720.0 at 50%)
void NOSC::tuneSnare(double freq)
{
	if(freq < 200.0) freq = 200.0;
	else if(freq > 1240.0) freq = 1240.0;
	setDrumTone(1, 1, snarePeakTime, snareDecayTime, 0.95, freq,  100 , 0.85, 1.2); // snare
	setDrumTone(4, 1, snarePeakTime, snareDecayTime, 0.5,  freq,  100 , 0.45, 1.2); // snare - quiet
	snareFreq = freq;	
}

// change snare's tuning frequency
// takes a double variable from 1200 to 3600 (default=2400.0 at 50%)
void NOSC::tuneHiHat(double freq)
{
	if(freq < 1200.0) freq = 1200.0;
	else if(freq > 3600.0) freq = 3600.0;
	setDrumTone(2, 1, hihatPeakTime, hihatDecayTime,  0.5, freq, 1   , 0.2, 2.0); // hat
	setDrumTone(5, 1, hihatPeakTime, hihatDecayTime,  0.2, freq, 1   , 0.1, 2.0); // hat - quiet
	hihatFreq = freq;	
}

void NOSC::setNewDrum(int dType)
{
	if(dType >= 65535) // means rest
		resting = true;
	else
	{
		resting = false;
		drumType = dType;
		pPitchFall = 0;
		pLevel = pStartLevel[drumType];
		setIncrement(dType);
		initializePhase();
		refreshEnvelope();
	}
}
	
void NOSC::advance()
{
	// advance on the sample nTable
	phase += 1.0;
	if(phase >= NOSC_NTABLE_SIZE)
		phase -= NOSC_NTABLE_SIZE;
	
	// advance on the sample pTable
	pPhase += increment;
	while(pPhase >= NOSC_PTABLE_SIZE)
	{
		pPhase -= NOSC_PTABLE_SIZE;
	}
	
	// keep adjusting increment for falling pitch part
	setIncrement(drumType);
	
	// decrease the pitch + pitch pt level
	if(pPitchFall > -pitchFallLimit[drumType])
		pPitchFall -= pitchFallDelta[drumType];
	
	pLevel -= levelFallDelta[drumType];
	if(pLevel <= 0.0f)
		pLevel = 0.0f;
		
	// advance envelope also
	advanceEnvelope();
}

void NOSC::setIncrement(int dType)
{
	double adjustedFrequency = frequency[dType] + pPitchFall;
	increment = static_cast<double>(NOSC_PTABLE_SIZE) /  (static_cast<double>(NOSC_SAMPLE_RATE) / adjustedFrequency);
	if(increment < 0)
		increment = 0;
}

void NOSC::initializePhase()
	{ phase = 0; }

// advance envelope frame by one frame
void NOSC::advanceEnvelope()
{
	if(!envFinished)
		envPos++;
	if(envPos >= nEnvFrames[drumType])
		envFinished = true;
}

void NOSC::refreshEnvelope()
{
	envPos = 0;
	envFinished = false;
}

float NOSC::getEnvelopeOutput()
{
	float output = 0.0f;
	
	if(resting) // currently playing a "REST", output just zero
	{
		output = 0.0f;
	}
	else if(envPos < nAttackFrames[drumType]) // in attack stage
	{
		output = ( (float) envPos / (float) nAttackFrames[drumType] ) * peakLevel[drumType];
	}
	else if( envPos < (nAttackFrames[drumType] + nPeakFrames[drumType]) ) // in peak stage
	{
		output = peakLevel[drumType];
	}
	else if( envPos < (nAttackFrames[drumType] + nPeakFrames[drumType] + nDecayFrames[drumType]) ) // in decay stage
	{
		int decayPos = envPos - (nAttackFrames[drumType] + nPeakFrames[drumType]);
		output = peakLevel[drumType] - ((float) decayPos / (float) nDecayFrames[drumType]) * peakLevel[drumType];
	}
	else if(envFinished)
	{
		output = 0.0f;
	}
	
	return output;
}

float NOSC::getPitchOutput()
{
	float pOut = 0.0f;
	int intPhase = (int)pPhase;
	pOut = pTable[ intPhase ] * pLevel;
	return pOut;
}

// get the summed output from wave nTable and envelope
float NOSC::getOutput()
{
	int ph = (int) phase;

	float noiseOut;
	
	if(noiseType[drumType]==1) // if one, use pink noise
		noiseOut = nPinkTable[ph] * noiseLevel;
	else // if zero, use white noise
		noiseOut = nTable[ph] * noiseLevel;	
		
		

	float pitchOut = getPitchOutput() * squareLevel;

	float out;
	
	// output the regular way
	if(!beefUp)
		out = ( noiseOut * gain + pitchOut * gain) * getEnvelopeOutput();
	
	// if beefUp is enabled
	else
		out = limit( 	pitchOut * gain * getEnvelopeOutput() * beefUpFactor +
						noiseOut * gain * getEnvelopeOutput() * beefUpFactorNoise );
	
	historyWriteWait++;
	if(historyWriteWait >= 8)
	{
		pushHistory(out);
		historyWriteWait = 0;
	}	
	
	return out;
	// return ( noiseOut * gain + pitchOut * gain) * getEnvelopeOutput();
}

// same compressor used for regular OSC channel ... might be kind of soft for drums though
float NOSC::compress(float in)
{
	float out = in;
	if(in >= 0.0f && in > compThreshold) // positive value
	{
		float delta = in - compThreshold;
		delta = delta / compRatio;
		out = compThreshold + delta;
		if(out>=0.99f) out = 0.99f;
	}
	else if(in <= 0.0f && in < -compThreshold) // negative value
	{
		float delta = in + compThreshold;
		delta = delta / compRatio;
		out = -compThreshold + delta;
		if(out<=-0.99f) out = -0.99f;
	}
	return out;
}

// alternate BEEFUP processing... brickwall limiter
float NOSC::limit(float in)
{
	float out = in;
	if(out>=0.99f) out = 0.99f;
	else if(out<=-0.99f) out = -0.99f;
	return out;
}

void NOSC::enableBeefUp()
{ beefUp = true; }

void NOSC::disableBeefUp()
{ beefUp = false; }

void NOSC::setBeefUpFactor(float factor)
{
	beefUpFactor = factor; // this works on the pitched element
	beefUpFactorNoise = ((factor - 1.0f) * 0.80f) + 1.0f; // works on the noise element
}

void NOSC::useWhiteNoise()
{ 
	for(int i=0; i<6; i++)
		noiseType[i] = 0;
}

void NOSC::usePinkNoise()
{ 
	for(int i=0; i<6; i++)
		noiseType[i] = 1;
}
	
void NOSC::setKickNoiseType(int type)
	{ noiseType[0] = type; noiseType[3] = type; }
	
void NOSC::setSnareNoiseType(int type)
	{ noiseType[1] = type; noiseType[4] = type; }

void NOSC::setHiHatNoiseType(int type)
	{ noiseType[2] = type; noiseType[5] = type; }
	
void NOSC::setNoiseLevel(float nLevel)
	{ noiseLevel = nLevel; }

void NOSC::setSquareLevel(float sqLevel)
	{ squareLevel = sqLevel; }
	
// push current data to table that keeps an array of historical data
// (used for meter visualization)
void NOSC::pushHistory(float g)
{
	if(g < 0) // flip sign when negative
		g = g * -1.0;
	history[historyWriteIndex] = g; // store value
	historyWriteIndex++;
	if(historyWriteIndex >= NOSC_HISTORY_SIZE)
		historyWriteIndex = 0;
}

// get the current averaged output
// (average derived from (HISTORY_SIZE * historyWriteWait) frames)
float NOSC::getHistoricalAverage()
{
	float sum = 0;
	for(int i=0;i<NOSC_HISTORY_SIZE; i++)
		sum += history[i];
	float avg = static_cast<float>(sum / NOSC_HISTORY_SIZE);
	return avg;
}

void NOSC::clearHistory()
{
	for(int i=0;i<NOSC_HISTORY_SIZE; i++)
		history[i] = 0.0f;
	historyWriteWait = 0;
	historyWriteIndex = 0;
}