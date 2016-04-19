#include <Windows.h>
#include <algorithm>
#include <cstring>
#include <string>
#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFML/System/String.hpp>
#include "GUI.h"
#include "Dialog.h"

using namespace std;

void GUI::initialize()
{
	anotherThreadRunning = false;

	// create window
	windowTitle = "*** BeepComp ***";
	window.create(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), windowTitle);
	adjustedWindowWidth = static_cast<double>(WINDOW_WIDTH);
	adjustedWindowHeight = static_cast<double>(WINDOW_HEIGHT);
	windowFocused = true;
	wPtr = &window;
	
	// bind this window to sfml mouse object
	mouse.bindWindow(&window);

	// load icon
	icon.loadFromFile("images/beepcomp_icon.png");
	window.setIcon(32, 32, icon.getPixelsPtr());

	// load logo
	if(!logoTexture.loadFromFile("images/beepcomp_logo.png"))
		cout << "Logo: load error!\n";
	logo.setPosition(sf::Vector2f(672,12));
	logo.setTexture(logoTexture);

	// load font for knob and buttons
	if (!miniFont.loadFromFile("fonts/uni0563.ttf"))
		{ cout << "Error reading font for the Knob object..\n"; }
	
	// create a knob - and initialize
	knob.initialize(std::string("Volume"), 56.0f, wPtr, miniFont);
	knob.setPosition(682.0f, 355.0f);
	
	// create buttons with shapes...
	const float XSPACING = 78;
	const float YSPACING = 70;
	playButton.initialize   (0, "PLAY",    680+XSPACING*0, 234+YSPACING*0, 60, 52, wPtr, miniFont);
	pauseButton.initialize  (0, "PAUSE",   680+XSPACING*1, 234+YSPACING*0, 60, 52, wPtr, miniFont);
	rewindButton.initialize (0, "REWIND",  680+XSPACING*0, 234+YSPACING*1, 60, 20, wPtr, miniFont);	
	forwardButton.initialize(0, "FORWARD", 680+XSPACING*1, 234+YSPACING*1, 60, 20, wPtr, miniFont);

	// create smaller text buttons...
	const float MINITBSPACING = 42;
	keyButton.initialize(1, "KEY", 768, 342 + MINITBSPACING * 0, 48, 26, wPtr, miniFont);
	docButton.initialize(1, "DOC", 768, 342 + MINITBSPACING * 1, 48, 26, wPtr, miniFont);
	dlyButton.initialize(1, "DLY", 768, 342 + MINITBSPACING * 2, 48, 26, wPtr, miniFont);
	
	// create bigger text buttons...
	const float TXBSPACING = 42;
	newButton.initialize   (1, "NEW",    680, 470 + TXBSPACING * 0, 136, 26, wPtr, miniFont);
	loadButton.initialize  (1, "LOAD",   680, 470 + TXBSPACING * 1, 136, 26, wPtr, miniFont);
	saveButton.initialize  (1, "SAVE",   680, 470 + TXBSPACING * 2, 136, 26, wPtr, miniFont);
	exportButton.initialize(1, "EXPORT", 680, 470 + TXBSPACING * 3, 136, 26, wPtr, miniFont);
	
	// set default userdata path
	defaultPath = "userdata/";
	currentPathAndFileName = defaultPath;

	// load work area font
	string fontfile = "fonts/EnvyCodeR.ttf";
	if (!font.loadFromFile(fontfile))
		{ cout << "Loading main font - error!" << endl; }

	charHeight = font.getLineSpacing(24);
	charWidth = 13;

	// set up text objects array for edit area
	for(int i=0; i<TEXT_HEIGHT; i++)
	{
 		text[i].setFont(font); // font is a sf::Font
		text[i].setCharacterSize(24); // in pixels, not points!
		text[i].setColor(sf::Color::Green);
		text[i].setPosition(TEXT_TOP_X, i * charHeight + TEXT_TOP_Y - 4);
	}

	// set up a cursor
	cursor.setFillColor(sf::Color(255, 225, 225, 128));
	cursor.setSize(sf::Vector2f(14, 24));

	// set up the selector mask rectangles (one for each line)
	for(int i=0; i< TEXT_HEIGHT; i++)
	{
		highlighter[i].setFillColor(sf::Color(170, 200, 255, 128));
		highlighter[i].setSize(sf::Vector2f(0,0));
		highlighter[i].setPosition(0,0);
	}

	source = "// MML source not set yet... //" + CH_EOF;

	// reset text display variables
	cursorX = 0;
	cursorY = 0;
	currentLine = 0;
	posInLine = 0;
	charPos = 0;
	topRenderLine = 0;

	typedChar = 0;
	typedWithShift = false;
	exitApp = false;

	// undo-system related variables
	eraseHistory();

	// initialize our MPlayer - this will be the 'music player'

	mplayer.initialize();

	cout << "GUI: MPlayer initialized...\n";

	// initialize our MML - this will be our MML music source parser
	mml.initialize(BC_SAMPLE_RATE, 120.0); // use default sample rate and tempo

	cout << "GUI: MML initialized...\n";

	// load up default music to start...
	mplayer.pause();
	mplayer.resetForNewSong();
	source = mml.loadFile("default.txt", &mplayer);
	if(source=="Error")
		cout << "File load error..." << endl;
	mml.parse(&mplayer);
	mplayer.goToBeginning();

	// DEBUG
	cout << "GUI: source loaded!\n\n";

	// set up strings for viewable area
	updateLineStats();
	setStrView();

	// set up text array for view
	for(int i=0; i<TEXT_HEIGHT; i++)
	{
		text[i].setString(strView[i]);
	}

	selecting = false;
	selectFinished = false;

	// initialize text selector variables
	emptySelection(); // this initializes to nothing selected

	cout << "char height = " << charHeight << endl;
	cout << "char width = " << charWidth << endl;

	// mouse related
	mouseLPressed = false;
	mouseRPressed = false;
	mouseLReleased = false;
	mouseRPressed = false;
	mouseX = 0;
	mouseY = 0;
	cx = 0;
	cy = 0;
	mouseWheelMoved = 0;

	// meter related
	meter.initialize(685, 94, 138, 92, 0.5f); // set size and position

	for(int i=0; i<10; i++)
		meter.set(i, 0.0f);
	
	// meter back panel
	backPanel.setSize(sf::Vector2f(138, 96));
	backPanel.setPosition(sf::Vector2f(680, 90));
	backPanel.setFillColor(sf::Color(40,40,40));
	backPanel.setOutlineThickness(2.0f);
	backPanel.setOutlineColor(sf::Color(80,80,80));
		
	// progress bar
	progress.initialize(&window);
	
	//
	// set up HELP text
	//
	help.initialize(&window, miniFont);
	float helpYSpacing = 14.0;
	float yup = 4.0;
	help.set(1,  "F1", playButton.x, playButton.y - yup);
	help.set(2,  "F2", pauseButton.x, pauseButton.y - yup);
	help.set(3,  "F3", rewindButton.x, rewindButton.y - yup);
	help.set(4,  "F4", forwardButton.x, forwardButton.y - yup);
	help.set(5,  "F5", keyButton.x, keyButton.y - yup);	
	help.set(6,  "F6", docButton.x, docButton.y - yup);	
	help.set(7,  "F7", dlyButton.x, dlyButton.y - yup);
	help.set(9,  "F9", newButton.x, newButton.y - yup);
	help.set(10, "F10", loadButton.x, loadButton.y - yup);
	help.set(11, "F11", saveButton.x, saveButton.y - yup);
	help.set(12, "F12", exportButton.x, exportButton.y - yup);
	help.set(13, "CTRL-UP   ", 640, 346);
	help.set(14, "CTRL-DOWN", 640, 360);
	float helpXTab = 150.0;
	help.set(15, "ESC",        6           ,  6 );
	help.set(16, ".... QUIT",  6 + helpXTab,  6 );
	help.set(17, "CTRL + Z",   6           ,  6 + helpYSpacing*1);
	help.set(18, ".... UNDO",  6 + helpXTab,  6 + helpYSpacing*1);
	help.set(19, "CTRL + C",   6           ,  6 + helpYSpacing*2);
	help.set(20, ".... COPY",  6 + helpXTab,  6 + helpYSpacing*2);
	help.set(21, "CTRL + V",   6           ,  6 + helpYSpacing*3);
	help.set(22, ".... PASTE", 6 + helpXTab,  6 + helpYSpacing*3);
	help.set(23, "CTRL + S",   6           ,  6 + helpYSpacing*4);
	help.set(24, ".... SAVE",  6 + helpXTab,  6 + helpYSpacing*4);
	help.set(25, "ALT + S",    6           ,  6 + helpYSpacing*5);
	help.set(26, ".... QUICK-SAVE" ,  6 + helpXTab,  6 + helpYSpacing*5);
	help.set(27, "CTRL + A",   6           ,  6 + helpYSpacing*6);
	help.set(28, ".... SELECT ALL" ,  6 + helpXTab,  6 + helpYSpacing*6);	
	help.set(29, "SHIFT + ARROW"   ,  6           ,  6 + helpYSpacing*7);
	help.set(30, ".... SELECT TEXT",  6 + helpXTab,  6 + helpYSpacing*7);
	help.set(31, "ALT"   ,     6           ,  6 + helpYSpacing*8);
	help.set(32, ".... CLEAR SELECTION",  6 + helpXTab,  6 + helpYSpacing*8);
	help.set(33, "ALT + V",    6           ,  6 + helpYSpacing*9);
	help.set(34, ".... SYSTEM VOLUME" ,  6 + helpXTab,  6 + helpYSpacing*9);
	help.set(35, "ALT + D",    6           ,  6 + helpYSpacing*10);
	help.set(36, ".... AUDIO DEVICE" ,  6 + helpXTab,  6 + helpYSpacing*10);
	help.set(37, "ALT + I",    6           ,  6 + helpYSpacing*11);
	help.set(38, ".... INITIALIZE AUDIO" ,  6 + helpXTab,  6 + helpYSpacing*11);
	help.set(39, "HOME"            ,  6,             6 + helpYSpacing*12);
	help.set(40, ".... TO TOP"     ,  6 + helpXTab,  6 + helpYSpacing*12);
	help.set(41, "END"             ,  6,             6 + helpYSpacing*13);
	help.set(42, ".... TO END"     ,  6 + helpXTab,  6 + helpYSpacing*13);
	
	help.setBlackOut(3, 3, 360, helpYSpacing*14+10);	
	help.deactivate(); // start inactive...
	
	//
	//
	//
	
	// clock to track meter update interval
	meterClock.restart();
	
	// for messaging
	messageShowing = false;
	
	// dialog - bind window in this class
	dialog.bindWindow(&window);
	string startFolder = dialog.getCurrentDir() + "\\userdata";
	dialog.setStartFolder(startFolder);	

	// clock to track blinking
	blinkState = 0;
	blinkClock.restart();
	
	// clock for auto-saving (every 5 min.)
	autoSaveClock.restart();

	window.requestFocus();
	
	// DEBUG
	cout << "At start - progressRatio() = " << mplayer.getProgressRatio() << endl;

	// start running GUI
	// run();
}

void GUI::run()
{
	while(!exitApp)
	{
        while (window.pollEvent(event))
        {
            if (!exitApp && event.type == sf::Event::Closed)
			{
				exitApp = true;
			}
			// see if any text char has been typed
			if (event.type == sf::Event::TextEntered)
			{
				if (event.text.unicode < 128 && event.text.unicode != 0)
				{
					typedChar = static_cast<char>(event.text.unicode);
					sf::String sfStr = event.text.unicode;
					strTypedChar = sfStr.toAnsiString();
					
					// if SHIFT was being pressed... set flag
					// this flag is used to clear shift command for text selection
					if(kbd.shift())
					{
						typedWithShift = true;
						cout << "Typed with shift!\n";
					}
					else
						typedWithShift = false;
				}
				else
					typedChar = 0;
			}
			if (event.type == sf::Event::LostFocus)
				windowFocused = false;

			if (event.type == sf::Event::GainedFocus)
				windowFocused = true;

			// check for mouse events now
			if (event.type == sf::Event::MouseButtonPressed)
			{
				if (event.mouseButton.button == sf::Mouse::Left)
					mouseLPressed = true;
				else if (event.mouseButton.button == sf::Mouse::Right)
					mouseRPressed = true;
				mouseLReleased = false;
				mouseRReleased = false;
			}
			else if (event.type == sf::Event::MouseButtonReleased)
			{
				if (event.mouseButton.button == sf::Mouse::Left)
					mouseLReleased = true;
				else if (event.mouseButton.button == sf::Mouse::Right)
					mouseRReleased = true;
				mouseLPressed = false;
				mouseRPressed = false;
			}
			else if (event.type == sf::Event::MouseMoved)
			{
				mouseX = event.mouseMove.x * WINDOW_WIDTH / adjustedWindowWidth;
				mouseY = event.mouseMove.y * WINDOW_HEIGHT / adjustedWindowHeight;

				// cx = (mouseX - TEXT_TOP_X) / static_cast<double>(charWidth);
				// cy = (mouseY - TEXT_TOP_Y) / static_cast<double>(charHeight);
			}

			// check for mouse wheel
			if (event.type == sf::Event::MouseWheelMoved)
			{
				mouseWheelMoved = event.mouseWheel.delta;
			}

			// handle resize event
			if(event.type == sf::Event::Resized)
			{
				adjustedWindowWidth = static_cast<double>(event.size.width);
				adjustedWindowHeight = static_cast<double>(event.size.height) ;
			}
        }

		handleInputs();
		handleTimedSaving();
		updateDisplay();

		// check for termination request
		if(exitApp)
		{
			//
			//   IMPORTANT! - autosaving right before exiting
			//				this is done REGARDLESS how you got here
			//
			autoSave();
			
			// terminate music player
			mplayer.close();

			// delete openFileDialog;
            window.close();
		}
	}
}

void GUI::handleInputs()
{
	// if another thread is running.. skip input-handling altogether
	if(anotherThreadRunning)
		return;

	// if window has lost focus, skip input-handling altogether
	if(!windowFocused)
		return;

	// cursor inputs
	if (kbd.right())
	{
		if(rightKeyFiring) // right key is firing now
		{
			fireClock.restart();
			while(fireClock.getElapsedTime().asSeconds() < 0.05f){}
		}

		// you processed right key once and came back - listen for 'fire' request
		if(rightProcessedCount == 1 && !rightKeyFiring)
		{
			fireClock.restart();
		}
		else if(rightProcessedCount > 1 && !rightKeyFiring)
		{
			if(fireClock.getElapsedTime().asSeconds() > 0.58f)
				rightKeyFiring = true;
		}

		// will process right key - only if you're first time pressing right key
		// or currently key is firing
		if(rightProcessedCount == 0 || rightKeyFiring)
		{
			if(!kbd.shift()) // check for clearning highlighter
			emptySelection();

			// otherwise, it's good to process right key
			if(posInLine==(TEXT_WIDTH-1) && source.at(charPos)!=CH_EOF) // at right edge
			{
				charPos++;
				posInLine = 0;
				currentLine++;
			}
			else if(source.at(charPos)==CH_NL) // on RET
			{
				posInLine = 0;
				currentLine++;
				calculateCharPos();
			}
			else if(source.at(charPos)!=CH_EOF && source.at(charPos)!=CH_NL)
			{
				charPos++;
				posInLine++;
			}
		}
		updateCursorPos();
		rightProcessedCount++;
	}
	else
	{
		rightKeyFiring = false;
		rightProcessedCount = 0;
	}

	if (kbd.left())
	{
		if(leftKeyFiring) // left key is firing now
		{
			fireClock.restart();
			while(fireClock.getElapsedTime().asSeconds() < 0.05f){}
		}

		// you processed left key once and came back - listen for 'fire' request
		if(leftProcessedCount == 1 && !leftKeyFiring)
		{
			fireClock.restart();
		}
		else if(leftProcessedCount > 1 && !leftKeyFiring)
		{
			if(fireClock.getElapsedTime().asSeconds() > 0.58f)
				leftKeyFiring = true;
		}

		// will process left key - only if you're first time pressing left key
		// or currently key is firing
		if(leftProcessedCount == 0 || leftKeyFiring)
		{
			if(!kbd.shift()) // check for clearning highlighter
				emptySelection();

			if(posInLine!=0)
			{
				charPos--;
				posInLine--;
			}
			else if(posInLine == 0 && currentLine!=0)
			{
				charPos--;
				currentLine--;
				posInLine = nCharsLine[currentLine] - 1;
			}
		}
		updateCursorPos();
		leftProcessedCount++;
	}
	else
	{
		leftKeyFiring = false;
		leftProcessedCount = 0;
	}

	if ( ( kbd.up() && !kbd.ctrl() ) || mouseWheelMoved > 0)
	{
		if(upKeyFiring) // up key is firing now
		{
			fireClock.restart();
			while(fireClock.getElapsedTime().asSeconds() < 0.05f){}
		}

		// you processed up key once and came back - listen for 'fire' request
		if(upProcessedCount == 1 && !upKeyFiring)
		{
			fireClock.restart();
		}
		else if(upProcessedCount > 1 && !upKeyFiring)
		{
			if(fireClock.getElapsedTime().asSeconds() > 0.58f)
				upKeyFiring = true;
		}

		// will process up key - only if you're first time pressing up key
		// or currently key is firing
		if(upProcessedCount == 0 || upKeyFiring || mouseWheelMoved > 0)
		{
			if(!kbd.shift()) // check for clearning highlighter
				emptySelection();

			if(currentLine>0)
			{
				//    AA/.......
				//    BBBBBBB/..

				if(nCharsLine[currentLine-1] < TEXT_WIDTH) // line above includes RET
				{
					if(posInLine < nCharsLine[currentLine-1]) // can move directly above
					{
						currentLine--;
						calculateCharPos();
					}
					else // one space up is right to the RET on line above - just land on RET
					{
						posInLine = nCharsLine[currentLine-1] - 1; // land right on RET
						currentLine--;
						calculateCharPos();
					}
				}
				else // otherwise, line above has no RET
				{
					currentLine--;
					calculateCharPos();
				}

				updateCursorPos();
			}
		}
		if(mouseWheelMoved > 0)
			mouseWheelMoved--;
		upProcessedCount++;
	}
	else
	{
		upKeyFiring = false;
		upProcessedCount = 0;
	}


	if ( (kbd.down() && !kbd.ctrl()) || mouseWheelMoved < 0)
	{

		if(downKeyFiring) // down key is firing now
		{
			fireClock.restart();
			while(fireClock.getElapsedTime().asSeconds() < 0.05f){}
		}

		// you processed down key once and came back - listen for 'fire' request
		if(downProcessedCount == 1 && !downKeyFiring)
		{
			fireClock.restart();
		}
		else if(downProcessedCount > 1 && !downKeyFiring)
		{
			if(fireClock.getElapsedTime().asSeconds() > 0.58f)
				downKeyFiring = true;
		}

		// will process down key - only if you're first time pressing down key
		// or currently key is firing
		if(downProcessedCount == 0 || downKeyFiring || mouseWheelMoved < 0)
		{
			if(!kbd.shift()) // check for clearning highlighter
				emptySelection();

			if(currentLine < (nLines-1)) // you can move down if you're not on last line
			{
					//   AAAAAA/...
					//   BBB/......

				// get n spaces for line you're on now
				if(nCharsLine[currentLine+1] < TEXT_WIDTH) // line below includes RET or EOF
				{
					if(posInLine < nCharsLine[currentLine+1]) // can move directly below
					{
						currentLine++;
						calculateCharPos();
					}
					else // one space down is right to the RET on line above - just land on RET (or EOF)
					{
						posInLine = nCharsLine[currentLine+1] - 1;
						currentLine++;
						calculateCharPos();
					}
				}
				else // otherwise, line below has no RET or EOF
				{
					currentLine++;
					calculateCharPos();
				}
				updateCursorPos();
			}
		}
		if(mouseWheelMoved < 0)
			mouseWheelMoved++;
		downProcessedCount++;
	}
	else
	{
		downKeyFiring = false;
		downProcessedCount = 0;
	}

	// if any key was typed... process
	if(typedChar && typedChar > 0)
	{
		cout << "Typed key = " << typedChar << endl;
		cout << "strTypedChar = " << strTypedChar << endl;

		if(typedChar==13) // return key
		{
			emptySelection();
			source.insert(charPos, 1, '\n');

			strTypedChar = "\n";
			addHistory("", strTypedChar, charPos, charPos, topRenderLine);

			// move forward by one
			charPos++;

			// you'll go to new line
			posInLine = 0;
			currentLine++;

			// update the whole text line info..
			updateLineStats();
			setStrView();

			// re-update, in case you're moving down over the bottom of screen
			updateCursorPos();
		}
		else if(typedChar==8) // backspace key
		{
			string strToErase;
			int eraseStartPos, nCharsToErase;

			// if a selection has been made, we'll erase the entire selection
			if(selectFinished)
			{
				if(selectEnd < selectStart) // if selection was made upward, swap positions
				{
					int temp = selectStart;
					selectStart = selectEnd;
					selectEnd = temp;
				}

				nCharsToErase = selectEnd - selectStart + 1;
				strToErase = source.substr(selectStart, nCharsToErase);
				eraseStartPos = selectStart;

				// avoid including EOF from str to erase
				if (strToErase.at(strToErase.length()-1) == CH_EOF)
				{
					strToErase = strToErase.erase(strToErase.length()-1, 1);
					nCharsToErase--;
				}

				source.erase(eraseStartPos, nCharsToErase); // erase the determined section
				// store this action in history (undo history stack)
				addHistory(strToErase, "", eraseStartPos, charPos, topRenderLine);

				// adjust line stats to new string
				updateLineStats();

				// move cursor to where the beginning of deleted string was
				charPos = eraseStartPos;

				// reconfigure posInLine and currentLine (screen coordinates)
				updateScreenCoordinates();
			}
			else // else, regular 1-char backspace
			{
				if(charPos > 0) // but exclude when you're at the charPos = 0
				{
					nCharsToErase = 1;
					strToErase = source.substr(charPos - 1, 1);
					eraseStartPos = charPos - 1;
					source.erase(eraseStartPos, nCharsToErase); // erase the determined section

					// store this action in history (undo history stack)
					addHistory(strToErase, "", eraseStartPos, charPos, topRenderLine);

					// adjust line stats to new string
					updateLineStats();

					// move cursor to where the beginning of deleted string was
					charPos = eraseStartPos;

					// reconfigure posInLine and currentLine (screen coordinates)
					updateScreenCoordinates();
				}
			}

			emptySelection();


			/*
			if(posInLine==0) // if you're at currently left edge, then move up one line
			{
				currentLine--; // move to line above
				posInLine = nCharsLine[currentLine] - 1; // go to last char on that above line
			}
			else // there's still char to erase to your left on your current line
			{
				posInLine--;
			}
			*/

			// update the whole text line info..
			setStrView();

			// re-update, in case you're moving up from the top of screen
			updateCursorPos();
		}
		else if(typedChar>=32 && typedChar<=126)
		{
			// source.insert(charPos, 1, typedChar); // instead of inserting a char...
			source.insert(charPos, strTypedChar); // let's try inserting a ANSI-converted string

			addHistory("", strTypedChar, charPos, charPos, topRenderLine);

			// move forward by one
			charPos++;

			if(posInLine==(TEXT_WIDTH-1)) // you're at right edge
			{
				posInLine = 0;
				currentLine++;
			}
			else // you're in the middle of line
				posInLine++;

			// update the whole text line info..
			updateLineStats();
			setStrView();

			// re-update, in case you're moving down over the bottom of screen
			updateCursorPos();
			
			// in case, selection is on, cancel
			emptySelection();
		}

		// refresh tracking variable
		typedChar = 0;
	}

	// check for escape key input
	if(kbd.escape())
	{
		while(kbd.escape())
		{ updateDisplay(); }
		
		quitDialog();
		adjustWindowSize();
	}

	//
	//
	// Handle button inputs now...
	// and check for function key inputs
	//
	//	
	
	// if dragging knob or selecting text right now... skip button inputs altogether!
	if(knob.isActive() || selecting)
		goto endOfButtonRoutines;
		
	
	// PLAY button or F1 key
	if( kbd.f1() || (mouse.left() && playButton.hovering(mouseX, mouseY)) )
	{
		playButton.activate();
		while(kbd.f1() || mouse.left())
		{ updateDisplay(); }		
		
		mplayer.pause();
		mplayer.resetForNewSong();
		mml.setSource(source);
		mml.parse(&mplayer);
		
		// if bookmark is placed, begin from that place
		if(mplayer.getBookmark()>0)
		{
			mplayer.goToBeginning();
			mplayer.seekAndStart(mplayer.getBookmark());
		}
		// no bookmark - start track normally
		else
			mplayer.start();
	}
	else if(playButton.hovering(mouseX, mouseY))
		playButton.highlight();
	else
		playButton.unhighlight();
		
	// PAUSE button or F2 key
	if(kbd.f2() || (mouse.left() && pauseButton.hovering(mouseX, mouseY)) )
	{
		pauseButton.activate();
		while(kbd.f2() || mouse.left())
		{ updateDisplay(); }
		
		if(mplayer.isPlaying())
		{
			mplayer.pause();
		}
		else
			mplayer.restart();
	}
	else if(pauseButton.hovering(mouseX, mouseY))
		pauseButton.highlight();
	else
		pauseButton.unhighlight();

	// REWIND button or F3 key
	if( kbd.f3() || (mouse.left() && rewindButton.hovering(mouseX, mouseY)) )
	{
		rewindButton.activate();
		while(kbd.f3() || mouse.left())
		{ updateDisplay(); }
	
		if(mplayer.isPlaying())
		{
			// rewind to a seek position just before current framePos
			long rewindTo = mplayer.getPreviousSeekPoint();
			mplayer.pause();
			mplayer.goToBeginning();
			mplayer.seekAndStart(rewindTo);
		}
		
	}
	else if(rewindButton.hovering(mouseX, mouseY))
		rewindButton.highlight();
	else
		rewindButton.unhighlight();
	
	// FORWARD button or F4 key
	if( kbd.f4() || (mouse.left() && forwardButton.hovering(mouseX, mouseY)) )
	{
		forwardButton.activate();
		while(kbd.f4() || mouse.left())
		{ updateDisplay(); }
	
		if(mplayer.isPlaying())
		{
			// forward to the next seek position relative to current framePos
			long forwardTo = mplayer.getNextSeekPoint();
			mplayer.pause();
			mplayer.goToBeginning();
			mplayer.seekAndStart(forwardTo);
		}
	}
	else if(forwardButton.hovering(mouseX, mouseY))
		forwardButton.highlight();
	else
		forwardButton.unhighlight();
	
	/*
	if(kbd.f5())
	{
		int type = mplayer.getTableType();
		type++;
		if(type==2)
			type = 0;
		mplayer.setTableType(type);
		while(kbd.f5()){}
	}
	*/
	
	// KEYs help button or F5 key
	if( kbd.f5() || (mouse.left() && keyButton.hovering(mouseX, mouseY)) )
	{
		help.activate();
		keyButton.activate();
		while(kbd.f5() || mouse.left())
		{
			updateMeter();
			updateDisplay();
		}
		
		help.deactivate();
	}
	else if(keyButton.hovering(mouseX, mouseY))
		keyButton.highlight();
	else
		keyButton.unhighlight();

	// DOC button or F6 key
	if( kbd.f6() || (mouse.left() && docButton.hovering(mouseX, mouseY)) )
	{
		docButton.activate();
		while(kbd.f6() || mouse.left())
		{ updateDisplay(); }
	
		// open a web browser and open the local documentation file
		// ... in /documentation folder
		
		string docPath = dialog.getCurrentDir() + "\\documentation\\beepcomp_users_guide.html";
		cout << "Opening Doc at: " << docPath << endl;
		
		// try to wrestle with the program dir with spaces...
		// I HATE WORKING WITH WINDOWS PATHS!!!
		// SPACES AND BACKSLASHES ARE EEEEEEVIL!!!
		
											// ... sorry, I just needed to get that out :)
		
		// double backslashes...
		unsigned int i = 0;
		char ch;
		while(i<docPath.length())
		{
			ch = docPath.at(i);
			if(ch=='\\')
			{
				docPath.insert(i, "\\");
				i++;
			}
			i++;
		}
		
		// DEBUG
		cout << "After doubling backslashes and replacing with env variable:\n" << docPath << endl;
		
		// WAIT... try a relative path
		docPath = "documentation\beepcomp_users_guide.html";
		string docPath2 = "documentation\\beepcomp_users_guide.html";
		
		// now try to open the html file...
		int result = 0;
		TCHAR app[MAX_PATH] = { 0 };
		
		result = (int)::FindExecutable(docPath.c_str(), NULL, app);
		if (result > 32) 
		{
			::ShellExecute(0, NULL, app,
				(docPath).c_str(), NULL, SW_SHOWNORMAL);
		}
		
		ShellExecute( NULL, "open", docPath.c_str(), NULL, NULL, SW_SHOWNORMAL );
		ShellExecute( NULL, "open", docPath2.c_str(), NULL, NULL, SW_SHOWNORMAL );
		ShellExecute( NULL, NULL, docPath.c_str(), NULL, NULL, SW_SHOWNORMAL );
		ShellExecute( NULL, "call", docPath.c_str(), NULL, NULL, SW_SHOWNORMAL );	
	}
	else if(docButton.hovering(mouseX, mouseY))
		docButton.highlight();
	else
		docButton.unhighlight();
	
	// DELAY ON / OFF - F7 key
	if( kbd.f7() || (mouse.left() && dlyButton.hovering(mouseX, mouseY)) )
	{
		dlyButton.activate();
		while( kbd.f7() || mouse.left() )
		{ updateDisplay(); }
		if(mplayer.delayIsEnabled())
		{
			mplayer.disableDelay();
		}
		else
		{
			mplayer.enableDelay();
		}
	}
	else if(dlyButton.hovering(mouseX, mouseY))
		dlyButton.highlight();
	else
		dlyButton.unhighlight();

	// DEBUG
	if(kbd.f8())
	{
		while(kbd.f8());
		cout << "charPos = " << charPos << endl;
		cout << "currentLine = " << currentLine << endl;
		cout << "getLineNumber(charPos) = " << getLineNumber(charPos) << endl;
		cout << "getStartPosInLine(getLineNumber(charPos)) = " << getStartPosInLine(getLineNumber(charPos)) << endl;
		cout << "mouse -> " << mouseX << ", " << mouseY << endl;
		cout << "current cx & cy -> " << cx << ", " << cy << endl;
		
		// DEBUG
		cout << "progress ratio=" << mplayer.getProgressRatio() << endl;
	}

	// NEW button or F9
	// start with a blank new file
	if( kbd.f9() || (mouse.left() && newButton.hovering(mouseX, mouseY)) )
	{
		newButton.activate();
		while(kbd.f9() || mouse.left())
		{ updateDisplay(); }
	
		startNewDialog();
		adjustWindowSize();
		progress.update(0.0f);
	}
	else if(newButton.hovering(mouseX, mouseY))
		newButton.highlight();
	else
		newButton.unhighlight();
	
	// LOAD button, F10 or CTRL + L
	// load a source file
	if(kbd.f10() || (mouse.left() && loadButton.hovering(mouseX, mouseY)) || kbd.ctrlL())
	{
		while(kbd.f10() || mouse.left() || kbd.ctrlL()){}
		
		loadDialog();
		adjustWindowSize();
		progress.update(0.0f);
	}
	else if(loadButton.hovering(mouseX, mouseY))
		loadButton.highlight();
	else
		loadButton.unhighlight();
	
	// SAVE button, F11 or CTRL + S
	// save a source file
	if(kbd.f11() || (mouse.left() && saveButton.hovering(mouseX, mouseY)) || kbd.ctrlS())
	{
		saveButton.activate();
		while(kbd.f11() || mouse.left() || kbd.ctrlS())
		{ updateDisplay(); }
		
		saveDialog();
		adjustWindowSize();
	}
	else if(saveButton.hovering(mouseX, mouseY))
		saveButton.highlight();
	else
		saveButton.unhighlight();
	
	// QUICK-SAVING ... alt + S
	if(kbd.altS())
	{
		saveButton.activate();
		while(kbd.altS())
		{ updateDisplay(); }
	
		// you have not saved once yet - let's do regular saving
		if(lastSavedPathAndFileName.empty())
		{
			saveDialog();
			adjustWindowSize();
		}
		// you have saved once already...
		// so let's quick-save to same place!
		else
		{
			quickSave();
			// set up a quick message (turns off automatically after a few seconds)
			help.setQuickMessage("Quick-saved!");
		}
	}
	
	// TRY to... invoke system volume control 
	if(kbd.altV())
	{
		while(kbd.altV())
		{ updateDisplay(); }

		// for XP
		// system("sndvol32.exe");

		// for Windows 7 ...
		// WinExec("sndvol.exe -f", SW_HIDE);
		
		string exePath = "C:\\Windows\\System32\\sndvol.exe"; // win 7
		string param = "-f";
		
		string exePath2 = "C:\\Windows\\System32\\sndvol32.exe"; // xp

		ShellExecute( NULL, NULL, exePath.c_str(), param.c_str(), NULL, SW_SHOWNORMAL );
		ShellExecute( NULL, NULL, exePath2.c_str(), NULL, NULL, SW_SHOWNORMAL );		
		cout << "Open - system volume\n";
	}
	
	// open up system audio device control
	if(kbd.altD())
	{
		while(kbd.altD())
		{ updateDisplay(); }
	
		string exeCommand = "mmsys.cpl";
		ShellExecute( NULL, NULL, exeCommand.c_str(), NULL, NULL, SW_SHOWNORMAL );
		cout << "Open - audio device properties\n";
	}

	// initialize audio device / port audio
	if(kbd.altI())
	{
		while(kbd.altI())
		{ updateDisplay(); }
	
		mplayer.pause();
		mplayer.close();
		cout << "Reinitializing portaudio...\n";
		mplayer.initialize();
	}
	
	// open up system audio device control
	if(kbd.altD())
	{
		while(kbd.altD())
		{ updateDisplay(); }
	
		string exeCommand = "mmsys.cpl";
		ShellExecute( NULL, NULL, exeCommand.c_str(), NULL, NULL, SW_SHOWNORMAL );
		cout << "Open - audio device properties\n";
	}

	// EXPORT button or F12
	if(kbd.f12() || (mouse.left() && exportButton.hovering(mouseX, mouseY)))
	{
		exportButton.activate();
		while(kbd.f12() || mouse.left())
		{ updateDisplay(); }

		exportDialog();
		adjustWindowSize();
		progress.update(0.0f);
	}
	else if(exportButton.hovering(mouseX, mouseY))
		exportButton.highlight();
	else
		exportButton.unhighlight();

endOfButtonRoutines:
	
	// CTRL + UP -> volume++
	if(kbd.ctrlUp())
	{
		float newGain = mplayer.getMasterGain() + 0.05f;
		if(newGain > 1.0f) newGain = 1.0f;

		mplayer.setMasterGain(newGain);
		knob.activate();
		while(kbd.ctrlUp()){
			updateDisplay();
		}
		knob.deactivate();
	}

	// CTRL + DOWN -> volume--
	if(kbd.ctrlDown())
	{
		float newGain = mplayer.getMasterGain() - 0.05f;
		if(newGain < 0) newGain = 0;

		mplayer.setMasterGain(newGain);
		knob.activate();
		while(kbd.ctrlDown()){
			updateDisplay();
		}
		knob.deactivate();
	}

	// handle volume knob...
	if(mouse.left() && knob.hovering(mouseX, mouseY))
		knob.activate();
	
	if(knob.isActive() && !mouse.left())
		knob.deactivate();
	
	if(knob.isActive())
	{
		int yDelta = mouse.getDelta();
		if(yDelta <= -1) // volume up
		{
			float newGain = mplayer.getMasterGain() + 0.02f;
			if(newGain > 1.0f) newGain = 1.0f;
			mplayer.setMasterGain(newGain);
		}
		else if(yDelta >= 1) // volume down
		{
			float newGain = mplayer.getMasterGain() - 0.02f;
			if(newGain < 0.0f) newGain = 0.0f;
			mplayer.setMasterGain(newGain);
		}
	}
	
	if(kbd.ctrlA())
	{
		while(kbd.ctrlA()){}
		emptySelection(); // clear up things first...
		selectStart = 0;
		selectEnd = source.length() - 1;
		selecting = false;
		selectFinished = true;
		setHighlighter();

		cout << "Selected: All\n";
	}
	if(kbd.ctrlC())
	{
		while(kbd.ctrlC()){}


		int copyStartPos;
		int nCharsToCopy;
		if(selectEnd < selectStart) // if selection went 'upward'
									// think opposite way...
		{
			copyStartPos = selectEnd ;
			nCharsToCopy = selectStart - selectEnd + 1;
		}
		// otherwise, we have a 'downward' selection
		else
		{
			copyStartPos = selectStart;
			nCharsToCopy = selectEnd - selectStart + 1;
		}
		strToCopy = source.substr(copyStartPos, nCharsToCopy);
		if(strToCopy.length() > 0 && strToCopy.at(strToCopy.length() - 1) == CH_EOF)
		{
			strToCopy.erase(strToCopy.length() - 1, 1);
			nCharsToCopy--;
		}
		setClipBd(strToCopy);

		// copy the content of the highlighted area
		cout << "copied: \n" << strToCopy << endl;
		emptySelection();
	}
	if(kbd.ctrlV()) // paste
	{
		while(kbd.ctrlV()){}
		cout << "CTRL-V!" << endl;

		// if anything is highlighted, clear
		emptySelection();

		// get content of clipboard and paste into str
		string strToPaste = getClipBd();

		// let's take out all tab characters
		bool checkDone = false;
		while(!checkDone)
		{
			char tab = '\t';
			size_t found = strToPaste.find(tab); // tab character
			if(found != string::npos) // tab found
				strToPaste.replace(found, 1, "  "); // replace with two spaces
			else
				checkDone = true;
		}

		// be careful - you don't want to paste a string that has EOF char somewhere in there
		checkDone = false;
		while(!checkDone)
		{
			size_t found = strToPaste.find(CH_EOF);
			if(found != string::npos) // EOF found! erase
				strToPaste.erase(found, 1);
			else
				checkDone = true;
		}

		// store this action in history (undo history stack)
		addHistory("", strToPaste, charPos, charPos, topRenderLine);

		// DEBUG!!!
		cout << "Just about to insert for CTRL-V..\n";
		cout << "charPos=" << charPos << ", source.length()=" << source.length() << endl;
		
		// amend the source string
		if(charPos>=0 && charPos<source.length()) // safeguarding...
			source.insert(charPos, strToPaste);

		// update the charPos position
		charPos += strToPaste.length();

		// reconfigure how many chars in each line
		updateLineStats();

		// reconfigure posInLine and currentLine (screen coordinates)
		updateScreenCoordinates();

		// update the whole text line info..
		setStrView();

		// re-update, in case you're moving down over the bottom of screen
		updateCursorPos();
	}
	if(kbd.ctrlZ())
	{
		while(kbd.ctrlZ()){}
		cout << "CTRL-Z!";
		emptySelection();

		undo();
	}

	// handle mouse inputs
	if(mouseLPressed) // mouse L button - move cursor to place pointed
	{
		if(selecting) // only when selecting - out-of-bound selecting allows for scrolling for cy
		{
			// update pointed char coordinates
			cx = (mouseX - TEXT_TOP_X) / static_cast<double>(charWidth);
			cy = (mouseY - TEXT_TOP_Y) / static_cast<double>(charHeight);
			
			// safeguarding for negative x values
			if(cx < 0) cx = 0;
			
			if(cy < 0) // out of bound - upwards
				cy = -1;
			else if(cy > TEXT_HEIGHT-1)
				cy = TEXT_HEIGHT;

			// put a tiny wait on mouse-L hold down (so scroll won't happen superfast)
			fireClock.restart();
			while(fireClock.getElapsedTime().asSeconds() < 0.05f && mouse.left()){}
		}
		else if(mouseX < 674.0)
		{
			// update pointed char coordinates
			cx = (mouseX - TEXT_TOP_X) / static_cast<double>(charWidth);
			cy = (mouseY - TEXT_TOP_Y) / static_cast<double>(charHeight);
			
			cy = max(0, cy);
			cy = min(TEXT_HEIGHT-1, cy);
		}
		currentLine = max(0, min(topRenderLine + cy, nLines-1)); // put limit - to very last line
		posInLine = min(nCharsLine[currentLine] - 1, cx);
		charPos = getStartPosInLine(currentLine) + posInLine;
		cout << "charPos=" << charPos << " currentLine=" << currentLine << " posinLine=" << posInLine << endl;

		updateCursorPos();
	}
	
	// if selectFinished but clicked outside text area - clear all selection
	if(selectFinished && mouseLPressed && mouseX > 674.0)
	{
		selecting = false;
		selectFinished = false;
	}
	

	if(kbd.home())
	{
		while(kbd.home()){}
		charPos = 0;
		updateScreenCoordinates(); 	// reconfigure posInLine and currentLine
		setStrView(); // update the whole text line info..
		updateCursorPos();	// re-update... topRenderLine etc.
	}
	if(kbd.end())
	{
		while(kbd.home()){}
		charPos = source.length()-1;
		updateScreenCoordinates(); 	// reconfigure posInLine and currentLine
		setStrView(); // update the whole text line info..
		updateCursorPos();	// re-update... topRenderLine etc.
	}
	// shift or mouse drag -> select snipet
	// (exlude shift-typing case!)
	if( kbd.shift() || mouseLPressed)
	{
		if(!selecting && typedWithShift==false) // shift-down (mouseL-Down) has just now begun
		{
			// exclude case where mouse pressed but out of text edit area
			if(mouseLPressed && mouseX > 674.0)
			{
				cout << "Clicked, but out of text area!\n";
			}
			// if dragging the volume knob, won't start selection
			else if(mouseLPressed && knob.isActive())
			{
				cout << "Clicked, but volume knob is active!\n";
			}
			else
			{
				// update pointed char coordinates
				cx = (mouseX - TEXT_TOP_X) / static_cast<double>(charWidth);
				cy = (mouseY - TEXT_TOP_Y) / static_cast<double>(charHeight);				
				
				selectStart = charPos;
				selectEnd = charPos;
				selectStartLine = getLineNumber(selectStart);
				selectEndLine = selectStartLine;
				selecting = true;
				setHighlighter();
	
				cout << "Select started!\n";
				cout << "selectStart = " << selectStart << " selectStartLine = " << selectStartLine << endl;
			}
		}
		// shift-down continues.. so selection will get updated
		if(selecting && typedWithShift==false)
		{
			// update pointed char coordinates
			cx = (mouseX - TEXT_TOP_X) / static_cast<double>(charWidth);
			cy = (mouseY - TEXT_TOP_Y) / static_cast<double>(charHeight);			
			
			if(charPos != selectEnd)
			{
				selectEnd = charPos;
				selectEndLine = getLineNumber(selectEnd);
				setHighlighter();
			}
		}
	}
	else if(!kbd.shift() || mouseLReleased) // shift (mouseL-down) isn't active, or it has just been released
	{
		// was just selecting - shift has just been released
		if(selecting)
		{
			// update pointed char coordinates
			cx = (mouseX - TEXT_TOP_X) / static_cast<double>(charWidth);
			cy = (mouseY - TEXT_TOP_Y) / static_cast<double>(charHeight);

			// EXPERIMENT - safeguarding for negative x, y values
			if(cx < 0) cx = 0;
			if(cy < 0) cy = 0;
			
			selectEnd = charPos;
			selectEndLine = getLineNumber(selectEnd);

			selecting = false;
			selectFinished = true;
			setHighlighter();
			
			// clear shift-typing flag
			typedWithShift = false;

			cout << "Select finished!\n";
			cout << "selectStart = " << selectStart << " selectEnd = " << selectEnd << endl;
			cout << "selected line numbers = " << selectStartLine << " - " << selectEndLine << endl;
		}
	}
	
	// if shift-typing flag is still on - when shift is let go, clear selection
	if(typedWithShift && !kbd.shift())
	{	
		emptySelection();
		typedWithShift = false;
	}

	if(kbd.alt() || mouseRPressed)
	{
		if(selecting || selectFinished)
		{
			emptySelection();
		}
	}
}

// every five mintues, your work gets saved!
void GUI::handleTimedSaving()
{
	if(autoSaveClock.getElapsedTime().asSeconds() > 300.0f)
	{
		autoSave();	
		autoSaveClock.restart();
	}
}

// update panel components that reflect realtime data
void GUI::updatePanel()
{
	// update gain meter from time to time
	if(meterClock.getElapsedTime().asSeconds() > 0.05f)
	{
		updateMeter();
		meterClock.restart();
	}
	
	// volume knob
	knob.update(static_cast<int>(mplayer.getMasterGain() * 100.0f));

	// delay button
	if(!dlyButton.highlighted)
	{
		if(mplayer.delayIsEnabled())
			dlyButton.delayOn();
		else
			dlyButton.delayOff();
	}
}

void GUI::updateDisplay()
{
	// if exiting app... skip display updating
	if(exitApp)
		return;

	blinkCursor();
	updatePanel();	
	
	// let's update gain meter here...
	//
	//
	
	// let's update progress bar here...
	progress.update(mplayer.getProgressRatio());
	
	// if message-show is on
	// just show the message, and not display anything else..
	if(messageShowing)
	{
		window.clear();
		window.draw(message);
		window.display();
		return;
	}
	
	// update objects on display

	// cursor.. toggle according to blink state
	if(blinkState==0)
	{
		cursor.setSize(sf::Vector2f(14, 24));
		cursor.setPosition(cursorX * charWidth + TEXT_TOP_X, cursorY * charHeight + TEXT_TOP_Y);
	}
	else
	{
		cursor.setSize(sf::Vector2f(14, 12));
		cursor.setPosition(cursorX * charWidth + TEXT_TOP_X, cursorY * charHeight + TEXT_TOP_Y + 12);
	}

	// display all elements on screen
	window.clear();

	// draw text objects..
	for(int i=0; i<TEXT_HEIGHT; i++)
		window.draw(text[i]);
	window.draw(cursor);

	// if selecting or selected, draw highlighter, line by line
	if(selecting || selectFinished)
	{
		for(int i=0; i<TEXT_HEIGHT; i++)
			window.draw(highlighter[i]);
	}

	// panel components now...
	
	// draw logo
	window.draw(logo);
	
	//
	// METER - activate again here!
	//
	
	// meter back panel
	window.draw(backPanel);
	
	// display meter
	for(int i=0; i<10; i++)
		window.draw(meter.strip[i]);
	
	// draw progress bar
	progress.draw();
	
	//
	//
	//
	
	// draw buttons
	playButton.draw();
	pauseButton.draw();
	rewindButton.draw();
	forwardButton.draw();
	keyButton.draw();
	docButton.draw();
	dlyButton.draw();
	newButton.draw();
	loadButton.draw();
	saveButton.draw();
	exportButton.draw();	
	
	// draw volume knob
	knob.draw();
	
	// if help is active...
	// draw HELP text for keys
	if(help.isActive())
		help.draw();
	
	// if quickMessage is on, display...
	if(help.quickMessageActive)
		help.drawQuickMessage();

	// now show the updated screen!
	window.display();
}

void GUI::updateLineStats()
{
	bool done = false;

	int lineChCount = 0;
	int lineIndex = 0;
	int searchPos = 0;
	char ch;

	for(int i=0; i<10000; i++)
		nCharsLine[i] = 0;

	while(!done)
	{
		ch = source.at(searchPos);
		lineChCount++;

		if(ch=='\n') // RET found
		{
			nCharsLine[lineIndex] = lineChCount;
			lineIndex++; // go on to next line
			lineChCount = 0;
		}
		else if(ch==CH_EOF) // end of file char found
		{
			nCharsLine[lineIndex] = lineChCount;
			nLines = lineIndex + 1;
			done = true;
		}

		if(lineChCount >= TEXT_WIDTH)
		{
			nCharsLine[lineIndex] = TEXT_WIDTH;
			lineIndex++; // go on to next line
			lineChCount = 0;
		}

		searchPos++;
	}

	for(int i=0; i<nLines; i++)
	{
		// cout << "Line stats updated!" << endl;
		// cout << "Line " << i << " - " << nCharsLine[i] << " chars" << endl;
	}

}

// get arrays of strings that represents viewable area
void GUI::setStrView()
{
	int pos = 0;
	if(topRenderLine > 0) // get the start char pos at top left corner
	{
		for(int i=0; i<topRenderLine; i++)
			pos += nCharsLine[i];
	}

	bool reachedEOF = false;
	for(int i=0; i<TEXT_HEIGHT; i++)
	{
		// if EOF has already been found, fill the rest of lines with empty lines
		if(reachedEOF)
			strView[i] = "";
		else
		{
			string sub = source.substr(pos, nCharsLine[i+topRenderLine]);
			size_t found = sub.find(CH_EOF);
			if(found != string::npos) // if CH_EOF is found..
			{
				reachedEOF = true; // set flag
				sub.replace(found, 1, 1, ' '); // and go ahead and hide it from display
			}

			strView[i] = sub;
			pos += nCharsLine[i+topRenderLine];
		}
	}

	// resend to text[n] array with updated strView[n]
	for(int i=0; i<TEXT_HEIGHT; i++)
		text[i].setString(strView[i]);

}

// recalculate 'charPos' from currentLine and posInLine variables
void GUI::calculateCharPos()
{
	int chIndex = 0;

	if(currentLine == 0)
		charPos = posInLine;
	else
	{
		// add up numbers of chars up to just one before current Line
		for(int i=0; i<currentLine; i++)
			chIndex += nCharsLine[i];
		chIndex += posInLine; // then add the pos at current line - result should be charPos
		charPos = chIndex;
	}
}

// update cursor X, Y as well as topRenderLine
void GUI::updateCursorPos()
{
	if(currentLine < topRenderLine) // went over top limit
	{
		topRenderLine = currentLine;
		setStrView(); // just scrolled up, so reset the view string array
		setHighlighter(); // update highlighter also
	}
	if( (currentLine-topRenderLine) > (TEXT_HEIGHT-1)) // went over bottom limit
	{
		topRenderLine = currentLine - (TEXT_HEIGHT - 1);
		setStrView(); // just scrolled down, so reset the view string array
		setHighlighter();
	}
	cursorX = posInLine;
	cursorY = currentLine - topRenderLine;
}

// set updated coordinates (posInLine and currentLine) from current charPos
void GUI::updateScreenCoordinates()
{
	int remaining = charPos;
	for(int i=0; i<nLines; i++)
	{
		if(remaining < nCharsLine[i])
		{
			posInLine = remaining;
			currentLine = i;
			break;
		}
		else
			remaining -= nCharsLine[i];
	}
}

// use this function right after loading - to re-adjust / re-initialize everything
void GUI::renewForNewSong()
{
	eraseHistory();
	emptySelection();

	cursorX = 0;
	cursorY = 0;
	currentLine = 0;
	posInLine = 0;
	charPos = 0;
	topRenderLine = 0;

	updateLineStats();
	setStrView();
}

// *** run in a new thread ***
// this function creates a dialog box for starting new file - start new if yes
void GUI::startNewDialog()
{
	string strNewDialog = "Please make sure your work has been saved!|Are you sure you want to {start over}?";
	bool newDialogResult = dialog.yesNoDialog(strNewDialog);

	if(newDialogResult) // if yes was chosen
	{
		if(mplayer.isPlaying())
			mplayer.pause();
		source = "";
		source.append<int>(1,0xFF);
		mplayer.resetForNewSong();
		mml.setSource(source);
		mml.parse(&mplayer);
		renewForNewSong();
		mplayer.goToBeginning(); // empty source... but this helps completely initialize

		// initialize current file name
		currentFileName = "";
		// size_t fpos = currentPathAndFileName.find_last_of("\\");
		currentPathAndFileName = defaultPath;

		cout << "Start new!\n";
		cout << "Now current path and filename is\n" << currentPathAndFileName << "\n";

		// set the window title
		window.setTitle(windowTitle);
		autoSaveClock.restart();
	}

	// set the window title
	if(currentFileName.empty())
		window.setTitle(windowTitle);
	else
		window.setTitle(windowTitle + " ... " + currentFileName);
}

void GUI::saveDialog()
{
	string saveFileName = dialog.getSaveFileName();
	cout << "Save dialog completed!" << endl;
	bool shouldCancel = dialog.cancelChosen;
	cout << "cancelChosen: " << shouldCancel << endl;
	
	if(!shouldCancel)
	{
		currentPathAndFileName = saveFileName;
		size_t fpos = currentPathAndFileName.find_last_of("\\");
		currentFileName = currentPathAndFileName.substr(fpos+1);

		// if the file name has no extension, add '.txt'
		if(currentFileName.find('.') == string::npos)
		{
			currentFileName += ".txt";
			currentPathAndFileName += ".txt";
		}

		// update with the source we have now - and parse
		// mplayer.pause();
		// mplayer.resetForNewSong();
		mml.setSource(source);
		// mml.parse(&mplayer);
		
		// DEBUG... turning OFF resetting/reparsing on saving... for now

		cout << "Save: " << currentPathAndFileName << endl;

		// save the file!
		mml.saveFile(currentPathAndFileName, &mplayer);
		// renewForNewSong();
		lastSavedPathAndFileName = currentPathAndFileName;
		autoSaveClock.restart();
	}
	// set the window title
	if(currentFileName.empty())
		window.setTitle(windowTitle);
	else
		window.setTitle(windowTitle + " ... " + currentFileName);
}

void GUI::loadDialog()
{	
	string loadFileName = dialog.getLoadFileName();
	cout << "Load dialog completed!" << endl;
	bool shouldCancel = dialog.cancelChosen;
	cout << "cancelChosen: " << shouldCancel << endl;
	
	if(!shouldCancel)
	{
		currentPathAndFileName = loadFileName;
		size_t fpos = currentPathAndFileName.find_last_of("\\");
		currentFileName = currentPathAndFileName.substr(fpos+1);

		cout << "Open: " << currentPathAndFileName << endl;
		mplayer.pause();
		mplayer.resetForNewSong();
		source = mml.loadFile(currentPathAndFileName, &mplayer); // must pass a c++ string
		mml.parse(&mplayer);
		mplayer.goToBeginning();
		renewForNewSong();
		
		// set the window title
		window.setTitle(windowTitle + " ... " + currentFileName);
		autoSaveClock.restart();
	}
	
	// set the window title
	if(currentFileName.empty())
		window.setTitle(windowTitle);
	else
		window.setTitle(windowTitle + " ... " + currentFileName);
}

void GUI::exportDialog()
{
	string ExportFileName = dialog.getExportFileName();
	cout << "Export dialog completed! " << endl;
	bool shouldCancel = dialog.cancelChosen;
	cout << "cancelChosen: " << shouldCancel << endl;
	
	if(!shouldCancel)
	{
		// set message now...
		setMessage("Please wait while your file's being processed...");
		
		string chosenPathAndFileName = ExportFileName;
		size_t fpos = chosenPathAndFileName.find_last_of("\\");
		string exportFileName = chosenPathAndFileName.substr(fpos+1);

		// if the file name has no extension, add '.wav'
		if(exportFileName.find('.') == string::npos)
			chosenPathAndFileName += ".wav";

		// update with the source we have now - and parse
		mplayer.pause(); // should pause first :)
		mplayer.resetForNewSong();
		mml.setSource(source);
		mml.parse(&mplayer);

		cout << "Export: " << chosenPathAndFileName << endl;

		// save the file!
		string exportResult = mplayer.exportToFile(chosenPathAndFileName);
		cout << exportResult << endl;

		// go back to beginning of the song
		mplayer.goToBeginning();
		
		// unset wait message
		unsetMessage();
	}
	
	// set the window title
	if(currentFileName.empty())
		window.setTitle(windowTitle);
	else
		window.setTitle(windowTitle + " ... " + currentFileName);
}

// this function creates a dialog box for starting new file - start new if yes
void GUI::quitDialog()
{
	string strQuitDialog = "Are you sure you want to {quit}?";
	bool quitDialogResult = dialog.yesNoDialog(strQuitDialog);

	if(quitDialogResult) // if yes was chosen
	{
		cout << "Quit selected! Exiting app now...\n";
		exitApp = true;
	}
	else
		cout << "Quit - canceled by user!\n";
}

string GUI::getClipBd()
{
	string strToReturn = "";
	if(OpenClipboard(NULL) != FALSE)
	{
		HANDLE clipData = GetClipboardData(CF_TEXT);
		char *c = static_cast<char*>(GlobalLock(clipData));

		if(clipData != NULL)
			strToReturn = c;

		GlobalUnlock(clipData);
		CloseClipboard();
	}
	return strToReturn;
}

void GUI::setClipBd(const string &str)
{
	if(OpenClipboard(NULL) != FALSE)
	{
		EmptyClipboard();
		HGLOBAL hg=GlobalAlloc(GMEM_MOVEABLE,str.size()+1);
		if(!hg)
		{
			CloseClipboard();
		return;
		}
		memcpy(GlobalLock(hg),str.c_str(),str.size()+1);
		GlobalUnlock(hg);
		SetClipboardData(CF_TEXT,hg);
		CloseClipboard();
		GlobalFree(hg);
	}
}

void GUI::addHistory(string deletedStr, string insertedStr, int actionAtThen, int charPosThen, int topRenderThen)
{
	cout << "Add history - deleted: " << deletedStr << " - inserted: " << insertedStr << endl;
	cout << "History Level: " << historyLevel << endl;

	// if history level has reached limit, roll back whole array and make room to insert
	if(historyLevel == MAX_UNDO_LEVEL)
	{
		for(int i=1; i<MAX_UNDO_LEVEL; i++)
		{
			deleted[i-1] = deleted[i];
			inserted[i-1] = inserted[i];
			performedAt[i-1] = performedAt[i];
			charPosHist[i-1] = charPosHist[i];
			topRenderLineHist[i-1] = topRenderLineHist[i];
		}
		historyLevel--;
	}

	// now store the passed action history
	deleted[historyLevel] = deletedStr;
	inserted[historyLevel] = insertedStr;
	performedAt[historyLevel] = actionAtThen;
	charPosHist[historyLevel] = charPosThen;
	topRenderLineHist[historyLevel] = topRenderThen;

	// and increment history level counter
	historyLevel++;
}

void GUI::undo()
{
	// if there is history record stored, we can undo
	if(historyLevel > 0)
	{
		cout << "in undo function..\ndeleted[prev] = " << deleted[historyLevel - 1] << " inserted[previous] = " << inserted[historyLevel - 1] << endl;
		cout << "History Level: " << historyLevel << endl;

		int previous = historyLevel -1;
		// if previous step was 'INSERT'
		if(deleted[previous].length()<1 && inserted[previous].length()>=1)
		{
			cout << "Undo: now DELETING to undo INSERTING!\n";

			// then, we will DELETE to roll back
			source.erase(performedAt[previous], inserted[previous].length());
			charPos = charPosHist[previous];
			historyLevel--;
		}
		// if previous step was 'DELETE'
		else if (deleted[previous].length()>=1 && inserted[previous].length()<1)
		{
			// then, we will INSERT to roll back

			cout << "Undo: now INSERTING to undo DELETING!\n";

			// then, we will INSERT to roll back
			source.insert(performedAt[previous], deleted[previous]);
			charPos = charPosHist[previous];
			historyLevel--;
		}
		else
			return;

		// adjust back n. of chars for all the lines
		// and all the screen-related parameters
		updateLineStats();
		updateScreenCoordinates();
		setStrView();
		updateCursorPos();
	}
}

// purges all undo history records
void GUI::eraseHistory()
{
	historyLevel = 0;
	for(int i=0;i<MAX_UNDO_LEVEL;i++)
	{
		deleted[i] = "";
		inserted[i] = "";
		performedAt[i] = 0;
		charPosHist[i] = 0;
		topRenderLineHist[i] = 0;
	}

}

// takes a line number and returns the starting char pos number
// for that line
int GUI::getStartPosInLine(int line)
{
	int count = 0;
	for(int i=0; i<line; i++)
		count += nCharsLine[i];
	return count;
}

int GUI::getLineNumber(int cPos)
{
	int remaining = cPos;
	int lineIndex = 0;
	int lineNumber = 999;

	while(remaining >= 0)
	{
		remaining -= nCharsLine[lineIndex];
		if(remaining < 0)
			lineNumber = lineIndex;
		else
			lineIndex++;
	}
	return lineNumber;
}

void GUI::emptySelection()
{
	if(!selecting && !selectFinished) // already cancelled.. just exit
		return;

	selecting = false;
	selectFinished = false;
	selectStart = 0;
	selectEnd = 0;
	selectStartLine = 0;
	selectEndLine = 0;
	setHighlighter(); // highlighter will now all get sized to zero (no show)

	cout << "Selection cancelled!\n";
}

void GUI::setHighlighter()
{
	// if not selecting or selection has been made (and not have been cancelled)
	// should not show any highlighter
	if(!selecting && !selectFinished)
	{
		for(int i=0; i<TEXT_HEIGHT; i++)
		{
			highlighter[i].setPosition(0,0);
			highlighter[i].setSize(sf::Vector2f(0,0));
		}
	}

	// get the positions and lines to highlighted
	// account for either directions (up + down) selection can go
	int markStartPos, markEndPos;
	int markStartLine, markEndLine;
	if(selectStart==selectEnd) // we only have one letter selected
	{
		markStartPos = selectStart;
		markEndPos = selectEnd;
		markStartLine = selectStartLine;
		markEndLine = selectStartLine;
	}
	else if(selectStart < selectEnd) // normal 'downward' selection
	{
		markStartPos = selectStart;
		markEndPos = selectEnd ;
		markStartLine = selectStartLine;
		markEndLine = getLineNumber(markEndPos);
	}
	else // 'upward' selection
	{
		markStartPos = selectEnd;
		markEndPos = selectStart;
		markStartLine = getLineNumber(markStartPos);
		markEndLine = selectStartLine;
	}

	// will design the rectangles to highlight for each line...

	int lineIndex;
	int hx, hy, hw, hh;

	// loop through each line for masking lines
	for(int i=0; i<TEXT_HEIGHT; i++)
	{
		lineIndex = topRenderLine + i;

		// check first if all your selection falls in a single line
		// then draw partially highlighted line that falls in the middle of the line
		if(lineIndex==markStartLine && lineIndex==markEndLine)
		{
			int markStartPosInLine = markStartPos - getStartPosInLine(markStartLine);
			// int markEndPosInLine = markEndPos - getStartPosInLine(markEndLine); unused
			int nCharsToHighlight = (markEndPos - markStartPos) + 1;
			hx = markStartPosInLine * charWidth + TEXT_TOP_X;
			hy = i * charHeight + TEXT_TOP_Y;
			hw = nCharsToHighlight * charWidth;
			hh = 24;
		}
		// check if this line is the selection 'top' line
		// then draw partially highlighted line (goes on right side)
		else if(lineIndex==markStartLine)
		{
			int markStartPosInLine = markStartPos - getStartPosInLine(markStartLine);
			hx = markStartPosInLine * charWidth + TEXT_TOP_X;
			hy = i * charHeight + TEXT_TOP_Y;
			hw = (nCharsLine[lineIndex] - markStartPosInLine) * charWidth;
			hh = 24;
		}
		// check if this line is the selection 'bottom' line
		// then draw partially highlited line (goes on left side)
		else if(lineIndex==markEndLine)
		{
			int markEndPosInLine = markEndPos - getStartPosInLine(markEndLine);
			hx = TEXT_TOP_X;
			hy = i * charHeight + TEXT_TOP_Y;
			hw = (markEndPosInLine + 1) * charWidth;
			hh = 24;
		}
		// check if this line is below top line and above bottom line
		// then highlight all characters in this line
		else if( (markStartLine < lineIndex) && (lineIndex < markEndLine) )
		{
			hx = TEXT_TOP_X;
			hy = i * charHeight + TEXT_TOP_Y;
			hw = nCharsLine[lineIndex] * charWidth;
			hh = 24;
		}
		// otherwise, this line won't be highlighted at all
		else
		{
			hx = 0; hy = 0; hw = 0; hh = 0;
		}

		// now set the highlighter for this line accordingly
		highlighter[i].setPosition(hx, hy);
		highlighter[i].setSize(sf::Vector2f(hw, hh));
	}
}

// toggle cursor shape at a regular interval to make it blink
void GUI::blinkCursor()
{
	if(!windowFocused)
		return;
	// if enough time has passed, then toggle cursor shape
	if(blinkClock.getElapsedTime().asSeconds() > 0.6f)
	{
		blinkState++;
		if(blinkState==2)
			blinkState = 0;
		blinkClock.restart();
	}
}

// update the meter panel with retrieved gain info for each channel
void GUI::updateMeter()
{
	for(int i=0; i<10; i++)
	{
		float value = mplayer.getHistoricalAverage(i);
		meter.set(i, value);
	}
}

void GUI::setMessage(string strMessage)
{
	message.setFont(font); // font is a sf::Font
	message.setCharacterSize(24); // in pixels, not points!
	message.setColor(sf::Color::Green);
	int messageX = (WINDOW_WIDTH/2) - (strMessage.length() * charWidth) / 2;
	message.setPosition(sf::Vector2f(messageX, (WINDOW_HEIGHT/2) - charHeight * 3));
	message.setString(strMessage);
	
	messageShowing = true;
	updateDisplay(); // update the screen once
	window.setActive(false); // stop window GL rendering	
}

void GUI::unsetMessage()
{
	messageShowing = false;
	window.setActive(true); // resstart window GL rendering
}

// in case you've come back from file dialog in a different-sized window...
void GUI::adjustWindowSize()
{
	sf::Vector2u size = window.getSize();
	adjustedWindowWidth = static_cast<double>(size.x);
	adjustedWindowHeight = static_cast<double>(size.y);	
}

// autosaving (done every so often or when exiting...)
void GUI::autoSave()
{
	string autoSavePath = defaultPath + "/__AUTOSAVED__.txt";
	cout << "Autosaving to... " << autoSavePath << endl;
	// save the file!
	mml.setSource(source);
	mml.saveFile(autoSavePath, &mplayer);
	help.setQuickMessage("Auto-saving...");
}

// quick-save ... called when ALT + S is pressed
void GUI::quickSave()
{
	if(lastSavedPathAndFileName.empty())
	{
		cout << "Error: Last saved location is empty...\n";
		return;
	}
	
	cout << "Quick-saving to... " << lastSavedPathAndFileName << endl;
	// save the file!
	mml.setSource(source);
	mml.saveFile(currentPathAndFileName, &mplayer);
}