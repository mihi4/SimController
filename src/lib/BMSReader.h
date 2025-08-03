#pragma once
#include "DataReader.h"
#include "FlightData.h"

class BMSReader :
    public DataReader
{
public:
    void readF16Data(F16Data * data);
    BMSReader(); // default constructor
    ~BMSReader(); // default constructor
};