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

void ControllerHandler::readControllerComms() {
    for (int i = 0; i < allControllers.size(); i++) {
        allControllers[i].readSerial();
    }
}


void ControllerHandler::createControllerVector()
{
    // TODO: read this data from config file!

    const int cCount = 2;
    std::string cNames[cCount] = { "RightAux", "CenterCons" };
    unsigned char cPortNums[cCount] = { 4, 3 };

 /*   Controller c1("RightAux", 4, 115200);
    Controller c2("CenterCons", 3, 115200);

    allControllers.push_back(c1);
    allControllers.push_back(c2); */
    for (int i = 0; i < cCount; i++) {
        //Controller tempC(cNames[i], cPortNums[i], 115200);
        std::cout << "adding controller " << std::dec << i << " COM" << std::dec << (int) cPortNums[i] << std::endl;
        //allControllers.push_back(tempC);        
        //allComports.emplace_back(WindowsSerial(cPortNums[i]));        
        allControllers.emplace_back(Controller(cNames[i],cPortNums[i], 115200));
    }

    contNumber = allControllers.size();
    
}

void ControllerHandler::setupControllers()
{
    createControllerVector();
    std::cout << "vector created\n";
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

