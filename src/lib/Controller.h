#pragma once
#include "ceserial.h"
#include <iostream>

class Controller
{
public:
    Controller(std::string name, std::string comPort, long baudrate, char datafieldCount);
    Controller();
    ~Controller();
    std::string getName();
    
    void setDataField(int pos, int value);
    unsigned char* getDatafields();

    void connect();

    void sendDataUpdate(std::string updateString);
    
    unsigned char* datafields;
private:
    ceSerial serialPort;
    long baudrate;
    
    std::string controllerName;



};


