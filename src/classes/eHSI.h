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
	float getXPosition();
	float getYPosition();


	double pi = 2 * acos(0.0);
	float hsiWinFactor = 0.0;
	float deviationLimitPixel = 300.0; // max pixel movement from eHSI.png

	bool running = true;	
	
	sf::RenderWindow hsiW;
	sf::Texture texture;
	sf::Font font;
	sf::Font numFont;

	sf::Sprite sprBackground;
	sf::Sprite sprHeadingTape; //0,1024, 1024x1024
	sf::Sprite sprCourseArrow; // 1024,0,1024,1024
	sf::Sprite sprBearingPointer;
	sf::Sprite sprCDI;
	sf::Sprite sprILS;
	sf::Sprite sprHeadingBug;
	sf::Sprite sprOwnShip;
	sf::Sprite sprTo;
	sf::Sprite sprFrom;
	sf::Sprite sprFaultILS;
	
	sf::Text hsiModeTextRight;
	sf::Text hsiModeTextLeft;
	sf::Text dmeText;
	sf::Text dmeBackText;
	sf::Text crsText;
	
	int winSize;
	float centerXPos;
	float centerYPos;

	float currentHeadingRotation;

	float desiredCrsRotation;
	float desiredHeadingRotation;
	float bearingPointerRotation;

	float deviationXPos; 
	float deviationYPos; 
};

