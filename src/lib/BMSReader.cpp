#include "BMSReader.h"

BMSReader::BMSReader(void) {
    


    std::cout << "BMSReader created\n";
}

BMSReader::~BMSReader(void) {
    if (gSharedMemPtr) {
        UnmapViewOfFile(gSharedMemPtr);
        gSharedMemPtr = NULL;
    }
    if (gSharedMemPtr2) {
        UnmapViewOfFile(gSharedMemPtr2);
        gSharedMemPtr2 = NULL;
    }
    CloseHandle(gSharedMemHandle);
    CloseHandle(gSharedMemHandle2);
    std::cout << "BMSReader deleted\n";
}

bool BMSReader::connectToSim() {

    if ((gSharedMemPtr) && (gSharedMemPtr2)) return true; // are pointers already mapped?

    TCHAR dataName[] = TEXT("FalconSharedMemoryArea");
    TCHAR data2Name[] = TEXT("FalconSharedMemoryArea2");
    gSharedMemHandle = OpenFileMapping(FILE_MAP_READ, TRUE, dataName);
    gSharedMemHandle2 = OpenFileMapping(FILE_MAP_READ, TRUE, data2Name);

    if ((gSharedMemHandle) && (gSharedMemHandle2)) {  // BMS is running
        gSharedMemPtr = MapViewOfFile(gSharedMemHandle, FILE_MAP_READ, 0, 0, 0);
        gSharedMemPtr2 = MapViewOfFile(gSharedMemHandle2, FILE_MAP_READ, 0, 0, 0);
        if ((gSharedMemPtr) && (gSharedMemPtr2)) return true;
    }
    return false;
}

void  BMSReader::readF16Data(F16Data* data) {
    // std::cout << "Reading from BMS!\n";
    flightData = (FlightData*)gSharedMemPtr;
    flightData2 = (FlightData2*)gSharedMemPtr2;

    data->fuelFWD = (long)flightData->fwd;
    std::cout << "\r" << flightData->fwd;
}
