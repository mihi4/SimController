#pragma once
#include <iostream>
#include "F16Data.h"

class DataReader
{
public:
    virtual void readF16Data(F16Data * data) { };
    virtual bool connectToSim() { return false; };
    virtual ~DataReader() { };
};

