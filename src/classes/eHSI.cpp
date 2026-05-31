#include "eHSI.h"

eHSI::eHSI(int size, int xPos, int yPos)
{
    hsiW.create(sf::VideoMode(size, size), "eHSI", sf::Style::None);
    hsiW.setFramerateLimit(60);

    winSize = size;
    centerXPos = winSize / 2.0;
    centerYPos = winSize / 2.0;

    if (!font.loadFromFile("fonts/techbold.ttf")) {
        std::cout << "error loading font\n";
        running = false;
        exit;
    }
    if (!numFont.loadFromFile("fonts/lucon.ttf")) {
        std::cout << "error loading numFont\n";
        running = false;
        exit;
    }
    if (!texture.loadFromFile("images/ehsi_new.png")) {
        std::cout << "error loading image\n";
        running = false;
        exit;
    }
    texture.setRepeated(false);

    hsiWinFactor = (float)size / 1024.0;



    std::cout << "centerXpos: " << centerXPos << " y: " << centerYPos << std::endl;
    sprBackground.setTexture(texture);
    sprBackground.setTextureRect(sf::IntRect(0, 0, 1024, 1024));
    sprBackground.setScale(sf::Vector2f(hsiWinFactor, hsiWinFactor));
    sprBackground.setPosition(sf::Vector2f(0.0, 0.0));

    sprHeadingTape.setTexture(texture);
    sprHeadingTape.setTextureRect(sf::IntRect(0, 1024, 1024, 1024));
    sprHeadingTape.setScale(sf::Vector2f(hsiWinFactor, hsiWinFactor));
    sprHeadingTape.setPosition(sf::Vector2f((size / 2), (size / 2)));
    sprHeadingTape.setOrigin(512.0, 512.0);

    sprCourseArrow.setTexture(texture);
    sprCourseArrow.setTextureRect(sf::IntRect(1024, 0, 1024, 1024));
    sprCourseArrow.setScale(sf::Vector2f(hsiWinFactor, hsiWinFactor));
    sprCourseArrow.setPosition(sf::Vector2f(centerXPos, centerYPos));
    sprCourseArrow.setOrigin(512.0, 512.0);

    sprBearingPointer.setTexture(texture);
    sprBearingPointer.setTextureRect(sf::IntRect(1024, 1024, 49, 1024));
    sprBearingPointer.setScale(sf::Vector2f(hsiWinFactor, hsiWinFactor));
    sprBearingPointer.setOrigin(25.0, 512.0);
    sprBearingPointer.setPosition(sf::Vector2f(centerXPos, centerYPos));

    sprHeadingBug.setTexture(texture);
    sprHeadingBug.setTextureRect(sf::IntRect(1196, 1024, 109, 950));
    sprHeadingBug.setScale(sf::Vector2f(hsiWinFactor, hsiWinFactor));
    sprHeadingBug.setPosition(sf::Vector2f(centerXPos, centerYPos));
    sprHeadingBug.setOrigin(109 / 2.0, 475);

    sprOwnShip.setTexture(texture);
    sprOwnShip.setTextureRect(sf::IntRect(1105, 1024, 91, 91));
    sprOwnShip.setScale(sf::Vector2f(hsiWinFactor, hsiWinFactor));
    sprOwnShip.setOrigin(91 / 2.0, 91 / 2.0);
    sprOwnShip.setPosition(sf::Vector2f(centerXPos, centerYPos));

    sprTo.setTexture(texture);
    sprTo.setTextureRect(sf::IntRect(1325, 1024, 420, 250));
    sprTo.setScale(sf::Vector2f(hsiWinFactor, hsiWinFactor));
    sprTo.setOrigin(210.0, 125.0);
    sprTo.setPosition(sf::Vector2f(centerXPos, centerYPos));

    sprFaultILS.setTexture(texture);
    sprFaultILS.setTextureRect(sf::IntRect(1352, 1100, 370, 274));
    sprFaultILS.setScale(sf::Vector2f(hsiWinFactor * -1, hsiWinFactor * -1));
    sprFaultILS.setOrigin(185.0, 137.0);
    sprFaultILS.setPosition(sf::Vector2f(centerXPos, centerYPos));

    sprFrom.setTexture(texture);
    sprFrom.setTextureRect(sf::IntRect(1325, 1024, 420, 250));
    sprFrom.setScale(sf::Vector2f(hsiWinFactor, hsiWinFactor * -1.0));
    sprFrom.setOrigin(210.0, 125.0);
    sprFrom.setPosition(sf::Vector2f(centerXPos, centerYPos));

    sprCDI.setTexture(texture);
    sprCDI.setTextureRect(sf::IntRect(1073, 1324, 16, 400));
    sprCDI.setScale(sf::Vector2f(hsiWinFactor, hsiWinFactor));
    sprCDI.setPosition(sf::Vector2f(centerXPos, centerYPos));
    sprCDI.setOrigin(8.0, 200.0);

    sprILS.setTexture(texture);
    sprILS.setTextureRect(sf::IntRect(1090, 1324, 16, 400));
    sprILS.setScale(sf::Vector2f(hsiWinFactor, hsiWinFactor));
    sprILS.setPosition(sf::Vector2f(centerXPos, centerYPos));
    sprILS.setOrigin(8.0, 200.0);



    unsigned short dmeCrsSize = 35;
    dmeText.setFont(numFont);
    dmeText.setCharacterSize(dmeCrsSize); // in pixels, not points!
    dmeText.setString("124");
    dmeText.setFillColor(sf::Color::White);    
    dmeText.setPosition(sf::Vector2f(10.0, 0.0));

    dmeBackText.setFont(numFont);
    dmeBackText.setCharacterSize(dmeCrsSize); // in pixels, not points!
    dmeBackText.setString("4");
    dmeBackText.setFillColor(sf::Color::Black);
    dmeBackText.setPosition(sf::Vector2f(80.0, 0.0));
    
    crsText.setFont(numFont);
    crsText.setCharacterSize(dmeCrsSize); // in pixels, not points!
    crsText.setString("079");
    crsText.setFillColor(sf::Color::White);
    crsText.setPosition(sf::Vector2f(535.0, 0.0));

    unsigned short modeSize = 40;
    hsiModeTextRight.setFont(font);
    hsiModeTextRight.setCharacterSize(modeSize); // in pixels, not points!
    hsiModeTextRight.setString("TCN");
    hsiModeTextRight.setFillColor(sf::Color::White);
    hsiModeTextRight.setPosition(sf::Vector2f((size/3.0*2.0), (size-45)));

    hsiModeTextLeft.setFont(font);
    hsiModeTextLeft.setCharacterSize(modeSize); // in pixels, not points!
    hsiModeTextLeft.setString("PLS");
    hsiModeTextLeft.setFillColor(sf::Color::White);
    hsiModeTextLeft.setPosition(sf::Vector2f((size*0.208), (size-45)));

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

    /*************************************************
    *   compass rose
    **************************************************/
    currentHeadingRotation = 360.0 - ((float)data->hsiCurrentHeading / FLOATMULT);
    if (currentHeadingRotation == 360) currentHeadingRotation = 0;

    sprHeadingTape.setRotation(currentHeadingRotation);
    hsiW.draw(sprHeadingTape);

    /*************************************************
    *   course arrow
    **************************************************/

    desiredCrsRotation = 360.0 - (float)(data->hsiCurrentHeading - data->hsiDesiredCourse) / FLOATMULT;
    if (desiredCrsRotation >= 360.0) desiredCrsRotation -= 360.0;

    sprCourseArrow.setRotation(desiredCrsRotation);
    hsiW.draw(sprCourseArrow);

    /*************************************************
    *   heading bug
    **************************************************/
    
    desiredHeadingRotation = 360.0 - (float)(data->hsiCurrentHeading - data->hsiDesiredHeading) / FLOATMULT;
    if (desiredHeadingRotation >= 360.0) desiredHeadingRotation -= 360.0;

    sprHeadingBug.setRotation(desiredHeadingRotation);
    hsiW.draw(sprHeadingBug);

    /*************************************************
    *   bearing pointer
    **************************************************/
    bearingPointerRotation = 360.0 - (float)(data->hsiCurrentHeading - data->hsiBearingToBeacon) / FLOATMULT;
    if (bearingPointerRotation >= 360.0) bearingPointerRotation -= 360.0;

    sprBearingPointer.setRotation(bearingPointerRotation);
    hsiW.draw(sprBearingPointer);

    /******************************************
    *    TO/FROM Indicator
    * *****************************************/

    sprFrom.setRotation(desiredCrsRotation);
    sprTo.setRotation(desiredCrsRotation);
    if (data->instrumentBits & INSTHSITO) hsiW.draw(sprTo);
    if (data->instrumentBits & INSTHSIFROM) hsiW.draw(sprFrom);

    /******************************************
    *    Fault indicators
    * *****************************************/

    sprFaultILS.setRotation(desiredCrsRotation);
    if (data->instrumentBits & INSTHSIILSWARN) hsiW.draw(sprFaultILS);

    /******************************************
    *    Deviation Bar
    * *****************************************/
    
    data->hsiDeviationLimit = 1000; //FIXXXME remove after testing

    float deviationLimitScaled = deviationLimitPixel * hsiWinFactor;
    float deviationFactor = deviationLimitScaled / data->hsiDeviationLimit;
    float localHsiCourseDeviation = 0;
    localHsiCourseDeviation = data->hsiCourseDeviation;
    
    // check, when "ILS" line is displayed!
    // (data->hsiMode == MODE_NAV) ? localHsiCourseDeviation = data->hsiCourseDeviation : localHsiCourseDeviation = data->hsiLocalizerCourse;

    if ((localHsiCourseDeviation > 0.0) && (localHsiCourseDeviation >= data->hsiDeviationLimit)) localHsiCourseDeviation = data->hsiDeviationLimit;
    if ((localHsiCourseDeviation < 0.0) && (localHsiCourseDeviation <= ((-1) * data->hsiDeviationLimit))) localHsiCourseDeviation = data->hsiDeviationLimit * (-1);

    float crsDeviationPixel = localHsiCourseDeviation * deviationFactor;
    
    float xAdd = (-1) * ((cos(desiredCrsRotation * pi / 180) * localHsiCourseDeviation)) * deviationFactor;
    float yAdd = (-1) * ((sin(desiredCrsRotation * pi / 180) * localHsiCourseDeviation)) * deviationFactor;

    if ((desiredCrsRotation == 0) || (desiredCrsRotation == 180)) {
        yAdd = 0;
        xAdd = (-1) * localHsiCourseDeviation * deviationFactor;
    }
    else if ((desiredCrsRotation == 90) || (desiredCrsRotation == 270)) {
        xAdd = 0;
        yAdd = (-1) * localHsiCourseDeviation * deviationFactor;
    }
    if (desiredCrsRotation == 270) yAdd *= -1;
    if (desiredCrsRotation == 180) xAdd *= -1;

    deviationXPos = winSize / 2.0 + xAdd;
    deviationYPos = winSize / 2.0 + yAdd;

    if (true) { // check, when to show CDI and when dotted line! (data->hsiMode == MODE_NAV) {
        sprCDI.setPosition(sf::Vector2f(deviationXPos, deviationYPos));
        sprCDI.setRotation(desiredCrsRotation);
        hsiW.draw(sprCDI);
    }
    else {
        sprILS.setPosition(sf::Vector2f(deviationXPos, deviationYPos));  
        sprILS.setRotation(desiredCrsRotation);
        hsiW.draw(sprILS);
    }
  
    // debug output 

    dmeText.setString(std::to_string(localHsiCourseDeviation));
    crsText.setString(std::to_string(data->hsiLocalizerCourse));
    hsiModeTextLeft.setString(std::to_string(data->hsiDeviationLimit));
    //hsiModeTextRight.setString(std::to_string(deviationYPos));

  
    /*********************************************
    *     Text Outputs
    **********************************************/

    switch (data->hsiMode) {
    case MODE_ILS_NAV:
        hsiModeTextLeft.setString("PLS");
        hsiModeTextRight.setString("NAV");
        break;
    case MODE_ILS_TACAN:
        hsiModeTextLeft.setString("PLS");
        hsiModeTextRight.setString("TCN");
        break;
    case MODE_NAV:
        hsiModeTextLeft.setString("");
        hsiModeTextRight.setString("NAV");
        break;
    case MODE_TACAN:
        hsiModeTextLeft.setString("");
        hsiModeTextRight.setString("TCN");
        break;
    }

    char buf[4];
    int intPart = static_cast<int>((data->hsiDesiredCourse / FLOATMULT));
    snprintf(buf, sizeof(buf), "%03d", intPart);
    std::string oss = buf;
    crsText.setString(oss);

    intPart = static_cast<int>((data->hsiDistanceToBeacon / FLOATMULT));
    snprintf(buf, sizeof(buf), "%03d", intPart);
    oss = buf;
    dmeText.setString(oss);

    sf::RectangleShape rect(sf::Vector2f(30, 43));
    rect.setFillColor(sf::Color::White);
    rect.setPosition(75, 0);
    hsiW.draw(rect);

    int dmeBackDigit = static_cast<int>((float)data->hsiDistanceToBeacon / (FLOATMULT / 10));
    dmeBackDigit %= 10;
    dmeBackText.setString(std::to_string(dmeBackDigit));


    hsiW.draw(hsiModeTextLeft);
    hsiW.draw(hsiModeTextRight);
    hsiW.draw(crsText);
    hsiW.draw(dmeText);
    hsiW.draw(dmeBackText);

    /*********************************************
    *     Ownship
    **********************************************/
    hsiW.draw(sprOwnShip);

    hsiW.display();

}