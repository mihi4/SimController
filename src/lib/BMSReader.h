#pragma once
#include "DataReader.h"
#include "FlightData.h"

class BMSReader :
    public DataReader
{
public:
    void readF16Data() {
        std::cout << "Reading from BMS!\n";
    }
    BMSReader(); // default constructor
    ~BMSReader(); // default constructor
};

BMSReader::BMSReader(void) {
    std::cout << "BMSReader created\n";
}

BMSReader::~BMSReader(void) {
    std::cout << "BMSReader deleted\n";
}