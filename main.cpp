//#include <iostream>
//#include <string>
//#include <fstream>
//#include <limits>
//#include "MData.h"
//#include "OSC.h"
//#include "MPlayer.h"
//#include "DelayLine.h"
//#include "MML.h"

#include "GUI.h"

using namespace std;

static const int SAMPLE_RATE = 44100;
static const int TABLE_SIZE = 4096;
static const int ENV_TABLE_SIZE = 1024;
const float TWO_PI = 6.283185307;
const int FRAMES_PER_BUFFER = 256;

int main()
{
	// initialize GUI
	// the GUI class is the main controller of this append
	GUI gui;
	gui.initialize();
	gui.run();

	return 0;
}