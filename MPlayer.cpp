// MPlayer.cpp ////////////////////////////////////////
// MPlayer Class - Implementation /////////////////////

// include dependencies

#include <algorithm>
#include <iostream>
#include <vector>
#include <string>
#include <math.h>
#include <cstdio>
#include <windows.h> // DEBUG

/*-----[BCPLAYER][STARTCOMMENTOUT]-----*/

#include <sndfile.h>
#include <lame/lame.h>

/*-----[BCPLAYER][ENDCOMMENTOUT]-----*/

#include "MPlayer.h"

const int MPlayer::SAMPLE_RATE = 44100;
const int MPlayer::FRAMES_PER_BUFFER = 256;

using namespace std;

// real port audio callback function
// (redirected from 'paCallback')

int MPlayer::playerCallback (
		const void *inputBuffer, void *outputBuffer,
		unsigned long framesPerBuffer,
		const PaStreamCallbackTimeInfo* timeInfo,
		PaStreamCallbackFlags statusFlags )
{
	float* out = static_cast<float*>(outputBuffer);
	// static_cast<int>(framesPerBuffer); // since this has no effect..
	static_cast<void>(timeInfo);
	static_cast<void>(statusFlags);
	static_cast<void>(inputBuffer);

	float soundAmplitudeLeft;
	float soundAmplitudeRight;

	for(unsigned long ii=0; ii<framesPerBuffer; ii++)
	{
		if(!playing) // if player is not playing or finished playing, just pass 0
		{
			soundAmplitudeLeft = 0.0f;
			soundAmplitudeRight = 0.0f;
			
			// place holder for BCPlayer...
			// in BCPlayer, if music is stopped, you get sound effects only :)
			/*-----[BCPLAYER][CALLSFXINCALLBACK]-----*/
			
			// soundAmplitudeLeft = sfx->getOutput(0);
			// soundAmplitudeRight = sfx->getOutput(1);
			
		}
		// player IS playing... get mix - output - advance frame - check for end of channel
		else
		{
			// get mix of all 9 channels (plus drums) at current position
			soundAmplitudeLeft = getMix(0); // get mix for LEFT channel
			soundAmplitudeRight = getMix(1); // get mix for RIGHT channel

			// if reached end of note, go to next index (for regular channels ch0 - 2)
			for(int i=0; i<9; i++)
			{
				if(!channelDone[i])
				{
					
					// if there are event requests, digest those first
					bool eventsDone = false;
					
					while(!eventsDone)
					{
						// if next event in vector is set to happen at this frame pos, process
						if( (data[i].eventFrame[eventIndex[i]] <= framePos) && (eventIndex[i] < data[i].nEvents) )
						{
							processEvent(i, data[i].eventType[eventIndex[i]], 
											data[i].eventParam[eventIndex[i]]);
							eventIndex[i]++;
						}
						else
							eventsDone = true;
					}					
					
					remainingFrames[i]--;
					if(remainingFrames[i] <= 0)
					{
						noteIndex[i]++;

						// and if you get to the end of MML signal (freq = -1.0), set flag
						if(data[i].freqNote[noteIndex[i]] < 0)
						{
							channelDone[i] = true;
							setToRest(i); // set to rest.. and let delay finish
							// disableChannel(i); // disable this channel
						}
						else
						{
							remainingFrames[i] = data[i].len[noteIndex[i]];
							freqNote[i] = data[i].freqNote[noteIndex[i]];

							// if this is a rest (freq = 65535), set this channel to rest
							if(freqNote[i]==65535.0)
								setToRest(i);
							// otherwise, this is a valid note - so set this note
							else
								setNewNote(i, freqNote[i]);
						}
					}
				}
			}

			// now handle drum channel!
			if(!dChannelDone)
			{
				// if there are event requests, digest those first
				bool eventsDone = false;

				while(!eventsDone)
				{
					// if next event in vector is set to happen at this frame pos, process
					if( (ddata.eventFrame[dEventIndex] <= framePos) && (dEventIndex < ddata.nEvents) )
					{
						// cout << "event found! for drums" << endl;
						processDrumEvent(ddata.eventType[dEventIndex], ddata.eventParam[dEventIndex]);
						dEventIndex++;
					}
					else
						eventsDone = true;
				}
				
				dRemainingFrames--;
				if(dRemainingFrames <= 0)
				{
					dNoteIndex++; // move onto the next drum note index

					// and if you get to the end of MML signal (drumNote = -1.0), set flag
					if(ddata.drumNote[dNoteIndex] < 0 || dNoteIndex >= ddata.getSize())
					{
						dChannelDone = true;
						restDrum(); // rest.. and let delay effect finish off
						// disableDrumChannel(); // disable this channel
					}
					else // not at end yet.. set new drum hit
					{
						dRemainingFrames = ddata.len[dNoteIndex];
						currentDrumNote = ddata.drumNote[dNoteIndex];
						setNewDrumHit(currentDrumNote);

						// if this is a rest (freq = 65535), set flag
						if(currentDrumNote == 65535)
						{
							restDrum();
						}
					}
				}
			}

			// if all channels have reached end... and loop is enabled, go back to beginning
			if(	channelDone[0] && channelDone[1] && channelDone[2] &&
				channelDone[3] && channelDone[4] && channelDone[5] &&
				channelDone[6] && channelDone[7] && channelDone[8] && dChannelDone)
			{	
				// final point check!
				// ... if there are events to process at this final moment... process them here
				
				for(int i=0;i<9;i++)
				{	
					bool eventsDone = false;
					while(!eventsDone)
					{
						// if next event in vector is set to happen at this frame pos, process
						if( (eventIndex[i] < data[i].nEvents) )
						{
							processEvent(i, data[i].eventType[eventIndex[i]], data[i].eventParam[eventIndex[i]]);
							eventIndex[i]++;
						}
						else
							eventsDone = true;
					}
				}
				
				// process drum events pending at the final point before loop
				bool eventsDone = false;
				while(!eventsDone)
				{
					// if next event in vector is set to happen at this frame pos, process
					if( (dEventIndex < ddata.nEvents) )
					{
						processDrumEvent(ddata.eventType[dEventIndex], ddata.eventParam[dEventIndex]);
						dEventIndex++;
					}
					else
						eventsDone = true;
				}
				
				if(loopEnabled)
				{
					cout << "Looping back to beginning...\n";
					// enable channels again
					enableChannels(true, true, true, true, true, true, true, true, true, true);

					// enable drum channel
					enableDrumChannel();

					// go back to the beginning
					goToBeginning();
				}
				else if(!loopEnabled && repeatsRemaining > 1) // if repeat times is left.. process
																// when set to 1, it's last time
				{
					repeatsRemaining--;
					cout << "Back to beginning... repeats remaining = " << repeatsRemaining << endl;
					
					// enable channels again
					enableChannels(true, true, true, true, true, true, true, true, true, true);

					// enable drum channel
					enableDrumChannel();

					// go back to the beginning
					goToBeginning();					
				}
			}

			// if song is not finished, update frame position - advance player
			if(!songFinished)
			{
				// update player position
				framePos++;
				advance();

				// if you have reached the absolute last frame position of the song
				// (including last delay effects) - only then end the track officially
				if(framePos >= songLastFrame)
				{
					songFinished = true;
					playing = false;
					
					/*-----[BCPLAYER][STARTCOMMENTOUT]-----*/
					
					// DEBUG
					cout << "song officially finished!\n";
					cout << "framePos = " << framePos << endl;
					cout << "getSongLastFrame() = " << getSongLastFrame() << endl;

					for(int i=0; i<9; i++)
						cout << "channel " << i << " length = " << data[i].totalFrames << "\n";
					cout << "d channel length = " << ddata.totalFrames << "\n";
					
					/*-----[BCPLAYER][ENDCOMMENTOUT]-----*/
				}
			}


		}

		//
		//	output to port audio buffer - variable 'sampleAmplitude' (float value for one frame)
		//

		*out = soundAmplitudeLeft; // write LEFT channel mix to buffer
		out++; // move buffer pointer
		*out = soundAmplitudeRight; // write RIGHT channel mix to buffer
		out++; // move buffer pointer
	}

	return paContinue;
}

////////////////////////////////////////////////////////

// portaudio stream has been dropped! ... attempt to reopen stream...
void MPlayer::playerStoppedCallback ()
{
	cout << "Stream finished callback called! step 2 :)\n";
	if(!appIsExiting)
		restartStream();
}

////////////////////////////////////////////////////////

MPlayer::MPlayer()
{
	appIsExiting = false; // when this is true, paStreamFinishedCallback will NOT automatically reopen stream
	
	for(int i=0;i<9;i++)
		silenced[i] = false;

	dSilenced = false;
	enableChannels(true, true, true, true, true, true, true, true, true, true);
	enableDrumChannel();

	for(int i=0;i<9;i++)
		osc[0].detune = 0;

	tableType = 1; // OSC's default type - square table
	masterGain = 0.7f;
	masterOutCap = 0.88f;

	compThreshold = 0.5f;
	compRatio = 8;

	framePos = 0;
	songLastFrame = 0;
	songLastFramePure = 0;
	bookmark = 0;
	for(int i=0; i<9; i++)
		eventIndex[i] = 0;
	dEventIndex = 0;

	// call this function once to set various parameters settings to default
	resetForNewSong();
}

MPlayer::~MPlayer()
{}

void MPlayer::handlePaError(PaError e)
{
	cout << "PortAudio Error - " << Pa_GetErrorText( e ) << endl;
}

void MPlayer::initialize()
{
	// initialize variables
	framePos = 0;	// the index for the audio data frame
	songLastFrame = 0;
	songLastFramePure = 0;
	playing = false;

	// initialize each channel
	setAllChannelGain(0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f);

	//
	// get portaudio ready now...
	//

	// initialize portaudio
    err = Pa_Initialize();
	if( err != paNoError ) handlePaError( err );

	// set up output parameters
    outputParameters.device = Pa_GetDefaultOutputDevice(); // use default device
    if (outputParameters.device == paNoDevice)
	{
      cout << "Error: No default output device." << endl;
    }
    outputParameters.channelCount = 2; // stereo
    outputParameters.sampleFormat = paFloat32; /* 32 bit floating point output */
    outputParameters.suggestedLatency = Pa_GetDeviceInfo( outputParameters.device )->defaultLowOutputLatency;
    outputParameters.hostApiSpecificStreamInfo = NULL;

	// open port audio stream
    err = Pa_OpenStream(
              &stream,
              NULL, /* no input */
              &outputParameters,
              SAMPLE_RATE,
              FRAMES_PER_BUFFER,
              paClipOff,      /* we won't output out of range samples so don't bother clipping them */
              paCallback,	// the name of port audio callback function
              this // pass this class to callback
		);
	if( err != paNoError ) handlePaError( err );
	
	// set up callback to be called in case stream gets intrrupted
	err = Pa_SetStreamFinishedCallback( &stream, paStoppedCallback );

	// start port audiostream
    err = Pa_StartStream( stream );
}

// this method should be used if portaudio drops off and stops its stream
void MPlayer::restartStream()
{
	if(appIsExiting)
	{
		cout << "pa stream restart requested, but app is exiting...\n(will not restart audio stream)\n";
		return;
	}
	
	// specifically stop stream first (as user guide recommends it)
	err = Pa_StopStream( stream );
	
	// open port audio stream
    err = Pa_OpenStream(
              &stream,
              NULL, /* no input */
              &outputParameters,
              SAMPLE_RATE,
              FRAMES_PER_BUFFER,
              paClipOff,      /* we won't output out of range samples so don't bother clipping them */
              paCallback,	// the name of port audio callback function
              this // pass this class to callback
		);
	if( err != paNoError ) handlePaError( err );
	
	err = Pa_StartStream( stream );
	if( err != paNoError ) handlePaError( err );
	else
		cout << "portaudio stream restarted - success!\n";
}

// DEBUG
void MPlayer::stopStream()
{
	err = Pa_StopStream( stream );
	cout << "requesting portaudio to stop stream...\n";
}

// 
void MPlayer::declareAppTermination()
	{ appIsExiting = true; }

// utility function - query portaudio stream state
std::string MPlayer::getStreamStateString()
{
	err = Pa_IsStreamStopped( stream );
	std::string strError = Pa_GetErrorText(err);
	return strError;
}

// return portaudio stream state in boolean (success is true)
bool MPlayer::getStreamState()
{
	err = Pa_IsStreamStopped( stream );
	if(err==0)
		return true;
	else
		return false;
}
	
// function to set back to default before loading new song (or 'play' current song again)
// reset all oscillator + delay settings etc. - before parsing source
void MPlayer::resetForNewSong()
{
	// loop is enabled by default
	loopEnabled = true;
	
	// repeat counts - Note: repeating is OFF if loop is enabled
	repeatsRemaining = 1;
	
	// reset frame info for now (in case you're starting a new empty file)
	songLastFrame = 0;
	songLastFramePure = 0;
	songFinished = true;
	
	// initialize oscillators for first note (so that if starting song with rest, OSC is muted)
	//for(int i=0; i<9; i++)
	//	osc[i].initializeForFirstNote();

	// clear delay buffer
	delay[0].clearBuffer(); // left delay
	delay[1].clearBuffer(); // right delay

	// reset delay time + gain parameters
	// ... first delay / delay time (in milliseconds) + gain
	delayEnabled = true;
	delay[0].setParameters(500, 500, 0.20f); // -> LEFT channel = 0
	delay[1].setParameters(750, 500, 0.20f); // -> RIGHT channel = 1

	// enable all channels and reset Envelope / Astro / LFO setting
	for(int i=0; i<9; i++)
	{
		// back to default envelope setting
		osc[i].setTable(1); // back to square wave
		osc[i].refreshForSongBeginning();
		osc[i].setEnvelope(22, 18, 250, 40, 0.9f, 0.5f);
		enableChannel(i);
		disableAstro(i);
		osc[i].disableLFO();
		osc[i].initializeLFO(); // resets lfo parameters
		osc[i].detune = 0;
		osc[i].setRiseToDefault();
		osc[i].setFallToDefault();
		osc[i].disableBeefUp();
		osc[i].setBeefUpFactor(1.0f);
		ringModEnabled[i] = false; // start with ring modulation cleared
		ringModFeed[i] = -1; // -1 means no feeder channel
		ringModMute[i] = false;
		osc[i].resetYFlip();
	}
	enableDrumChannel();
	nosc.resetDrumTones();
	nosc.disableBeefUp();
	nosc.setBeefUpFactor(1.0f);
	// nosc.useWhiteNoise();

	// clear all osc history data used for meter visualization
	for(int i=0; i<9; i++)
		osc[i].clearHistory();
	nosc.clearHistory();
	
	for(int i=0; i<9; i++)
		channelDone[i] = false;
	dChannelDone = false;
}

void MPlayer::close()
{
	// close port audio stream and terminate

	err = Pa_StopStream( stream );
	if( err != paNoError ) handlePaError( err );

	cout << "pa stream stopped...";

	err = Pa_CloseStream( stream );
	if( err != paNoError ) handlePaError( err );

	cout << "pa stream closed...";

    Pa_Terminate();
}

void MPlayer::start()
{
	goToBeginning();

	// set enabler flag
	playing = true;
}

void MPlayer::pause()
	{ playing = false; }

void MPlayer::restart()
	{ playing = true; }

void MPlayer::cleanUpForNewFile()
{
	songLastFrame = 0;
	songLastFramePure = 0;
	songFinished = true;
}
	
void MPlayer::goToBeginning()
{
	// get the total song frame length
	songLastFrame = getSongLastFrame();
	songLastFramePure = getSongLastFramePure();
	songFinished = false;

	// back to very beginning... initialize player variables
	framePos = 0;
	for(int i=0; i<9; i++)
	{
		channelDone[i] = false;
		remainingFrames[i] = 0;
		freqNote[i] = 0;
		noteIndex[i] = 0;
		eventIndex[i] = 0;
	}

	// for drum channel
	dChannelDone = false;
	dRemainingFrames = 0;
	currentDrumNote = 0;
	dNoteIndex = 0;
	dEventIndex = 0;
	
	// set the starting note for each music channel (ch 1 to 9)
	for(int i=0; i<9; i++)
	{
		// if there are event requests (freq >= 70000), digest those first
		bool eventsDone = false;
		
		while(!eventsDone)
		{
			// if next event in vector is set to happen at this frame pos, process
			if( (data[i].eventFrame[eventIndex[i]] == 0) && (eventIndex[i] < data[i].nEvents) )
			{
				processEvent(i, data[i].eventType[eventIndex[i]], 
								data[i].eventParam[eventIndex[i]]);
				eventIndex[i]++;
			}
			else
				eventsDone = true;
		}

		remainingFrames[i] = data[i].len[noteIndex[i]];
		freqNote[i] = data[i].freqNote[noteIndex[i]];

		// if very first note is a rest (freq = 65535), silence channel
		if(freqNote[i]==65535.0)
		{
			setToRest(i); // set this channel to rest
			osc[i].confirmFirstNoteIsRest();
		}
		else if(freqNote[i]<0) // if first note is already end flag (empty MML data)
		{
			channelDone[i] = true;
			disableChannel(i);
		}
		else // otherwise, we have a legit note! go ahead and set to play that note
			setNewNote(i, freqNote[i]);
	}

	// handle drum channel!
	//
	// set starting note for drum channel

	// if there are event requests (freq >= 70000), digest those first
	bool eventsDone = false;
	
	while(!eventsDone)
	{
		// if next event in vector is set to happen at this frame pos, process
		if( (ddata.eventFrame[dEventIndex] == 0) && (dEventIndex < ddata.nEvents) )
		{
			processDrumEvent(ddata.eventType[dEventIndex], ddata.eventParam[dEventIndex]);
			dEventIndex++;
		}
		else
			eventsDone = true;
	}

	dRemainingFrames = ddata.len[dNoteIndex];
	currentDrumNote = ddata.drumNote[dNoteIndex];
	activateDrumChannel();
	setNewDrumHit(currentDrumNote);

	// if this is a rest (note = 65535), rest drum channel
	if(currentDrumNote == 65535)
	{
		restDrum();
	}
	else if(currentDrumNote < 0) // if first note is already end flag (empty MML data)
	{
		dChannelDone = true;
		disableDrumChannel();
	}
}

// calculates the song's frame length - including time for last delay effects
long MPlayer::getSongLastFrame()
{
	// find the max length of all MData and DData entities
	long maxLen = 0;
	for(int i=0; i<9; i++)
	{
		if(data[i].totalFrames > maxLen)
			maxLen = data[i].totalFrames;
	}
	if(ddata.totalFrames > maxLen)
		maxLen = ddata.totalFrames;

	// add time for delay effect to complete
	int totalDelayTime = max( delay[0].totalDelayFrames, delay[1].totalDelayFrames );

	return maxLen + totalDelayTime + 1000; // add safeguarding frames :)
}

// calculates the song's frame length - including time for last delay effects
// EXCLUDING up to 2-second delay cushion at the end
long MPlayer::getSongLastFramePure()
{
	// find the max length of all MData and DData entities
	long maxLen = 0;
	for(int i=0; i<9; i++)
	{
		if(data[i].totalFrames > maxLen)
			maxLen = data[i].totalFrames;
	}
	if(ddata.totalFrames > maxLen)
		maxLen = ddata.totalFrames;

	return maxLen;
}

// returns the current framePos (where player's position in music)
long MPlayer::getFramePos()
	{ return framePos; }

// checks if player is currently playing
bool MPlayer::isPlaying()
	{ return playing; }

// switches all oscillators' table type
void MPlayer::setTableType(int type)
{
	tableType = type;
	for(int i=0; i<9; i++)
		osc[i].setTable(type);
}

void MPlayer::setAstro(int channel, int nCyclesPerSecond)
{
	osc[channel].enableAstro();
	osc[channel].setAstroSpeed(nCyclesPerSecond);
}

void MPlayer::disableAstro(int channel)
	{ osc[channel].disableAstro(); }
	
void MPlayer::enableRingMod(int channel, int modulatorChannel)
{
	ringModEnabled[channel] = true; // turn on ring modulation for this channel
	ringModFeed[channel] = modulatorChannel; // assign the modulator/feeder channel
	ringModMute[modulatorChannel] = true; // feeder channel should be muted in main mix
}

void MPlayer::disableRingMod(int channel)
{
	bool found = false;
	int searchChannel = ringModFeed[channel]; // do search for - currently assigned modulator for this channel
	ringModFeed[channel] = -1; // -1 to clear target for this channel to begin...
	int channelToCheck = ringModFeed[channel]; // check for the assigned modulator channel
												// if it's not by any other channel, then we can REVIVE the modulator
	for(int i=0; i<9; i++)
	{
		if( ringModFeed[i] == searchChannel )
			found = true;
	}
	if(!found) // if no other channel has to keep using this modulator that has been used by this channel
		ringModMute[searchChannel] = false; // revive this feeder channel - comes alive in mix again!
	ringModEnabled[channel] = false; // then disable ring modulation for this channel
}

void MPlayer::setNewNote(int channel, double freq)
{
	osc[channel].setNewNote(freq);
}

void MPlayer::setToRest(int channel)
{
	osc[channel].setToRest();
}

void MPlayer::setNewDrumHit(int drumType)
{
	nosc.setNewDrum(drumType);
}

void MPlayer::restDrum()
{
	nosc.rest();
}

void MPlayer::setAllChannelGain(float gain1, float gain2, float gain3, float gain4,
	float gain5, float gain6, float gain7, float gain8, float gain9, float gainD)
{
	osc[0].setGain(gain1); osc[1].setGain(gain2); osc[2].setGain(gain3);
	osc[3].setGain(gain4); osc[4].setGain(gain5); osc[5].setGain(gain6);
	osc[6].setGain(gain7); osc[7].setGain(gain8); osc[8].setGain(gain9);
	nosc.setGain(gainD);
}

void MPlayer::setChannelGain(int channel, float gain)
	{ osc[channel].setGain(gain); }

float MPlayer::getChannelGain(int channel)
	{ return osc[channel].getGain(); }

void MPlayer::setDChannelGain(float gain)
	{ nosc.setGain(gain); }

float MPlayer::getDChannelGain()
	{ return nosc.getGain(); }

void MPlayer::enableChannels(bool enable1, bool enable2, bool enable3, bool enable4,
	bool enable5, bool enable6, bool enable7, bool enable8, bool enable9, bool enableD)
{
	enabled[0] = enable1; enabled[1] = enable2; enabled[2] = enable3;
	enabled[3] = enable4; enabled[4] = enable5; enabled[5] = enable6;
	enabled[6] = enable7; enabled[7] = enable8; enabled[8] = enable9;
	dEnabled = enableD;
}

void MPlayer::enableChannel(int channel)
	{ enabled[channel] = true; }

void MPlayer::disableChannel(int channel)
	{ enabled[channel] = false; }

void MPlayer::enableDrumChannel()
	{ dEnabled = true; }

void MPlayer::disableDrumChannel()
	{ dEnabled = false; }

void MPlayer::silenceChannel(int channel)
	{ silenced[channel] = true; }

void MPlayer::silenceDrumChannel()
	{ dSilenced = true; }

void MPlayer::activateChannel(int channel)
	{ silenced[channel] = false; }

void MPlayer::activateDrumChannel()
	{ dSilenced = false; }

void MPlayer::enableLooping()
	{ loopEnabled = true; }

void MPlayer::disableLooping()
	{ loopEnabled = false; }
	
void MPlayer::setRepeatsRemaining(int value)
	{ repeatsRemaining = value; }
	
void MPlayer::advance()
{
	// advance each music oscillator
	for(int i=0; i<9; i++)
		osc[i].advance();

	// advance noise (drum) oscillator
	nosc.advance();
}

// returns one frame float value of the mix of all channels
// at current framePos
//
// param - LEFT channel = 0, RIGHT channel = 1
float MPlayer::getMix(int channel)
{
	float mix = 0.0;

	// mix all 9 channels
	for(int i=0; i<9; i++)
	{
		if(enabled[i] && silenced[i] == false && !ringModEnabled[i] && !ringModMute[i])
			mix += compress(osc[i].getOutput());
		else if(ringModEnabled[i] && enabled[i] && silenced[i] == false && ringModFeed[i]!=-1)
			mix += compress(osc[i].getOutput() * osc[ringModFeed[i]].getOutput());
	}

	// mix drum channel, too
	if(dEnabled && dSilenced == false)
		mix += compress(nosc.getOutput());

	// update delay - delay output is returned - so add to mix
	if(delayEnabled)
		mix += delay[channel].update(mix);

	// apply master gain and compress
	mix = compress(mix * masterGain);
	
	// place holder for BCPlayer...
	// in BCPlayer, add sound effects on top of music!
	/*-----[BCPLAYER][CALLSFX]-----*/
	
	// mix += sfx->getOutput(channel); // add the entire SFX mix
										// for BCPlayer only!	

	// limit
	if(mix >= masterOutCap)
		mix = masterOutCap;
	if(mix <= -masterOutCap)
		mix = -masterOutCap;

	return mix;
}

// compress master mix signal
float MPlayer::compress(float input)
{
	float output;

	// if positive
	if(input >= 0)
	{
		if(input < compThreshold)
			return input;
		else
		{
			output = compThreshold + ((input - compThreshold) / compRatio);
		}
	}

	// if negative
	if(input < 0)
	{
		if(input > -compThreshold)
			return input;
		else
		{
			output = -compThreshold + ((input + compThreshold) / compRatio);
		}
	}

	return output;
}

// returns the table type currently set
int MPlayer::getTableType()
	{ return tableType; }

// delay related
bool MPlayer::delayIsEnabled()
	{ return delayEnabled; }

void MPlayer::enableDelay()
	{ delayEnabled = true; }

void MPlayer::disableDelay()
{ 
	delayEnabled = false; 
	// clear delay buffer
	delay[0].clearBuffer(); // left delay
	delay[1].clearBuffer(); // right delay
}

float MPlayer::getMasterGain()
	{ return masterGain; }

void MPlayer::setMasterGain(float g)
	{ masterGain = g; }


// process one event at current frame for a specified channel
void MPlayer::processEvent(int channel, int eType, int eParam)
{
	// cout << "Event: ch=" << channel << " type=" << eType << " param=" << eParam << "\t";

	// -1 is end of event series for the channel
	if(eType==-1)
	{
		// cout << "Channel " << channel << ": end of events!\n";
	}
	
	// type 0 - specify the volume
	else if(eType==0)
	{
		// convert the passed value(1-10) to float (0.0 to 0.5f)
		float gainToSet
			= static_cast<float>(eParam) / 20.0f;
		setChannelGain(channel, gainToSet);
		// cout << "VOLUME=" << eParam << endl;
	}
	// type 1 - increment volume
	else if(eType==1)
	{
		float gainToSet = min(0.5f, getChannelGain(channel)+0.05f);
		setChannelGain(channel, gainToSet);		
		cout << "channel " << channel << " VOLUME++ new gain=" << gainToSet << "\n";
	}
	// type 2 - decrement volume
	else if(eType==2)
	{
		float gainToSet = max(0.001f, getChannelGain(channel)-0.05f);
		setChannelGain(channel, gainToSet);
		cout << "channel " << channel << " VOLUME-- new gain=" << gainToSet << "\n";
	}
	// type 10 - set waveform 
	else if(eType==10)
	{
		osc[channel].setTable(eParam); // set wavetable for this value
		// cout << "WAVEFORM=" << eParam << endl;
	}
	// type 11 - flip waveform vertically (helpful for pulse waves etc.)
	else if(eType==11)
	{
		osc[channel].flipYAxis(); // set flipping status to INVERTED
		cout << "channel " << channel << " - WAVEFLIP" << endl;
	}
	// type 1000 - "DEFAULTTONE"
	else if(eType==1000)
	{
		osc[channel].setTable(1); // square wave
		osc[channel].setEnvelope(0, 0, 0, 0, 0.65f, 0.65f);
		// cout << "DEFAULTTONE\n";
	}
	// type 1001 - "PRESET=BEEP"
	else if(eType==1001)
	{
		osc[channel].setTable(1); // square wave
		osc[channel].setEnvelope(0, 0, 0, 0, 0.65f, 0.65f);
		// cout << "PRESET=BEEP\n";
	}
	// type 1002 - "PRESET=POPPY"
	else if(eType==1002)
	{
		osc[channel].setTable(1); // square wave
		osc[channel].setEnvelope(0, 50, 10, 50, 0.90f, 0.40f);
		// cout << "PRESET=POPPY\n";
	}
	// type 1002 - "PRESET=POPPYVIB"
	else if(eType==1003)
	{
		osc[channel].setTable(1); // square wave
		osc[channel].setEnvelope(0, 50, 10, 50, 0.90f, 0.40f);
		osc[channel].enableLFO();
		osc[channel].setLFOrange(22);
		osc[channel].setLFOwaitTime(250);
		osc[channel].setLFOspeed(6.0);
		// cout << "PRESET=POPPYVIB\n";
	}
	// type 1003 - "PRESET=BELL"
	else if(eType==1004)
	{
		osc[channel].setTable(1); // square wave
		osc[channel].setEnvelope(0, 0, 800, 0, 0.80f, 0.0f);
		// cout << "PRESET=BELL\n";
	}
	// type 20 - "ATTACKTIME="
	else if(eType==20)
	{
		osc[channel].setAttackTime(eParam); // set attack time to this value
		// cout << "ATTACKTIME=" << eParam << endl;
	}
	// type 21 - "PEAKTIME="
	else if(eType==21)
	{
		osc[channel].setPeakTime(eParam); // set peak time to this value
		// cout << "PEAKTIME=" << eParam << endl;
	}
	// type 22 - "DECAYTIME="
	else if(eType==22)
	{
		osc[channel].setDecayTime(eParam); // set decay time to this value
		// cout << "DECAYTIME=" << eParam << endl;
	}
	// type 23 - "RELEASETIME="
	else if(eType==23)
	{
		osc[channel].setReleaseTime(eParam); // set release time to this value
		// cout << "RELEASETIME=" << eParam << endl;
	}
	// type 24 - "PEAKLEVEL="
	else if(eType==24)
	{
		float valuef = static_cast<float>(eParam) / 100.0f;
		osc[channel].setPeakLevel(valuef); // set peak level to this value
		// cout << "PEAKLEVEL=" << eParam << endl; 
	}
	// type 25 - "SUSTAINLEVEL="
	else if(eType==25)
	{
		float valuef = static_cast<float>(eParam) / 100.0f;
		osc[channel].setSustainLevel(valuef); // set sustain level to this value
		// cout << "SUSTAINLEVEL=" << eParam << endl;
	}
	// type 30 - "LFO=ON/OFF"
	else if(eType==30)
	{
		if(eParam==1)
		{
			osc[channel].enableLFO();
			// cout << "LFO=ON\n";
		}
		else if(eParam==0)
		{
			osc[channel].disableLFO();
			// cout << "LFO=ON\n";
		}
	}
	// type 31 - "LFORANGE="
	else if(eType==31)
	{
		osc[channel].setLFOrange(eParam); // set LFO range to this value
		// cout << "LFORANGE=" << eParam << endl;
	}
	// type 32 - "LFOSPEED="
	else if(eType==32)
	{
		double valued = static_cast<double>(eParam);
		osc[channel].setLFOspeed(valued); // set LFO speed to this value
		// cout << "LFOSPEED=" << eParam << endl;
	}
	// type 33 - "LFOWAIT="
	else if(eType==33)
	{
		osc[channel].setLFOwaitTime(eParam); // set LFO wait time to this value
		// cout << "LFOWAIT=" << eParam << endl;
	}
	// type 40 - "ASTRO="
	else if(eType==40)
	{
		if(eParam==0)
		{
			disableAstro(channel);
		}
		else
			setAstro(channel, eParam); // set astro to this value
		// cout << "ASTRO=" << eParam << endl;
	}
	// type 41 - "ASTRO=OFF"
	else if(eType==41)
	{
		disableAstro(channel);
		// cout << "ASTRO=OFF" << endl;
	}
	// type 50 - "FALL"
	else if(eType==50)
	{
		osc[channel].startFall();
		// cout << "FALL!" << endl;
	}
	// type 51 - "FALLSPEED="
	else if(eType==51)
	{
		double valued = static_cast<double>(eParam);
		osc[channel].setFallSpeed(valued); // set LFO speed to this value
		// cout << "FALLSPEED=" << eParam << endl;
	}
	// type 52 - "FALLWAIT="
	else if(eType==52)
	{
		double valued = static_cast<double>(eParam);
		osc[channel].setFallWait(valued); // set LFO speed to this value
		// cout << "FALLWAIT=" << eParam << endl;
	}
	// type 60 - "RISE"
	else if(eType==60)
	{
		osc[channel].startRise();
		// cout << "RISE!" << endl;
	}
	// type 61 - "RISESPEED="
	else if(eType==61)
	{
		double valued = static_cast<double>(eParam);
		osc[channel].setRiseSpeed(valued); // set LFO speed to this value
		// cout << "RISESPEED=" << eParam << endl;
	}
	// type 62 - "RISERANGE="
	else if(eType==62)
	{
		double valued = static_cast<double>(eParam);
		osc[channel].setRiseRange(valued); // set LFO speed to this value
		// cout << "RISERANGE=" << eParam << endl;
	}
	// type 70 - "BEEFUP="
	else if(eType==70)
	{
		cout << "BEEFUP=" << eParam << endl;
		double valuef = static_cast<float>(eParam);
		if(eParam<0.1f) // if zero, turn off beef-up
		{
			osc[channel].disableBeefUp();
			cout << "BEEFUP disabled!\n";
		}
		else
		{
			osc[channel].enableBeefUp();
			osc[channel].setBeefUpFactor( (valuef*3.0f/100.0f) + 1.0f ); // set BEEFUP to this value
		}
	}
	// type 80 - "RINGMOD="
	else if(eType==80)
	{
		cout << "channel " << channel << " - RINGMOD = " << eParam << endl;
		if(eParam==0) // if 0 is passed, turn OFF
		{
			cout << "RINGMOD=OFF, channel = " << channel << endl;
			disableRingMod(channel); // stop ring modulation for this channel now
		}
		else if(eParam >= 1) // if 1 or bigger is passed, assign ring modulator channel
			enableRingMod(channel, eParam-1); // start ring modulation for this channel now
											// the passed parameter is the modulator/feeder channel number
		
	}
	// type 81 - "RINGMOD=OFF"
	else if(eType==81)
	{
		cout << "RINGMOD=OFF, channel = " << channel << endl;
		disableRingMod(channel); // stop ring modulation for this channel now
	}
}


// process one event at current frame for a specified channel
void MPlayer::processDrumEvent(int eType, int eParam)
{
	// cout << "Event: Drums type=" << eType << "\t";

	// -1 is end of event series for the channel
	if(eType==-1)
	{
		// cout << "Drum channel : end of events!\n";
	}
	
	// type 0 - specify the volume
	else if(eType==0)
	{
		// convert the passed value(1-10) to float (0.0 to 0.5f)
		float gainToSet
			= static_cast<float>(eParam) / 20.0f;
		setDChannelGain(gainToSet);	
		// cout << "VOLUME=" << eParam << endl;
	}
	// type 1 - increment volume
	else if(eType==1)
	{
		float gainToSet = min(0.5f, getDChannelGain()+0.05f);
		setDChannelGain(gainToSet);		
		// cout << "VOLUME++\n";
	}
	// type 2 - decrement volume
	else if(eType==2)
	{
		float gainToSet = max(0.001f, getDChannelGain()-0.05f);
		setDChannelGain(gainToSet);
		// cout << "VOLUME--\n";
	}
	// type 500 - reset all drum settings
	else if(eType==500)
	{
		nosc.resetDrumTones();
		cout << "RESETDRUMS\n";
	}
	// type 510 - tune kick
	// passed value = 0 to 100 (scale), set to freq ranging between 50 and 350hz
	else if(eType==510)
	{
		double tuneFreq = (static_cast<double>(eParam) / 100.0) * 300.0 + 50.0;
		nosc.tuneKick(tuneFreq);
		cout << "KICKPITCH - " << eParam << "%, " << tuneFreq << "hz\n";
	}
	// type 511 - tune snare
	// passed value = 0 to 100 (scale), set to freq ranging between 200 and 1240hz
	else if(eType==511)
	{
		double tuneFreq = (static_cast<double>(eParam) / 100.0) * 1040.0 + 200.0;
		nosc.tuneSnare(tuneFreq);
		cout << "SNAREPITCH - " << eParam << "%, " << tuneFreq << "hz\n";
	}
	// type 512 - tune HiHat
	// passed value = 0 to 100 (scale), set to freq ranging between 1200 and 3600hz
	else if(eType==512)
	{
		double tuneFreq = (static_cast<double>(eParam) / 100.0) * 2400.0 + 1200.0;
		nosc.tuneHiHat(tuneFreq);
		cout << "HIHATPITCH - " << eParam << "%, " << tuneFreq << "hz\n";
	}
	// type 520 - "BEEFUP="
	else if(eType==520)
	{
		cout << "Drum BEEFUP=" << eParam << endl;
		double valuef = static_cast<float>(eParam);
		if(eParam<0.1f) // if zero, turn off beef-up
		{
			nosc.disableBeefUp();
			cout << "Drum - BEEFUP disabled!\n";
		}
		else
		{
			nosc.enableBeefUp();
			nosc.setBeefUpFactor( (valuef*1.6f/100.0f) + 1.0f ); // set BEEFUP to this value
		}
	}
	// type 530 - use white noise
	else if(eType==530)
	{
		nosc.useWhiteNoise();
		cout << "WHITENOISE - All drums\n";
	}
	// type 531 - use pink noise
	else if(eType==531)
	{
		nosc.usePinkNoise();
		cout << "PINKNOISE - All drums\n";
	}
	// type 532 - set kick noise type
	else if(eType==532)
	{
		nosc.setKickNoiseType(eParam);
		cout << "set kick noise type =" << eParam << "\n";
	}
	// type 533 - set snare noise type
	else if(eType==533)
	{
		nosc.setSnareNoiseType(eParam);
		cout << "set snare noise type =" << eParam << "\n";
	}
	// type 534 - set hihat noise type
	else if(eType==534)
	{
		nosc.setHiHatNoiseType(eParam);
		cout << "set hihat noise type =" << eParam << "\n";
	}
	// type 540 - set kick length
	// passed value = 0 to 400 (milliseconds), set to int value ranging between 0 and 400 msec
	else if(eType==540)
	{
		nosc.setKickLength(eParam);
		cout << "KICKLENGTH - " << eParam << "msec\n";
	}
	// type 541 - set snare length
	// passed value = 0 to 1000 (milliseconds), set to int value ranging between 0 and 1000 msec
	else if(eType==541)
	{
		nosc.setSnareLength(eParam);
		cout << "SNARELENGTH - " << eParam << "msec\n";
	}
	// type 542 - set hihat length
	// passed value = 0 to 1000 (milliseconds), set to int value ranging between 0 and 1000 msec
	else if(eType==542)
	{
		nosc.setHiHatLength(eParam);
		cout << "HIHATLENGTH - " << eParam << "msec\n";
	}
	// type 550 - set square wave element mix level (default is 100 = 1.0f)
	// passed value = 0 to 100 (percent), set to float value ranging between 0 and 1.0f
	else if(eType==550)
	{
		float valuef = static_cast<float>(eParam) / 100.0f;
		nosc.setSquareLevel(valuef);
		cout << "SQUARELEVEL - " << eParam << "%, passing value = " << valuef << "\n";
	}
	// type 551 - set noise element mix level (default is 100 = 1.0f)
	// passed value = 0 to 100 (percent), set to float value ranging between 0 and 1.0f
	else if(eType==551)
	{
		float valuef = static_cast<float>(eParam) / 100.0f;
		nosc.setNoiseLevel(valuef);
		cout << "NOISELEVEL - " << eParam << "%, passing value = " << valuef << "\n";
	}
}

	


/*-----[BCPLAYER][STARTCOMMENTOUT]-----*/

std::string MPlayer::exportToFile(string filename)
{

	// get the extension part of filename
	string strExt = filename.substr(filename.find_last_of('.'));
	std::transform(strExt.begin(), strExt.end(), strExt.begin(), ::tolower); // to lowercase

	// set up info to pass to libsndfile
	SF_INFO info;
	info.channels = 2;
	info.samplerate = 44100;

	// number of total frames we need
	long songFrameLen = getSongLastFrame();

	// just to fill up with zeros...
	for(int i=0;i<88200;i++)
		sndBuffer[i] = 0;

	if(strExt==".mp3") // mp3 file - will use lame encoder
	{
		// try with 8192 in one chunk
		 // * mp3buf_size in bytes = 1.25 * num_samples + 7200
		 //    1.25 x 8192 + 7200 = 17440
		int writeChunkSize = 8192;
		const int MP3_SIZE = 17440;
		unsigned char mp3_buffer[MP3_SIZE];

		FILE *mp3 = fopen(filename.c_str(), "wb");

		// initialize lame object
		lame_t lame = lame_init();
		lame_set_in_samplerate(lame, 44100);
		lame_set_VBR(lame, vbr_default);
		lame_init_params(lame);

		int read, write;
		long currentSongFrame = 0;

		// go to the beginning of the song
		goToBeginning();

		bool done = false;

		while(!done)
		{
			// let's fill the sound buffer with 8192 samples
			read = fillExportBuffer(sndBuffer, writeChunkSize, currentSongFrame, songFrameLen);
			currentSongFrame += read;

			// if reached end of song, will get out of loop and finish
			if(framePos >= songFrameLen)
				done = true;

			write = lame_encode_buffer_interleaved_ieee_float
					(lame, sndBuffer, read, mp3_buffer, MP3_SIZE);
			fwrite(mp3_buffer, write, 1, mp3);
		}

		// finish and close lame
		write = lame_encode_flush(lame, mp3_buffer, MP3_SIZE);
		lame_close(lame);
		fclose(mp3);

		return "Finished writing mp3: " + filename;
	}

	else if(strExt==".wav" || strExt==".ogg") // for wav and ogg, will use libsndfile
	{
		if(strExt==".wav")
			info.format = SF_FORMAT_WAV | SF_FORMAT_PCM_16;
		if(strExt==".ogg")
			info.format = SF_FORMAT_OGG | SF_FORMAT_VORBIS;

		// open sound file for writing...
		SNDFILE *sndFile = sf_open(filename.c_str(), SFM_WRITE, &info);
		if(sndFile==NULL)
		{
			// delete [] sndBuffer; // release memory
			string errMsg = "Error opening sound file: ";
			errMsg += sf_strerror(sndFile);
			return errMsg;
		}

		// go to the beginning of the song
		goToBeginning();
		// songFrameLen -> total frame length of song
		long currentFrame = 0;
		int writeChunkSize = 4096; // n of frames to write in each call
		bool done = false;

		while(!done)
		{
			// fill up sound buffer with a chunk of music data
			int nFramesWritten = fillExportBuffer(sndBuffer, writeChunkSize, currentFrame, songFrameLen);

			// write to file just this much
			int framesWrittenFile = sf_writef_float(sndFile, sndBuffer, writeChunkSize);

			// currentFrame += nFramesWritten; // update current position
			if(framePos >= songFrameLen) // reached end...
				done = true;
		}

		sf_write_sync(sndFile);
		sf_close(sndFile);

		return "Finished writing file: " + filename;
	}
	else
	{
		return "Invalid file type";
	}
}

/*-----[BCPLAYER][ENDCOMMENTOUT]-----*/


// fill the export buffer with music data for exporting
// just a chunk at a time - from startFrame in the song
// returns the number of frames written
int MPlayer::fillExportBuffer(float* buffer, int framesToWrite, long startFrame, int songFrameLen)
{

	bool writeFinished = false;
	long writeIndex = 0;
	int framesWritten = 0;

	while(!writeFinished)
	{
		// get mix of all 9 channels (plus drums) at current position
		//soundAmplitudeLeft = getMix(0); // get mix for LEFT channel
		//soundAmplitudeRight = getMix(1); // get mix for RIGHT channel

		//
		//	output to export buffer
		//

		buffer[writeIndex] = getMix(0); // write LEFT channel mix to buffer
		writeIndex++; // move buffer pointer
		buffer[writeIndex] = getMix(1); // write RIGHT channel mix to buffer
		writeIndex++; // move buffer pointer

		framesWritten++;

		// if reached end of note, go to next index (for regular channels ch0 - 2)
		for(int i=0; i<9; i++)
		{
			if(!channelDone[i])
			{
				
				// if there are event requests, digest those first
				bool eventsDone = false;
				
				while(!eventsDone)
				{
					// if next event in vector is set to happen at this frame pos, process
					if( (data[i].eventFrame[eventIndex[i]] <= framePos) && (eventIndex[i] < data[i].nEvents) )
					{
						processEvent(i, data[i].eventType[eventIndex[i]], 
										data[i].eventParam[eventIndex[i]]);
						eventIndex[i]++;
					}
					else
						eventsDone = true;
				}	
				
				remainingFrames[i]--;
				if(remainingFrames[i] <= 0)
				{
					noteIndex[i]++;

					// cout << "Frame " << framePos << " channel " << i << " - note index = " <<  noteIndex[i];

					// and if you get to the end of MML signal (freq = -1.0), set flag
					if(data[i].freqNote[noteIndex[i]] < 0)
					{
						channelDone[i] = true;
						setToRest(i); // set to rest.. and let delay finish
						// disableChannel(i); // disable this channel
					}
					else
					{
						remainingFrames[i] = data[i].len[noteIndex[i]];
						freqNote[i] = data[i].freqNote[noteIndex[i]];

						// if this is a rest (freq = 65535), set this channel to rest
						if(freqNote[i]==65535.0)
							setToRest(i);
						// otherwise, this is a valid note - so set this note
						else
							setNewNote(i, freqNote[i]);
					}
				}
			}
		}

		// now handle drum channel!
		if(!dChannelDone)
		{
			
			// if there are event requests, digest those first
			bool eventsDone = false;
			
			while(!eventsDone)
			{
				// if next event in vector is set to happen at this frame pos, process
				if( (ddata.eventFrame[dEventIndex] <= framePos) && (dEventIndex < ddata.nEvents) )
				{
					processDrumEvent(ddata.eventType[dEventIndex], ddata.eventParam[dEventIndex]);
					dEventIndex++;
				}
				else
					eventsDone = true;
			}
			
			dRemainingFrames--;
			if(dRemainingFrames <= 0)
			{
				dNoteIndex++; // move onto the next drum note index

				// and if you get to the end of MML signal (drumNote = -1.0), set flag
				if(ddata.drumNote[dNoteIndex] < 0 || dNoteIndex >= ddata.getSize())
				{
					dChannelDone = true;
					restDrum(); // rest.. and let delay effect finish off
					// disableDrumChannel(); // disable this channel
				}
				else // not at end yet.. set new drum hit
				{
					dRemainingFrames = ddata.len[dNoteIndex];
					currentDrumNote = ddata.drumNote[dNoteIndex];
					setNewDrumHit(currentDrumNote);

					// if this is a rest (freq = 65535), set flag
					if(currentDrumNote == 65535)
					{
						restDrum();
					}
				}
			}
		}

		
		// if all channels have reached end... and loop is enabled, go back to beginning
		if(	channelDone[0] && channelDone[1] && channelDone[2] &&
			channelDone[3] && channelDone[4] && channelDone[5] &&
			channelDone[6] && channelDone[7] && channelDone[8] && dChannelDone)
		{
			
			// final point check!
			// ... if there are events to process at this final moment... process them here
			
			for(int i=0;i<9;i++)
			{
				bool eventsDone = false;
				while(!eventsDone)
				{
					// if next event in vector is set to happen at this frame pos, process
					if( (eventIndex[i] < data[i].nEvents) )
					{
						processEvent(i, data[i].eventType[eventIndex[i]], data[i].eventParam[eventIndex[i]]);
						eventIndex[i]++;
					}
					else
						eventsDone = true;
				}
			}
			
			// process drum events pending at the final point before loop
			bool eventsDone = false;
			while(!eventsDone)
			{
				// if next event in vector is set to happen at this frame pos, process
				if( (dEventIndex < ddata.nEvents) )
				{
					processDrumEvent(ddata.eventType[dEventIndex], ddata.eventParam[dEventIndex]);
					dEventIndex++;
				}
				else
					eventsDone = true;
			}			
			
			
			if(repeatsRemaining > 1) // if repeat times is left.. process
															// when set to 1, it's last time
			{
				repeatsRemaining--;
				cout << "Back to beginning... repeats remaining = " << repeatsRemaining << endl;
				
				// enable channels again
				enableChannels(true, true, true, true, true, true, true, true, true, true);

				// enable drum channel
				enableDrumChannel();

				// go back to the beginning
				goToBeginning();					
			}
		}		
		

		// if song is not finished, update frame position - advance player
		if(!writeFinished)
		{
			// update player position
			framePos++;
			advance();

			// if you have reached the absolute last frame position of the song
			// (including last delay effects) - only then end the track officially
			if(framePos >= songFrameLen)
			{
				writeFinished = true;

				// DEBUG
				cout << "Export to write buffer - finished!\n";
				cout << "framePos = " << framePos << endl;
				cout << "getSongLastFrame() = " << getSongLastFrame() << endl;

				for(int i=0; i<9; i++)
					cout << "channel " << i << " length = " << data[i].totalFrames << "\n";
				cout << "d channel length = " << ddata.totalFrames << "\n";

				return framesWritten;
			}
		}

		// if you reached the size write chunk size, you can exit function
		if(framesWritten >= framesToWrite)
		writeFinished = true;
	}

	// returns how many float values have been written
	return framesWritten;

}

// used for meter visualization
// returns a particular channel's current output level
// based on a short history of gain changes (like 32*8 frames)
float MPlayer::getHistoricalAverage(int channel)
{
	if(!playing)
		return 0.0f;

	if(channel==9) // drum channel
	{
		if(dEnabled)
			return nosc.getHistoricalAverage();
		else
			return 0.0f;
	}
	else if(channel >= 0 && channel <= 8) // regular channel
	{
		if(enabled[channel])
			return osc[channel].getHistoricalAverage();
		else
			return 0.0f;
	}
	else
		return 0.0f;
}

// fast-forward (or rewind) the MPlayer position to a particular point
// in the track
void MPlayer::seek(long destination)
{
	// if requested destination is further than the last point of track
	// make it the last point of the track
	if(destination > songLastFrame)
		destination = songLastFrame;
	
	bool seekDone = false; // flag to keep track of task completion
	goToBeginning(); // go to beginning of track first
	
	//
	//  now seek to destination...
	//
	
	while(!seekDone)
	{
		for(int i=0; i<9; i++)
		{
			long seekPos = 0;
			
			// for each music channel...
			// zap through until very last note before the seekpoint
			// including events
			
			// first process all the events up to destination
			bool eventsZappingDone = false;
			while(!eventsZappingDone)
			{
				// if next event in vector is set to happen at this frame pos, process
				if( (data[i].eventFrame[eventIndex[i]] <= destination) && (eventIndex[i] < data[i].nEvents) )
				{
					processEvent(i, data[i].eventType[eventIndex[i]], 
									data[i].eventParam[eventIndex[i]]);
					eventIndex[i]++;
				}
				else
					eventsZappingDone = true;
			}
			
			bool zappingDone = false;
			
			while(!zappingDone)
			{

				// if the next note is end signal, finish this channel
				if(data[i].freqNote[noteIndex[i]] < 0)
				{
					channelDone[i] = true;
					setToRest(i);
					zappingDone = true;
				}
				
				// if not finished yet, advance note index.. til very last note before destination
				if(!channelDone[i])
				{
					// if adding next note will cause to go past destination, stop here
					if( ( seekPos + static_cast<long>(data[i].len[noteIndex[i]]) ) >= destination )
					{
						zappingDone = true;
						// DEBUG
						cout << "Zapping for ch " << i << " done at: " << seekPos << endl;
						
						// now set up the channel ready for this note
						remainingFrames[i] = data[i].len[noteIndex[i]];
						freqNote[i] = data[i].freqNote[noteIndex[i]];
						
						// if it's a rest, hadle accordingly
						if(freqNote[i]==65535.0)
							setToRest(i);
						else // otherwise we have a note
							setNewNote(i, freqNote[i]);
							
					}
					// otherwise it's good to add this next note length
					else
					{
						seekPos += data[i].len[noteIndex[i]];
						noteIndex[i]++; // advance to next note
						remainingFrames[i] = data[i].len[noteIndex[i]];
					}
				}				
			}
		
			// now process this last note
			bool lastNoteProcessingDone = false;
			
			while(!lastNoteProcessingDone)
			{
				// first off, if your channel is done for, exit!
				if(channelDone[i])
					lastNoteProcessingDone = true;
				// if you've reached destination, quit!
				else if(seekPos >= destination)
					lastNoteProcessingDone = true;
				// otherwise advance seekPos and process note
				else
				{
					seekPos++;
					osc[i].advance();
					remainingFrames[i]--;
				}
			}
			
			cout << "SEEK done: Channel " << i << " seekPos = " << seekPos << endl;
		}
		
		// for drum channel...
		// zap through until very last note before the seekpoint

			long seekPos = 0;
			
			// first process all the events up to destination
			bool eventsZappingDone = false;

			while(!eventsZappingDone)
			{
				// if next event in vector is set to happen at this frame pos, process
				if( (ddata.eventFrame[dEventIndex] <= destination) && (dEventIndex < ddata.nEvents) )
				{
					processDrumEvent(ddata.eventType[dEventIndex], ddata.eventParam[dEventIndex]);
					dEventIndex++;
				}
				else
					eventsZappingDone = true;
			}
			
			// for each music channel...
			// zap through until very last note before the seekpoint
			// including events
			
			bool zappingDone = false;
			
			while(!zappingDone)
			{

				// if the next note is end signal, finish this channel
				if(ddata.drumNote[dNoteIndex] < 0 || dNoteIndex >= ddata.getSize())
				{
					dChannelDone = true;
					restDrum();
					zappingDone = true;
				}
				
				// if not finished yet, advance note index.. til very last note before destination
				if(!dChannelDone)
				{
					
					long nextDNoteLen = static_cast<long>( ddata.len[dNoteIndex] );
					
					// if adding next note will cause to go past destination, stop here
					if( ( seekPos + nextDNoteLen ) >= destination )
					{
						zappingDone = true;
						// DEBUG
						cout << "Zapping for drum ch done at: " << seekPos << endl;
						
						// now set up the channel ready for this note
						dRemainingFrames = ddata.len[dNoteIndex];
						currentDrumNote = ddata.drumNote[dNoteIndex];
						
						// if it's a rest, hadle accordingly
						if(currentDrumNote==65535.0)
							restDrum();
						else // otherwise we have a note
							setNewDrumHit(currentDrumNote);
							
					}
					// otherwise it's good to add this next note length
					else
					{
						seekPos += ddata.len[dNoteIndex];
						dNoteIndex++; // advance to next note
						dRemainingFrames = ddata.len[dNoteIndex];
					}
				}				
			}
		
			// now process this last note
			bool lastNoteProcessingDone = false;
			
			while(!lastNoteProcessingDone)
			{
				// first off, if your channel is done for, exit!
				if(dChannelDone)
					lastNoteProcessingDone = true;
				// if you've reached destination, quit!
				else if(seekPos >= destination)
					lastNoteProcessingDone = true;
				// otherwise advance seekPos and process note
				else
				{
					seekPos++;
					nosc.advance();
					dRemainingFrames--;
				}
			}
			
			cout << "SEEK done: Drum Channel seekPos = " << seekPos << endl;		
		
		// finally all our seeking tasks are done!
		seekDone = true;
	}
	
	framePos = destination;
	cout << "SEEK done: Player advanced to position " << framePos << endl;
}

// will seek to a particular position THEN START PLAYING
void MPlayer::seekAndStart(long destination)
{
	seek(destination); // first, seek...
	playing = true; // then start playing!
}

// get ratio of current framePos/songLastFrame (0 to 1.0)
float MPlayer::getProgressRatio()
{
	if(songFinished) // if song has finished... should always return 0
		return 0.0f;
	float r = static_cast<float>(framePos) / static_cast<float>(songLastFramePure);
	return r;
}

// get the position value of the next seek point relative to current framePos
long MPlayer::getNextSeekPoint()
{
	long blockSize = songLastFramePure / 16;
	long seekDestination = framePos + blockSize;
	if(seekDestination > songLastFramePure && loopEnabled)
		seekDestination = 0;
	
	// guard against last frame...
	if(seekDestination > songLastFrame)
		seekDestination = songLastFrame-16; // with a little safeguarding :)
	
	if(seekDestination < 0) seekDestination = 0;
	
	return seekDestination;
}

// get the previous seek point position relative to current framePos
long MPlayer::getPreviousSeekPoint()
{	
	long blockSize = songLastFramePure / 16;
	if(blockSize < 22050) // let's make it at least 0.5 second
		blockSize = 22050;
	long seekDestination = framePos - blockSize;
	if(seekDestination < 0)
		seekDestination = 0;
	return seekDestination;
}

// set the player's temp start point
void MPlayer::setBookmark(long bm)
{ bookmark = bm; }

// get the bookmark value - if 0, no bookmark
long MPlayer::getBookmark()
{ return bookmark; }

bool MPlayer::reachedSongLastFramePure()
{ return (framePos >= songLastFramePure); }

bool MPlayer::isSongFinished()
{ return songFinished; }

/*-----[BCPLAYER][ADDSFXBINDER]-----*/

//void MPlayer::bindSFX(SFX *sfxObj)
//{ sfx = sfxObj; }