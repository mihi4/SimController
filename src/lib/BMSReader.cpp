#include "BMSReader.h"

BMSReader::BMSReader(void) {
    std::cout << "BMSReader created\n";
}

BMSReader::~BMSReader(void) {
    std::cout << "BMSReader deleted\n";
}

void  BMSReader::readF16Data(F16Data * data) {
    std::cout << "Reading from BMS!\n";
    data->cabinPress = 5000;
}