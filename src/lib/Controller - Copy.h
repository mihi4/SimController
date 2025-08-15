#pragma once
#include <iostream>
#include <vector>
#include <bit>
#include <string>
#include <functional>
#include "F16Data.h"
#include "SerialPortHandler.h"


class SerialPortHandler;

class Controller
{
public:
    Controller(std::string name, const std::string& portName, long baudrate, std::vector<unsigned char> datafieldsIn);
    Controller(std::string name, const std::string& portName, long baudrate);
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

private:

    SerialPortHandler* serialPort;

    // Memberfunktion für empfangene Daten  
    void onSerialDataReceived(const std::string& data);

    //WindowsSerial serPort;
    //ceSerial serialPort;
    long baudrate;
    
    std::string controllerName;
    std::vector<unsigned char> datafields;
    bool connected = false;

    std::vector<unsigned char> splitValue(int value, int size);

    void buildVarStringBegin(int varNum, unsigned char size, std::vector<char>& updateString);
    void buildVarStringEnd(std::vector<char>& updateString);
    void buildVarString(int varNum, unsigned char value, std::vector<char>& updateString);    
    void buildVarString(int varNum, unsigned short value, std::vector<char>& updateString);
    void buildVarString(int varNum, unsigned int value, std::vector<char>& updateString);    

    void addVarDataToUpdateString(int i, std::vector<char> &updateString, F16Data * data, F16Data * prevData);
    void addByteToUpdateString(std::vector<char>* updateString, char byte);
};


