#ifndef KNOB_H
#define KNOB_H

#include <string>
#include <SFML/Graphics.hpp>

using namespace std;

class Knob
{
public:

	static const int TWOPI = 6.2831853072;

	float x, y, w, h, size;
	float centerX, centerY;
	float markerX, markerY;
	float inRadius;
	float outRadius;
	float markerSize;
	std::string label;
	
	int value;
	float radian;
	bool active;

	sf::CircleShape circle;
	sf::CircleShape marker;
	sf::ConvexShape minLine;
	sf::ConvexShape maxLine;
	sf::Text labelText;
	sf::Font labelFont;
	sf::FloatRect textBounds;
	sf::RenderWindow* targetWindow;

	Knob(std::string knobLabel, float knobSize, sf::RenderWindow* wnd, const sf::Font &knobFont);
	Knob(){}
	~Knob(){}

	void initialize(std::string knobLabel, float knobSize, sf::RenderWindow* wnd, const sf::Font &knobFont);
	void bindWindow(sf::RenderWindow* windowObj);
	void setText(std::string knobLabel);
	void setSize(float s);
	void setPosition(float xx, float yy);
	void activate();
	void deactivate();
	void setColor();
	bool isActive();
	bool hovering(float mx, float my);
	void update(int newValue);
	void draw();
	std::string toString ( int Number );
};

#endif