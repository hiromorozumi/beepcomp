// Fall class //////////////////////////////////////////////////////////////
// Implementation //////////////////////////////////////////////////////////

#include <iostream>
#include <math.h>
#include "Fall.h"

using namespace std;

const double Fall::FALL_SAMPLE_RATE = 44100.0;
const double Fall::FREQ_FLOOR = 20.0;

Fall::Fall()
{
	// set default params for now...
	enabled = false;
	fallSpeed = 600.0;
	fallFactor = 1.0;
	octTraveled = 0.0;
	octDeltaPerFrame = getDeltaPerFrame(fallSpeed);
	waitPos = 0;
	waitFrames = 0;
}

void Fall::setToDefault()
{
	// set default params for now...
	enabled = false;
	fallSpeed = 600.0;
	fallFactor = 1.0;
	octTraveled = 0.0;
	octDeltaPerFrame = getDeltaPerFrame(fallSpeed);
	waitPos = 0;
	waitFrames = 0;
}

double Fall::getDeltaPerFrame(double fSpeed)
{
	return (fSpeed / 1200.0) / FALL_SAMPLE_RATE;
}

double Fall::setSpeed(double fSpeed)
{
	fallSpeed = fSpeed;
	octDeltaPerFrame = getDeltaPerFrame(fSpeed);
}

double Fall::setWaitTime(double waitTimeMS)
{
	waitFrames = static_cast<int>(waitTimeMS / 1000.0 * FALL_SAMPLE_RATE);
}

void Fall::refresh()
{
	waitPos = 0;
	octTraveled = 0.0;
	fallFactor = 1.0;
}

void Fall::start()
{
	refresh();
	enabled = true;
}

void Fall::stop()
{
	enabled = false;
	refresh();
}

double Fall::process(double freq)
{
	if(waitPos < waitFrames)
	{
		waitPos++;
		return freq;
	}
	else
	{
		octTraveled += octDeltaPerFrame;
		if(octTraveled > 8.0) octTraveled = 8.0; // safeguard!
		freq *= pow(0.5, octTraveled);
		if(freq < FREQ_FLOOR) freq = FREQ_FLOOR; // safeguard!
		return freq;
	}
}

//////////////////////////////////////////////////////////////

const double Rise::RISE_SAMPLE_RATE = 44100.0;
const double Rise::FREQ_FLOOR = 20.0;

Rise::Rise()
{
	// set default params for now...
	enabled = false;
	pos = 0;
	riseSpeed = 1200.0;
	riseRange = 100.0;
	riseFactor = 1.0;
	octDeviation = 100.0 / 1200.0;
	octDeltaPerFrame = getDeltaPerFrame(riseSpeed);
}

void Rise::setToDefault()
{
	enabled = false;
	pos = 0;
	riseSpeed = 1200.0;
	riseRange = 100.0;
	riseFactor = 1.0;
	octDeviation = 100.0 / 1200.0;
	octDeltaPerFrame = getDeltaPerFrame(riseSpeed);	
}

double Rise::getDeltaPerFrame(double rSpeed)
{
	return (rSpeed / 1200.0) / RISE_SAMPLE_RATE;
}

void Rise::setSpeed(double rSpeed)
{
	riseSpeed = rSpeed;
	octDeltaPerFrame = getDeltaPerFrame(rSpeed);
}

void Rise::setRange(double rRange)
{
	riseRange = rRange;
	octDeviation = rRange / 1200.0;
}

void Rise::refresh()
{
	setRange(riseRange);
	pos = 0;
}

void Rise::start()
{
	refresh();
	enabled = true;
}

void Rise::stop()
{
	enabled = false;
	refresh();
}

double Rise::process(double freq)
{
	pos++;
	
	if(octDeviation == 0.0)
	{
		return freq;
	}
	else
	{
		octDeviation -= octDeltaPerFrame;
		if(octDeviation < 0.0) octDeviation = 0.0; // reached target freq!
		freq *= pow(0.5, octDeviation);
		if(freq < FREQ_FLOOR) freq = FREQ_FLOOR; // safeguard!
		return freq;
	}
}