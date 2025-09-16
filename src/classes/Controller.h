#pragma once

#include <iostream>
#include <sstream>
#include <iomanip>
#include <vector>
#include <bit>
#include <string>
#include <functional>
#include <chrono>
#include "../lib/F16Data.h"
#include "../lib/miUtils.h"
#include "CommsHandler.h"

#define ARDUINOTIMEOUT  20

class Controller
{
public:
    
    Controller(std::string name, int comPortNum, long baudrate);
    
    ~Controller();

    std::string getName();
    int getComport();
    int getBaudrate();
    
    void setDataField(int pos, int value);
    void addDataField(unsigned char value);
    std::vector<unsigned char> getDatafields();

    void connect();
    void disconnect();

    //void sendDataUpdate(std::vector<char> updateString);
    void updateController(F16Data * data, F16Data * prevData);
    void initController(F16Data* data, F16Data* prevData);
    void readSerial();
    bool connectArduino();

    bool serialConnected();
    bool arduinoConnected();

private:
    // std::shared_ptr<WindowsSerial> serialPort;
    miUtility util;

    std::string controllerName;
    int comPortNum;
    int baudrate;

    std::vector<unsigned char> datafields;

    CommsHandler serialHandler;

    
    /* data for serial comms
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
    */


    std::vector<unsigned char> splitValue(int value, int size);

    void buildVarStringBegin(int varNum, unsigned char size, std::vector<char>& updateString);
    void buildVarStringEnd(std::vector<char>& updateString);
    void buildVarString(int varNum, unsigned char value, std::vector<char>& updateString);    
    void buildVarString(int varNum, unsigned short value, std::vector<char>& updateString);
    void buildVarString(int varNum, unsigned int value, std::vector<char>& updateString);    
    void buildVarString(int varNum, int value, std::vector<char>& updateString);
    void buildVarString(unsigned char varNum, std::string valueString, std::vector<char>& updateString);

    void addVarDataToUpdateString(unsigned char varNum, std::vector<char> &updateString, F16Data * data, F16Data * prevData);
    void addByteToUpdateString(std::vector<char>& updateString, char byte);
};


