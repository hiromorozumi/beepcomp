// NOSC.cpp ////////////////////////////////////////////
// NOSC class - Implementation /////////////////////////

#include <algorithm>
#include <iostream>
#include <cstdlib>
#include <windows.h>
#include <vector>
#include "NOSC.h"

using namespace std;

const int NOSC::NOSC_NTABLE_SIZE = 4096;
const int NOSC::NOSC_PTABLE_SIZE = 4096;
const double NOSC::NOSC_SAMPLE_RATE = 44100.0;

NOSC::NOSC()
{	
	// set up a noise wave nTable
	nTable.resize(NOSC_NTABLE_SIZE);
	pTable.resize(NOSC_PTABLE_SIZE);
	setTable();
	
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
	setDrumTone(0, 3, 30,  20,  0.8, 200.0,  50  , 0.9, 2.0); // kick
	setDrumTone(1, 3, 20, 120, 0.95, 720.0,  100 , 0.85, 1.2); // snare
	setDrumTone(2, 3, 17,  3,  0.5, 2400.0, 1   , 0.2, 2.0); // hat
	
	setDrumTone(3, 3, 30,  20,  0.4, 200.0,  50  , 0.5, 2.0); // kick - quiet
	setDrumTone(4, 3, 20, 120, 0.5, 720.0,  100 , 0.45, 1.2); // snare - quiet
	setDrumTone(5, 3, 17,  3,  0.2, 2400.0, 1   , 0.1, 2.0); // hat - quiet

	envPos = 0;	
	
	envFinished = false;
	
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
	
	for(int i=0; i<midTablePoint; i++)
		pTable[i] = 0.95f;
	for(int i=midTablePoint; i<NOSC_PTABLE_SIZE; i++)
		pTable[i] = -0.95f;
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
	
	cout << "attack=" << nAttackFrames[dType] << " peakTime=" << nPeakFrames[dType] << " decayTime" << nDecayFrames[dType] << " envFrames=" << nEnvFrames[dType] << endl;
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
	float noiseOut = nTable[ph];
	float pitchOut = getPitchOutput();
	float out = ( noiseOut * gain + pitchOut * gain) * getEnvelopeOutput();
	
	historyWriteWait++;
	if(historyWriteWait >= 8)
	{
		pushHistory(out);
		historyWriteWait = 0;
	}	
	
	return out;
	// return ( noiseOut * gain + pitchOut * gain) * getEnvelopeOutput();
}

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