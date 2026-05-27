#pragma once
#include <iostream>
#include <SFML/Graphics.hpp>
#include "../lib/F16Data.h"

class eHSI
{
public:
	eHSI(int size, int xPos, int yPos);

	~eHSI();

	void quit();
	bool isRunning();

	void update(F16Data * data);

private:
	sf::RenderWindow hsiW;
	sf::Texture texture;
	sf::Font font;

	sf::Sprite sprBackground;
	sf::Sprite sprHeadingTape; //0,1024, 1024x1024
	sf::Sprite sprCourseArrow; // 1024,0,1024,1024
	sf::Text hsiModeTextRight;
	sf::Text hsiModeTextLeft;
	sf::Text dmeText;
	sf::Text crsText;


	bool running = true;

};

