#pragma once
#include "DataReader.h"

class MSFSReader :
    public DataReader
{
public:
    void readF16Data() {
        std::cout << "Reading from MSFS!\n";

    }
    MSFSReader();
};

MSFSReader::MSFSReader(void) {
    std::cout << "MSFSReader created\n";
}
