#pragma once
#include <iostream>
#include "../lib/F16Data.h"
#include "../lib/miUtils.h"

class DataReader
{
public:
    virtual void readF16Data(F16Data * data) { };
    virtual bool connectToSim() { return false; };
    virtual ~DataReader() { };

private: 
    miUtility util;
};

