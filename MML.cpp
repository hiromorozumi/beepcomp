/// MML Class - Implementation ////////////////

#include <cstdlib>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <math.h>
#include <vector>
#include <string>
#include <windows.h>
#include "MML.h"
#include "MData.h"
#include "DData.h"
#include "MPlayer.h"

MML::MML()
{}

MML::MML(double sampleRate, double tempo)
{
	initialize(sampleRate, tempo);
}

MML::~MML()
{}

void MML::initialize(double sampleRate, double tempo)
{
	this->sampleRate = sampleRate;
	this->tempo = tempo;
	calculateTiming();

	octave = 4;						// default octave is 4
	noteLength = baseLength * 2;	// set default to 8th notes

	semitoneRatio = pow(2, 1.0/12.0);
	middleC = 220.0 * pow(semitoneRatio, 3); // middle C is C4
	cZero = middleC * pow(0.5, 4.0);

	for(int i=0; i<9; i++)
	{
		source[i] = "   ";
	}
	dsource = "    ";
	gsource = "    ";
}

void MML::calculateTiming()
{
	baseLength = static_cast<int>(this->sampleRate * 7.5 / tempo); // n of frames for 32nd note
	measureLength = baseLength * 32;
	halfMeasureLength = baseLength * 16;
	quarterNoteLength = baseLength * 8;
}

string MML::setSource(string masterStr)
{
	// save untouched original source
	originalSource = masterStr;

	masterStr += "   "; // safeguarding...
	size_t found;

	// take out all comments from the string
	masterStr = takeOutComments(masterStr);
	masterStr = takeOutSpaces(masterStr);

	// initialize all source strings

	for(int i=0; i<9; i++)
	{
		source[i] = "    ";
	}

	dsource = "    ";
	gsource = "    ";

	int masterStrLen = masterStr.length();
	for(int i=0; i<masterStrLen; i++)
	{
		if(masterStr.at(i)=='@')
		{
			char nextCh = masterStr.at(i+1); // get the char after '@'

			if(nextCh >= '1' && nextCh <= '9') // if a number is found, music source
			{
				int channel = nextCh - '1'; // @ number (1 to 9) to channel number (0 to 8)

				source[channel] = masterStr.substr(i+2);
				found = source[channel].find('@');  // try to search for next '@'
												 // - and extract part just before it
				if(found!=string::npos)
					source[channel] = source[channel].substr(0, found);

				/*
				// DEBUG
				cout << "channel source extracted - " << channel << "\n";
				cout << source[channel] << "\n\n";
				while(!GetAsyncKeyState(VK_SPACE)){}
				while(GetAsyncKeyState(VK_SPACE)){}
				*/
			}
			else if(nextCh=='d' || nextCh=='D') // drum channel source
			{
				dsource = masterStr.substr(i+2);
				found = dsource.find('@');
				if(found!=string::npos)
					dsource = dsource.substr(0, found);
			}
			else if(nextCh=='g' || nextCh=='G') // global definition source found
			{
				gsource = masterStr.substr(i+2);
				found = gsource.find('@');
				if(found!=string::npos)
					gsource = gsource.substr(0, found);
			}
		}
	}

	return "success";
}

string MML::getSource()
{
	return originalSource;
}

// function to take out all comments from source string
string MML::takeOutComments(string masterStr)
{
	string str = masterStr + "  $$$$$$$$";
	string comment;
	string comments = "";
	char ch, ch2;
	bool done = false;
	int i=0;
	size_t found, found2, found3;

	while(!done)
	{
		ch = str.at(i);
		ch2 = str.at(i+1);

		if(ch=='$') // now at last char
		{
			done = true;
		}
		else if(ch==47 && ch2==47) // two backslashes found
		{
			comment = str.substr(i);
			found = comment.find('\n');
			found2 = comment.find('$');
			found3 = comment.find('\r');
			if(found!=string::npos) // '\n' was found
				comment = comment.substr(0, found);
			else if(found2!=string::npos) // '$' was found
				comment = comment.substr(0, found2);
			else if(found3!=string::npos) // '\r' was found
				comment = comment.substr(0, found3);
			else
				comment = "";

			// take out the comment that was just found
			str.replace(i, comment.length(), " ");
			comments += comment + '\n';
			i++;
		}
		else // nothing found, move on to next
		{
			i++;
		}
	}

	// cout << "Eliminated comments:" << endl << comments << endl << endl;

	return str;
}

string MML::takeOutSpaces(string str)
{
	bool done = false;
	size_t found;
	while(!done)
	{
		found = str.find(' ');
		if(found != string::npos)
			str.erase(found, 1);
		else
			done = true;
	}

	// DEBUG
	// cout << "After taking out spaces... resulting string:" << endl << endl << str << endl;

	// take out RETURN char (\n) now 
	done = false;
	while(!done)
	{
		found = str.find('\n');
		if(found != string::npos)
			str.erase(found, 1);
		else
			done = true;
	}
	
	// DEBUG
	// cout << "After taking out RET '\n' chars... resulting string:" << endl << endl << str << endl;
	
	return str;
}

double MML::getFrequency(int toneNum)
{
	return cZero * pow(semitoneRatio, toneNum);
}

// hub function to parse ALL MML source strings
string MML::parse(MPlayer* player)
{

	// initialize parameters back to default
	tempo = 120.0;
	octave = 4;						// default octave is 4
	noteLength = baseLength * 2;	// set default to 8th notes
	
	// player's bookmarked start is of course off in default
	player->setBookmark(0);

	// parse global source
	parseGlobalSource(player);

	// initialize all existing MData
	for(int i=0; i<9; i++)
	{
		player->data[i].clear(); // clear regular channel data
	}
	player->ddata.clear(); // clear drum channel data

	// now parse music channel data
	for(int i=0; i<9; i++)
	{
		parseChannelSource(player, i);
	}

	// parse drum channel data
	parseDrumSource(player);

	return "success";
}

// function to parse an MML string for a particular channel
// will fill the MData object with the data that is read
string MML::parseChannelSource(MPlayer* player, int channel)
{
	// choose the MData object to write to.
	if(channel >= 0 && channel <= 8)
		output = &player->data[channel]; // gets pointer to MData object
	else
		return "Error - choose valid channel!";

	// channel source string to work on
	string str = source[channel];

	//
	// first parse the configuration part
	//
	
	// start by resetting... wavetable = 1 (just to safeguard!)
	player->osc[channel].setTable(1); // square wave	

	bool configDone = false; // when all config statements are parsed, this gets set to true
	size_t found;

	while(!configDone)
	{
		configDone = true;
		
		// follow format -> setEnvelope(int attackTimeMS, int peakTimeMS, int decayTimeMS, 
		//								int releaseTimeMS, float peakLV, float sustainLV)

		// reset tone... to default
		found = str.find("DEFAULTTONE");
		if(found != string::npos)
		{
			configDone = false;
			player->osc[channel].setEnvelope(0, 0, 0, 0, 0.99f, 0.99f);
			str.erase(found, 11); // erase this statement
		}
		
		// sets up a preset tone... pure beep!
		found = str.find("PRESET=BEEP");
		if(found != string::npos)
		{
			configDone = false;
			player->osc[channel].setTable(1); // square wave
			player->osc[channel].setEnvelope(0, 0, 0, 0, 0.65f, 0.65f);
			str.erase(found, 11); // erase this statement
		}
		
		found = str.find("WAVEFORM=");
		if(found != string::npos)
		{
			configDone = false;
			string strValue = str.substr(found+9,2); // get 2 digits following '='
			int valueDigits = countDigits(strValue);
			strValue = strValue.substr(0, valueDigits);
			int value = atoi(strValue.c_str());
			value = min(99, max(0, value)); // floor + ceil the value
			player->osc[channel].setTable(value); // set wavetable for this value
			str.erase(found, 9+valueDigits); // erase this statement
		}

		found = str.find("ATTACKTIME=");
		if(str.find("ATTACKTIME=") != string::npos)
		{
			configDone = false;
			string strValue = str.substr(found+11,4); // get 4 digits following '='
			int valueDigits = countDigits(strValue);
			strValue = strValue.substr(0, valueDigits);
			int value = atoi(strValue.c_str());
			value = min(9999, max(1, value)); // floor + ceil the value

			player->osc[channel].setAttackTime(value); // set attack time to this value
			str.erase(found, 11+valueDigits); // erase this statement
		}

		found = str.find("PEAKTIME=");
		if(str.find("PEAKTIME=") != string::npos)
		{
			configDone = false;
			string strValue = str.substr(found+9,4); // get 4 digits following '='
			int valueDigits = countDigits(strValue);
			strValue = strValue.substr(0, valueDigits);
			int value = atoi(strValue.c_str());
			value = min(9999, max(1, value)); // floor + ceil the value

			player->osc[channel].setPeakTime(value); // set peak time to this value
			str.erase(found, 9+valueDigits); // erase this statement
		}

		found = str.find("DECAYTIME=");
		if(str.find("DECAYTIME=") != string::npos)
		{
			configDone = false;
			string strValue = str.substr(found+10,4); // get 4 digits following '='
			int valueDigits = countDigits(strValue);
			strValue = strValue.substr(0, valueDigits);
			int value = atoi(strValue.c_str());
			value = min(9999, max(1, value)); // floor + ceil the value

			player->osc[channel].setDecayTime(value); // set decay time to this value
			str.erase(found, 10+valueDigits); // erase this statement
		}

		found = str.find("RELEASETIME=");
		if(str.find("RELEASETIME=") != string::npos)
		{
			configDone = false;
			string strValue = str.substr(found+12,4); // get 4 digits following '='
			int valueDigits = countDigits(strValue);
			strValue = strValue.substr(0, valueDigits);
			int value = atoi(strValue.c_str());
			value = min(9999, max(1, value)); // floor + ceil the value

			player->osc[channel].setReleaseTime(value); // set release time to this value
			str.erase(found, 12+valueDigits); // erase this statement
		}

		found = str.find("PEAKLEVEL=");
		if(str.find("PEAKLEVEL=") != string::npos)
		{
			configDone = false;
			string strValue = str.substr(found+10,8); // get 8 digits following '='
			int valueDigits = countDigits(strValue);
			strValue = strValue.substr(0, valueDigits);
			double value = static_cast<double>( atoi(strValue.c_str()) );
			value = min(100.0, max(0.01, value)); // floor + ceil the value
			float valuef = static_cast<float>(value / 100.0);

			player->osc[channel].setPeakLevel(valuef); // set peak level to this value
			str.erase(found, 10+valueDigits); // erase this statement
		}

		found = str.find("SUSTAINLEVEL=");
		if(str.find("SUSTAINLEVEL=") != string::npos)
		{
			configDone = false;
			string strValue = str.substr(found+13,8); // get 8 digits following '='
			int valueDigits = countDigits(strValue);
			strValue = strValue.substr(0, valueDigits);
			double value =  static_cast<double>( atoi(strValue.c_str()) );
			value = min(100.0, max(0.01, value)); // floor + ceil the value
			float valuef = static_cast<float>(value / 100.0);

			player->osc[channel].setSustainLevel(valuef); // set sustain level to this value
			str.erase(found, 13+valueDigits); // erase this statement
		}

		found = str.find("ASTRO=");
		if(str.find("ASTRO=") != string::npos)
		{
			configDone = false;
			string strValue = str.substr(found+6,3); // get 3 digits following '='
			int valueDigits = countDigits(strValue);
			strValue = strValue.substr(0, valueDigits);
			int value = atoi(strValue.c_str());
			value = min(100, max(1, value) ); // floor + ceil the value

			player->setAstro(channel, value); // set astro to this value
			str.erase(found, 6+valueDigits); // erase this statement
		}

		found = str.find("LFO=ON");
		if(found != string::npos)
		{
			configDone = false;
			player->osc[channel].enableLFO();
			str.erase(found, 6); // erase this statement
		}
		
		found = str.find("LFO=OFF");
		if(found != string::npos)
		{
			configDone = false;
			player->osc[channel].disableLFO();
			str.erase(found, 7); // erase this statement
		}

		found = str.find("LFORANGE=");
		if(str.find("LFORANGE=") != string::npos)
		{
			configDone = false;
			string strValue = str.substr(found+9,4); // get 4 digits following '='
			int valueDigits = countDigits(strValue);
			strValue = strValue.substr(0, valueDigits);
			int value = atoi(strValue.c_str());
			value = min(2400, max(1, value)); // floor + ceil the value

			player->osc[channel].setLFOrange(value); // set LFO range to this value
			str.erase(found, 9+valueDigits); // erase this statement
		}

		found = str.find("LFOSPEED=");
		if(str.find("LFOSPEED=") != string::npos)
		{
			configDone = false;
			string strValue = str.substr(found+9,8); // get 3 digits following '='
			int valueDigits = countDigits(strValue);
			strValue = strValue.substr(0, valueDigits);
			double value = static_cast<double>( atof(strValue.c_str()) );
			value = min(100.0, max(0.1, value)); // floor + ceil the value

			player->osc[channel].setLFOspeed(value); // set LFO speed to this value
			str.erase(found, 9+valueDigits); // erase this statement
		}

		found = str.find("LFOWAIT=");
		if(str.find("LFOWAIT=") != string::npos)
		{
			configDone = false;
			string strValue = str.substr(found+8,4); // get 4 digits following '='
			int valueDigits = countDigits(strValue);
			strValue = strValue.substr(0, valueDigits);
			int value = atoi(strValue.c_str());
			value = min(3000, max(1, value)); // floor + ceil the value

			player->osc[channel].setLFOwaitTime(value); // set LFO wait time to this value
			str.erase(found, 8+valueDigits); // erase this statement
		}

	}

	// cout << "after parsing the configuration..." << endl;
	// cout << "channel " << channel << " string is now.." << endl << str << endl;

	//
	// parse the music part
	//

	// first, parse the repeat signs
	// (any repeated parts will be duplicated)

	str = str + "$$$$$$"; // to signal end of string
	bool done = false;

	int i = 0;
	vector<int> leftBraces;
	vector<int> repeatTimes;
	char ch = ' ';
	int leftBracePos = 0;
	string strToCopy = "";
	int nCharsToCopy = 0;

	while(!done)
	{
		ch = str.at(i);

		if(ch=='{') // if left brace is found
		{
			leftBraces.push_back(i); // push this position into stack
			str.erase(i,1); // go ahead and erase this '{'
			
			char chNext = str.at(i); // this should be the char right after '{'
			int numberRead = 0;
			
			// check if a number is followed...
			if(chNext >= '0' && chNext <= '9')
			{
				numberRead = chNext - '0'; // set the num of times to duplicate at right brace
				str.erase(i,1); // go ahead and erase this digit
				if(numberRead==0) numberRead = 1;
				
				// make sure there aren't any more digits after this
				while( (str.at(i)>='0'&&str.at(i)<='9') )
					str.erase(i,1); // erase this digit
			}
			else
				numberRead = 2; // repeat times not specified -> set to twice
			
			// push this number... it'll be popped when right brace is found
			repeatTimes.push_back(numberRead);

		}
		else if (ch=='}') // right brace to close repeat
		{
			str.erase(i,1); // go ahead and erase this '}'
			if(leftBraces.size() > 0) // if stack is empty, ignore
			{
				// pop last element from stack - gets the nearest pos of '{'
				leftBracePos = leftBraces.back();
				leftBraces.pop_back();
				
				// and get the number of times we should duplicate
				int timesToDuplicate = repeatTimes.back();
				repeatTimes.pop_back();
				timesToDuplicate -= 1; // if repeat is x3, duplicate twice :)
				
				nCharsToCopy = i - leftBracePos; // n of chars to duplicate
				strToCopy = str.substr(leftBracePos, nCharsToCopy); // str to be duplicated
				
				if(timesToDuplicate >=1 && timesToDuplicate <=8)
				{
					for(int i=0; i<timesToDuplicate; i++)
						str.insert(leftBracePos+nCharsToCopy, strToCopy);
				}
			}
		}
		else if (ch=='$') // end of parse string
		{
			done = true;
		}
		else
		{
			i++; // irrelevant char, so go to next char
		}
	}

	// DEBUG
	// cout << "took out all repeats. str is now: \n" << str << "\n\n";
	// while(!GetAsyncKeyState(VK_SPACE)){}
	// while(GetAsyncKeyState(VK_SPACE)){}

	done = false;
	string result = "";
	ch = ' ';
	i = 0;
	// int len = str.length();
	long framesWritten = 0;

	if(str.empty() || str.length() <= 1)
	{
		done = true;
	}

	octave = 4;						// default octave is 4
	noteLength = baseLength * 2;	// set default to 8th notes

	while(!done)
	{
		ch = str.at(i);
		// cout << "Read = " << ch << " at " << i << endl;

		// if the next token is a note
		if(ch=='C'||ch=='D'||ch=='E'||ch=='F'||ch=='G'||ch=='A'||ch=='B')
		{
			int toneNum = 0;
			int j = 0;
			int noteLengthToAssign = noteLength;
			string search = "C D EF G A B";
			while(j<12)
			{
				if(ch == search.at(j))
					toneNum = j + octave * 12;
				j++;
			}

			// advance index
			i++;

			// peak into next char

			// bool peakDone = false;

			if(str.at(i)=='#') // sharp
			{
				toneNum++;
				i++;
			}
			else if(str.at(i)=='b') // flat
			{
				toneNum--;
				i++;
			}
			
			if(str.at(i)=='~') // tie to another note unit
			{
				noteLengthToAssign += noteLength;
				i++;
				while(str.at(i)=='~')
				{
					noteLengthToAssign += noteLength;
					i++;
				}
			}

			// get the frequency

			// push to mData now (frequency and length of this note)
			double freqToWrite = getFrequency(toneNum);
			output->freqNote.push_back(freqToWrite);
			output->len.push_back(noteLengthToAssign);
			output->param.push_back(0);
			output->totalFrames += noteLengthToAssign;
			framesWritten += noteLengthToAssign;

			// DEBUG
			// cout << ch << " tone=" << toneNum << " octave=" << octave << " length=" << noteLengthToAssign << endl;

		}

		else if(ch=='<') // octave down
		{
			octave--;
			if(octave < 0)
				octave = 0;
			i++;
		}

		else if(ch=='>') // octave up
		{
			octave++;
			if(octave > 9)
				octave = 9;
			i++;
		}

		else if(ch=='L') // change note length
		{
			i++;

			if(str.at(i)>='0' && str.at(i)<='9') // we have a number - set note length
			{
				int numberRead = str.at(i) - '0';
				i++;
				if(str.at(i)>='0' && str.at(i)<='9') // if 2nd digit exists
				{
					numberRead = numberRead * 10 + (str.at(i) - '0');
					i++;
						while(str.at(i)>='0' && str.at(i)<='9') // 3rd digits and after - ignore
							i++;
				}

				// now set the new note length
				noteLength = measureLength / numberRead;

				// cout << "noteLength is now = " << noteLength << endl;
			}
		}

		else if(ch=='O') // change octave
		{
			i++;

			if(str.at(i)>='0' && str.at(i)<='9') // we have a number - set octave
			{
				int numberRead = static_cast<int>(str.at(i) - '0');
				i++;

				octave = numberRead;
				// cout << "octave is now = " << octave << endl;
			}

		}

		else if(ch=='[') // tuplets
		{
			i++;

			bool tupletReadDone = false;
			int notes[20] = {0};
			int tie[20] = {0};
			int nNotes = 0;
			int nTied = 0;
			int tupletIndex = 0;
			int wholeLength = noteLength; // default length to set for now

			while(!tupletReadDone)
			{
				// cout << "read: " << i << " ";

				if( str.at(i)=='@' ) // safeguard for infinite loop
					tupletReadDone = true;

				if(str.at(i)>='0' && str.at(i)<='9') // we have a number - set length for whole
				{
					int numberRead = str.at(i) - '0';
					i++;
					if(str.at(i)>='0' && str.at(i)<='9') // if 2nd digit exists
					{
						numberRead = numberRead * 10 + (str.at(i) - '0');
						i++;
							while(str.at(i)>='0' && str.at(i)<='9') // 3rd digits and after - ignore
								i++;
					}

					// now set the length for the whole
					wholeLength = measureLength / numberRead;
				}

				else if(str.at(i)>='A' && str.at(i)<='G') // now we have a note
				{
					int toneNum = 0;
					ch=str.at(i);

					// get the tone number
					string search = "C D EF G A B";
					int k=0;
					while(k<12)
					{
						if(ch == search.at(k))
							toneNum = k + (octave * 12);
						k++;
					}

					// advance index...
					i++;

					if(str.at(i)=='#') // sharp
					{
						toneNum++;
						i++;
					}
					else if(str.at(i)=='b') // flat
					{
						toneNum--;
						i++;
					}

					notes[tupletIndex] = toneNum;

					nNotes++;
					tupletIndex++;
				}
				
				else if(str.at(i)==':') // we have a rest...
				{
					i++; // advance index...
					notes[tupletIndex] = 65535; // freq 65535 for rest
					nNotes++;
					tupletIndex++;
				}

				else if(str.at(i)=='~') // tie last note
				{
					tie[tupletIndex] = 1;
					nTied++;
					i++;
				}

				else if(str.at(i)=='<') // oct down
				{
					octave--;
					i++;
				}

				else if(str.at(i)=='>') // oct up
				{
					octave++;
					i++;
				}

				else if (str.at(i) == ']') // closing brace - finalize tupletDone
				{
					if( (nNotes + nTied) > 0) // if we have empty braces - skip altogether! (avoid div by 0)
					{
						int division = nNotes + nTied;
						int eachTupletLength = wholeLength / division;
						int remainder = wholeLength % division;
	
						/*
						cout << "TUPLETS" << endl << "wholeLength=" << wholeLength << " division=" << division << endl;
						cout << " nNotes=" << nNotes << " nTied =" << nTied << endl;
						for(int itr=0; itr<nNotes; itr++)
							cout << " note=" << notes[itr] << " tie=" << tie[itr] << " / ";
						cout << endl;
						*/
	
						// push tuplet data to mData
						for(int j=0; j<nNotes; j++)
						{
							// get
							int lengthToWrite = eachTupletLength;
							lengthToWrite += tie[j] * eachTupletLength;
							if(j==0)
								lengthToWrite += remainder;
							// cout << "Writing length = " << lengthToWrite << endl;
							
							// get frequency of the note...
							double freqToWrite;
							if(notes[j]==65535) // then we have a rest
								freqToWrite = notes[j]; // use 65535 as freq, to signify a rest
							else
								freqToWrite = getFrequency(notes[j]);

							// cout << "Writing freq = " << freqToWrite << endl;
	
							// push this note data to mData object
							output->freqNote.push_back(freqToWrite);
							output->len.push_back(lengthToWrite);
							output->param.push_back(0);
							output->totalFrames += lengthToWrite;
							framesWritten += lengthToWrite;
						}
					}

					i++;
					tupletReadDone = true;
				}

				else // something else - advance index anyway
				{
					i++;
				}

			}

		}

		else if(str.at(i)==':') // rest, ':' colon
		{
			int lengthToWrite = noteLength;
			double freqToWrite = 65535;

			// cout << "Rest, length = " << lengthToWrite << endl;

			// push this note data to mData object
			output->freqNote.push_back(freqToWrite);
			output->len.push_back(lengthToWrite);
			output->param.push_back(0);
			output->totalFrames += lengthToWrite;
			framesWritten += lengthToWrite;

			i++;
		}

		else if(str.at(i)=='V') // Volume change request
		{
			// read the next 2 characters
			string strValue = str.substr(i+1,2); // get 3 digits following 'V'
			// int valueDigits = countDigits(strValue);
			int value = atoi(strValue.c_str());
			value = min(10, max(1, value)); // floor + ceil the value

			// push this 'event' data to mData object
			output->freqNote.push_back(70000); // event# 70000 is 'specify volume'
			output->len.push_back(0);
			output->param.push_back(value);	 // 10-scaled value is passed as param
			i++;
		}

		else if(str.at(i)=='^') // Volume increment request
		{
			// push this 'event' data to mData object
			output->freqNote.push_back(71000); // event# 71000 is 'increment volume'
			output->len.push_back(0);
			output->param.push_back(0);	 // no param needed
			i++;
		}

		else if(str.at(i)=='_') // Volume decrement request
		{
			// push this 'event' data to mData object
			output->freqNote.push_back(72000); // event# 72000 is 'decrement volume'
			output->len.push_back(0);
			output->param.push_back(0);	 // no param needed
			i++;
		}
		
		// '%%' is for bookmarking
		else if(str.at(i)=='%')
		{
			i++;
			if(str.at(i)=='%')
			{
				// if requested place (totalFrames at current parsing position)
				// is later than already bookmarked place, set it as new bookmark
				if(framesWritten > player->getBookmark())
				{
					player->setBookmark(framesWritten);
					
					cout << "Bookmarked! at ... " << player->getBookmark() << endl;
				}
				i++;
			}
		}

		else if(ch=='$') // end of string
		{
			// insert data to signify end
			output->freqNote.push_back(-1.0);
			output->len.push_back(-1);
			output->param.push_back(0);

			// integrity check

			// int r = framesWritten % quarterNoteLength;

			// cout << "End of parsing a channel... num of framesWritten=" << framesWritten << endl;
			// cout << "Dividing by quarter note length, remainder=" << r << endl << endl;

			// record the total frame length for this channel
			// output->totalFrames = framesWritten;

			done = true;
		}
		else	// default case - move pointer anyway
		{
			i++;
		}

		// DEBUG
		// cout << endl;

	}

	return result;
}

// function to parse an MML string for a drum channel
// will fill the DData object with the data that is read
string MML::parseDrumSource(MPlayer* player)
{
	dOutput = &player->ddata; // gets pointer to MData object

	// channel source string to work on
	string str = dsource;

	// first, parse the repeat signs
	// (any repeated parts will be duplicated)

	str = str + "$$$$$$"; // to signal end of string
	bool done = false;

	int i = 0;
	vector<int> leftBraces;
	vector<int> repeatTimes;
	char ch = ' ';
	int leftBracePos = 0;
	string strToCopy = "";
	int nCharsToCopy = 0;

	while(!done)
	{
		ch = str.at(i);

		if(ch=='{') // if left brace is found
		{
			leftBraces.push_back(i); // push this position into stack
			str.erase(i,1); // go ahead and erase this '{'
			
			char chNext = str.at(i); // this should be the char right after '{'
			int numberRead = 0;
			
			// check if a number is followed...
			if(chNext >= '0' && chNext <= '9')
			{
				numberRead = chNext - '0'; // set the num of times to duplicate at right brace
				str.erase(i,1); // go ahead and erase this digit
				if(numberRead==0) numberRead = 1;
				
				// make sure there aren't any more digits after this
				while( (str.at(i)>='0'&&str.at(i)<='9') )
					str.erase(i,1); // erase this digit
			}
			else
				numberRead = 2; // repeat times not specified -> set to twice
			
			// push this number... it'll be popped when right brace is found
			repeatTimes.push_back(numberRead);
		}
		else if (ch=='}') // right brace to close repeat
		{
			str.erase(i,1); // go ahead and erase this '}'
			if(leftBraces.size() > 0) // if stack is empty, ignore
			{
				// pop last element from stack - gets the nearest pos of '{'
				leftBracePos = leftBraces.back();
				leftBraces.pop_back();
				
				// and get the number of times we should duplicate
				int timesToDuplicate = repeatTimes.back();
				repeatTimes.pop_back();
				timesToDuplicate -= 1; // if repeat is x3, duplicate twice :)

				nCharsToCopy = i - leftBracePos; // n of chars to duplicate
				strToCopy = str.substr(leftBracePos, nCharsToCopy); // str to be duplicated
				
				if(timesToDuplicate >=1 && timesToDuplicate <=8)
				{
					for(int i=0; i<timesToDuplicate; i++)
						str.insert(leftBracePos+nCharsToCopy, strToCopy);
				}
			}
		}
		else if (ch=='$') // end of parse string
		{
			done = true;
		}
		else
		{
			i++; // irrelevant char, so go to next char
		}
	}

	// DEBUG
	// cout << "took out all repeats. str is now: \n" << str << "\n\n";
	// while(!GetAsyncKeyState(VK_SPACE)){}
	// while(GetAsyncKeyState(VK_SPACE)){}

	done = false;
	string result = "";
	ch = ' ';
	i = 0;
	// int len = str.length();
	long framesWritten = 0;

	while(!done)
	{
		ch = str.at(i);
		// cout << "Read = " << ch << " at " << i << endl;

		// if the next token is a drum note
		if(ch=='K'||ch=='S'||ch=='H'||ch=='k'||ch=='s'||ch=='h')
		{
			int drumNote = 0;
			int noteLengthToAssign = noteLength;
			string search = "KSHksh";
			size_t found = search.find(ch);
			if(found != string::npos)
				drumNote = found;

			// advance index
			i++;

			// peak into next char

			// bool peakDone = false;

			if(str.at(i)=='~') // tie to another note unit
			{
				noteLengthToAssign += noteLength;
				i++;
				while(str.at(i)=='~')
				{
					noteLengthToAssign += noteLength;
					i++;
				}
			}

			// push to mData now (frequency and length of this note)
			dOutput->drumNote.push_back(drumNote);
			dOutput->len.push_back(noteLengthToAssign);
			dOutput->param.push_back(0);
			dOutput->totalFrames += noteLengthToAssign;
			framesWritten += noteLengthToAssign;

			// DEBUG
			// cout << ch << " tone=" << toneNum << " octave=" << octave << " length=" << noteLengthToAssign << endl;

		}

		else if(ch=='L') // change note length
		{
			i++;

			if(str.at(i)>='0' && str.at(i)<='9') // we have a number - set note length
			{
				int numberRead = str.at(i) - '0';
				i++;
				if(str.at(i)>='0' && str.at(i)<='9') // if 2nd digit exists
				{
					numberRead = numberRead * 10 + (str.at(i) - '0');
					i++;
						while(str.at(i)>='0' && str.at(i)<='9') // 3rd digits and after - ignore
							i++;
				}

				// now set the new note length
				noteLength = measureLength / numberRead;

				// cout << "noteLength is now = " << noteLength << endl;
			}
		}

		else if(ch=='[') // tuplets
		{
			i++;

			bool tupletReadDone = false;
			int notes[20] = {0};
			int tie[20] = {0};
			int nNotes = 0;
			int nTied = 0;
			int tupletIndex = 0;
			int wholeLength = noteLength; // default length to set for now

			while(!tupletReadDone)
			{
				// cout << "read: " << i << " ";

				ch = str.at(i);

				if( str.at(i)=='$' ) // safeguard for infinite loop
					tupletReadDone = true;

				if(str.at(i)>='0' && str.at(i)<='9') // we have a number - set length for whole
				{
					int numberRead = str.at(i) - '0';
					i++;
					if(str.at(i)>='0' && str.at(i)<='9') // if 2nd digit exists
					{
						numberRead = numberRead * 10 + (str.at(i) - '0');
						i++;
							while(str.at(i)>='0' && str.at(i)<='9') // 3rd digits and after - ignore
								i++;
					}

					// now set the length for the whole
					wholeLength = measureLength / numberRead;
				}

				else if(ch=='K'||ch=='S'||ch=='H'||ch=='k'||ch=='s'||ch=='h') // now we have a note
				{
					int drumNote = 0;
					ch=str.at(i);

					// get the tone number
					string search = "KSHksh";
					size_t found = search.find(ch);
					if(found != string::npos)
						drumNote = found;

					// advance index...
					i++;

					notes[tupletIndex] = drumNote;

					nNotes++;
					tupletIndex++;
				}

				else if(str.at(i)=='~') // tie last note
				{
					tie[tupletIndex] = 1;
					nTied++;
					i++;
				}
				
				else if(str.at(i)==':') // we have a rest...
				{
					i++; // advance index...
					notes[tupletIndex] = 65535; // freq 65535 for rest
					nNotes++;
					tupletIndex++;
				}

				else if (str.at(i) == ']') // closing brace - finalize tupletDone
				{
					if( (nNotes + nTied) > 0) // if we have a empty set of braces - skip altogether!
					{
						int division = nNotes + nTied;
						int eachTupletLength = wholeLength / division;
						int remainder = wholeLength % division;
	
						/*
						cout << "TUPLETS" << endl << "wholeLength=" << wholeLength << " division=" << division << endl;
						cout << " nNotes=" << nNotes << " nTied =" << nTied << endl;
						for(int itr=0; itr<nNotes; itr++)
							cout << " note=" << notes[itr] << " tie=" << tie[itr] << " / ";
						cout << endl;
						*/
	
						// push tuplet data to dData
						for(int j=0; j<nNotes; j++)
						{
							// get
							int lengthToWrite = eachTupletLength;
							lengthToWrite += tie[j] * eachTupletLength;
							if(j==0)
								lengthToWrite += remainder;
							// cout << "Writing length = " << lengthToWrite << endl;
	
							// get frequency of the note
							int noteToWrite = notes[j];
							// cout << "Writing freq = " << notesToWrite << endl;
	
							// push this note data to mData object
							dOutput->drumNote.push_back(noteToWrite);
							dOutput->len.push_back(lengthToWrite);
							dOutput->param.push_back(0);
							dOutput->totalFrames += lengthToWrite;
							framesWritten += lengthToWrite;
						}
					}

					i++;
					tupletReadDone = true;
				}

				else // something else - advance index anyway
				{
					i++;
				}

			}

		}

		else if(str.at(i)==':') // rest, ':' colon
		{
			int lengthToWrite = noteLength;
			int noteToWrite = 65535;

			// cout << "Rest, length = " << lengthToWrite << endl;

			// push this note data to mData object
			dOutput->drumNote.push_back(noteToWrite);
			dOutput->len.push_back(lengthToWrite);
			dOutput->param.push_back(0);
			dOutput->totalFrames += lengthToWrite;
			framesWritten += lengthToWrite;

			i++;
		}

		else if(str.at(i)=='V') // Volume change request
		{
			// read the next 2 characters
			string strValue = str.substr(i+1,2); // get 3 digits following 'V'
			// int valueDigits = countDigits(strValue);
			int value = atoi(strValue.c_str());
			value = min(10, max(1, value)); // floor + ceil the value

			// push this 'event' data to mData object
			dOutput->drumNote.push_back(70000); // event# 70000 is 'specify volume'
			dOutput->len.push_back(0);
			dOutput->param.push_back(value);	 // 10-scaled value is passed as param
			i++;
		}

		else if(str.at(i)=='^') // Volume increment request
		{
			// push this 'event' data to mData object
			dOutput->drumNote.push_back(71000); // event# 71000 is 'increment volume'
			dOutput->len.push_back(0);
			dOutput->param.push_back(0);	 // no param needed
			i++;
		}

		else if(str.at(i)=='_') // Volume decrement request
		{
			// push this 'event' data to mData object
			dOutput->drumNote.push_back(72000); // event# 72000 is 'decrement volume'
			dOutput->len.push_back(0);
			dOutput->param.push_back(0);	 // no param needed
			i++;
		}

		// '%%' is for bookmarking
		else if(str.at(i)=='%')
		{
			i++;
			if(str.at(i)=='%')
			{				
				// if requested place (totalFrames at current parsing position)
				// is later than already bookmarked place, set it as new bookmark
				if(framesWritten > player->getBookmark())
				{
					player->setBookmark(framesWritten);
					
					cout << "Dr channel - Bookmarked! at ... " << player->getBookmark() << endl;
					// while(!GetAsyncKeyState(VK_SPACE)){} // DEBUG
				}
				i++;
			}
		}

		else if(ch=='$') // end of string
		{
			// insert data to signify end
			dOutput->drumNote.push_back(-1);
			dOutput->len.push_back(0);
			dOutput->param.push_back(0);

			// integrity check

			// int r = framesWritten % quarterNoteLength;

			// cout << "End of parsing a channel... num of framesWritten=" << framesWritten << endl;
			// cout << "Dividing by quarter note length, remainder=" << r << endl << endl;

			// write the total frame length written
			// output->totalFrames = framesWritten;

			done = true;
		}
		else	// default case - move pointer anyway
		{
			i++;
		}

		// DEBUG
		// cout << endl;

	}

	return result;
}

void MML::parseGlobalSource(MPlayer* player)
{
	bool done = false;
	int i=0;
	char ch, ch2;
	string strValue;
	int value;
	float gain[9] = {0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f};

	// default values
	double tpo = 120.0;
	float gainD = 0.5f;

	string str = gsource + "    $$$$";
	size_t fpos;

	// DEBUG
	// cout << "Now let's parse global source:\n\n" << str << "\n\n";
	// while(!GetAsyncKeyState(VK_SPACE)){}
	// while(GetAsyncKeyState(VK_SPACE)){}

	// search for following items
	//		DELAY, DELAYTIME, LOOP, TEMPO

	while(!done)
	{
		done = false;

		if(str.find("TEMPO=") != string::npos)
		{
			fpos = str.find("TEMPO=");
			string strValue = str.substr(fpos+6,3); // get 3 digits following '='
			int valueDigits = countDigits(strValue);
			int value = atoi(strValue.c_str());
			value = min(400, max(40, value)); // floor + ceil the value

			// set tempo to the value that was read
			tempo = static_cast<double>(value);
			tpo = tempo;
			str.erase(fpos, 6+valueDigits);

		}
		else if(str.find("LOOP=ON") != string::npos)
		{
			player->loopEnabled = true; // enable loop
			str.erase(str.find("LOOP=ON"), 7);
		}
		else if(str.find("LOOP=OFF") != string::npos)
		{
			player->loopEnabled = false; // disable loop
			str.erase(str.find("LOOP=OFF"), 8);
		}
		else if(str.find("DELAY=ON") != string::npos)
		{
			player->delayEnabled = true; // turn delay on
			str.erase(str.find("DELAY=ON"), 8);
		}
		else if(str.find("DELAY=OFF") != string::npos)
		{
			player->delayEnabled = false;// turn delay off
			str.erase(str.find("DELAY=OFF"), 9);
		}
		else if(str.find("DELAYTIME=") != string::npos)
		{
			fpos = str.find("DELAYTIME=");
			string strValue = str.substr(fpos+10,4); // get 4 digits following '='
			int valueDigits = countDigits(strValue);
			int value = atoi(strValue.c_str());
			value = min(999, max(10, value)); // floor + ceil the value

			// set delay parameters - first delay, delay time, gain (negative for no change)
			player->delay[0].setParameters(value, value, -0.1f); // -> LEFT channel = 0
			player->delay[1].setParameters(value*3/2, value, -0.1f); // -> RIGHT channel = 1
			str.erase(fpos, 10+valueDigits);
		}
		else if(str.find("DELAYLEVEL=") != string::npos)
		{
			fpos = str.find("DELAYLEVEL=");
			string strValue = str.substr(fpos+11,3); // get 3 digits following '='
			int valueDigits = countDigits(strValue);
			int value = atoi(strValue.c_str());
			value = min(99, max(1, value)); // floor + ceil the value
			float valuef = static_cast<float>(value) / 100.0f;

			// set delay parameters - first delay, delay time, gain (negative for no change)
			player->delay[0].setParameters(-1, -1, valuef); // -> LEFT channel = 0
			player->delay[1].setParameters(-1, -1, valuef); // -> RIGHT channel = 1
			str.erase(fpos, 11+valueDigits);
		}
		else if(str.find("MASTERVOLUME=") != string::npos)
		{
			fpos = str.find("MASTERVOLUME=");
			string strValue = str.substr(fpos+13,3); // get 3 digits following '='
			int valueDigits = countDigits(strValue);
			int value = atoi(strValue.c_str());
			value = min(99, max(1, value)); // floor + ceil the value
			float valuef = static_cast<float>(value) / 100.0f;

			// set master gain
			player->setMasterGain(valuef);
			str.erase(fpos, 13+valueDigits);
		}
		// if none of above can be found anymore - finally done!
		else
			done = true;

		// DEBUG
		// cout << "Now our global source is..:\n\n" << str << "\n\n";
		// while(!GetAsyncKeyState(VK_SPACE)){}
		// while(GetAsyncKeyState(VK_SPACE)){}

	}

	done = false;

	// check for older-style declarations...
	while(!done)
	{
		ch = str.at(i);
		if(ch=='T') // tempo definition found
		{
			i++;
			ch = str.at(i);
			if(ch=='=')
			{
				strValue = str.substr(i+1, 3); // read tempo value
				tpo = static_cast<double>(atoi(strValue.c_str()));
				tpo = min(320.0, max(40.0, tpo));
			}
		}
		else if(ch=='V') // volume definition found
		{
			i++;
			ch = str.at(i);
			ch2 = str.at(i+1);

			if(ch>='1' && ch<='9' && ch2=='=')
			{
				int targetChannel = ch - '1';
				strValue = str.substr(i+2, 2); // read volume value (2 digits)
				value = atoi(strValue.c_str());
				value = min(10, max(0, value)); // takes a value from 0 to 10
				gain[targetChannel] =
					static_cast<float>(value) / 20.0f; // convert to scale 0 to 0.5f
			}

			/*
			if(ch=='1' && ch2=='=') // channel 1
			{
				strValue = str.substr(i+2, 2); // read volume value (2 digits)
				value = atoi(strValue.c_str());
				value = min(10, max(0, value));
				gain1 = (float) value / 20.0f;
			}
			else if(ch=='2' && ch2=='=') // channel 2
			{
				strValue = str.substr(i+2, 2); // read volume value (2 digits)
				value = atoi(strValue.c_str());
				value = min(10, max(0, value));
				gain2 = (float) value / 20.0f;
			}
			else if(ch=='3' && ch2=='=') // channel 3
			{
				strValue = str.substr(i+2, 2); // read volume value (2 digits)
				value = atoi(strValue.c_str());
				value = min(10, max(0, value));
				gain3 = (float) value / 20.0f;
			}
			*/

			else if( (ch=='d' || ch=='D') && ch2=='=') // drum channel
			{
				strValue = str.substr(i+2, 2); // read volume value (2 digits)
				value = atoi(strValue.c_str());
				value = min(10, max(0, value)); // takes a value from 0 to 10
				gainD = static_cast<float>(value) / 20.0f; // scale 0 to 0.5f
			}
		}
		else if(ch=='$') // end signal found
		{
			done = true;
		}
		else // all else - just advance index
		{
			i++;
		}
	}

	// set player according to values that were read
	player->setAllChannelGain(gain[0], gain[1], gain[2], gain[3], gain[4], gain[5],
		gain[6], gain[7], gain[8], gainD);
	tempo = tpo;
	calculateTiming(); // recalculate base note lengths
}

// this function will load a MML file and then parse
// returns the loaded string
string MML::loadFile(string filename, MPlayer* player)
{
	// try to open file
	ifstream inFile;
	inFile.open(filename.c_str(), ifstream::in);

	// if read error - return false
	if(!inFile)
	{
		errLog("Error loading file: ", filename);
		return "Error";
	}

	string fileContent = "";
	char ch = inFile.get();
	fileContent += ch;

	while(inFile.good())
	{
		ch = inFile.get();
		fileContent += ch;
	}
	inFile.close();

	// check if the text has EOF char = 255
	// if not, add it to end
	char chEOF = 255;
	size_t found = fileContent.find(chEOF);
	string strEOF = "\xFF";
	if(found == string::npos)
		fileContent += strEOF;
		// fileContent.append<char>(1,0xFF);

	// reset source MML
	setSource(fileContent);

	// clear all MData
	for(int i=0; i<9; i++)
		{ player->data[i].clear(); }

	// parse the new source MML
	parse(player);

	return fileContent;
}

// this function will save the original MML source stored to a file
string MML::saveFile(string filename, MPlayer* player)
{
	string result;
	string strToWrite = originalSource;

	// erase ALL EOF chars before saving...
	char chEOF = 255;
	size_t found = strToWrite.find(chEOF);
	while(found!=string::npos) // if chEOF found..
	{
		strToWrite.erase(found,1); // erase that chEOF
		found = strToWrite.find(chEOF);
	}
	
	// try to open file
	ofstream outFile(filename.c_str());
	if (outFile.is_open())
	{
		outFile << strToWrite;
		outFile.close();
		result = "File written successfully.";
	}
	else
	{
		cout << "Unable to open file";
		errLog("Error opening file: ", filename);
		result = "Error";
	}

	return result;
}

// helper function to count the number of consecutive digits in the string
int MML::countDigits(string snippet)
{
	bool done = false;
	int pos = 0;
	int count = 0;
	snippet += "$$$";
	while(!done)
	{
		if(snippet.at(pos) >= '0' && snippet.at(pos) <= '9')
			count++;
		// else if(snippet.at(pos) == '.') // maybe we don't need decimals at all!
			// count++;
		else if(snippet.at(pos)=='$')
			done = true;
		else
			done = true;

		pos++;
	}
	return count;
}

// prints an error message to a log file
void MML::errLog(std::string errText1, std::string errText2)
{
	std::ofstream ofs;
	ofs.open("_errors_mml.txt", std::ofstream::out | std::ofstream::app);
	ofs << errText1 << errText2 << endl;
	ofs.close();
}