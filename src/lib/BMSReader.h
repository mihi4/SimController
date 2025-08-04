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

    void setCautionLightbits(F16Data* data);
    void setCPBit(F16Data* data, unsigned long bit);
    void deleteCPBit(F16Data* data, unsigned long bit);

};