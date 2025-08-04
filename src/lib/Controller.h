#pragma once
#include "ceserial.h"

class Controller
{
public:
    Controller(std::string name, std::string comPort, long baudrate, unsigned short dataVars[]);
    ~Controller();
    std::string getName();
    
private:
    ceSerial serialPort;
    long baudrate;
    unsigned short* datafields;
    std::string controllerName;



};


