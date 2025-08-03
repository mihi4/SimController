#pragma once
#include "DataReader.h"


class DCSReader :
    public DataReader
{
public:
    void readF16Data() {
        std::cout << "Reading from DCS!\n";

    }
    DCSReader();
};

DCSReader::DCSReader(void) {
    std::cout << "DCSReader created\n";
}

