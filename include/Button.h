#ifndef BUTTON_H
#define BUTTON_H

#include <string>
#include <SFML/Graphics.hpp>

class Button
{
public:

	float x, y, w, h;
	int type; // 0 -> shape, 1-> text
	std::string label;
	float outlineThickness;
	
	bool flashing;
	bool highlighted;
	bool paused;
	bool delayTurnedOff;
	
	sf::RectangleShape rect;
	sf::ConvexShape shape1;
	sf::ConvexShape shape2;
	sf::Text labelText;
	sf::Font labelFont;
	sf::FloatRect textBounds;
	
	sf::Color buttonColor;
	sf::Color shapeColor;
	sf::Color textColor;
	
	sf::RenderWindow* targetWindow;
	
	Button(int btype, const std::string &name, float xx, float yy, float ww, float hh, sf::RenderWindow* wnd, const sf::Font &buttonFont);
	Button(){}
	~Button(){}
	
	void initialize(int btype, const std::string &name, float xx, float yy, float ww, float hh, sf::RenderWindow* wnd, const sf::Font &buttonFont);
	void highlight();
	void activate();
	void delayOff();
	void delayOn();
	void pause();
	void unpause();
	void flash();
	void flashToggle();
	void unflash();
	void unhighlight();
	bool hovering(float mx, float my);
	void draw();
};


#endif