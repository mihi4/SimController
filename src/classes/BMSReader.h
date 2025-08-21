#pragma once
#include "DataReader.h"
#include "../lib/FlightData.h"
#include <stdio.h>
#include <chrono>
#include <cmath>
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

    //F16DataHelper helper;

    HANDLE gSharedMemHandle = NULL;
    void* gSharedMemPtr = NULL;
    HANDLE gSharedMemHandle2 = NULL;
    void* gSharedMemPtr2 = NULL;

    miUtility util;

    void setSimVersion(F16Data* data, FlightData2* flightData2);

    // blinking logic data
    int intervals[BLINKBITNUM] = {BLINKSLOW, BLINKFAST, BLINKSLOW, BLINKFAST, BLINKFAST, BLINKFAST, BLINKSLOW, BLINKSLOW, BLINKSLOW, BLINKSLOW, BLINKSLOW, BLINKFAST, BLINKSLOW, };
    bool status[BLINKBITNUM];
    std::chrono::steady_clock::time_point lastToggleTimes[BLINKBITNUM]; // Letzte Toggle-Zeitpunkte  
    bool getBlinkStatus(FlightData2* flightdata2, FlightData2::BlinkBits blinkBit);


    void setDatabit(unsigned int& var, unsigned int bit);
    void setDatabit(unsigned char &var, unsigned int bit);
    void clearDatabit(unsigned int &var, unsigned int bit);
    void clearDatabit(unsigned char &var, unsigned int bit); 

    void checkPowerbit(F16Data* data, FlightData2* flightData2, FlightData2::PowerBits bmsBit, int scBit);
    void setPowerbits(F16Data* data, FlightData2* flightData2);
    
    void setECMBits(F16Data* data, FlightData2* flightdata2);

    void setCPBit(F16Data* data, unsigned long bit);
    void clearCPBit(F16Data* data, unsigned long bit);
    void setCautionLightbits(F16Data* data, FlightData* flightdata);
    // overloaded function for different lightbits
    void checkCPBit(F16Data* data, FlightData* flightdata, FlightData::LightBits bmsBit, int scBit);
    void checkCPBit(F16Data* data, FlightData* flightdata, FlightData::LightBits2 bmsBit, int scBit);
    void checkCPBit(F16Data* data, FlightData* flightdata, FlightData::LightBits3 bmsBit, int scBit);
    
    std::string trimDED_PFD(std::string line, char inv[]);
    
    
    
};