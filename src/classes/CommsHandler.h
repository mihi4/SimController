#pragma once
#include "../lib/ArduSerial.h"
#include "../lib/F16Data.h"
#include <memory>
#include <iostream>
#include <vector>

class CommsHandler
{
public:
    CommsHandler();

    ~CommsHandler();

    bool initializeComport(int comPortNum, long baudrate, std::string controllerNameIn);

    void sendDataUpdate(std::vector<char> updateString);
    void readSerial(std::vector<unsigned char>& datafields);
    bool sendCommand(std::string cmdString);

    unsigned char getComPortNum();
    bool isSerialConnected();
    bool isArduinoConnected();

private:

    std::string controllerName;

    //std::shared_ptr<WindowsSerial> serialPort;    
    
    WindowsSerial serialPort;
    

    long baudrate;
    unsigned char portNum;

    bool serialConnected = false;
    bool arduinoConnected = false;

    bool parseControllerInfo(std::vector<unsigned char>& datafields);
    void readFromPort();
    bool parseSerialInput(std::vector<unsigned char>& datafields);

    static const byte numBytes = 50;
    char receivedBytes[numBytes] = { 0 };
    byte numReceived = 0;
    bool newData = false;
    const byte msgLen = 50;

};

