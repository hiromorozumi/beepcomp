/// OSC class - Implementation /////////////////////////

#include <iostream>
#include <math.h>
#include "OSC.h"

using namespace std;

const int OSC::OSC_TABLE_SIZE = 8192;
const int OSC::ENV_TABLE_SIZE = 1024;
const double OSC::OSC_SAMPLE_RATE = 44100.0;
const float OSC::TWO_PI = 6.283185307;

OSC::OSC()
{
	table.resize(OSC_TABLE_SIZE); // wave table (vector)
	
	setTable(1); // default - set up a square table
	phase = 0.0;
	increment = 0.0;
	freq = 10.0; // not to set to zero to safeguard
	adjustedFreq = 0;
	detune = 0;
	gain = 0.5f; // default gain
	
	resting = false;
	
	nAttackFrames = 1000;
	nPeakFrames = 1000;
	nDecayFrames = 9600;
	nEnvFrames = nAttackFrames + nPeakFrames + nDecayFrames;
	decayStartPos = nAttackFrames + nPeakFrames;
	
	peakLevel = 0.9f;
	sustainLevel = 0.5f;
	decayAmount = peakLevel - sustainLevel;
	envPos = 0;
	envADfinished = false;
	
	nReleaseFrames = 2000;
	releasePos = 0;
	envRfinished = true;
	
	astroEnabled = false;
	lfoEnabled = false;
	
	// initialize history table
	clearHistory();
}

OSC::~OSC()
{}

void OSC::setTable(int type)
{
	switch(type)
	{
		float maxAmp;
		int oneCycleFrames;
		
		// sine table
		case 0:
			maxAmp = 0.99f;
			for(int i=0; i<OSC_TABLE_SIZE; i++)
			{
				table[i] = sin( TWO_PI * (static_cast<float>(i) / static_cast<float>(OSC_TABLE_SIZE) ) ) * maxAmp;
			}
			break;
			
		// square table
		case 1:
		
			for(int i=0; i<OSC_TABLE_SIZE/2; i++)
			{
				table[i] = 0.85f;
			}
			for(int i=OSC_TABLE_SIZE/2; i<OSC_TABLE_SIZE; i++)
			{
				table[i] = -0.85f;
			}
			break;
		
		// sawthooth wave
		case 2:
			for(int i=0; i<OSC_TABLE_SIZE; i++)
				table[i] = -0.99f  + (static_cast<float>(i) / static_cast<float>(OSC_TABLE_SIZE)) * 1.98f;
			break;

		// triangle wave
		case 3:
			for(int i=0; i<OSC_TABLE_SIZE/2; i++)
				table[i] = -0.99f  + (static_cast<float>(i) / static_cast<float>(OSC_TABLE_SIZE/2)) * 1.98f;	
			for(int i=OSC_TABLE_SIZE/2; i<OSC_TABLE_SIZE; i++)
				table[i] = 0.99f  - (static_cast<float>(i-OSC_TABLE_SIZE/2) / static_cast<float>(OSC_TABLE_SIZE/2)) * 1.98f;
			break;
		
		// sine wave with 3rd, 6th, 9th, 12th harmonics
		case 4:
		
			maxAmp = 0.90f;
			
			// first order sine as base
			for(int i=0; i<OSC_TABLE_SIZE; i++)
				table[i] = sin( TWO_PI * (static_cast<float>(i) / static_cast<float>(OSC_TABLE_SIZE) ) ) * maxAmp;
			
			// then add 3rd harmonics
			oneCycleFrames = OSC_TABLE_SIZE / 3;
			for(int i=0; i<OSC_TABLE_SIZE; i++)
				table[i] += sin(TWO_PI * (static_cast<float>(i%oneCycleFrames) / static_cast<float>(oneCycleFrames)))
							* (maxAmp / 3.0f);
			
			// then add 6rd harmonics
			oneCycleFrames = OSC_TABLE_SIZE / 6;
			for(int i=0; i<OSC_TABLE_SIZE; i++)
				table[i] += sin(TWO_PI * (static_cast<float>(i%oneCycleFrames) / static_cast<float>(oneCycleFrames)))
							* (maxAmp / 6.0f);
							
			// then add 9rd harmonics
			oneCycleFrames = OSC_TABLE_SIZE / 9;
			for(int i=0; i<OSC_TABLE_SIZE; i++)
				table[i] += sin(TWO_PI * (static_cast<float>(i%oneCycleFrames) / static_cast<float>(oneCycleFrames)))
							* (maxAmp / 9.0f);			

			// then add 12th harmonics
			oneCycleFrames = OSC_TABLE_SIZE / 12;
			for(int i=0; i<OSC_TABLE_SIZE; i++)
				table[i] += sin(TWO_PI * (static_cast<float>(i%oneCycleFrames) / static_cast<float>(oneCycleFrames)))
							* (maxAmp / 12.0f);		
	
		break;
	
		// sine wave with 2nd, 3rd, 4th harmonics
		case 5:
		
			maxAmp = 0.68f;
			
			// first order sine as base
			for(int i=0; i<OSC_TABLE_SIZE; i++)
				table[i] = sin( TWO_PI * (static_cast<float>(i) / static_cast<float>(OSC_TABLE_SIZE) ) ) * maxAmp;
			
			// then add 2rd harmonics
			oneCycleFrames = OSC_TABLE_SIZE / 2;
			for(int i=0; i<OSC_TABLE_SIZE; i++)
				table[i] += sin(TWO_PI * (static_cast<float>(i%oneCycleFrames) / static_cast<float>(oneCycleFrames)))
							* (maxAmp / 2.0f);
			
			// then add 3rd harmonics
			oneCycleFrames = OSC_TABLE_SIZE / 3;
			for(int i=0; i<OSC_TABLE_SIZE; i++)
				table[i] += sin(TWO_PI * (static_cast<float>(i%oneCycleFrames) / static_cast<float>(oneCycleFrames)))
							* (maxAmp / 3.0f);
							
			// then add 4rd harmonics
			oneCycleFrames = OSC_TABLE_SIZE / 4;
			for(int i=0; i<OSC_TABLE_SIZE; i++)
				table[i] += sin(TWO_PI * (static_cast<float>(i%oneCycleFrames) / static_cast<float>(oneCycleFrames)))
							* (maxAmp / 4.0f);			

			// then add 5rd harmonics
			oneCycleFrames = OSC_TABLE_SIZE / 5;
			for(int i=0; i<OSC_TABLE_SIZE; i++)
				table[i] += sin(TWO_PI * (static_cast<float>(i%oneCycleFrames) / static_cast<float>(oneCycleFrames)))
							* (maxAmp / 5.0f);								
			
			break;

		// default is sine wave...
		default:
			maxAmp = 0.99f;
			for(int i=0; i<OSC_TABLE_SIZE; i++)
				table[i] = sin( TWO_PI * (static_cast<float>(i) / static_cast<float>(OSC_TABLE_SIZE) ) ) * maxAmp;
			
			break;
			
	}
	
	// limit...
	for(int i=0; i<OSC_TABLE_SIZE; i++)
	{
		if(table[i]>0.99f) table[i] = 0.99f;
		else if(table[i] < -0.99f) table[i] = -0.99f;
	}	
	
}

void OSC::setGain(float g)
	{ gain = g; }

float OSC::getGain()
	{ return gain; }

void OSC::advanceEnvelope()
{
	if(!resting) // currently playing a note
	{
		if(!envADfinished)
		{
			envPos++;
			if(envPos >= nEnvFrames)
				envADfinished = true;
		}
	}
	else // you are on a rest now
	{
		if(!envRfinished)
		{
			releasePos++;
			if(releasePos >= nReleaseFrames)
			{
				envRfinished = true;
			}
		}
	}
}

// go back to the beginning of envelope
void OSC::refreshEnvelope()
{
	envPos = 0;
	envADfinished = false;
	releasePos = 0;
	envRfinished = false;
}

float OSC::getEnvelopeOutput()
{
	float output;
	
	if(!resting) // when you're not on a rest - playing a note now
	{
		if(envPos < nAttackFrames) // in attack stage
		{
			output = peakLevel * 
			(static_cast<float>(envPos) / static_cast<float>(nAttackFrames));
		}
		else if(envPos < (nAttackFrames + nPeakFrames)) // in peak stage
		{
			output = peakLevel;
		}
		else if(envPos < nEnvFrames) // in decay stage
		{
			output = peakLevel - decayAmount * ( static_cast<float>(envPos - decayStartPos) / static_cast<float> (nDecayFrames) );
		}
		else if(envADfinished) // in sustain stage
		{
			output = sustainLevel;
		}
	}
	
	// if resting flag is on, means you're in release stage
	if(resting)
	{
		if(!envRfinished)
			output = sustainLevel * ( static_cast<float>(nReleaseFrames - releasePos) / static_cast<float>(nReleaseFrames) );
		else
			output = 0.0f;
	}
	
	return output;
}

void OSC::setToRest()
{
	resting = true;
}

void OSC::advance()
{
	// advance on the sample table
	phase += increment;
	while(phase >= OSC_TABLE_SIZE)
	{
		phase -= OSC_TABLE_SIZE;
	}
	
	// if astro is enabled, process and adjust frequency
	if(astroEnabled)
	{
		adjustedFreq = astro.process(freq);
		if(astro.stateChanged())
			setIncrement(adjustedFreq);
	}
	// if LFO is enabled, process and adjust frequency
	else if(lfoEnabled)
	{
		adjustedFreq = lfo.process(freq);
		if(adjustedFreq < 10.0)
			adjustedFreq = 10.0;
		setIncrement(adjustedFreq);
	}
	
	// advance envelope also
	advanceEnvelope();
}

void OSC::setNewNote(double newFreq)
{
	setFrequency(newFreq);
	initializePhase();
	refreshEnvelope();
	resting = false;
}

// set the frequency and phase increment at once
void OSC::setFrequency(double noteFreq)
{
	freq = noteFreq;
	adjustedFreq = freq;
	setIncrement(freq);
	
	// refresh LFO so it starts from beginning
	if(lfoEnabled)
		lfo.refresh();
	
	// refresh Astro effect so it starts from beginning
	if(astroEnabled)
		astro.refresh();
}

// set the phase increment to travel across wave table every frame
void OSC::setIncrement(double noteFreq)
{
	// calculate with detune
	adjustedFreq = noteFreq + detune;

	// finally, set the phase increment
	increment = ( static_cast<double>(OSC_TABLE_SIZE) / ( OSC_SAMPLE_RATE / adjustedFreq ) );
	
	if(increment < 0)
		increment = 0;
}

void OSC::enableAstro()
	{ astroEnabled = true; }

void OSC::disableAstro()
	{ astroEnabled = false; }
	
void OSC::setAstroSpeed(int nCyclesPerSecond)
	{ astro.setSpeed(nCyclesPerSecond); }

void OSC::enableLFO()
	{ lfoEnabled = true; }

void OSC::disableLFO()
	{ lfoEnabled = false; }
	
void OSC::setLFOwaitTime(int milliseconds)
	{ lfo.setWaitTime(milliseconds); }

void OSC::setLFOrange(int cents)
	{ lfo.setRange(cents); }

void OSC::setLFOspeed(double cyclePerSeconds)
	{ lfo.setSpeed(cyclePerSeconds); }

void OSC::setAttackTime(int attackTimeMS)
{
	nAttackFrames = static_cast<int> (OSC_SAMPLE_RATE * attackTimeMS / 1000.0);
	readjustEnvParams();
}

void OSC::setPeakTime(int peakTimeMS)
{
	nPeakFrames = static_cast<int> (OSC_SAMPLE_RATE * peakTimeMS / 1000.0);
	readjustEnvParams();
}

void OSC::setDecayTime(int decayTimeMS)
{
	nDecayFrames = static_cast<int> (OSC_SAMPLE_RATE * decayTimeMS / 1000.0);
	readjustEnvParams();
}

void OSC::setReleaseTime(int releaseTimeMS)
{ 
	nReleaseFrames = static_cast<int> (OSC_SAMPLE_RATE * releaseTimeMS / 1000.0);
	readjustEnvParams();
}
	
void OSC::setPeakLevel(float peakLV)
{
	peakLevel = peakLV;
	readjustEnvParams();
}

void OSC::setSustainLevel(float sustainLV)
{
	sustainLevel = sustainLV;
	readjustEnvParams();
}	

void OSC::setEnvelope(int attackTimeMS, int peakTimeMS, int decayTimeMS, int releaseTimeMS,
						float peakLV, float sustainLV)
{
	setAttackTime(attackTimeMS);
	setPeakTime(peakTimeMS);
	setDecayTime(decayTimeMS);
	setReleaseTime(releaseTimeMS);
	setPeakLevel(peakLV);
	setSustainLevel(sustainLV);
	readjustEnvParams();
}

void OSC::readjustEnvParams()
{
	nEnvFrames = nAttackFrames + nPeakFrames + nDecayFrames;
	decayStartPos = nAttackFrames + nPeakFrames;
	decayAmount = peakLevel - sustainLevel;	
}
	
void OSC::initializePhase()
{
	phase = 0;
}

float OSC::getOutput()
{
	/*
	// trying out linear interpolation...
	deviation = modf(phase, &dblPhaseIntPart);
	intPhase = static_cast<int>(dblPhaseIntPart);
	intPhaseNext = intPhase++;
	if(intPhaseNext >= OSC_TABLE_SIZE)
		intPhaseNext = 0;
	
	float out = (table[intPhase] + (table[intPhaseNext] - table[intPhase]) * deviation) 
		* getEnvelopeOutput() * gain;
	*/
	
	int ph = static_cast<int> (phase);
	// cout << "phase=" << phase << "..";
	
	float out = table[ph] * getEnvelopeOutput() * gain;
	
	historyWriteWait++;
	if(historyWriteWait >= 8)
	{
		pushHistory(out);
		historyWriteWait = 0;
	}
	
	return out;
}

// push current data to table that keeps an array of historical data
// (used for meter visualization)
void OSC::pushHistory(float g)
{
	if(g < 0) // flip sign when negative
		g = g * -1.0;
	history[historyWriteIndex] = g; // store value
	historyWriteIndex++;
	if(historyWriteIndex >= OSC_HISTORY_SIZE)
		historyWriteIndex = 0;
}

// get the current averaged output
// (average derived from (HISTORY_SIZE * historyWriteWait) frames)
float OSC::getHistoricalAverage()
{
	float sum = 0;
	for(int i=0;i<OSC_HISTORY_SIZE; i++)
		sum += history[i];
	float avg = static_cast<float>(sum / OSC_HISTORY_SIZE);
	return avg;
}

void OSC::clearHistory()
{
	for(int i=0;i<OSC_HISTORY_SIZE; i++)
		history[i] = 0.0f;
	historyWriteWait = 0;
	historyWriteIndex = 0;
}