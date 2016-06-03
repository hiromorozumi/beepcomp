// MPlayer.h //////////////////////////////////////////
// MPlayer Class - Definition /////////////////////////

#ifndef MPLAYER_H
#define MPLAYER_H

// forward declared dependencies

class MData;
class DData;
class OSC;
class NOSC;
class DelayLine;

#include <string>
#include "OSC.h"
#include "NOSC.h"
#include "DelayLine.h"
#include "MData.h"
#include "DData.h"
#include <portaudio.h>

/*-----[BCPLAYER][INCLUDESFXHEADER]-----*/

//#include "SFX.h"


class MPlayer
{
	
static const int SAMPLE_RATE;
static const int FRAMES_PER_BUFFER;
	
public:
	
	OSC osc[9]; // can use nine channels max
	NOSC nosc; // noise channel
	DelayLine delay[2]; // stereo, thus 2 channels
	MData data[9]; // this holds the music data
	DData ddata; // this holds the drum track data
	
	bool playing;
	bool enabled[9];
	bool dEnabled;
	bool silenced[9];
	bool dSilenced;
	long framePos;
	long songLastFrame;
	long songLastFramePure;
	long bookmark;
	
	float masterGain;
	int tableType;
	float masterOutCap;
	float compThreshold;
	float compRatio;
	bool delayEnabled;
	bool ringModEnabled[9];
	int ringModFeed[9];
	bool ringModMute[9];
	bool loopEnabled;
	int repeatsRemaining;
	bool songFinished;

	bool channelDone[9];
	bool dChannelDone;
	int remainingFrames[9];
	int dRemainingFrames;
	double freqNote[9];
	int noteIndex[9];
	int dNoteIndex;
	int eventIndex[9];
	int dEventIndex;
	int currentDrumNote;

	float sndBuffer[88200];
	
	PaStreamParameters outputParameters;
	PaStream* stream;
	PaError err;
	bool appIsExiting;

	MPlayer();
	~MPlayer();

	void setNewNote(int channel, double freq);
	void setToRest(int channel);
	void setNewDrumHit(int drumType);
	void restDrum();
	void setAllChannelGain(float gain1, float gain2, float gain3, float gain4, float gain5, 		float gain6, float gain7, float gain8, float gain9, float gainD);
	void setChannelGain(int channel, float gain);
	float getChannelGain(int channel);
	void setDChannelGain(float gain);
	float getDChannelGain();
	void enableChannels(bool enable1, bool enable2, bool enable3, bool enable4, bool enable5, 		bool enable6, bool enable7, bool enable8, bool enable9, bool enableD);
	void enableChannel(int channel);
	void disableChannel(int channel);
	void enableDrumChannel();
	void disableDrumChannel();
	void silenceChannel(int channel);
	void silenceDrumChannel();
	void activateChannel(int channel);
	void activateDrumChannel();
	void enableLooping();
	void disableLooping();
	void setRepeatsRemaining(int value);
	void advance();
	float getMix(int channel);
	float compress(float input);
	float getMasterGain();
	void setMasterGain(float g);

	int playerCallback (	const void *inputBuffer, void *outputBuffer,
							unsigned long framesPerBuffer,
							const PaStreamCallbackTimeInfo* timeInfo,
							PaStreamCallbackFlags statusFlags );
	
	// this static function will only redirect to audio engine callback (above)
	static int paCallback(  const void *inputBuffer, void *outputBuffer,
							unsigned long framesPerBuffer,
							const PaStreamCallbackTimeInfo* timeInfo,
							PaStreamCallbackFlags statusFlags,
							void *userData )
			{
				return ((MPlayer*)userData)
					->playerCallback(inputBuffer, outputBuffer,
					framesPerBuffer, timeInfo, statusFlags);
			}
	
	void playerStoppedCallback();
	
	// this static function will only redirect to stream-stopped callback (above)
	static void paStoppedCallback( void *userData )
			{	cout << "stream finished callback called! step 1\n"; // DEBUG
				((MPlayer*)userData)->playerStoppedCallback(); }

	void handlePaError( PaError e );
	void initialize();
	void restartStream();
	void stopStream();
	void declareAppTermination();
	std::string getStreamStateString();
	bool getStreamState();
	void resetForNewSong();
	void close();
	void start();
	void pause();
	void restart();
	void cleanUpForNewFile();
	void goToBeginning();
	long getSongLastFrame();
	long getSongLastFramePure();
	long getFramePos();
	bool isPlaying();
	int getTableType();
	void setTableType(int type);
	bool delayIsEnabled();
	void enableDelay();
	void disableDelay();
	void setAstro(int channel, int nCyclesPerSecond);
	void disableAstro(int channel);
	void enableRingMod(int channel, int modulatorChannel);
	void disableRingMod(int channel);
	void processEvent(int channel, int eType, int eParam);
	void processDrumEvent(int eType, int eParam);
	std::string exportToFile(string filename);
	int fillExportBuffer(float* buffer, int framesToWrite, long startFrame, int songFrameLen);
	float getHistoricalAverage(int channel);
	void seek(long destination);
	void seekAndStart(long destination);
	float getProgressRatio();
	long getNextSeekPoint();
	long getPreviousSeekPoint();
	void setBookmark(long bm);
	long getBookmark();
	bool reachedSongLastFramePure();
	bool isSongFinished();
	
	
	
	/*-----[BCPLAYER][ADDSFXPROPERTIES]-----*/
	
	// void bindSFX(SFX* sfxObj);
	// SFX* sfx;

};

#endif