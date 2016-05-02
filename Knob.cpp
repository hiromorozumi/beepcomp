#include <iostream>
#include <cmath>
#include <sstream>

#include "Knob.h"

// constructor - set up a knob
Knob::Knob(std::string knobLabel, float knobSize, sf::RenderWindow* wnd, const sf::Font &knobFont)
{
	initialize(knobLabel, knobSize, wnd, knobFont);
}

void Knob::initialize(std::string knobLabel, float knobSize, sf::RenderWindow* wnd, const sf::Font &knobFont)
{
	// set the window to draw the knob on
	bindWindow(wnd);
	
	// initialize..
	value = 0;
	active = false;
	
	// set text for the knob
	label = knobLabel;
	setText(knobLabel);

	// set up lines
	minLine.setPointCount(2);
	maxLine.setPointCount(2);
	minLine.setOutlineThickness(4);
	maxLine.setOutlineThickness(4);
	
	// set up all the shape sizes
	setSize(knobSize);
	
	// labelFont = knobFont;
	labelText.setFont(knobFont);
	labelText.setCharacterSize(14);
	labelText.setColor(sf::Color(160,120,80));
	
	// start with inactive & default coloring scheme
	deactivate();	
}

void Knob::bindWindow(sf::RenderWindow* windowObj)
{
	targetWindow = windowObj;
}
	
void Knob::setText(std::string knobLabel)
{
	label = knobLabel;
	labelText.setString(knobLabel);
}

void Knob::setSize(float s)
{
	size = s;
	w = size;
	h = size;
	outRadius = size/2;
	inRadius = outRadius * 0.8;
	
	// set circule radius
	circle.setRadius(outRadius);
	circle.setOutlineThickness(1);
	circle.setOutlineColor(sf::Color(70, 60, 40));
	
	// set marker radius
	markerSize = size * 0.30;
	marker.setRadius(markerSize/2);
	// marker.setOutlineThickness(1);
	// marker.setOutlineColor(sf::Color(100, 100, 100));
}

void Knob::setPosition(float xx, float yy)
{
	x = xx;
	y = yy;
	centerX = x + w / 2;
	centerY = y + h / 2;
	
	// set circle position
	circle.setPosition(sf::Vector2f(x, y));
	
	// set end points for min/max lines
	minLine.setPoint(0, sf::Vector2f(x + w/2, y + h/2));
	minLine.setPoint(1, sf::Vector2f(x, y + h));
	maxLine.setPoint(0, sf::Vector2f(x + w/2, y + h/2));
	maxLine.setPoint(1, sf::Vector2f(x + w, y + h));

	// set position of text
	labelText.setPosition(sf::Vector2f(x, y + (h*1.1)));
	
	// set up marker end points
	update(value);
}

void Knob::activate()
{
	active = true;
	labelText.setString(toString(value));
	
	// set color scheme to 'active'
	circle.setFillColor(sf::Color(240,200,180));
	marker.setFillColor(sf::Color(170,100,80));
	minLine.setFillColor(sf::Color(0,180,180));
	maxLine.setFillColor(sf::Color(0,180,180));	
}

void Knob::deactivate()
{
	active = false;
	labelText.setString(label);
	
	textBounds = labelText.getGlobalBounds();
	labelText.setPosition(x + size/2 - textBounds.width/2, y + (h*1.1));
	
	// set color scheme to 'normal'
	circle.setFillColor(sf::Color(220,180,130));
	marker.setFillColor(sf::Color(190,150,80));
	minLine.setFillColor(sf::Color(200,140,140));
	maxLine.setFillColor(sf::Color(200,140,140));
}

bool Knob::isActive()
{
	return active;
}

bool Knob::hovering(float mx, float my)
{
	// if inside the knob's area... return true
	return	(x <= mx && mx <= x+w && y <= my && my <= y+h);
}

// update marker placement according to the passed current value
void Knob::update(int newValue)
{
	value = newValue;
	
	// if knob is active, rewrite the label with new value
	if(active)
		labelText.setString(toString(value));
	
	textBounds = labelText.getGlobalBounds();
	labelText.setPosition(x + size/2 - textBounds.width/2, y + (h*1.1));
	
	
	// update the marker position
	// first get the radian value from knob value, then set marker line ends
	radian = ( -0.125f + ( static_cast<float>(100-value) / 100.0f * 0.75f)) * TWOPI;
	// if (radian < 0) radian += TWOPI;
	
	markerX =  cos(radian)*inRadius + centerX - markerSize/2;
	markerY = -sin(radian)*inRadius + centerY - markerSize/2;
	marker.setPosition( sf::Vector2f( markerX, markerY) );
	// cout << "radian=" << radian << ", marker(x,y)=" << markerX << "," << markerY << "\n";
}

// draw this knob object to the assigned window
void Knob::draw()
{	
	// draw all the shapes that belong to the knob
	targetWindow->draw(minLine);
	targetWindow->draw(maxLine);
	targetWindow->draw(circle);
	targetWindow->draw(marker);
	targetWindow->draw(labelText);
}

string Knob::toString ( int Number )
{
	ostringstream ss;
	ss << Number;
	return ss.str();
}