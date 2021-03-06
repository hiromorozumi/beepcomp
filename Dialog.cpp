#include <Windows.h>
#include <shlobj.h>
#include <iostream>
#include <string>
#include <vector>
#include <cstdio>
#include <sstream>
#include "Dialog.h"

#pragma comment(lib, "shell32.lib")

using namespace std;

Dialog::Dialog()
{

	exitDialog = false;
	exitBasicDialog = false;
	exitGoToDialog = false;
	windowClosed = false;
	windowFocused = true;
	cancelChosen = false;
	atBaseDialogLevel = true;
	
	adjustedWindowWidth = WINDOW_WIDTH;
	adjustedWindowHeight = WINDOW_HEIGHT;

	// load main font
	// string fontfile = "fonts/EnvyCodeR.ttf";
	string fontfile = getCurrentDir() + "\\fonts\\UbuntuMono-R.ttf";
	if (!font.loadFromFile(fontfile))
		{ cout << "Loading font - error!" << endl; }

	// load mini font
	string miniFontFilePath = getCurrentDir() + "\\fonts\\04B09.ttf";
	if (!miniFont.loadFromFile(miniFontFilePath))
		{ cout << "Error reading font for the Knob object..\n"; }
	
	// set up help screen...
	help.initialize(miniFont);
	float helpYSpacing = 14.0;
	float helpXTab = 134.0;
	help.set(0 , "ESC",							12,				8 );
	help.set(1 , "....  CANCEL",				12 + helpXTab,	8 );
	help.set(2 , "F1", 		  					12           ,	8 + helpYSpacing*1);
	help.set(3 , "....  TOGGLE FILE TYPE",		12 + helpXTab,	8 + helpYSpacing*1);
	help.set(4 , "F2",   						12           ,	8 + helpYSpacing*2);
	help.set(5 , "....  DISPLAY HELP",			12 + helpXTab,	8 + helpYSpacing*2);
	help.set(6 , "F12",   						12           ,	8 + helpYSpacing*3);
	help.set(7 , "....  LOAD / SAVE / EXPORT",	12 + helpXTab,	8 + helpYSpacing*3);
	help.set(8 , "RETURN",   					12           ,	8 + helpYSpacing*4);
	help.set(9 , "....  SELECT FILE",			12 + helpXTab,	8 + helpYSpacing*4);
	help.set(10, "HOME",						12           ,	8 + helpYSpacing*5);
	help.set(11, "....  TO DEFAULT DIR",		12 + helpXTab,	8 + helpYSpacing*5);
	help.set(12, "ALT + O",						12           ,	8 + helpYSpacing*6);
	help.set(13, "....  OPEN CURRENT DIR",		12 + helpXTab,	8 + helpYSpacing*6);
	help.set(14, "ALT + D",						12           ,	8 + helpYSpacing*7);
	help.set(15, "....  TO DESKTOP",			12 + helpXTab,	8 + helpYSpacing*7);
	help.set(16, "CTRL + Z",					12           ,	8 + helpYSpacing*8);
	help.set(17, "....  BACK TO PREVIOUS DIR",	12 + helpXTab,	8 + helpYSpacing*8);
	help.set(18, "CTRL + G",					12           ,	8 + helpYSpacing*9);
	help.set(19, "....  GO TO DRIVE / FOLDER",	12 + helpXTab,	8 + helpYSpacing*9);
	help.set(20, "CTRL + R",					12           ,	8 + helpYSpacing*10);
	help.set(21, "....  REFRESH",				12 + helpXTab,	8 + helpYSpacing*10);
	
	help.setBlackOut(0, 0, 436, helpYSpacing*11+20);	
	help.deactivate(); // start inactive...

	// main char size
	charHeight = font.getLineSpacing(25);
	charWidth = 13;

	// set uniform blue color
	dialogGreen = sf::Color(180,255,180,255);
	dialogGreenLight = sf::Color(255,255,200,255);
	dialogRed = sf::Color(255,170,150,255);
	dialogRedLight = sf::Color(255,200,180,255);
	dialogRedDark = sf::Color(170,90,70,255);
	dialogBlue = sf::Color(180,180,255,255);
	
	// for broswer area
	filesFolders.resize(0);
	
	// initialize viewable string arrays
	initializeStrView();
	
	// mouse related
	mouseX = 0;
	mouseY = 0;
	mouseYprev = 0;
	mouseWheelMoved = 0;
	mouseLPressed = false;
	mouseLReleased = true;
	dragging = false;
	
	// create and set position for slider
	makeSlider(&slider, 16, 30, 294);
	positionSlider(&slider, 740, TEXT_TOP_Y + 4);
	
	// input line section
	inputLine.setSize(sf::Vector2f(charWidth*(INPUT_WIDTH+2), charHeight*1.30)); // was *1.25
	inputLine.setPosition(sf::Vector2f(INPUT_TOP_X, INPUT_TOP_Y));
	inputLine.setFillColor(sf::Color(200, 88, 66, 80));
	inputLineLabel.setFont(font);
	inputLineLabel.setColor(dialogGreen);
	inputLineLabel.setCharacterSize(22);
	inputLineLabel.setString("Filename");
	inputLineLabel.setPosition(sf::Vector2f(94, INPUT_TOP_Y));
	inputLineText.setFont(font);
	inputLineText.setColor(dialogRed);
	inputLineText.setCharacterSize(25);
	inputLineText.setPosition(220 + charWidth, INPUT_TOP_Y - 2 + charHeight*0.05);
	
	// selector mask
	selectMask.setSize(sf::Vector2f(charWidth*(TEXT_WIDTH+1), charHeight*1.12));
	selectMask.setFillColor(sf::Color(220,220,220,100));
	
	// current working directory display
	cwd.setFont(font);
	cwd.setCharacterSize(22);
	cwd.setColor(dialogGreen);
	cwd.setPosition(sf::Vector2f(92, 38));
	
	// for goToDialog - set drive group and folders group label text
	drivesGroupLabel.setFont(font);
	drivesGroupLabel.setCharacterSize(25);
	drivesGroupLabel.setColor(dialogGreen);
	foldersGroupLabel.setFont(font);
	foldersGroupLabel.setCharacterSize(25);
	foldersGroupLabel.setColor(dialogGreen);
	
	// set default current directory
	installDir = getCurrentDir();
	defaultDir = "C:\\";
	// defaultDir = installDir + "\\userdata"; // DECOMMENT THIS LINE FOR *** PORTABLE ***
	currentDir = defaultDir;
	cwd.setString(trimLeft(currentDir, CWD_WIDTH));
	pathNameRect = cwd.getGlobalBounds();
	requestedStartFolder = "";
	
	// default fileFilter
	fileFilter = ".txt";
	
	// initialize cursor
	cursorPos = 0;
	startRenderPos = 0;
	blinkState = 0;
	cursor.setFillColor(sf::Color(255, 225, 225, 128));
	cursor.setSize(sf::Vector2f(14, 24));
	cursor.setPosition(INPUT_TOP_X + charWidth, INPUT_TOP_Y + 5);
	blinkClock.restart();
}

Dialog::~Dialog()
{}

void Dialog::setDefaultDir(const std::string &defDir)
{
	defaultDir = defDir;
	currentDir = defaultDir;
}

string Dialog::getCurrentDir()
{
    char buffer[256];
    GetModuleFileName( NULL, buffer, MAX_PATH );
    string::size_type pos = string( buffer ).find_last_of( "\\/" );
    return string( buffer ).substr( 0, pos);
}

vector<string> Dialog::getFileNamesInDir(string folder, string filter)
{
    vector<string> names;
    char search_path[512];
	string strToPass = "%s/*" + filter;
    sprintf(search_path, strToPass.c_str(), folder.c_str());
    WIN32_FIND_DATA fd; 
    HANDLE hFind = ::FindFirstFile(search_path, &fd); 
    if(hFind != INVALID_HANDLE_VALUE) { 
        do { 
            // read all (real) files in current folder
            // , delete '!' read other 2 default folder . and ..
            if(! (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) ) 
			{
				names.push_back(fd.cFileName);
            }
        }while(::FindNextFile(hFind, &fd)); 
        ::FindClose(hFind); 
    } 
    return names;
}

vector<string> Dialog::getDirNamesInDir(string folder)
{
    vector<string> names;
    char search_path[512];
    sprintf(search_path, "%s/*.*", folder.c_str());
    WIN32_FIND_DATA fd; 
    HANDLE hFind = ::FindFirstFile(search_path, &fd); 
    if(hFind != INVALID_HANDLE_VALUE) { 
        do { 
            // read all (real) files in current folder
            // , delete '!' read other 2 default folder . and ..
            if( (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) )
			{
                string fileName = fd.cFileName;
				if(fileName!="." && fileName !="..")
					names.push_back(fd.cFileName);
            }
        }while(::FindNextFile(hFind, &fd)); 
        ::FindClose(hFind); 
    } 
    return names;
}
	
void Dialog::bindWindow(sf::RenderWindow* windowObj)
{
	w = windowObj;
	help.bindWindow(w);
}

std::string Dialog::getLoadFileName()
{
	// change window title
	w->setTitle("Load from a file");
	
	// make buttons to be used
	makeButton(&loadButton, " Load ", 0);
	makeButton(&cancelButton, " Cancel ", 0);
	makeButton(&helpButton, " Help ", 2);
	makeButton(&goButton, "<", 3);
	positionButton(&loadButton, 544, 560);
	positionButton(&cancelButton, 650, 560);
	positionButton(&helpButton, 435, 560);
	positionButton(&goButton, 734, 42);
	addHelp(&loadButton, "F12", 1);
	addHelp(&cancelButton, "ESC", 1);
	addHelp(&helpButton, "F2", 1);
	// addHelp(&goButton, "F3", 1);
	
	// file filter button
	fileFilter = ".txt";
	makeButton(&filterButton, "  File Type: Text  ", 1);
	positionButton(&filterButton, 92, 560);
	addHelp(&filterButton, "F1", 0);

	dialogMode = 0;
	fileNameToReturn = "";
	runFileDialog();
	return fileNameToReturn;
}

std::string Dialog::getSaveFileName()
{
	// change window title
	w->setTitle("Save to a file");	
	
	// make buttons to be used
	makeButton(&saveButton, " Save ", 0); // "Save"
	makeButton(&cancelButton, " Cancel ", 0);
	makeButton(&helpButton, " Help ", 2);
	makeButton(&goButton, "<", 3);
	positionButton(&saveButton, 544, 560);
	positionButton(&cancelButton, 650, 560);
	positionButton(&helpButton, 435, 560);
	positionButton(&goButton, 734, 42);
	addHelp(&saveButton, "F12", 1);
	addHelp(&cancelButton, "ESC", 1);
	addHelp(&helpButton, "F2", 1);
	
	// file filter button
	fileFilter = ".txt";
	makeButton(&filterButton, "  File Type: Text  ", 1);
	positionButton(&filterButton, 92, 560);
	addHelp(&filterButton, "F1", 0);

	dialogMode = 1;
	fileNameToReturn = "";
	runFileDialog();
	return fileNameToReturn;
}

std::string Dialog::getExportFileName()
{
	// change window title
	w->setTitle("Export to a file");		
	
	// make buttons to be used
	makeButton(&exportButton, " Export ", 0);
	makeButton(&cancelButton, " Cancel ", 0);
	makeButton(&helpButton, " Help ", 2);
	makeButton(&goButton, "<", 3);
	positionButton(&exportButton, 528, 560);
	positionButton(&cancelButton, 650, 560);
	positionButton(&helpButton, 419, 560);
	positionButton(&goButton, 734, 42);
	addHelp(&exportButton, "F12", 1);
	addHelp(&cancelButton, "ESC", 1);
	addHelp(&helpButton, "F2", 1);
	
	// file filter button
	fileFilter = ".*";
	// makeButton(&filterButton, "  File Type: Text File  ", 1);
	// positionButton(&filterButton, 92, 560);
	// addHelp(&filterButton, "F1", 0);

	supportedFilesText.setFont(font);
	supportedFilesText.setColor(dialogBlue);
	supportedFilesText.setCharacterSize(20);
	supportedFilesText.setString("Supported: .wav .mp3 .ogg");
	supportedFilesText.setPosition(sf::Vector2f(94,560));
	
	dialogMode = 2;
	fileNameToReturn = "";
	runFileDialog();
	return fileNameToReturn;
}

void Dialog::runFileDialog()
{
	// in case starting with a different-sized window...
	sf::Vector2u size = w->getSize();
	adjustedWindowWidth = static_cast<double>(size.x);
	adjustedWindowHeight = static_cast<double>(size.y);
	
	// DEBUG - mysterious 'E' character?? - clear buffer anyway
	typedChar = 0;
	strTypedChar = "";
	
	// initialize values before running...
	mouseLPressed = false;
	mouseLReleased = true;
	dragging = false;
	exitDialog = false;
	exitBasicDialog = false;
	cancelChosen = false;
	atBaseDialogLevel = true;
	topRenderLine = 0;
	selectedIndex = 0;
	selectedIndexPrevious = 0;
	strInput = "";
	cursorPos = 0;
	fireClock.restart();
	pageClickCount = 0;
	historyLevel = 0;
	undoing = false;
	refreshing = false;
	calledExplorerAlready = false;
	
	startingOut = true; // this is the only time this flag is used!
	
	// if start folder has been specified, start there (currentDir gets updated)
	if(!requestedStartFolder.empty() && dirExists(requestedStartFolder))
		readDir(requestedStartFolder, fileFilter);
	else
		readDir(defaultDir, fileFilter); // otherwise, use defaultDir (install folder or last-used folder)
	
	startingOut = false; // turn off this flag - no other place will use this flag!
		
	// patch... for save / export dialogs, force starting on the save placeholder line (2nd line)
	if(dialogMode==1 || dialogMode==2)
		selectedIndex = 1;
	
	cout << "selectedIndex at start " << selectedIndex << endl;
	
	// will start in current dir (or last used dir)
	// readDir(currentDir, fileFilter);
	
	while(!exitDialog)
	{	
		pollEvents();
		
		handleInputFileDialog();
		drawFileDialog();	
	}
}

void Dialog::pollEvents()
{
		while (w->pollEvent(event))
		{
			// handle x-ing out the window
			if (event.type == sf::Event::Closed)
			{
				// if you're coming from yes/no dialog or drive 'go-to' dialog, simply return to base file dialog
				if(!exitBasicDialog || !exitGoToDialog)
				{
					exitBasicDialog = true;
					exitGoToDialog = true;
					windowClosed = true;
				}
				// otherwise you're exiting from base dialog - return to GUI, pass cancelChosen = true
				else
				{
					exitDialog = true;
					exitBasicDialog = true;
					exitGoToDialog = true;
					windowClosed = true;
				
					cancelChosen = true;
				}
			}
			/*
			if(event.type == sf::Event::Closed && atBaseDialogLevel)
			{
				cancelChosen = true;
				exitDialog = true;
			}
			*/
			// check for mouse events now
			if (event.type == sf::Event::MouseButtonPressed)
			{
				if (event.mouseButton.button == sf::Mouse::Left)
					mouseLPressed = true;
				mouseLReleased = false;
			}
			else if (event.type == sf::Event::MouseButtonReleased)
			{
				if (event.mouseButton.button == sf::Mouse::Left)
					mouseLReleased = true;
				mouseLPressed = false;
			}
			else if (event.type == sf::Event::MouseMoved)
			{
				mouseX = event.mouseMove.x * WINDOW_WIDTH / adjustedWindowWidth;
				mouseY = event.mouseMove.y * WINDOW_HEIGHT / adjustedWindowHeight;
			}
			
			// see if any text char has been typed
			if (event.type == sf::Event::TextEntered)
			{
				if (event.text.unicode < 128 && event.text.unicode != 0)
				{
					typedChar = static_cast<char>(event.text.unicode);
					sf::String sfStr = event.text.unicode;
					strTypedChar = sfStr.toAnsiString();
				}
				else
					typedChar = 0;
			}
			if (event.type == sf::Event::LostFocus)
				windowFocused = false;

			if (event.type == sf::Event::GainedFocus)
				windowFocused = true;
			
			// handle resize event
			if(event.type == sf::Event::Resized)
			{
				adjustedWindowWidth = static_cast<double>(event.size.width);
				adjustedWindowHeight = static_cast<double>(event.size.height);
			}
			
			// check for mouse wheel
			if (event.type == sf::Event::MouseWheelMoved)
			{
				mouseWheelMoved = event.mouseWheel.delta;
			}
		}
		
		// mouse! this might just be a better way to get coordinates
		mouseX = static_cast<double>(sf::Mouse::getPosition().x - w->getPosition().x -4)  * WINDOW_WIDTH / adjustedWindowWidth;
		mouseY = static_cast<double>(sf::Mouse::getPosition().y - w->getPosition().y - 23)  * WINDOW_HEIGHT / adjustedWindowHeight;
		
		// determine mouse delta amount
		mouseXdelta = mouseX - mouseXprev;
		mouseYdelta = mouseY - mouseYprev;
		mouseXprev = mouseX;
		mouseYprev = mouseY;
}

void Dialog::handleInputFileDialog()
{
	// if window has lost focus, skip input-handling altogether
	if(!windowFocused)
		return;
	
	// browser box range..
	//   x:  TEXT_TOP_X  ... TEXT_TOP_X + charWidth*TEXT_WIDTH
	//   y:  TEXT_TOP_Y  ... TEXT_TOP_Y + charHeight*TEXT_HEIGHT
	
	// if clicked on a filename line, select that itemText
	if(mouse.left()
		&& mouseX >= TEXT_TOP_X && mouseX <= TEXT_TOP_X + charWidth * TEXT_WIDTH
		&& mouseY >= TEXT_TOP_Y && mouseY <= TEXT_TOP_Y + charHeight * TEXT_HEIGHT + 5)
	{
		pageClickCount++;
		while(mouse.left()){}
		
		selectedIndexPrevious = selectedIndex; // for checking for double-clicking the same item...
		
		// line number that's selected on browser display
		int selectedLineDisplay = (mouseY-TEXT_TOP_Y-8)/charHeight;
		
		// translate that with topRenderLine, and also check you're not out of bound
		int possiblySelectedIndex = selectedLineDisplay + topRenderLine;
		if(possiblySelectedIndex<nItems)
			selectedIndex = possiblySelectedIndex;
		// otherwise... make no change to selection
		
		// if file is selected...change the input line string
		if(itemTypes[selectedIndex]==2)
		{
			strInput = filesFolders[selectedIndex];
			cursorPos = strInput.length();
		}
		
		//  check for double-clicking now...
		bool doubleClickedSameItem = (selectedIndexPrevious==selectedIndex);
		
		// DEBUG
		if(clickTimer.getElapsedTime().asSeconds()<0.5f && pageClickCount>=2 && !doubleClickedSameItem)
			cout << "double-clicked, but different item!\n";

		// check if this item has been DOUBLE-CLICKED
		if(clickTimer.getElapsedTime().asSeconds()<0.5f && pageClickCount>=2 && doubleClickedSameItem)
		{
			cout << "double clicked!\n";
			
			// now.. go ahead and change current directory
			if(itemTypes[selectedIndex]==0) // parent folder chosen
			{
				if(currentDir != "C:\\" && currentDir.length() > 3)
				{
					// get the parent dir name
					string newDir = currentDir;
					size_t found = currentDir.find_last_of("\\/");
					if(found!=string::npos && !currentDir.length()<=3)
													// exclude root dir case
						newDir = currentDir.substr(0,found);
					cout << "new dir: " << newDir << endl;
					
					readDir(newDir, fileFilter); // go to parent folder
				}
				else
					cout << "you're at root! no change..\n";
			}
			else if(itemTypes[selectedIndex]==1) // chose folder
			{
				string newDir = currentDir + "\\" + filesFolders[selectedIndex];
				readDir(newDir, fileFilter); // now go to that folder
			}
			else if(itemTypes[selectedIndex]==2) // chose file - go ahead and load
			{
				if(dialogMode==0)
				{
					strInput = filesFolders[selectedIndex];
					drawFileDialog(); // update screen once
					cancelChosen = false;
					fileNameToReturn = trimSpaceRight(currentDir+"\\"+strInput);
					exitDialog = true;
				}
			}
			// or you're on a place holder - for saving / exporting
			else if(itemTypes[selectedIndex]==3 && !strInput.empty())
			{
				// if the requested filename already exists, check!
				bool sameNameExists = overwriting(strInput);
				string strExtension = "";
				if(dialogMode==1 && strInput.find(".")==string::npos && overwriting(strInput+".txt"))
					{ sameNameExists = true; strExtension = ".txt"; }
				if(dialogMode==2 && strInput.find(".")==string::npos && overwriting(strInput+".wav"))
					{ sameNameExists = true; strExtension = ".wav"; }
				bool overwriteOkay = false;
				if(sameNameExists)
				{
					// file by this name exists in this folder. confirm
					cout << "File by this name already exists..." << endl;
					cout << "Will confirm with yes-no dialog! \n";
					
					string fname = trimRight(strInput, 26) + strExtension;
					string strQuestion = "File named {" + fname + "} already exists.|Are you sure you want to {overwrite}?";
					overwriteOkay = yesNoDialog(strQuestion);
					cout << "dialog result = " << overwriteOkay << endl;
				}
				// only if not overwriting, or overwriting is confirmed.. proceed
				if(!sameNameExists || overwriteOkay)
				{
					// go ahead and return this file as target
					cancelChosen = false;
					fileNameToReturn = trimSpaceRight(currentDir+"\\"+strInput);
					exitDialog = true;
					if(overwriteOkay){cout << "overwring..." << endl;}
				}
				else
					cout << "save cancelled!" << endl;					
			}
		}
		
		// well you just clicked, so reset the double click timer
		clickTimer.restart();
		
		// cout << "in...selected:" << selectedIndex << "..";	
	}
	
	// handle right mouse click - reverts to previous path
	if( mouse.right() || kbd.end() || kbd.ctrlZ() )
	{
		while(mouse.right() || kbd.end() || kbd.ctrlZ() ){}
		string previousDir = popHistory();
		if(!previousDir.empty()) // if there's no record to pop, you get empty string
								// make sure there is record to retrieve
		{
			undoing = true; // this flag prevents pushing of history
			readDir(previousDir, fileFilter); // if record for previous path exists, revert
			undoing = false; // this is the ONLY place when undoing flag should be used :)
		}
		else
			cout << "un-doing requested but history is empty! (no change :))\n";
		
	}
	
	// handle typed events
	if((dialogMode==1 || dialogMode==2 ) && typedChar > 0)
	{
		cout << "typedChar = " << typedChar << endl;

		if(typedChar==8) // backspace key
		{
			if(strInput.length()>0)
			{
				strInput.erase(strInput.length()-1, 1);
				cursorPos = strInput.length();
			}
		}
		// otherwise, regular typing, so append this char
		else if(typedChar>=32 && typedChar<=126)
		{
			strInput.append(1, typedChar);
			cursorPos = strInput.length();
		}	
		typedChar = 0;
		
	}

	// handle cursor keys
	if(kbd.up() || mouseWheelMoved > 0)
	{
		mouseWheelMoved = 0;
		if(!upFiring)
		{
			fireClock.restart();
			while(kbd.up()&&fireClock.getElapsedTime().asSeconds()<0.5f)
			{}
			upFiring = true;
		}
		else
		{
			fireClock.restart();
			while(fireClock.getElapsedTime().asSeconds()<0.06f){}
		}
		
		if(selectedIndex>0)
		{
			selectedIndex--;
			if(selectedIndex<topRenderLine && topRenderLine>0)
			{
				topRenderLine--;
				int sPos = slider.range*topRenderLine/(nItems-TEXT_HEIGHT);
				setSliderPos(&slider, sPos); // update slider
			}
			setStrView();
		}
	}
	else
		upFiring = false;

	if(kbd.down() || mouseWheelMoved < 0)
	{
		mouseWheelMoved = 0;
		if(!downFiring)
		{
			fireClock.restart();
			while(kbd.down()&&fireClock.getElapsedTime().asSeconds()<0.5f)
			{}
			downFiring = true;
		}
		else
		{
			fireClock.restart();
			while(fireClock.getElapsedTime().asSeconds()<0.06f){}
		}		

		if(selectedIndex<nItems-1)
		{
			selectedIndex++;
			if(selectedIndex-topRenderLine >= TEXT_HEIGHT)
			{
				topRenderLine++;
				int sPos = slider.range*topRenderLine/(nItems-TEXT_HEIGHT);
				setSliderPos(&slider, sPos); // update slider	
			}
			setStrView();
		}
		downProcessedCount++;
	}
	else
		downFiring = false;
	
	if(kbd.left())
	{
		while(kbd.left()){}
		
		// if 'folder' is currently chosen, go ahead and change current directory
		if(itemTypes[selectedIndex]==0) // parent folder chosen
		{
			if( currentDir != "C:" || currentDir.length()>2 )
			{
				// get the parent dir name
				string newDir = currentDir;
				size_t found = currentDir.find_last_of("\\/");
				if(found!=string::npos && !currentDir.length()<=3)
												// exclude root dir case
					newDir = currentDir.substr(0,found);
				cout << "new dir: " << newDir << endl;
				
				readDir(newDir, fileFilter); // go to parent folder
			}
			else
				cout << "you're at root! no change..\n";
		}
	}
	
	if(kbd.right()) // right key - go into folder or send filename to input line
	{
		while(kbd.right()){}
		
		if(itemTypes[selectedIndex]==1) // chose folder
		{
			string newDir = currentDir + "\\" + filesFolders[selectedIndex];
			readDir(newDir, fileFilter); // now go to that folder
		}
		// if file is currently chosen, send that filename to the input line
		else if(itemTypes[selectedIndex]==2)
		{
			strInput = filesFolders[selectedIndex];
			cursorPos = strInput.length();
		}
	}
	
	// handle return key
	if(kbd.ret())
	{	
		if(dialogMode==0 && itemTypes[selectedIndex]==2) // you're on a file
		// only for load dialog.. select filename and load at once
		{
			// only when you're choosing for second time - okay to proceed to load
			if(strInput == filesFolders[selectedIndex])
			{
				activateButton(&loadButton);
				drawFileDialog(); // update screen once	
				while(kbd.ret()){}
				cancelChosen = false;
				fileNameToReturn = trimSpaceRight(currentDir+"\\"+strInput);
				exitDialog = true;
			}
			else
			{
				while(kbd.ret()){}
				strInput = filesFolders[selectedIndex];
				cursorPos = strInput.length();
			}
			
		}
		if(dialogMode==0 && itemTypes[selectedIndex]==0)
		{
			while(kbd.ret()){} // to parent dir...
			if(currentDir != "C:")
			{
				// get the parent dir name
				string newDir = currentDir;
				size_t found = currentDir.find_last_of("\\/");
				if(found!=string::npos && !currentDir.length()<=3)
												// exclude root dir case
					newDir = currentDir.substr(0,found);
				cout << "new dir: " << newDir << endl;
				
				readDir(newDir, fileFilter); // go to parent folder
			}
			else
				cout << "you're at root! no change..\n";				
		}
		if(dialogMode==0 && itemTypes[selectedIndex]==1) // to child folder
		{
			while(kbd.ret()){}
			string newDir = currentDir + "\\" + filesFolders[selectedIndex];
			readDir(newDir, fileFilter); // now go to that folder
		}
		
		// you're on a file in save or export mode
		if((dialogMode==1||dialogMode==2) && !strInput.empty())
		{
			if(strInput != filesFolders[selectedIndex] && itemTypes[selectedIndex]==2)
			{
				while(kbd.ret()){}
				strInput = filesFolders[selectedIndex];
				cursorPos = strInput.length();
			}
			else if(itemTypes[selectedIndex]==2 && filesFolders[selectedIndex]==strInput)
			{
				if(dialogMode==1)
					activateButton(&saveButton);
				else
					activateButton(&exportButton);
				drawFileDialog(); // update screen once	
				while(kbd.ret()){}
				
				// if the requested filename already exists, check!
				bool sameNameExists = overwriting(strInput);
				bool overwriteOkay = false;
				if(sameNameExists)
				{
					// file by this name exists in this folder. confirm
					cout << "File by this name already exists..." << endl;
					cout << "Will confirm with yes-no dialog! \n";
					
					string fname = trimRight(strInput, 26);
					string strQuestion = "File named {" + fname + "} already exists.|Are you sure you want to {overwrite}?";
					overwriteOkay = yesNoDialog(strQuestion);
					cout << "dialog result = " << overwriteOkay << endl;
				}
				// only if not overwriting, or overwriting is confirmed.. proceed
				if(!sameNameExists || overwriteOkay)
				{
					// go ahead and return this file as target
					cancelChosen = false;
					fileNameToReturn = trimSpaceRight(currentDir+"\\"+strInput);
					exitDialog = true;
					if(overwriteOkay){cout << "overwring..." << endl;}
				}
				else
					cout << "save cancelled!" << endl;	
			}
		}	
	
		// save / export mode - if input line is empty and you're on a filename...
		if((dialogMode==1||dialogMode==2) && strInput.empty() 
			&& itemTypes[selectedIndex]==2)
		{
			while(kbd.ret()){}
			strInput = filesFolders[selectedIndex];
			cursorPos = strInput.length();	
		}
		if(dialogMode!=0 && itemTypes[selectedIndex]==0) // to parent folder
		{
			while(kbd.ret()){}
			if(currentDir != "C:")
			{
				// get the parent dir name
				string newDir = currentDir;
				size_t found = currentDir.find_last_of("\\/");
				if(found!=string::npos && !currentDir.length()<=3)
												// exclude root dir case
					newDir = currentDir.substr(0,found);
				cout << "new dir: " << newDir << endl;
				
				readDir(newDir, fileFilter); // go to parent folder
			}
			else
				cout << "you're at root! no change..\n";			
		}
		if(dialogMode!=0 && itemTypes[selectedIndex]==1) // to child folder
		{
			while(kbd.ret()){}
			string newDir = currentDir + "\\" + filesFolders[selectedIndex];
			readDir(newDir, fileFilter); // now go to that folder			
		}
		
		// you're on a placeholder space - you can write with return
		if(itemTypes[selectedIndex]==3)
		{
			if(!strInput.empty())
			{
				while(kbd.ret()){}
				
				// if the requested filename already exists, check!
				bool sameNameExists = overwriting(strInput);
				string strExtension = "";
				if(dialogMode==1 && strInput.find(".")==string::npos && overwriting(strInput+".txt"))
					{ sameNameExists = true; strExtension = ".txt"; }
				if(dialogMode==2 && strInput.find(".")==string::npos && overwriting(strInput+".wav"))
					{ sameNameExists = true; strExtension = ".wav"; }
				bool overwriteOkay = false;
				if(sameNameExists)
				{
					// file by this name exists in this folder. confirm
					cout << "File by this name already exists..." << endl;
					cout << "Will confirm with yes-no dialog! \n";
					
					string fname = trimRight(strInput, 26) + strExtension;
					string strQuestion = "File named {" + fname + "} already exists.|Are you sure you want to {overwrite}?";
					overwriteOkay = yesNoDialog(strQuestion);
					cout << "dialog result = " << overwriteOkay << endl;
				}
				// only if not overwriting, or overwriting is confirmed.. proceed
				if(!sameNameExists || overwriteOkay)
				{
					// go ahead and return this file as target
					cancelChosen = false;
					fileNameToReturn = trimSpaceRight(currentDir+"\\"+strInput);
					exitDialog = true;
					if(overwriteOkay){cout << "overwring..." << endl;}
				}
				else
					cout << "save cancelled!" << endl;	
			}
			
		}
	}
	
	// home key - revert to default home dir
	if(kbd.home() && !kbd.leftCtrl() && !kbd.leftAlt() )
	{
		while(kbd.home()){}
		readDir(defaultDir, fileFilter);
	}
	// secret command just for me! lol - direct to currently installed dir :)
	if( kbd.home() && kbd.leftCtrl() && kbd.leftAlt() )
	{
		cout << "secret buttons pressed! going to 'CURRENT' dir's userdata...\n";
		while(kbd.home()){}
		string secretDir = installDir + "\\userdata";
		readDir(secretDir, fileFilter);
	}
	// handle slider...
	if(hovering(&slider))
		highlightSlider(&slider);
	else
		dehighlightSlider(&slider);
	
	// handle drag action...
	if(hovering(&slider) && mouse.left() && !dragging) // first time dragging
	{
		dragging = true;
		mouseYprev = mouseY;
	}
	else if(mouse.left() && dragging) // continuing to drag
	{
		moveSlider(&slider, mouseYdelta);
		if(nItems > TEXT_HEIGHT)
		{
			float ratio = sliderPosRatio(&slider); // get slider pos on scale of 1
			topRenderLine = 
			static_cast<int>( (static_cast<float>(nItems - TEXT_HEIGHT) * ratio + 0.5) );
			setStrView(); // rearrange the view to the new position
		}
	}
	else
		dragging = false;
	
	// 'load' button, only for load dialog
	if(dialogMode == 0) // load dialog
	{
		if( (hovering(&loadButton) && !dragging) || kbd.f12() ) 
			// handle 'load' button
		{
			if(mouse.left() || kbd.f12())
			{
				activateButton(&loadButton);
				drawFileDialog(); // update screen once
				while(mouse.left() || kbd.f12()){}
				if(!strInput.empty())
				{
					cancelChosen = false;
					fileNameToReturn = trimSpaceRight(currentDir+"\\"+strInput);
					exitDialog = true;
				}
			}
			else
				highlightButton(&loadButton);
		}
		else
			dehighlightButton(&loadButton);
	}

	// 'save' button, only for save dialog
	if(dialogMode == 1) // save dialog
	{
		if( (hovering(&saveButton) && !dragging) || kbd.f12() ) 
			// handle 'save' button
		{
			if(mouse.left() || kbd.f12())
			{
				activateButton(&saveButton);
				drawFileDialog(); // update screen once
				while(mouse.left() || kbd.f12()){}
				if(!strInput.empty())
				{
					// if the requested filename already exists, check!
					bool sameNameExists = overwriting(strInput);
				string strExtension = "";
				if(strInput.find(".")==string::npos && overwriting(strInput+".txt"))
					{ sameNameExists = true; strExtension = ".txt"; }
					bool overwriteOkay = false;
					if(sameNameExists)
					{
						// file by this name exists in this folder. confirm
						cout << "File by this name already exists..." << endl;
						cout << "Will confirm with yes-no dialog! \n";
						
						string fname = trimRight(strInput, 26) + strExtension;
						string strQuestion = "File named {" + fname + "} already exists.|Are you sure you want to {overwrite}?";
						overwriteOkay = yesNoDialog(strQuestion);
						cout << "dialog result = " << overwriteOkay << endl;
					}
					// only if not overwriting, or overwriting is confirmed.. proceed
					if(!sameNameExists || overwriteOkay)
					{
						// go ahead and return this file as target
						cancelChosen = false;
						fileNameToReturn = trimSpaceRight(currentDir+"\\"+strInput);
						exitDialog = true;
						if(overwriteOkay){cout << "overwring..." << endl;}
					}
					else
						cout << "save cancelled!" << endl;	
				}
			}
			else
				highlightButton(&saveButton);
		}
		else
			dehighlightButton(&saveButton);
	}
	
	// 'export' button, only for export dialog
	if(dialogMode == 2)
	{
		if( (hovering(&exportButton) && !dragging) || kbd.f12() ) 
			// handle 'export' button
		{
			if(mouse.left() || kbd.f12())
			{
				activateButton(&exportButton);
				drawFileDialog(); // update screen once
				while(mouse.left() || kbd.f12()){}
				if(!strInput.empty())
				{
					// if the requested filename already exists, check!
					bool sameNameExists = overwriting(strInput);
					string strExtension = "";
					if(strInput.find(".")==string::npos && overwriting(strInput+".wav"))
						{ sameNameExists = true; strExtension = ".wav"; }
					bool overwriteOkay = false;
					if(sameNameExists)
					{
						// file by this name exists in this folder. confirm
						cout << "File by this name already exists..." << endl;
						cout << "Will confirm with yes-no dialog! \n";
						
						string fname = trimRight(strInput, 26) + strExtension;
						string strQuestion = "File named {" + fname + "} already exists.|Are you sure you want to {overwrite}?";
						overwriteOkay = yesNoDialog(strQuestion);
						cout << "dialog result = " << overwriteOkay << endl;
					}
					// only if not overwriting, or overwriting is confirmed.. proceed
					if(!sameNameExists || overwriteOkay)
					{
						// go ahead and return this file as target
						cancelChosen = false;
						fileNameToReturn = trimSpaceRight(currentDir+"\\"+strInput);
						exitDialog = true;
						if(overwriteOkay){cout << "overwring..." << endl;}
					}
					else
						cout << "save cancelled!" << endl;	
				}
			}
			else
				highlightButton(&exportButton);
		}
		else
			dehighlightButton(&exportButton);		
		
	}
	
	// handle 'help' button - all file dialogs
	if( ( hovering(&helpButton) && !dragging ) || kbd.f2())
	{
		if(mouse.left() || kbd.f2())
		{
			activateButton(&helpButton);
			drawFileDialog(); // update screen once
			while(mouse.left() || kbd.f2())
			{
				//
				//	perform - display help!
				//
				
				help.activate();
				drawFileDialog(); // keep updating screen!
			}
			
			help.deactivate();
		}
		else
			highlightButton(&helpButton);	
	}
	else
		dehighlightButton(&helpButton);
	
	// handle "refresh" - when ctrl + R are pressed
	if( kbd.ctrlR() )
	{
		while( kbd.ctrlR() ){}
		
		// refresh!
		cout << "refresh at current dir...\n";
		selectedIndex = 0;
		topRenderLine = 0;
		refreshing = true; // the only time this flag turns on!
		readDir(currentDir, fileFilter);
		refreshing = false; // this must remain false in all other cases
	}

	// handle 'go' button - all file dialogs
	if( ( hovering(&goButton) && !dragging ) || kbd.ctrlG() )
	{
		if( mouse.left() || kbd.ctrlG() )
		{
			activateButton(&goButton);
			drawFileDialog(); // update screen once
			while( mouse.left() || kbd.ctrlG() ){}
			
			//
			// perform - "go-to" dialog!
			//
			string goToDestination = goToDialog();
			cout << "goToDestination = " << goToDestination << endl;
			
			// if desktop was selected...
			if(goToDestination=="DESKTOP")
			{
				string desktopPath = string(getenv("USERPROFILE"));
				desktopPath += "\\Desktop";
		
				cout << "To Desktop: " << desktopPath << endl;
				readDir(desktopPath, fileFilter);
				
			}
			// if documents folder was selected...
			else if(goToDestination=="DOCUMENTS")
			{
				string documentsFolderPath = getUserDocPathFromSystem();
				cout << "To Documents folder: " << documentsFolderPath << endl;
				readDir(documentsFolderPath, fileFilter);				
			}
			// if drive letter was selected...
			else if(goToDestination.length()==2 && goToDestination.at(1)==':')
			{
				string goToDir = goToDestination;
				cout << "Go to drive - " << goToDir << endl;
				readDir(goToDir, fileFilter);
			}
		}
		else
			highlightButton(&goButton);	
	}
	else
		dehighlightButton(&goButton);
	
	// handle 'cancel' button - all file dialogs
	if( ( hovering(&cancelButton) && !dragging ) || kbd.escape())
	{
		if(mouse.left() || kbd.escape())
		{
			activateButton(&cancelButton);
			drawFileDialog(); // update screen once
			while(mouse.left() || kbd.escape()){}
			cancelChosen = true;
			exitDialog = true;
		}
		else
			highlightButton(&cancelButton);	
	}
	else
		dehighlightButton(&cancelButton);
	
	// toggling file filter button - load dialog and save dialog
	if(dialogMode == 0 || dialogMode == 1)
	{
		if( (hovering(&filterButton) && !dragging) || kbd.f1() )
		{
			highlightButton(&filterButton);
			drawFileDialog(); // update screen once
			if(mouse.left() || kbd.f1() )
			{
				while(mouse.left() || kbd.f1()){}
				if(fileFilter==".txt")
				{
					fileFilter = ".*";
					changeButtonText(&filterButton, "  File Type: Any  ");
					readDir(currentDir, fileFilter);
				}
				else
				{
					fileFilter = ".txt";
					changeButtonText(&filterButton, "  File Type: Text  ");
					readDir(currentDir, fileFilter);
				}
			}
		}
		else
			dehighlightButton(&filterButton);
	}
	
	// see if the path name is being clicked - or pressed ALT + O
	if( hoveringOnPathName() || kbd.altO() )
	{
		// being clicked over
		if( ( mouse.left() || kbd.altO() ) && !calledExplorerAlready)
		{
			cwd.setColor(sf::Color(255, 170, 100));
			while( mouse.left() || kbd.altO() ){
				drawFileDialog(); // update screen once				
			}
		
			ShellExecute( NULL, "open", currentDir.c_str(), NULL, NULL, SW_SHOWNORMAL );
			ShellExecute( NULL, NULL, currentDir.c_str(), NULL, NULL, SW_SHOWNORMAL );
			ShellExecute( NULL, "call", currentDir.c_str(), NULL, NULL, SW_SHOWNORMAL );
			
			calledExplorerAlready = true;
		}
		// just hovering over
		else
		{
			cwd.setColor(dialogGreenLight);
		}
	}
	else
	{
		cwd.setColor(dialogGreen);
		calledExplorerAlready = false;
	}
	
	// ALT + D will get you to Desktop
	if( kbd.altD() )
	{
		while( kbd.altD() ){}
		string desktopPath = string(getenv("USERPROFILE"));
		desktopPath += "\\Desktop";
		
		cout << "To desktop: " << desktopPath << endl;
		readDir(desktopPath, fileFilter);
	}
	
	// update cursor position and blink states
	updateCursor();
}

void Dialog::drawFileDialog()
{
	w->clear();
	
	// current directory
	w->draw(cwd);
	
	// draw browser
	
	// browser text
	for(int i=0; i<TEXT_HEIGHT; i++)
		w->draw(itemText[i]);
	
	// selector mask
	if(selectedIndex >= topRenderLine && selectedIndex < topRenderLine+TEXT_HEIGHT)
	{
		int lineIndexDisplay = selectedIndex - topRenderLine;
		selectMask.setPosition(TEXT_TOP_X - charWidth / 2, lineIndexDisplay*charHeight + TEXT_TOP_Y + 3);
		w->draw(selectMask);
	}
	
	// draw slider
	w->draw(slider.rail);
	w->draw(slider.handle);
	
	// input line, label, text
	w->draw(inputLine);
	w->draw(inputLineLabel);
	string str = strInput.substr(startRenderPos);
								// leave room for when cursor is right most
	inputLineText.setString(str);
	w->draw(inputLineText);
	
	// cursor - appears on save and export dialogs
	if(dialogMode==1 || dialogMode==2)
	{	
		w->draw(cursor);
	}
	
	// 'load' button - load dialog
	if(dialogMode==0)
	{
		// draw buttons
		w->draw(loadButton.rect);
		w->draw(loadButton.label);
		w->draw(loadButton.help);
	}
	
	// 'save' button - save dialog
	if(dialogMode==1)
	{
		// draw buttons
		w->draw(saveButton.rect);
		w->draw(saveButton.label);
		w->draw(saveButton.help);
	}	

	// 'export' button and format notice - export dialog
	if(dialogMode==2)
	{
		// draw 'export' button
		w->draw(exportButton.rect);
		w->draw(exportButton.label);
		w->draw(exportButton.help);
		
		// supported format text
		w->draw(supportedFilesText);
	}
	
	// filter button - load or save dialog
	if(dialogMode==0 || dialogMode==1)
	{
		w->draw(filterButton.rect);
		w->draw(filterButton.label);
		w->draw(filterButton.help);	
	}
	
	// draw help button - all dialogs
	w->draw(helpButton.rect);
	w->draw(helpButton.label);
	w->draw(helpButton.help);

	// draw Go "<" button - all dialogs
	w->draw(goButton.rect);
	w->draw(goButton.label);
	w->draw(goButton.help);

	// draw cancel button - all dialogs
	w->draw(cancelButton.rect);
	w->draw(cancelButton.label);
	w->draw(cancelButton.help);
	
	// if active, draw help screen...
	if(help.isActive())
		help.draw();	
	
	w->display();
}

void Dialog::makeButton(Button* b, const std::string& lbl, int type)
{
	b->label.setFont(font);
	b->label.setCharacterSize(22);
	b->label.setString(lbl);
	sf::FloatRect fRect = b->label.getGlobalBounds();
	
	b->type = type;
	b->strLabel = lbl;
	
	b->w = (fRect.width) + static_cast<float>(charWidth) * 0.9;
	b->h =  static_cast<float>(charHeight) * 1.40;
	b->sizeWOffset = 0;
	b->sizeHOffset = 0;
	
	b->x = 0;
	b->y = 0;
	b->labelYOffset = 0;
	b->labelXOffset = 0;

	b->rect.setPosition(sf::Vector2f(0,0));
	b->rect.setSize(sf::Vector2f(b->w, b->h));
	// b->label.setString(lbl);
	b->label.setPosition(sf::Vector2f(static_cast<float>(charWidth * 0.24), static_cast<float>(charHeight * 0.36)));

	b->hasHelpText = false;
	
	if(b->type == 0) // regular red button
	{
		b->rect.setFillColor(sf::Color::Black);
		b->rect.setOutlineColor(dialogRedLight);
		b->rect.setOutlineThickness(-3.0f);
		b->label.setColor(dialogRedLight);
	}
	else if(b->type == 1) // file type filter button
	{
		b->rect.setFillColor(sf::Color::Black);
		b->rect.setOutlineColor(dialogGreen);
		b->rect.setOutlineThickness(-3.0f);
		b->label.setColor(dialogGreen);		
	}
	else if(b->type == 2) // "?" help button
	{
		b->rect.setFillColor(sf::Color::Black);
		b->rect.setOutlineColor(dialogRedDark);
		b->rect.setOutlineThickness(-3.0f);
		b->label.setColor(dialogRedDark);

		/*
		// force a special size for this one...
		// b->sizeWOffset = 80;
		b->w = 28;
		b->rect.setSize(sf::Vector2f(b->w, b->h)); // resize!
		b->labelXOffset = 4;
		b->labelYOffset = 0;
		*/
	}
	else if(b->type == 3) // "<" go to folder button
	{
		b->rect.setFillColor(sf::Color::Black);
		b->rect.setOutlineColor(dialogRedDark);
		b->rect.setOutlineThickness(-3.0f);
		b->label.setColor(dialogRedDark);		

		// force a special size for this one...
		// b->sizeWOffset = 80;
		b->w = 25;
		b->h = 24;
		b->rect.setSize(sf::Vector2f(b->w, b->h)); // resize!
		b->labelXOffset = 2;
		b->labelYOffset = -7;		
	}
}

void Dialog::addHelp(Button* b, string helpStr, int position)
{
	b->hasHelpText = true;
	b->help.setFont(font);
	b->help.setCharacterSize(15);
	b->help.setString(helpStr);
	b->help.setColor(sf::Color(255,255,255,132));
	
	if(position==0)
		b->help.setPosition(b->x, b->y - 22);
	else
		b->help.setPosition(b->x + b->w - helpStr.length()*7.5, b->y - 22);
	
	// special case: "<" button
	if(b->type==3)
		b->help.setPosition(b->x + 9, b->y - 19);
}

void Dialog::positionButton(Button* b, int xx, int yy)
{
	b->x = xx;
	b->y = yy;
	b->rect.setPosition(sf::Vector2f(xx, yy));
	b->label.setPosition(sf::Vector2f(xx + (static_cast<float>(charWidth)*0.4f) + b->labelXOffset, yy + (static_cast<float>(charHeight) * 0.10f) + b->labelYOffset ));
}

void Dialog::highlightButton(Button* b)
{
	if(b->type == 0) // regular red button
	{
		b->rect.setOutlineColor(dialogRed);
		b->rect.setFillColor(sf::Color(255,180,180,255));
		b->label.setColor(sf::Color(100,0,0,255));
	}
	else if(b->type == 1) // file filter button
	{
		b->rect.setFillColor(dialogGreen);
		b->label.setColor(sf::Color::Black);		
	}
	else if(b->type == 2 || b->type == 3) // help or go button
	{
		b->rect.setOutlineColor(dialogRedDark);
		b->rect.setFillColor(dialogRedDark);
		b->label.setColor(sf::Color::Black);		
	}
}

void Dialog::dehighlightButton(Button* b)
{
	if(b->type == 0) // regular red button
	{
		b->rect.setOutlineColor(dialogRed);
		b->rect.setFillColor(sf::Color::Black);
		b->label.setColor(dialogRed);
	}
	else if(b->type == 1) // file filter button
	{
		b->rect.setFillColor(sf::Color::Black);
		b->label.setColor(dialogGreen);			
	}
	else if(b->type == 2 || b->type == 3) // help or go button
	{
		b->rect.setOutlineColor(dialogRedDark);
		b->rect.setFillColor(sf::Color::Black);
		b->label.setColor(dialogRedDark);		
	}
}

void Dialog::activateButton(Button* b)
{
	b->rect.setOutlineColor(sf::Color(180,180,180,255));
	b->rect.setFillColor(sf::Color::White);
	b->label.setColor(sf::Color::Black);	
}

void Dialog::changeButtonText(Button* b, string newLabel)
{
	b->strLabel = newLabel;
	b->label.setString(newLabel);
}

bool Dialog::hovering(Button* b)
{
	if( b->x < mouseX && mouseX < (b->x + b->w) && b->y < mouseY && mouseY < (b->y + b->h) )
		return true;
	else
		return false;
}

void Dialog::makeSlider(Slider* s, int ww, int hh, int range)
{
	s->range = range;
	s->w = ww;
	s->h = hh;
	s->topX = 0;
	s->topY = 0;
	s->pos = 0;
	s->handle.setSize(sf::Vector2f(ww, hh));
	s->handle.setFillColor(dialogGreen);
	s->rail.setSize(sf::Vector2f(2, range + hh));
	s->rail.setFillColor(dialogGreen);
	positionSlider(s, 0, 0);
}

void Dialog::positionSlider(Slider* s, int xx, int yy)
{
	s->topX = xx;
	s->topY = yy;
	s->handle.setPosition(sf::Vector2f(xx, yy + s->pos));
	s->rail.setPosition(sf::Vector2f(s->topX + (s->w / 2) - 2, s->topY));
}

float Dialog::sliderPosRatio(Slider* s)
{
	return static_cast<float>(s->pos) / static_cast<float>(s->range);
}

bool Dialog::hovering(Slider* s)
{
	if( s->topX < mouseX && mouseX < (s->topX + s->w)
		&& s->topY + s->pos < mouseY && mouseY < (s->topY + s->pos + s->h) )
		return true;
	else
		return false;		
}

void Dialog::moveSlider(Slider* s, int delta)
{
	// if(delta > 5) // handle only when movement is large enough
		s->pos += delta;
	s->pos = max(0, min(s->range, s->pos));
	setSliderPos(s, s->pos);
}

void Dialog::setSliderPos(Slider* s, int p)
{
	s->pos = max(0, min(s->range, p));
	s->handle.setPosition(sf::Vector2f(s->topX, s->topY + s->pos));
}

void Dialog::highlightSlider(Slider* s)
	{ s->handle.setFillColor(dialogGreenLight); }

void Dialog::dehighlightSlider(Slider* s)
	{ s->handle.setFillColor(dialogGreen); }

void Dialog::readDir(string path, string strFilter)
{
	bool goingToNewDir = false;
	
	// if you're requesting to go to new dir... push history
	// exclude the case you're just un-doing!
	if(path != currentDir && !undoing && !refreshing && !startingOut)
	{
		// before setting up new dir... lets store current path in record
		pushHistory(currentDir);
		goingToNewDir = true;
	}
	
	// bow go ahead and set new path as our current directory
	currentDir = path;
	cwd.setString(trimLeft(currentDir, CWD_WIDTH));
	// only if cwd ended up with two chars - drive letter and ':' - adjust by adding '\' (just for display)
	if(currentDir.length()<=2 && currentDir.at(1)==':')
		cwd.setString(string(currentDir+"\\"));
	pathNameRect = cwd.getGlobalBounds();
	
	// clear the input for load dialog only
	if(dialogMode == 0)
	{
		strInput = "";
		cursorPos = 0;
		startRenderPos = 0;
	}
	
	// read contents of the this path...
	
	// first initialize the vectors
	filesFolders.resize(0);
	filesFolders.clear();
	itemTypes.resize(0);
	itemTypes.clear();
	
	// first add ".." (to parent folder)
	filesFolders.push_back("<<<");
	itemTypes.push_back(0);
	
	// if save or export dialog, insert a placeholder for saving to new file
	if(dialogMode==1)
	{
		filesFolders.push_back("---> SAVE TO NEW FILE HERE <---");
		itemTypes.push_back(3);
	}
	if(dialogMode==2)
	{
		filesFolders.push_back("---> EXPORT TO NEW FILE HERE <---");
		itemTypes.push_back(3);
	}
	
	// get folder names
	vector<string> folders = getDirNamesInDir(currentDir);
	for(unsigned int i=0; i<folders.size(); i++)
	{
		filesFolders.push_back(folders[i]);
		itemTypes.push_back(1);
	}
	
	// get file names
	vector<string> files = getFileNamesInDir(currentDir, fileFilter);
	for(unsigned int i=0; i<files.size(); i++)
	{
		filesFolders.push_back(files[i]);
		itemTypes.push_back(2);
	}

	// keep track of number of items total
	nItems = min(filesFolders.size(), itemTypes.size());
	
	// if you're going to new dir, reset selection to top position
	if(goingToNewDir)
	{
		selectedIndex = 0;
		topRenderLine = 0; // go back to very top
		setSliderPos(&slider, 0); //reset slider, too
		
		// for save / export mode, let's start on the placeholder
		if(dialogMode==1 || dialogMode==2)
			selectedIndex = 1;
	}
	else // otherwise, you're just changing your filter
	{
		if(selectedIndex >= nItems) // adjust if you have went over
		{
			selectedIndex = nItems - 1;
			topRenderLine = nItems - TEXT_HEIGHT;
			topRenderLine = max(0, topRenderLine); // if went to negative, make it 0
		}
		
		if(topRenderLine>=(nItems-1)) // safeguard - if nItems got reduced, make sure topRenderLine didn't go over the last possible line (nItems-1)
		{
			topRenderLine = nItems - TEXT_HEIGHT;
			topRenderLine = max(0,topRenderLine);
		}
		
		// DEBUG
		
		// cout << "selectedIndex=" << selectedIndex << endl;
		// cout << "nItems=" << nItems << endl;
		// cout << "topRenderLine=" << topRenderLine << endl;
		// cout << "TEXT_HEIGHT=" << TEXT_HEIGHT << endl;
		
		// wherever you end up... let's adjust slider pos
		if(nItems < TEXT_HEIGHT) // if everything fits in one screen
			setSliderPos(&slider, 0); // then bring slider back to top
		else // otherwise
		{
			float ratio = max(0.0f, min(1.0f, static_cast<float>(topRenderLine) / static_cast<float>(nItems - TEXT_HEIGHT)));
			setSliderPos(&slider, static_cast<int>(slider.range * ratio));
		}
	}
	
	// go ahead and populate the browser screen data
	setStrView();
	
	// reset double click count
	// (prevent from carrying click timing over from previous page)
	pageClickCount = 0;
}

// initialize all sfml text objects for rendering
void Dialog::initializeStrView()
{
	filesFolders.resize(TEXT_HEIGHT);
	itemTypes.resize(TEXT_HEIGHT);
	
	// for file dialogs
	for(int i=0; i<TEXT_HEIGHT; i++)
	{	
		strView[i] = "";
		itemTypes[i] = 2; // file type
		itemText[i].setFont(font);
		itemText[i].setCharacterSize(25);
		itemText[i].setString("");
		itemText[i].setColor(sf::Color(180,255,180,255)); // default color
		itemText[i].setPosition(sf::Vector2f(TEXT_TOP_X, TEXT_TOP_Y + charHeight * i));
	}
	
	// for basic dialogs
	for(int i=0; i<10; i++)
	{
		basicText[i].setFont(font);
		basicText[i].setCharacterSize(25);
		basicText[i].setString("");
		basicText[i].setColor(dialogGreen);
		basicTextColor[i].setFont(font);
		basicTextColor[i].setCharacterSize(25);
		basicTextColor[i].setString("");
		basicTextColor[i].setColor(dialogRed);		
	}
	
}

void Dialog::setStrView()
{
	for(int i=0;i<TEXT_HEIGHT; i++)
	{
		unsigned int lineIndex = topRenderLine + i;
		if(lineIndex < filesFolders.size())
		{
			strView[i] = filesFolders[lineIndex];
			itemText[i].setString(trimRight(filesFolders[lineIndex], TEXT_WIDTH));
			if(itemTypes[lineIndex] == 0) // ".."
				itemText[i].setColor(dialogRed);
			else if(itemTypes[lineIndex] == 1) // folder
			{
				itemText[i].setColor(dialogBlue);
				itemText[i].setString
					(trimRight(">>> " + filesFolders[lineIndex], TEXT_WIDTH));
			}
			else if(itemTypes[lineIndex]==2) // this is a file
				itemText[i].setColor(dialogGreen);
			else if(itemTypes[lineIndex]==3) // all else, this is write placeholder
				itemText[i].setColor(dialogRedLight);
		}
		else // went out of bound.. set to empty string
		{
			strView[i] = "";
			itemText[i].setString("");
		}
	}
}

string Dialog::trimLeft(string str, int max)
{
	if(str.length()>max)
		str = "..." + str.substr(str.length() - max + 3 , max - 3);
	return str;
}

string Dialog::trimRight(string str, int max)
{
	if(str.length()>max)
		str = str.substr(0, max-3) + "...";
	return str;
}

string Dialog::trimSpaceRight(string str)
{
	bool done = false;
	int searchPos = str.length() - 1;
	char ch;
	while(!done)
	{
		ch = str.at(searchPos);
		if(ch==' ' || ch=='-')
			str = str.erase(searchPos,1);
		else
			done = true;
		searchPos--;
	}
	return str;
}

// update the cursor position and blink states
void Dialog::updateCursor()
{
	// update startRenderPos...
	if(cursorPos>=INPUT_WIDTH)
	{
		int nCharsToDisplay = INPUT_WIDTH - 1; 
					// subtract this from strInput's last position
		startRenderPos = strInput.length() - nCharsToDisplay;
	}
	else
		startRenderPos = 0;
	
	blinkCursor();
	if(blinkState==0)
	{
		cursor.setSize(sf::Vector2f(14, 24));
		cursor.setPosition(INPUT_TOP_X + charWidth + (cursorPos-startRenderPos)*charWidth,
		INPUT_TOP_Y + 5);
	}
	if(blinkState==1)
	{
		cursor.setSize(sf::Vector2f(14, 12));
		cursor.setPosition(INPUT_TOP_X + charWidth + (cursorPos-startRenderPos)*charWidth,
		INPUT_TOP_Y + 17);		
	}
}
			
// toggle cursor shape at a regular interval to make it blink
void Dialog::blinkCursor()
{
	// if enough time has passed, then toggle cursor shape
	if(blinkClock.getElapsedTime().asSeconds() > 0.6f)
	{
		blinkState++;
		if(blinkState==2)
			blinkState = 0;
		blinkClock.restart();
	}
}

// check to see if a filename already exists in this dir
bool Dialog::overwriting(string file)
{
	bool sameNameFound = false;
	for(unsigned int i=0;i<filesFolders.size();i++)
	{
		if(itemTypes[i]==2 && filesFolders[i] == file)
			sameNameFound = true;
	}
	return sameNameFound;
}

// stores the path where you have just come from
void Dialog::pushHistory(string path)
{
	// if you're now at end of array, roll over everything and make room
	if(historyLevel >= HIST_SIZE)
	{
		for(int i=1;i<HIST_SIZE;i++)
			history[i-1] = history[i];
		historyLevel--; // and move index back by one
	}
	
	cout << "PUSHED at history level " << historyLevel << ":" << endl;
	cout << path << endl;
	
	// now push your path history
	history[historyLevel] = path;
	historyLevel++;
}

// go back to previous directory you were just in
string Dialog::popHistory()
{
	string strToReturn = ""; // if there's no history to pop, empty string is returned
	if(historyLevel>0) // level zero means very starting point (with no record stored)
	{
		historyLevel--; // first go back in time
		strToReturn = history[historyLevel]; // pop that data
		history[historyLevel] = ""; // and erase that data
		
		cout << "POP - reverted to history level " << historyLevel << endl;
		cout << "Revert to: " << strToReturn << endl;
	}
	return strToReturn;
}

// sets up a default start-up folder for any dialog to start in
void Dialog::setStartFolder(string path)
 { requestedStartFolder = path; }

void Dialog::unsetStartFolder()
{ requestedStartFolder = ""; }

void Dialog::messageDialog(std::string message)
{
	basicDialogMode = 0; // just message and 'ok' button
	
	runBasicDialog();
}

// show a dialog to get user input for yes/no
bool Dialog::yesNoDialog(std::string question)
{
	// in case starting with a different-sized window...
	sf::Vector2u size = w->getSize();
	adjustedWindowWidth = static_cast<double>(size.x);
	adjustedWindowHeight = static_cast<double>(size.y);
	windowClosed = false;
	exitDialog = false;
	
	yesNoDialogResult = false;
	basicDialogMode = 1; // get yes / no 

	// parse the passed question - sets up the text viewables array
	parseBasicDialog(question);
	
	// make buttons to be used
	makeButton(&yesButton, " Yes ", 0);
	makeButton(&noButton, " No ", 0);
	positionButton(&yesButton, WINDOW_WIDTH/2 - 100, messageBottomY+80);
	positionButton(&noButton, WINDOW_WIDTH/2 + 34, messageBottomY+80);
	addHelp(&yesButton, "Y", 1);
	addHelp(&noButton, "N", 1);
	
	runBasicDialog();
	cout << "yes/no dialog finished, result = " << yesNoDialogResult << endl;
	return yesNoDialogResult;
}

// run a basic dialog - message(question) and some buttons
void Dialog::runBasicDialog()
{
	exitBasicDialog = false;
	
	while(!exitBasicDialog)
	{
		pollEvents();
		if(!windowClosed) // in case screen x-ed out
		{
			handleInputBasicDialog();
			drawBasicDialog();
		}
	}
}

void Dialog::parseBasicDialog(std::string str)
{
	// initialize the display string arrays etc.
	for(int i=0; i<10; i++)
	{
		basicStrView[i] = "";
		basicStrViewColor[i] = "";
		strX[i] = 0;
		strY[i] = 0;
		strColorX[i] = 0;
		strColorY[i] = 0;
	}
	nLinesBasicDialog = 0;
	
	// first split the passed string at '|'
	size_t found;
	bool done = false;
	int lineToWrite = 0;
	while(!done)
	{
		// search for dividing char |
		found = str.find('|');
		if(found==string::npos)
		{
			basicStrView[lineToWrite] = str; // store last remaining piece
			nLinesBasicDialog = lineToWrite + 1; // record how many lines in this dialog - needs this for placing buttons
			done = true;
		}
		else // symbol found, split now
		{
			basicStrView[lineToWrite] = str.substr(0,found);
			str = str.substr(found+1); // new search portion starts after |
			lineToWrite++;
		}
	}

	int center = WINDOW_WIDTH / 2;
	messageTopY = (WINDOW_HEIGHT/2) - ((nLinesBasicDialog+2) * charHeight);
	messageBottomY = messageTopY + nLinesBasicDialog * charHeight;
	
	// determine the coordinates for the string array
	for(int i=0; i<10; i++)
	{
		strX[i] = center - (basicStrView[i].length() * charWidth / 2);
		strY[i] = messageTopY + i * charHeight;
	}
	
	// now search for area enclosed with { } (colored)
	int lineToRead = 0;
	int coloredItemCount = 0;
	size_t foundOpen;
	size_t foundClose;
	done = false;
	while(!done)
	{
		// search for {}
		foundOpen = basicStrView[lineToRead].find('{');
		foundClose = basicStrView[lineToRead].find('}');
		
		if(foundOpen!=string::npos && foundClose!=string::npos) // if both found
		{
			int nCharsPhrase = (foundClose - foundOpen) - 1;
			basicStrViewColor[coloredItemCount] = basicStrView[lineToRead].substr(foundOpen+1, nCharsPhrase);
			strColorX[coloredItemCount] = strX[lineToRead] + foundOpen * charWidth;
			strColorY[coloredItemCount] = strY[lineToRead];
			basicStrView[lineToRead].replace(foundOpen+1, nCharsPhrase, nCharsPhrase, ' '); // whitespace out the colored snippet
			basicStrView[lineToRead].erase(foundClose, 1); // erase }
			basicStrView[lineToRead].erase(foundOpen, 1); // erase {
			coloredItemCount++; // so we can go on to next colored item
		}
		else // no more braces found, go to next line to process
			lineToRead++;
			
		if(lineToRead==10) // done with last line, exit
			done = true;
	}
	
	// DEBUG
	/*
	cout << "parse report...\n";
	cout << "basicStrView[n]...\n";
	for(int i=0;i<10;i++)
	{
		cout << "   " << i << " " << basicStrView[i] << endl;
		cout << "      x=" << strX[i] << ", y=" << strY[i] << endl;
	}
	cout << "basicStrViewColor[n]...\n";
	for(int i=0;i<10;i++)
	{
		cout << "   " << i << " " << basicStrViewColor[i] << endl;
		cout << "      x=" << strColorX[i] << ", y=" << strColorY[i] << endl;
	}
	*/
	
	// now set up the sfml text objects for rendering
	for(int i=0;i<10; i++) // regular text
	{
		basicText[i].setString(basicStrView[i]);
		basicText[i].setPosition(sf::Vector2f(strX[i],strY[i]));
	}
	for(int i=0;i<10; i++) // colored snippets
	{
		basicTextColor[i].setString(basicStrViewColor[i]);
		basicTextColor[i].setPosition(sf::Vector2f(strColorX[i],strColorY[i]));		
	}	
}

void Dialog::handleInputBasicDialog()
{
	
	// yes / no dialog - handle button input
	if(basicDialogMode==1)
	{
		if(hovering(&yesButton))
			highlightButton(&yesButton);
		else
			dehighlightButton(&yesButton);

		if((hovering(&yesButton) && mouse.left()) || kbd.y()) // button pressed
		{
			activateButton(&yesButton);
			drawBasicDialog(); // draw once to update
			while(mouse.left() || kbd.y()){}
			{
				exitBasicDialog = true;
				yesNoDialogResult = true;
			}
		}
			
		if(hovering(&noButton))
			highlightButton(&noButton);
		else
			dehighlightButton(&noButton);
		
		if((hovering(&noButton) && mouse.left()) || kbd.n()) // button pressed
		{
			activateButton(&noButton);
			drawBasicDialog(); // draw once to update
			while(mouse.left() || kbd.n()){}
			{
				exitBasicDialog = true;
				yesNoDialogResult = false;
			}
		}
	}
	
	// clear keyboard buffer variables (not used here)
	typedChar = 0;
	strTypedChar = "";
}

void Dialog::drawBasicDialog()
{
	w->clear();

	// output regular text
	for(int i=0;i<10;i++)
	{
		w->draw(basicText[i]);
	}
	// output colored text
	for(int i=0;i<10;i++)
	{
		w->draw(basicTextColor[i]);
	}
	
	// 'yes' 'no' buttons - yes/no dialog
	if(basicDialogMode==1)
	{
		// draw buttons
		w->draw(yesButton.rect);
		w->draw(yesButton.label);
		w->draw(yesButton.help);
		w->draw(noButton.rect);
		w->draw(noButton.label);
		w->draw(noButton.help);			
	}
	
	w->display();
}

bool Dialog::hoveringOnPathName()
{
	return ( pathNameRect.left < mouseX && mouseX < pathNameRect.left + pathNameRect.width
	   && pathNameRect.top < mouseY && mouseY < pathNameRect.top + pathNameRect.height );
}

vector<std::string> Dialog::getDriveLetters()
{
	DWORD mydrives = 100;// buffer length
	char lpBuffer[100];// buffer for drive string storage	
	DWORD result = GetLogicalDriveStrings( mydrives, lpBuffer);	
	
	string str = "";
	for(int i=0; i<100; i++)
	{
		str += lpBuffer[i];
	}
	
	size_t found;
	string target = ":\\";
	bool done = false;
	
	vector<std::string> driveLetter;
	
	while(!done)
	{
		found = str.find(target);
		if(found != string::npos && found != 0) // found!
		{
			string letterStr = str.substr(found-1, 3);
			driveLetter.push_back(letterStr);
			str.erase(found-1, 3);
		}
		else
			done = true;
	}
	
	return driveLetter;
}

vector<int> Dialog::getDriveTypes(vector<std::string> drives)
{
	vector<int> types;
	for(int i=0; i<drives.size(); i++)
	{
		int result = GetDriveType(drives[i].c_str());		
		types.push_back(result);
	}
	return types;
}

// show a dialog to let user select drive or special folders to go to
std::string Dialog::goToDialog()
{
	// in case starting with a different-sized window...
	sf::Vector2u size = w->getSize();
	adjustedWindowWidth = static_cast<double>(size.x);
	adjustedWindowHeight = static_cast<double>(size.y);
	windowClosed = false;
	exitDialog = false;
	exitGoToDialog = false;
	
	goToDialogResult = "";

	// make buttons to be used
	
	// string text above the drive letter buttons
	drivesGroupLabel.setString("Drives:");
	sf::FloatRect fRect = drivesGroupLabel.getGlobalBounds();
	drivesGroupLabel.setPosition(WINDOW_WIDTH/2 - (fRect.width/2), 90);
	
	// create drive letter buttons
	drives = getDriveLetters();
	
	// DEBUG
	// drives.push_back("A:");drives.push_back("B:");drives.push_back("D:");drives.push_back("E:");
	// drives.push_back("F:");drives.push_back("H:");drives.push_back("I:");drives.push_back("J:");	
	// drives.push_back("K:");drives.push_back("L:");drives.push_back("M:");drives.push_back("N:");	
	// drives.push_back("O:");drives.push_back("P:");drives.push_back("Q:");drives.push_back("R:");
	// drives.push_back("S:");drives.push_back("T:");drives.push_back("U:");drives.push_back("V:");
	// drives.push_back("X:");drives.push_back("Y:");drives.push_back("Z:");
	
	nDrives = drives.size();
	if(nDrives>21) nDrives = 21;
	nLinesDrive = nDrives / 7 + 1;
	int nButtons[3];
	float firstButtonX[3] = {0};
	float gap[3] = {0};
	driveButtonTop = 140;
	driveButtonW = 50;
	driveButtonH = 35;
	driveButtonYStep = 64;
	float lineW[3] = {0};
	
	if(nLinesDrive > 3) nLinesDrive = 3; // safeguarding
	
	// now create each drive button to begin...
	for(int j=0; j<nLinesDrive; j++)
	{
		for(int i=0; i<7; i++)
		{
			int index = j*7+i;
			if(index < nDrives) // make sure we haven't gone over the vector size
			{
				if(drives[index].length()>2) drives[index] = drives[index].substr(0,2);
				string driveName = " " + drives[index] + " ";
				makeButton(&(driveButton[index]), driveName, 0);
				
				if(driveButton[index].w > driveButtonW) // readjust drive button width
					driveButtonW = driveButton[index].w;
			}
		}
	}
	
	// get the sizing elements based on how many drive buttons for each line
	for(int i=0; i<nLinesDrive; i++)
	{
		nButtons[i] = nDrives - i*7;
		if(nButtons[i]>7) nButtons[i] = 7;
		if(nButtons[i]<0) nButtons[i] = 0;
		
		if(nButtons[i] <= 1) // if only one item for this line
		{
			gap[i] = 0;
			lineW[i] = driveButtonW;
			firstButtonX[i] = WINDOW_WIDTH/2 - driveButtonW / 2; // place it in dead center
		}
		else // if we have more than one item for this line
		{
			gap[i] = 80 - (nButtons[i]-2) * 10;
			lineW[i] = nButtons[i]*driveButtonW + (nButtons[i]-1)*gap[i]; // total length of this line
			firstButtonX[i] = WINDOW_WIDTH/2 - lineW[i] / 2;
		}
	}
	
	// finally, position all the buttons
	for(int j=0; j<nLinesDrive; j++)
	{
		int currentX = firstButtonX[j];
		int currentY = driveButtonTop + driveButtonYStep * j;
		for(int i=0; i<7; i++)
		{
			int index = j*7+i;
			if(index < nDrives) // make sure we haven't gone over the vector size
			{
				positionButton(&driveButton[index], currentX, currentY);
				string letterName = "";
				if(!drives[index].empty())
					letterName = drives[index].substr(0,1);
				addHelp(&(driveButton[index]), letterName, 1);
				
				currentX += driveButtonW + gap[j];
			}
		}
	}
	
	// string text above the 'desktop' and 'documents' buttons
	foldersGroupLabel.setString("Folders:");
	fRect = foldersGroupLabel.getGlobalBounds();
	foldersGroupLabel.setPosition(WINDOW_WIDTH/2 - (fRect.width/2), 
			driveButtonTop + 40 + (nLinesDrive * driveButtonYStep));
	
	// 'desktop' and 'documents' buttons
	float folderButtonY = driveButtonTop + 100 + (nLinesDrive * driveButtonYStep);
	makeButton(&desktopButton, " Desktop ", 0);
	makeButton(&documentsButton, " Documents ", 0);
	float folderButtonGap = 60;
	float folderButtonsW = desktopButton.w + documentsButton.w + folderButtonGap;
	positionButton(&desktopButton, WINDOW_WIDTH/2 - (folderButtonsW/2), folderButtonY);
	positionButton(&documentsButton, WINDOW_WIDTH/2 + (folderButtonsW/2) - documentsButton.w, folderButtonY);
	addHelp(&desktopButton, "F1", 1);
	addHelp(&documentsButton, "F2", 1);

	// string text to notify ESC key will cancel
	
	// 'cancel' button
	makeButton(&goToCancelButton, " Cancel ", 2);
	positionButton(&goToCancelButton, WINDOW_WIDTH/2 - (goToCancelButton.w/2), folderButtonY + 104);
	addHelp(&goToCancelButton, "ESC", 1);
	
	runGoToDialog();
	cout << "'goto' dialog finished, result = " << yesNoDialogResult << endl;
	return goToDialogResult;
}

// run a 'goto' dialog
void Dialog::runGoToDialog()
{
	exitGoToDialog = false;
	
	while(!exitGoToDialog)
	{
		pollEvents();
		if(!windowClosed) // in case screen x-ed out
		{
			handleInputGoToDialog();
			drawGoToDialog();
		}
	}
}

// handle input for 'goto' dialog
void Dialog::handleInputGoToDialog()
{
	
	// check for mouse hover - all drive buttons
	for(int i=0; i<nDrives; i++)
	{
		if(hovering(&driveButton[i]))
		{
			highlightButton(&driveButton[i]);
			if(mouse.left())
			{
				activateButton(&driveButton[i]);
				while(mouse.left())
				{
					drawGoToDialog(); // draw to update...
				}
				// now, perform!
				exitGoToDialog = true;
				goToDialogResult = drives[i];
			}
		}
		else // not hovering on this button - deactivate
			dehighlightButton(&driveButton[i]);
	}
	// check for keyboard input - for all drive button
	for(int i=0; i<nDrives; i++)
	{
		string targetLetter = "";
		if(drives[i].length()>0) targetLetter = drives[i].substr(0,1);
		if(kbd.letterKey(targetLetter))
		{
			activateButton(&driveButton[i]);
			while(kbd.letterKey(targetLetter))
			{
				drawGoToDialog(); // draw to update...
			}
			// now, perform!
			exitGoToDialog = true;
			goToDialogResult = drives[i];
			
		}
	}
	
	// check for 'desktop' button
	if(hovering(&desktopButton))
		highlightButton(&desktopButton);
	else
		dehighlightButton(&desktopButton);

	if((hovering(&desktopButton) && mouse.left()) || kbd.f1()) // f1 button pressed
	{
		activateButton(&desktopButton);
		drawGoToDialog(); // draw once to update
		while(mouse.left() || kbd.f1()){}
		{
			exitGoToDialog = true;
			goToDialogResult = "DESKTOP";
		}
	}
	
	// check for 'documents' button
	if(hovering(&documentsButton))
		highlightButton(&documentsButton);
	else
		dehighlightButton(&documentsButton);
	
	if((hovering(&documentsButton) && mouse.left()) || kbd.f2()) // f2 button pressed
	{
		activateButton(&documentsButton);
		drawGoToDialog(); // draw once to update
		while(mouse.left() || kbd.f2()){}
		{
			exitGoToDialog = true;
			goToDialogResult = "DOCUMENTS";
		}
	}

	// check for 'cancel' button
	if(hovering(&goToCancelButton))
		highlightButton(&goToCancelButton);
	else
		dehighlightButton(&goToCancelButton);
	
	if((hovering(&goToCancelButton) && mouse.left()) || kbd.escape()) // escape button pressed
	{
		activateButton(&goToCancelButton);
		drawGoToDialog(); // draw once to update
		while(mouse.left() || kbd.escape()){}
		{
			exitGoToDialog = true;
			cout << "Canceled by user...\n";
		}
	}
	
	// clear keyboard buffer variables (not used here)
	typedChar = 0;
	strTypedChar = "";
}


void Dialog::drawGoToDialog()
{
	w->clear();
	
	// draw the labels for each button group ... and "(ESC to cancel)"
	w->draw(drivesGroupLabel);
	w->draw(foldersGroupLabel);

	// draw drive buttons
	for(int i=0;i<nDrives;i++)
	{
		w->draw(driveButton[i].rect);
		w->draw(driveButton[i].label);
		w->draw(driveButton[i].help);
	}
	
	// draw folders buttons
	w->draw(desktopButton.rect);
	w->draw(desktopButton.label);
	w->draw(desktopButton.help);
	w->draw(documentsButton.rect);
	w->draw(documentsButton.label);
	w->draw(documentsButton.help);			
	w->draw(goToCancelButton.rect);
	w->draw(goToCancelButton.label);
	w->draw(goToCancelButton.help);
	
	w->display();
}

// get current user's documents path from system...
std::string Dialog::getUserDocPathFromSystem()
{
    CHAR my_documents[MAX_PATH];
    HRESULT result = SHGetFolderPath(NULL, CSIDL_PERSONAL, NULL, SHGFP_TYPE_CURRENT, my_documents);

	string strResult = "";
	
	if (result != S_OK)
	{
		cout << "Error getting the User Documents path...\n";
		return strResult;
	}
	stringstream ss;
	ss << my_documents;
	strResult = ss.str();
	return strResult;
}

// check if a directory of the passed name exists
bool Dialog::dirExists(const std::string& dirName_in)
{
	DWORD ftyp = GetFileAttributesA(dirName_in.c_str());
	if (ftyp == INVALID_FILE_ATTRIBUTES)
		return false;  //something is wrong with your path!

	if (ftyp & FILE_ATTRIBUTE_DIRECTORY)
		return true;   // this is a directory!

	return false;    // this is not a directory!
}