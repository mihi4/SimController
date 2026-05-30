// SimControl.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include "SimControl.h"
#include <bitset>


bool checkParameter(int argNum) {
    if (argNum == 2) return false;
    return true;
}

char parseSimParameter(char* argv[]) {
    // std::cout << "Parsing " << argv[1] << std::endl;
    if (strcmp(argv[1], "BMS") == 0) {
        // std::cout << "parsed BMS\n";
        return BMS;
    }
    else if (strcmp(argv[1], "DCS") == 0) {
        // std::cout << "parsed DCS\n";
        return DCS;
    }
    else if (strcmp(argv[1], "MSFS") == 0) {
        // std::cout << "parsed MSFS\n";
        return MSFS;
    }
    else {
        return NULL;
    }

}

 
std::unique_ptr<DataReader> createDataReader(short selectedSim) {
    switch (selectedSim) {
    case BMS:
        return std::make_unique<BMSReader>();
        break;
    case DCS:
        return std::make_unique<DCSReader>();
        break;
    case MSFS:
        return std::make_unique<MSFSReader>();
        break;
    default:
        return nullptr;
        break;
    }    
}

int main(int argc, char* argv[])
{   
    std::cout << "*************************************************************\n";
    std::cout << "*                                                           *\n";
    std::cout << "* Hello, Pilot! Press LSHIFT/LCTRL/LALT + BACKSPACE to quit *\n";
    std::cout << "*                                                           *\n";
    std::cout << "*************************************************************\n";

    /*********************************************
       check if commandline parameter fit 
       if not, print message and quit
    **********************************************/
    // 
    if (checkParameter(argc)) {
        std::cout << "Wrong parameter! Usage: SimControl.exe BMS|DCS|MSFS";
        return 0;
    }

    short selectedSim = parseSimParameter(argv);
    if (!selectedSim) {
        std::cout << "Sim parameter not supported, Usage: SimControl.exe BMS|DCS|MSFS";
        return 0;
    }

    auto reader = createDataReader(selectedSim);

    if (!reader) {
        std::cout << "!!!!   ERROR creating DataReader   !!!!";
        return 0;
    }
    
    /*********************************************
      setup the needed variables and objects
    **********************************************/

    F16Data data;
    F16Data prevData;
    miUtility util;
    
    bool simConnected = false;

    /**********************************
        create SFML data
    **********************************/

    sf::RenderWindow appW(sf::VideoMode(250, 30), "SimController");
    sf::Font font;
    if (!font.loadFromFile("fonts/FalconDED.ttf")) std::cout << "error loading font\n";

    sf::Text quitText;
    quitText.setFont(font);
    quitText.setString("CLOSE ME TO QUIT");
    quitText.setCharacterSize(20); // in pixels, not points!
    quitText.setFillColor(sf::Color::White);

    if (appW.isOpen()) {
        appW.clear(sf::Color::Black);
        appW.setPosition(sf::Vector2i(1500, 1000));
        appW.draw(quitText);
        appW.display();
    }

    eHSI hsi(600, 1200, 1100);      

    /****************************************

             Controller Handling

    *****************************************/



    std::cout << "------ Setting up Controllers ------\n";    
    
    ControllerHandler cHandler(CTRLCONFIGFILE);  

    cHandler.setupControllers();
       
    cHandler.showControllers();    
    
    std::cout << "------ Controller Setup done  ------\n";
    std::cout << std::endl;
    
    /****************************************
     
               connecting and init
    
    *****************************************/    
    
    while(true) {        
        if (!simConnected) {
            std::cout << "connecting to sim...\r";
            if (reader->connectToSim()) {
                simConnected = true;
                reader->readF16Data(&data);                
                std::cout << "\nConnected to " << argv[1] << data.simVersion << "\n";
                cHandler.initControllers(&data, &prevData);
                break;
            }
        }
        if ((GetKeyState(VK_LCONTROL) & 0x8000) && (GetKeyState(VK_LSHIFT) & 0x8000) && (GetKeyState(VK_LMENU) & 0x8000) && (GetKeyState(VK_BACK) & 0x8000)) { break; }
    }   

    std::cout << "running mainloop window\n";
    /****************************************
     
                    main loop
    
    *****************************************/        
            
    data.hsiCurrentHeading = 0;
    char buf[100];
    float rotation = 0.0;
    float needleRotation = 0.0;
    int rotFactor = 1;
    // while (true) {               
    while (appW.isOpen()) {

        sf::Event event;
        while (appW.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)                
                appW.close();

            /*if (event.type == sf::Event::KeyPressed) {
                if (event.key.scancode == sf::Keyboard::Scan::A) { 
                    if (data.hsiCurrentHeading == 0) data.hsiCurrentHeading = 36000;
                    data.hsiCurrentHeading -= 100;                    
                }
                if (event.key.scancode == sf::Keyboard::Scan::S) {
                    data.hsiCurrentHeading += 100;
                    if (data.hsiCurrentHeading == 36000) data.hsiCurrentHeading = 0;
                }
                
                if (event.key.scancode == sf::Keyboard::Scan::D) {
                    if (data.hsiDesiredHeading == 0) data.hsiDesiredHeading = 36000;
                    data.hsiDesiredHeading -= 100;                    
                }
                if (event.key.scancode == sf::Keyboard::Scan::F) {                    
                    data.hsiDesiredHeading += 100;
                    if (data.hsiDesiredHeading == 36000) data.hsiDesiredHeading = 0;
                }
                if (event.key.scancode == sf::Keyboard::Scan::Z) {
                    if (data.hsiDesiredCourse == 0) data.hsiDesiredCourse = 36000;
                    data.hsiDesiredCourse -= 100;                    
                }
                if (event.key.scancode == sf::Keyboard::Scan::X) {                    
                    data.hsiDesiredCourse += 100;
                    if (data.hsiDesiredCourse == 36000) data.hsiDesiredCourse = 0;
                }

                if (event.key.scancode == sf::Keyboard::Scan::C) data.hsiCourseDeviation -= 5;
                if (event.key.scancode == sf::Keyboard::Scan::V) data.hsiCourseDeviation += 5;

                if (event.key.scancode == sf::Keyboard::Scan::T) data.instrumentBits = INSTHSITO;
                if (event.key.scancode == sf::Keyboard::Scan::Y) data.instrumentBits = INSTHSIFROM;
                



                std::cout << "deviation: " << data.hsiCourseDeviation << " *** ";
                std::cout << "current: " << data.hsiCurrentHeading << " *** ";
                std::cout << "crs: " << data.hsiDesiredCourse << " *** ";
                std::cout << "heading: " << data.hsiDesiredHeading << std::endl;
                
                if (event.key.scancode == sf::Keyboard::Scan::Q) { 
                    if (hsi.isRunning()) {
                        hsi.quit();
                    }
                    else {
                        std::cout << "eHSI already closed!\n";
                    }
                    
                }
                
            } */

        }

        if (reader->connectToSim()) {
            reader->readF16Data(&data);
            //std::bitset<32> y(data.cautionPanelLights);
            //sprintf_s(buf, "fwd: %5u aft: %5u total: %5u hydA: %6u hydB %6u CP: 0x%8x bits: ", data.fuelFWD, data.fuelAFT, data.fuelTotal, data.hydA, data.hydB, data.cautionPanelLights);            
            //std::cout << buf << y << std::endl;
            if (!prevData.isSameAs(data)) {  // only send data if anything has changed                 
                /*std::bitset<32> y(data.cautionPanelLights);
                sprintf_s(buf, "CP: 0x%8x bits: ", data.cautionPanelLights);
                std::cout << buf << y << std::endl;*/
                cHandler.updateControllers(&data, &prevData);
                if (hsi.isRunning()) hsi.update(&data);

            }
            prevData = data;
            //std::cout << "mapping" << util.map(data.fuelFWD, 0,  42000, 0, 65534) << "\r";
        }
        else {
            simConnected = false; // try again next run
        }

        //std::cout << "altPointer: " << data.altPointer << " slip: " << data.adiSideslip << " pitch: " << data.adiPitch << " roll: " << data.adiRoll << " ilsHor: " << data.adiIlsHorPos << " ilsVer: " << data.adiIlsVerPos << "\n";        
        cHandler.readControllerComms();

        // check for quit keycommand LCTRL+LSHIFT+LALT+BACKSPACE
        if ((GetKeyState(VK_LCONTROL) & 0x8000) && (GetKeyState(VK_LSHIFT) & 0x8000) && (GetKeyState(VK_LMENU) & 0x8000) && (GetKeyState(VK_BACK) & 0x8000)) { break; }

        Sleep(10);
    }

    std::cout << "\n\nquitting!\n";   

    return 0;
}

