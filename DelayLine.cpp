// DelayLine class - implementation //////////////////////////////////////

#include "DelayLine.h"

const int DelayLine::DELAY_TABLE_SIZE = 88200;
const int DelayLine::DELAY_SAMPLE_RATE = 44100;

DelayLine::DelayLine()
{
	// set up tables (vector)
	buffer1.resize(DELAY_TABLE_SIZE);
	buffer2.resize(DELAY_TABLE_SIZE);

	// set max buffer lengths as default
	buffer1len = DELAY_TABLE_SIZE;
	buffer2len = DELAY_TABLE_SIZE;
	totalDelayFrames = 0;
	readIndex1 = 1;
	writeIndex1 = 0;
	readIndex2 = 1;
	writeIndex2 = 0;
	out1 = 0.0f;
	out2 = 0.0f;
	outGain1 = 0.5f;
	outGain2 = 0.2f;
}

DelayLine::~DelayLine()
{}

//
// function to set delay parameters
// firstDelayTime / delayTime in milliseconds, delayGain in float (about 0.2f is recommended)
//
void DelayLine::setParameters(int firstDelayTime, int delayTime, float delayGain)
{
	if(firstDelayTime>=0 && delayTime>=0) // if negative was passed, don't make change
	{
		buffer1len = (DELAY_SAMPLE_RATE * firstDelayTime) / 1000;
		buffer2len = (DELAY_SAMPLE_RATE * delayTime) / 1000;

		totalDelayFrames = buffer1len + buffer2len * 2;

		// reset the indexes, too, while you're at it (just to be safe)
		readIndex1 = 1;
		writeIndex1 = 0;
		readIndex2 = 1;
		writeIndex2 = 0;
	}

	if(delayGain > -0.001f) // if negative was passed, don't make change
	{
		outGain1 = delayGain;
		outGain2 = delayGain * 0.4f;
	}
}

// call to update delayline every frame
// returns the delay output for current frame
float DelayLine::update(float input)
{
	// handle buffer1 write & read
	buffer1[writeIndex1] = input;
	out1 = buffer1[readIndex1];

	// handle buffer2 write & read
	buffer2[writeIndex2] = out1;
	out2 = buffer2[readIndex2];

	// update buffer1 + buffer2 indexes
	writeIndex1++;
	if(writeIndex1 >= buffer1len)
		writeIndex1 = 0;
	readIndex1++;
	if(readIndex1 >= buffer1len)
		readIndex1 = 0;
	writeIndex2++;
	if(writeIndex2 >= buffer2len)
		writeIndex2 = 0;
	readIndex2++;
	if(readIndex2 >= buffer2len)
		readIndex2 = 0;

	// return the summed output for the whole delay line
	return min(out1 * outGain1, 1.0f) + min(out2 * outGain2, 1.0f);
}

// erase all buffer tables
void DelayLine::clearBuffer()
{
	for(int i=0; i<DELAY_TABLE_SIZE; i++)
		buffer1[i] = 0;
	for(int i=0; i<DELAY_TABLE_SIZE; i++)
		buffer2[i] = 0;

	// reset the indexes, too
	readIndex1 = 1;
	writeIndex1 = 0;
	readIndex2 = 1;
	writeIndex2 = 0;
}
