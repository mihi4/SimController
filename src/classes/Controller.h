#pragma once
#include <iostream>
#include <vector>
#include <bit>
#include <string>
#include <functional>
#include "../lib/F16Data.h"
//#include "SerialPortHandler.h"
#include "../lib/ArduSerial.h"


class Controller
{
public:
    // Controller(std::string name, unsigned char portNum, long baudrate, std::vector<unsigned char> datafieldsIn);
    Controller(std::string name, WindowsSerial* serPort, long baudrate);
    // Controller();
    ~Controller();
    std::string getName();
    
    void setDataField(int pos, int value);
    void addDataField(unsigned char value);
    std::vector<unsigned char> getDatafields();

    void connect();
    void disconnect();

    void sendDataUpdate(std::vector<char> updateString);
    void updateController(F16Data * data, F16Data * prevData); 
    
    void setConnected(bool status);
    bool isConnected();

    bool initialize();

    void readSerial();

private:
    
    std::string controllerName;
    std::vector<unsigned char> datafields;

    
    // data for serial comms
    long baudrate;
    unsigned char portNum;

    bool connected = false;

    bool parseControllerInfo();
    void readFromPort();
    bool parseSerialInput();

    static const byte numBytes = 50;
    char receivedBytes[numBytes] = { 0 };
    byte numReceived = 0;
    bool newData = false;
    const byte msgLen = 50;



    std::vector<unsigned char> splitValue(int value, int size);

    void buildVarStringBegin(int varNum, unsigned char size, std::vector<char>& updateString);
    void buildVarStringEnd(std::vector<char>& updateString);
    void buildVarString(int varNum, unsigned char value, std::vector<char>& updateString);    
    void buildVarString(int varNum, unsigned short value, std::vector<char>& updateString);
    void buildVarString(int varNum, unsigned int value, std::vector<char>& updateString);    

    void addVarDataToUpdateString(int i, std::vector<char> &updateString, F16Data * data, F16Data * prevData);
    void addByteToUpdateString(std::vector<char>* updateString, char byte);
};


