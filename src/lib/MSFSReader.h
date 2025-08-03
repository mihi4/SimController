#pragma once
#include "DataReader.h"

class MSFSReader :
    public DataReader
{
public:
    void readF16Data() {
        std::cout << "Reading from MSFS!\n";

    }
    bool connectToSim() {
        return false;
    }
    MSFSReader();
    ~MSFSReader();
};

MSFSReader::MSFSReader(void) {
    std::cout << "MSFSReader created\n";
}
MSFSReader::~MSFSReader(void) {
    std::cout << "MSFSReader deleted\n";
}