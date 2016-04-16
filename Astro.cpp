// Astro.cpp /////////////////////////////////////////////
// Astro class - implementation //////////////////////////

#include <algorithm>
#include "Astro.h"

using namespace std;

const int Astro::ASTRO_SAMPLE_RATE = 44100;

Astro::Astro()
{
	frameCount = 0;
	processedFrequency = 440.0;
	oneCycleFrames = 4410;
	setSpeed(12); // default - 10 cycles per second
	statusChanged = false;
}

Astro::~Astro()
{}

void Astro::setSpeed(int nCyclesPerSecond)
{
	nCyclesPerSecond = min(100, nCyclesPerSecond);
	nCyclesPerSecond = max(1, nCyclesPerSecond);
	oneCycleFrames = ASTRO_SAMPLE_RATE / nCyclesPerSecond;
	middlePoint = oneCycleFrames / 2;
}

double Astro::process(double freq)
{
	if( frameCount == middlePoint ) // at middle of range - go to octave higher
	{
		processedFrequency = freq * 2.0;
		statusChanged = true;
	}
	else if(frameCount == 0 ) // back to beginning - go to base octave
	{
		processedFrequency = freq;
		statusChanged = true;
	}
	else
		statusChanged = false;
	
	// update frame count
	frameCount++;
	if(frameCount >= oneCycleFrames)
		frameCount = 0;
	
	return processedFrequency;
}

// whenever Astro changes the frequency, statusChanged flag turns on
bool Astro::stateChanged()
	{ return statusChanged; }

// forces the Astro effect cycle to begin from beginning (frameCount = 0)
void Astro::refresh()
	{ frameCount = 0;}