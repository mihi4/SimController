#pragma once
#include "ceserial.h"
#include <iostream>
#include <vector>

class Controller
{
public:
    Controller(std::string name, std::string comPort, long baudrate, std::vector<unsigned char> datafieldsIn);
    Controller();
    ~Controller();
    std::string getName();
    
    void setDataField(int pos, int value);
    std::vector<unsigned char> getDatafields();

    void connect();

    void sendDataUpdate(std::vector<char> updateString);
    void updateController(); 
    
private:
    ceSerial serialPort;
    long baudrate;
    
    std::string controllerName;

    std::vector<unsigned char> datafields;


};


