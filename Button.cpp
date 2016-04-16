#include <iostream>
#include "button.h"

using namespace std;

Button::Button(int btype, const std::string &name, float xx, float yy, float ww, float hh, sf::RenderWindow* wnd, const sf::Font &buttonFont)
{
	initialize(btype, name, xx, yy, ww, hh, wnd, buttonFont);
}

// set parameters for this button
void Button::initialize(int btype, const std::string &name, float xx, float yy, float ww, float hh, sf::RenderWindow* wnd, const sf::Font &buttonFont)
{
	sf::Color colorNormal = sf::Color(180, 140, 90);
	sf::Color colorBrighter = sf::Color(220, 180, 130);
	sf::Color colorText = sf::Color(200, 160, 140);
	
	// bind the target window object
	targetWindow = wnd;
	
	// dimensions...
	x = xx;
	y = yy;
	w = ww;
	h = hh;
	
	// set up outer rectangle
	rect.setSize(sf::Vector2f(w, h));
	rect.setPosition(sf::Vector2f(x, y));
	rect.setFillColor(sf::Color::Black);
	rect.setOutlineColor(colorNormal);
	outlineThickness = 3.2f;
	rect.setOutlineThickness(outlineThickness);
	
	type = btype;
	
	if(btype==0) // button with shape
	{
		if(name=="PLAY")
		{
			buttonColor = colorBrighter;
			shapeColor = colorBrighter;
			rect.setOutlineColor(buttonColor);
			shape1.setFillColor(shapeColor);
			// shape2.setFillColor(shapeColor);
			
			shape1.setPointCount(3);
			shape1.setPoint(0, sf::Vector2f(x + w/4,   y + h/4  ));
			shape1.setPoint(1, sf::Vector2f(x + w/4*3, y + h/2  ));
			shape1.setPoint(2, sf::Vector2f(x + w/4,   y + h/4*3));
		}
		else if(name=="PAUSE")
		{
			buttonColor = colorBrighter;
			shapeColor = colorBrighter;
			rect.setOutlineColor(buttonColor);
			shape1.setFillColor(shapeColor);
			shape2.setFillColor(shapeColor);
			
			float barWidth = w/7;
			float centerX = x + w/2;
			float devXFromCenter = w/9;
			
			shape1.setPointCount(4);
			shape1.setPoint(0, sf::Vector2f(centerX - devXFromCenter - barWidth, y + h/4  ));
			shape1.setPoint(1, sf::Vector2f(centerX - devXFromCenter - barWidth, y + h*3/4));
			shape1.setPoint(2, sf::Vector2f(centerX - devXFromCenter, y + h*3/4));
			shape1.setPoint(3, sf::Vector2f(centerX - devXFromCenter, y + h/4  ));
			
			shape2.setPointCount(4);
			shape2.setPoint(0, sf::Vector2f(centerX + devXFromCenter, y + h/4  ));
			shape2.setPoint(1, sf::Vector2f(centerX + devXFromCenter, y + h*3/4));
			shape2.setPoint(2, sf::Vector2f(centerX + devXFromCenter + barWidth, y + h*3/4));
			shape2.setPoint(3, sf::Vector2f(centerX + devXFromCenter + barWidth, y + h/4  ));
		}
		else if(name=="REWIND")
		{
			buttonColor = colorNormal;
			shapeColor = colorNormal;
			rect.setOutlineColor(buttonColor);
			shape1.setFillColor(shapeColor);
			shape2.setFillColor(shapeColor);
	
			float centerX = x + w/2;
			float centerY = y + h/2;
			float triangleWidth = w/4;
			float triangleHeight = h/1.7;
			
			shape1.setPointCount(3);
			shape1.setPoint(0, sf::Vector2f(centerX                , centerY - triangleHeight/2));
			shape1.setPoint(1, sf::Vector2f(centerX - triangleWidth, centerY                   ));
			shape1.setPoint(2, sf::Vector2f(centerX                , centerY + triangleHeight/2));
			
			shape2.setPointCount(3);
			shape2.setPoint(0, sf::Vector2f(centerX + triangleWidth, centerY - triangleHeight/2));
			shape2.setPoint(1, sf::Vector2f(centerX                , centerY                   ));
			shape2.setPoint(2, sf::Vector2f(centerX + triangleWidth, centerY + triangleHeight/2));	
		}
		else if(name=="FORWARD")
		{
			buttonColor = colorNormal;
			shapeColor = colorNormal;
			rect.setOutlineColor(buttonColor);
			shape1.setFillColor(shapeColor);
			shape2.setFillColor(shapeColor);

			float centerX = x + w/2;
			float centerY = y + h/2;
			float triangleWidth = w/4;
			float triangleHeight = h/1.7;
			
			shape1.setPointCount(3);
			shape1.setPoint(0, sf::Vector2f(centerX - triangleWidth, centerY - triangleHeight/2));
			shape1.setPoint(1, sf::Vector2f(centerX,                 centerY                   ));
			shape1.setPoint(2, sf::Vector2f(centerX - triangleWidth, centerY + triangleHeight/2));
			
			shape2.setPointCount(3);
			shape2.setPoint(0, sf::Vector2f(centerX,                 centerY - triangleHeight/2));
			shape2.setPoint(1, sf::Vector2f(centerX + triangleWidth, centerY                   ));
			shape2.setPoint(2, sf::Vector2f(centerX,                 centerY + triangleHeight/2));	
		}
	}
	else // type==1, button with text
	{
		buttonColor = colorNormal;
		textColor = colorText;
		rect.setOutlineColor(buttonColor);
		labelText.setColor(textColor);
		
		labelText.setFont(buttonFont);
		labelText.setCharacterSize(16);
		label = name;
		labelText.setString(name);
		
		// get the boundary of text area, then center the label within button
		textBounds = labelText.getGlobalBounds();
		labelText.setPosition(x + w/2 - textBounds.width/2, y + h*0.1f);
	}	
}

// if mouse moves over to a button, "highlight"
void Button::highlight()
{
	highlighted = true;
	
	rect.setOutlineColor(buttonColor);
	rect.setFillColor(buttonColor);
	shape1.setFillColor(sf::Color::Black);
	shape2.setFillColor(sf::Color::Black);
	labelText.setColor(sf::Color::Black);
}

// while mouse key is being pressed down, button is "activated"
void Button::activate()
{
	rect.setOutlineColor(sf::Color::White);
	rect.setFillColor(sf::Color::Black);
	shape1.setFillColor(sf::Color::White);
	shape2.setFillColor(sf::Color::White);
	labelText.setColor(sf::Color::White);
}

// this is specifically for dly button... (turns off delay)
void Button::delayOff()
{
	if(label!="DLY")
		return;
	delayTurnedOff = true;
	sf::Color grayColor = sf::Color(120,120,120);
	rect.setOutlineColor(grayColor);
	rect.setFillColor(sf::Color::Black);
	labelText.setColor(grayColor);
}

void Button::delayOn()
{
	if(label!="DLY")
		return;
	delayTurnedOff = false;
	rect.setOutlineColor(buttonColor);
	rect.setFillColor(sf::Color::Black);
	labelText.setColor(textColor);
}

// for pause button...
void Button::pause()
{
	if(label!="PAUSE")
		return;
}

void Button::unpause()
{
	if(label!="PAUSE")
		return;
}

void Button::flash()
{
	flashing = true;
}

void Button::flashToggle()
{
	
}

void Button::unflash()
{
	flashing = false;
}

void Button::unhighlight()
{
	highlighted = false;
	
	if(label=="PLAY"&&flashing)
	{
		flashToggle();
	}
	else if(label=="DLY" && delayTurnedOff)
	{
		delayOff();
	}
	else if(label=="PAUSE" && paused)
	{
		pause();
	}
	else
	{
		rect.setOutlineColor(buttonColor);
		rect.setFillColor(sf::Color::Black);
		shape1.setFillColor(shapeColor);
		shape2.setFillColor(shapeColor);	
		labelText.setColor(textColor);
	}
}

bool Button::hovering(float mx, float my)
{
	// if inside the knob's area... return true
	return	(x - outlineThickness <= mx && mx <= x + w + outlineThickness
		 && y - outlineThickness <= my && my <= y + h + outlineThickness);
}

void Button::draw()
{
	// outline box
	targetWindow->draw(rect);
	
	if(type==0) // button with shape
	{
		targetWindow->draw(shape1);
		targetWindow->draw(shape2);
	}
	else // button with text
	{
		targetWindow->draw(labelText);
	}
}