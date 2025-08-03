#pragma once
#include "DataReader.h"


class DCSReader :
    public DataReader
{
public:
    void readF16Data(F16Data * data) {
        std::cout << "Reading from DCS!\n";

    }
    bool connectToSim() {
        return false;
    }
    DCSReader();
    ~DCSReader();
};

DCSReader::DCSReader(void) {
    std::cout << "DCSReader created\n";
}
DCSReader::~DCSReader(void) {
    std::cout << "DCSReader deleted\n";
}
