// LFO.cpp ////////////////////////////////////////
// LFO Class - Implementation /////////////////////

#include <iostream>
#include <math.h>
#include <vector>
#include "LFO.h"

using namespace std;

const double LFO::LFO_SAMPLE_RATE = 44100.0;
const int LFO::LFO_TABLE_SIZE = 4096;
const double LFO::LFO_TWO_PI = 6.28318530718;

LFO::LFO()
{
	// set table size, then set table to sine wave table
	table.resize(LFO_TABLE_SIZE);
	setTable(0); // default table - sine wave
	
	// initialize variables with default settings
	phase = 0;
	waitPos = 0;
	setWaitTime(200); // default wait - n milliseconds
	setRange(24); // default LFO range - n cents of semitone
	setSpeed(6.0); // defalt LFO speed - n cycles per second 
}

LFO::~LFO()
{}
	
void LFO::setTable(int type)
{
	switch(type)
	{
		// 0 - sine wave
		case 0:
		
			for(int i=0; i<LFO_TABLE_SIZE; i++)
			{
				double radian = ( static_cast<double>(i) / static_cast<double>(LFO_TABLE_SIZE) ) * LFO_TWO_PI;
				table[i] = sin(radian);
			}

			break;
			
		case 1:
		
			break;
			
		default:
			break;
	}
	
}

void LFO::setWaitTime(int milliseconds)
{
	waitTimeMSec = milliseconds;
	waitFrames = LFO_SAMPLE_RATE * milliseconds / 1000.0;
}

void LFO::setRange(int cents)
{
	rangeCents = static_cast<double>(cents);
}

void LFO::setSpeed(double cyclesPerSeconds)
{
	cyclesPerSec = cyclesPerSeconds;
	increment = static_cast<double>(LFO_TABLE_SIZE) * cyclesPerSec / LFO_SAMPLE_RATE;
}

// restart LFO mechanism - at start of every note
void LFO::refresh()
{
	phase = 0;
	waitPos = 0;
}

// takes the original frequency value from oscillator
// then returns the value processed with LFO
double LFO::process(double originalFreq)
{
	double freqToReturn;
	
	// if still in 'wait' stage - no LFO processing
	if(waitPos < waitFrames)
	{
		waitPos++;
		freqToReturn = originalFreq;
	}
	// wait stage is over - will return LFO-processed output
	else
	{
		double LFOfactor = pow(2.0, (static_cast<double>(table[(int)phase]) * rangeCents / 100.0 / 12.0 ) );
		freqToReturn = originalFreq * LFOfactor;

		// advance phase - let LFO move through table
		phase += increment;
		if(phase >= LFO_TABLE_SIZE)
		phase -= LFO_TABLE_SIZE;		
	}
	
	return freqToReturn;
}