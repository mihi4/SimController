#include "BMSReader.h"

BMSReader::BMSReader(void) {
    bool check = connectToSim();


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
    return true;

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

void BMSReader::setCPBit(F16Data* data, unsigned long bit) {


}

void BMSReader::deleteCPBit(F16Data* data, unsigned long bit) {


}

void BMSReader::setCautionLightbits(F16Data* data) {

    // check if MAL/IND LIGHT button is pressed and light up everything
    if ((flightData->lightBits & flightData->AllLampBitsOn) && (flightData->lightBits2 & flightData->AllLampBits2On) && (flightData->lightBits3 & flightData->AllLampBits3On)) {
        data->cautionPanelLights = 0xFFFFFFFF;
        return;
    }
    


    
    
    return;

    /*
    if (flightData->IsSet2(flightData->FwdFuelLow)) {
        std::cout << "fwdLow ON ";
    } else {
        std::cout << "fwdLow OFF";
    }
    if (flightData->lightBits2 & flightData->AftFuelLow) {
        std::cout << " - aftLow ON \n";
    }
    else {
        std::cout << " - aftLow OFF\n";
    } */
}

void BMSReader::readF16Data(F16Data* data) {
    // std::cout << "Reading from BMS!\n";
    setCautionLightbits(data);

    return;

    flightData = (FlightData*)gSharedMemPtr;
    flightData2 = (FlightData2*)gSharedMemPtr2;
    
    // fuel data    
    data->fuelFWD = (unsigned short)flightData->fwd; // util.map((long)flightData->fwd, 0, 42000, 0, 65535);
    data->fuelAFT = (unsigned short)flightData->aft;  // util.map((long)flightData->aft, 0, 42000, 0, 65535);
    data->fuelTotal = ((unsigned short)flightData->total)/100;
    
    // HYD PRESS
    data->hydA = (unsigned short)flightData2->hydPressureA;
    data->hydB = (unsigned short)flightData2->hydPressureB;

    // EPU
    //std::cout << "fdEPU " << flightData->epuFuel << "fdCab " << flightData2->cabinAlt << "\n";
    data->epuFuel = (unsigned short) util.map((long)flightData->epuFuel*100, 0, 10000, 0, 65535);
    data->cabinPress = (unsigned short)flightData2->cabinAlt;

    
}
