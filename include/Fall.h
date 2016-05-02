#ifndef FALL_H
#define FALL_H


class Fall
{

public:

	static const double FALL_SAMPLE_RATE;
	static const double FREQ_FLOOR;
	
	bool enabled;
	double fallSpeed;
	double fallFactor;
	double octTraveled;
	double octDeltaPerFrame;
	int waitPos;
	int waitFrames;
	
	Fall();
	~Fall(){}

	void setToDefault();
	double getDeltaPerFrame(double fSpeed);
	double setSpeed(double fSpeed);
	double setWaitTime(double waitTimeSec);
	void refresh();
	void start();
	void stop();
	double process(double freq);

};

class Rise
{

public:

	static const double RISE_SAMPLE_RATE;
	static const double FREQ_FLOOR;
	
	int pos;
	bool enabled;
	double riseSpeed;
	double riseRange;
	double riseFactor;
	double octDeviation;
	double octDeltaPerFrame;
	
	Rise();
	~Rise(){}

	void setToDefault();
	double getDeltaPerFrame(double rSpeed);
	void setSpeed(double rSpeed);
	void setRange(double rRange);
	void refresh();
	void start();
	void stop();
	double process(double freq);

};

#endif