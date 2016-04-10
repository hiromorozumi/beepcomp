#ifndef DIALOG_H
#define DIALOG_H

#include <vector>
#include <string>
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include "Kbd.h"
#include "Mouse.h"

class Dialog
{
	
public:

	static const int TEXT_HEIGHT = 11;
	static const int TEXT_WIDTH = 44; // DEBUG
	static const int CWD_WIDTH = 54; // DEBUG
	static const int INPUT_WIDTH = 39;
	static const int WINDOW_WIDTH = 840;
	static const int WINDOW_HEIGHT = 640;
	static const int TEXT_TOP_X = 92;
	static const int TEXT_TOP_Y = 112;
	static const int INPUT_TOP_X = 220;
	static const int INPUT_TOP_Y = 470;
	static const int HIST_SIZE = 32;
	
	int adjustedWindowWidth;
	int adjustedWindowHeight;

	Kbd kbd;
	Mouse mouse;
	sf::RenderWindow* w;
	sf::Event event;
	sf::Clock clickTimer;
	sf::Clock fireClock;
	sf::Font font;
	sf::Color dialogGreen;
	sf::Color dialogGreenLight;
	sf::Color dialogRed;
	sf::Color dialogRedLight;
	sf::Color dialogBlue;
	sf::Text itemText[TEXT_HEIGHT];
	sf::Text cwd;
	sf::Text supportedFilesText;
	sf::Text basicText[10];
	sf::Text basicTextColor[10];
	
	std::vector<std::string> filesFolders;
	std::vector<int> itemTypes;
	std::string currentDir;
	std::string defaultDir;
	std::string requestedStartFolder;
	int nItems;
	std::string strView[TEXT_HEIGHT];
	int itemTypesView[TEXT_HEIGHT];
	int topRenderLine;
	int selectedIndex;
	
	bool upFiring;
	int upProcessedCount;
	bool downFiring;
	int downProcessedCount;
	int cursorPos;
	int startRenderPos;
	
	bool exitDialog;
	bool windowClosed;
	std::string fileNameToReturn;
	int dialogMode;
	bool cancelChosen;
	std::string fileFilter;

	sf::RectangleShape inputLine;
	sf::Text inputLineLabel;
	sf::Text inputLineText;
	std::string strInput;
	
	sf::RectangleShape cursor;
	sf::Clock blinkClock;
	int blinkState;
	sf::RectangleShape selectMask;
	
	struct Button
	{
		int type;
		int x;
		int y;
		int w;
		int h;
		bool hasHelpText;
		sf::RectangleShape rect;
		sf::Text label;
		std::string strLabel;
		sf::Text help;
	};
	
	Button loadButton;
	Button cancelButton;
	Button filterButton;
	Button saveButton;
	Button exportButton;
	Button okButton;
	Button yesButton;
	Button noButton;
	
	struct Slider
	{
		int topX;
		int topY;
		int w;
		int h;
		int pos;
		int range;
		sf::RectangleShape handle;
		sf::RectangleShape rail;
	};
	
	Slider slider;
	
	int charHeight;
	int charWidth;
	
	int mouseX, mouseY;
	int mouseXprev, mouseYprev, mouseXdelta, mouseYdelta;
	bool mouseLPressed;
	bool mouseLReleased;
	int mouseWheelMoved;
	bool dragging;
	int pageClickCount;
	
	char typedChar;
	std::string strTypedChar;
	
	std::string history[HIST_SIZE];
	int historyLevel;
	bool undoing;
	
	bool yesNoDialogResult;
	bool basicDialogCancelled;
	int basicDialogMode;
	std::string basicStrView[10];
	std::string basicStrViewColor[10];
	int strX[10];
	int strY[10];
	int strColorX[10];
	int strColorY[10];
	int nLinesBasicDialog;
	int messageTopY;
	int messageBottomY;
	bool exitBasicDialog;
	sf::FloatRect pathNameRect;
	bool calledExplorerAlready;
	
	Dialog();
	~Dialog();
	
	std::string getCurrentDir();
	std::vector<std::string> getFileNamesInDir(std::string folder, std::string filter);
	std::vector<std::string> getDirNamesInDir(std::string folder);	
	void bindWindow(sf::RenderWindow* windowObj);
	std::string getLoadFileName();
	std::string getSaveFileName();
	std::string getExportFileName();
	void runFileDialog();
	void pollEvents();
	void handleInputFileDialog();
	void drawFileDialog();
	
	void makeButton(Button* b, const std::string& lbl, int type);
	void addHelp(Button* b, std::string helpStr, int position);
	void positionButton(Button* b, int xx, int yy);
	void highlightButton(Button* b);
	void dehighlightButton(Button* b);
	void activateButton(Button* b);
	void changeButtonText(Button* b, std::string newLabel);
	bool hovering(Button* b);
	
	void makeSlider(Slider* s, int ww, int hh, int range);
	void positionSlider(Slider* s, int xx, int yy);
	bool hovering(Slider* s);
	void moveSlider(Slider* s, int delta);
	void setSliderPos(Slider* s, int p);
	float sliderPosRatio(Slider* s);
	void highlightSlider(Slider* s);
	void dehighlightSlider(Slider* s);
	
	void readDir(std::string path, std::string strFilter);
	void initializeStrView();
	void setStrView();
	std::string trimRight(std::string str, int max);
	std::string trimLeft(std::string str, int max);
	std::string trimSpaceRight(std::string str);
	
	void updateCursor();
	void blinkCursor();

	bool overwriting(std::string file);
	
	void pushHistory(std::string path);
	std::string popHistory();
	void setStartFolder(std::string path);
	void unsetStartFolder();
	
	void messageDialog(std::string message);
	bool yesNoDialog(std::string question);
	void runBasicDialog();
	void parseBasicDialog(std::string str);
	void handleInputBasicDialog();
	void drawBasicDialog();
	
	bool hoveringOnPathName();
};

#endif