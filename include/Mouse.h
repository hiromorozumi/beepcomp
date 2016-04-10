#ifndef MOUSE_H
#define MOUSE_H

#include <algorithm>

class Mouse
{

public:

	//static const int MOUSE_WINDOW_WIDTH = 840;
	//static const int MOUSE_WINDOW_HEIGHT = 640;

	sf::RenderWindow* w;
	int intX;
	int intY;
	int intPrevY;
	int intYDelta;

	Mouse()
	{}
	~Mouse(){}

	void bindWindow(sf::RenderWindow* windowObj)
	{
		if(windowObj){ w = windowObj; }
	}

	bool left()
		{ if( sf::Mouse::isButtonPressed(sf::Mouse::Left) ) return true; else return false;  }
	bool right()
		{ if( sf::Mouse::isButtonPressed(sf::Mouse::Right) ) return true; else return false; }
	int x()
		{ return std::max(0, 
			std::min(840, sf::Mouse::getPosition().x) - w->getPosition().x ); }
	int y()
		{ intPrevY = intY; intY = sf::Mouse::getPosition().y - w->getPosition().y; return intY; }
	int getDelta()
		{ intPrevY = intY; intY = sf::Mouse::getPosition().y - w->getPosition().y;
			intYDelta = intY - intPrevY; return intYDelta; }

};

#endif