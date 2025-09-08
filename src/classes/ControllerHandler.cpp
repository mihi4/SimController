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
       std::cout << i << ": " << allControllers.at(i).getName() << " portNum: " << allControllers.at(i).getComport() << " baudrate: " << allControllers.at(i).getBaudrate() << std::endl;
    }
}

void ControllerHandler::readControllerComms() {
    for (int i = 0; i < allControllers.size(); i++) {
        allControllers[i].readSerial();
    }
}


void ControllerHandler::createControllerVector()
{
    // TODO: read this data from config file!

    const int cCount = 1;
    std::string cNames[cCount] = { "RightAux" };
    unsigned char cPortNums[cCount] = { 4 };

    for (int i = 0; i < cCount; i++) {
        std::cout << "adding controller " << std::dec << i << " COM" << std::dec << (int) cPortNums[i] << std::endl;
        allControllers.emplace_back(Controller(cNames[i],cPortNums[i], 115200));
    }

    contNumber = (int) allControllers.size();
    
}

void ControllerHandler::setupControllers()
{
    createControllerVector();
    // std::cout << "vector created\n";
    for (int i = 0; i < allControllers.size(); i++) {
        allControllers[i].connect();
    }

}

void ControllerHandler::updateControllers(F16Data* data, F16Data *prevData)
{
    for (int i = 0; i < contNumber; i++) {
        allControllers.at(i).updateController(data, prevData);
    }
}

