#include "eHSI.h"

eHSI::eHSI(int size, int xPos, int yPos)
{
	hsiW.create(sf::VideoMode(size, size), "eHSI", sf::Style::None);   
    hsiW.setFramerateLimit(60);

    if (!font.loadFromFile("fonts/FalconDED.ttf")) {
        std::cout << "error loading font\n"; 
        running = false;
        exit;
    }
    if (!texture.loadFromFile("images/ehsi.png")) {
        std::cout << "error loading image\n";
        running = false;
        exit;
    }
    float hsiWinFactor = (float)size / 1024.0;

    sprBackground.setTexture(texture);
    sprBackground.setTextureRect(sf::IntRect(0, 0, 1024, 1024));
    sprBackground.setScale(sf::Vector2f(hsiWinFactor, hsiWinFactor));
    sprBackground.setPosition(sf::Vector2f(0.0, 0.0));

    sprHeadingTape.setTexture(texture);
    sprHeadingTape.setTextureRect(sf::IntRect(0, 1024, 1024, 1024));
    sprHeadingTape.setScale(sf::Vector2f(hsiWinFactor, hsiWinFactor));
    sprHeadingTape.setPosition(sf::Vector2f((size/2), (size/2)));
    sprHeadingTape.setOrigin(512.0, 512.0);

    sprCourseArrow.setTexture(texture);
    sprCourseArrow.setTextureRect(sf::IntRect(1024, 0, 1024, 1024));
    sprCourseArrow.setScale(sf::Vector2f(hsiWinFactor, hsiWinFactor));
    sprCourseArrow.setPosition(sf::Vector2f((size / 2), (size / 2)));
    sprCourseArrow.setOrigin(512.0, 512.0);
    
    sprBearingPointer.setTexture(texture);
    sprBearingPointer.setTextureRect(sf::IntRect(1024, 1024, 49, 1004));
    sprBearingPointer.setScale(sf::Vector2f(hsiWinFactor, hsiWinFactor));
    sprBearingPointer.setOrigin(25.0, 502.0);
    sprBearingPointer.setPosition(sf::Vector2f((size / 2), (size / 2)));
    
    sprOwnShip.setTexture(texture);
    sprOwnShip.setTextureRect(sf::IntRect(1105, 1024, 91, 91));
    sprOwnShip.setScale(sf::Vector2f(hsiWinFactor, hsiWinFactor));
    sprOwnShip.setOrigin(91 / 2.0, 91 / 2.0);
    sprOwnShip.setPosition(sf::Vector2f((size / 2), (size / 2)));

    sprCDI.setTexture(texture);
    sprCDI.setTextureRect(sf::IntRect(1024, 0, 1024, 1024));
    sprCDI.setScale(sf::Vector2f(hsiWinFactor, hsiWinFactor));
    sprCDI.setPosition(sf::Vector2f((size / 2), (size / 2)));
    sprCDI.setOrigin(512.0, 512.0);

    sprHeadingBug.setTexture(texture);
    sprHeadingBug.setTextureRect(sf::IntRect(1024, 0, 1024, 1024));
    sprHeadingBug.setScale(sf::Vector2f(hsiWinFactor, hsiWinFactor));
    sprHeadingBug.setPosition(sf::Vector2f((size / 2), (size / 2)));
    sprHeadingBug.setOrigin(512.0, 512.0);

    sprToFrom.setTexture(texture);
    sprToFrom.setTextureRect(sf::IntRect(1024, 0, 1024, 1024));
    sprToFrom.setScale(sf::Vector2f(hsiWinFactor, hsiWinFactor));
    sprToFrom.setPosition(sf::Vector2f((size / 2), (size / 2)));
    sprToFrom.setOrigin(512.0, 512.0);

    dmeText.setFont(font);
    dmeText.setCharacterSize(45); // in pixels, not points!
    dmeText.setString("124");
    dmeText.setFillColor(sf::Color::White);
    
    dmeText.setPosition(sf::Vector2f(10.0, -10.0));
    
    crsText.setFont(font);
    crsText.setCharacterSize(45); // in pixels, not points!
    crsText.setString("079");
    crsText.setFillColor(sf::Color::White);
    crsText.setPosition(sf::Vector2f(505.0, -10.0));

    hsiModeTextRight.setFont(font);
    hsiModeTextRight.setCharacterSize(35); // in pixels, not points!
    hsiModeTextRight.setString("TCN");
    hsiModeTextRight.setFillColor(sf::Color::White);
    hsiModeTextRight.setPosition(sf::Vector2f((size/3.0*2.0), (size-40)));

    hsiModeTextLeft.setFont(font);
    hsiModeTextLeft.setCharacterSize(35); // in pixels, not points!
    hsiModeTextLeft.setString("PLS");
    hsiModeTextLeft.setFillColor(sf::Color::White);
    hsiModeTextLeft.setPosition(sf::Vector2f((size*0.208), (size-40)));

    if (hsiW.isOpen()) {
        hsiW.clear(sf::Color::Black);
        hsiW.setPosition(sf::Vector2i(xPos, yPos));
        hsiW.display();
    }   

    std::cout << "eHSI created\n";
}


eHSI::~eHSI()
{
    quit();
    std::cout << "eHSI deleted\n";
}


void eHSI::quit() {
    running = false;
    if (hsiW.isOpen()) hsiW.close();
}

bool eHSI::isRunning()
{
    return running;
}

void eHSI::update(F16Data* data)
{
    hsiW.clear(sf::Color::Black);
    hsiW.draw(sprBackground);
    sprHeadingTape.setRotation((data->hsiCurrentHeading) / FLOATMULT );
    hsiW.draw(sprHeadingTape);
    hsiW.draw(sprCourseArrow);
    hsiW.draw(sprBearingPointer);

    hsiW.draw(sprOwnShip);
    hsiW.draw(hsiModeTextLeft);
    hsiW.draw(hsiModeTextRight);
    hsiW.draw(dmeText);
    hsiW.draw(crsText);

    hsiW.display();
}

/*

        hsiW.clear(sf::Color::Black);
        hsiW.draw(sprBackground);
        sprHeadingTape.setRotation(rotation);
        hsiW.draw(sprHeadingTape);
        sprCourseArrow.setRotation(needleRotation);
        hsiW.draw(sprCourseArrow);
        hsiW.draw(hsiModeTextLeft);
        hsiW.draw(DmeText);

        hsiW.display();

*/
/*
    sf::RenderWindow hsiW(sf::VideoMode(600, 600), "eHSI", sf::Style::None);
    


    sf::Sprite sprHeadingTape; //0,1024, 1024x1024


    sf::Sprite sprCourseArrow; // 1024,0,1024,1024




*/

/*    if (hsiW.isOpen()) {
        hsiW.clear(sf::Color::Black);
        hsiW.setPosition(sf::Vector2i(1500, 400));
        hsiW.display();
    } */