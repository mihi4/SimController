#pragma once
#include "DataReader.h"
#include "../lib/FlightData.h"
#include <stdio.h>
#include <Windows.h>

class BMSReader :
    public DataReader
{
public:
    void readF16Data(F16Data * data);
    bool connectToSim();
    BMSReader(); // default constructor
    ~BMSReader(); // default constructor

private:
    FlightData* flightData;
    FlightData2* flightData2;

    HANDLE gSharedMemHandle = NULL;
    void* gSharedMemPtr = NULL;
    HANDLE gSharedMemHandle2 = NULL;
    void* gSharedMemPtr2 = NULL;

    miUtility util;

    void setSimVersion(F16Data* data, FlightData2* flightData2);

    void setDatabit(unsigned int &var, unsigned int bit);
    void setDatabit(unsigned char &var, unsigned int bit);
    void clearDatabit(unsigned int &var, unsigned int bit);
    void clearDatabit(unsigned char &var, unsigned int bit);

    void checkPowerbit(F16Data* data, FlightData2* flightData2, FlightData2::PowerBits bmsBit, int scBit);
    void setPowerbits(F16Data* data, FlightData2* flightData2);
    void setCautionLightbits(F16Data* data, FlightData* flightdata);
    void setCPBit(F16Data* data, unsigned long bit);
    void clearCPBit(F16Data* data, unsigned long bit);
    // overloaded function for different lightbits
    void checkCPBit(F16Data* data, FlightData* flightdata, FlightData::LightBits bmsBit, int scBit);
    void checkCPBit(F16Data* data, FlightData* flightdata, FlightData::LightBits2 bmsBit, int scBit);
    void checkCPBit(F16Data* data, FlightData* flightdata, FlightData::LightBits3 bmsBit, int scBit);
    
    std::string trimPFL(std::string pfl, bool firsthalf);
    
};