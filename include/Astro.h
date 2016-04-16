// Astro.h /////////////////////////////////////////////
// Astro class - definition ////////////////////////////

#ifndef ASTRO_H
#define ASTRO_H

class Astro
{

static const int ASTRO_SAMPLE_RATE;

public:

	int frameCount;
	int oneCycleFrames;
	int middlePoint;
	double processedFrequency;
	bool statusChanged;

	Astro();
	~Astro();

	void setSpeed(int nCyclesPerSecond);
	double process(double freq);
	bool stateChanged();
	void refresh();
};

#endif