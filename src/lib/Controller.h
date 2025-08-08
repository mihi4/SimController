#pragma once
#include "ceserial.h"
#include <iostream>
#include <vector>
#include "F16Data.h"

class Controller
{
public:
    Controller(std::string name, std::string comPort, long baudrate, std::vector<unsigned char> datafieldsIn);
    Controller();
    ~Controller();
    std::string getName();
    
    void setDataField(int pos, int value);
    std::vector<unsigned char> getDatafields();

    bool connect();

    void sendDataUpdate(std::vector<char> updateString);
    void updateController(F16Data * data, F16Data * prevData); 
    
private:

    ceSerial serialPort;
    long baudrate;
    
    std::string controllerName;
    std::vector<unsigned char> datafields;

    void addVarDataToUpdateString(int i, std::vector<char> &updateString, F16Data * data, F16Data * prevData);
    void addByteToUpdateString(std::vector<char>* updateString, char byte);
};


