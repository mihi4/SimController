#pragma once
#include "DataReader.h"
#include "FlightData.h"
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

    void setCautionLightbits(F16Data* data, FlightData* flightdata);
    void setCPBit(F16Data* data, unsigned long bit);
    void clearCPBit(F16Data* data, unsigned long bit);

    void checkCPBit(F16Data* data, FlightData* flightdata, FlightData::LightBits bmsBit, int scBit);
    void checkCPBit(F16Data* data, FlightData* flightdata, FlightData::LightBits2 bmsBit, int scBit);
    void checkCPBit(F16Data* data, FlightData* flightdata, FlightData::LightBits3 bmsBit, int scBit);
    
};