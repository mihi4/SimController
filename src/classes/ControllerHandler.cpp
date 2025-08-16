#include "ControllerHandler.h"

ControllerHandler::ControllerHandler(std::string cfgFileName)
{
}

ControllerHandler::~ControllerHandler()
{
}

void ControllerHandler::showControllers()
{
    for (int i = 0; i < contNumber; i++) {
        std::cout << i << ": " << allControllers.at(i).getName() << std::endl;
    }
}

void ControllerHandler::createControllerVector()
{
    const int cCount = 2;
    std::string cNames[cCount] = { "RightAux", "CenterCons" };
    unsigned char cPortNums[cCount] = { 4, 3 };
    for (int i = 0; i < cCount; i++) {
        WindowsSerial tempSerPort(cPortNums[i]);
        allComports.push_back(tempSerPort);
        
    }
    for (int i = 0; i < cCount; i++) {
        Controller tempC(cNames[i], &allComports.at(i), 115200);
        allControllers.push_back(tempC);
    }
    contNumber = allControllers.size();
}

void ControllerHandler::setupControllers()
{
    createControllerVector();
}

