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
    data->cautionPanelLights |= bit;
}

void BMSReader::clearCPBit(F16Data* data, unsigned long bit) {
    data->cautionPanelLights &= ~bit;
}

/* sets the value of the corresponding internal bit to the value of the BMS bit */
void BMSReader::checkCPBit(F16Data* data, FlightData* flightdata, FlightData::LightBits bmsBit, int scBit) {   
        if (flightdata->IsSet(bmsBit)) {
            setCPBit(data, scBit);
        }
        else {
            clearCPBit(data, scBit);
        }
}

void BMSReader::checkCPBit(F16Data* data, FlightData* flightdata, FlightData::LightBits2 bmsBit, int scBit) {
    if (flightdata->IsSet2(bmsBit)) {
        setCPBit(data, scBit);
    }
    else {
        clearCPBit(data, scBit);
    }
}

void BMSReader::checkCPBit(F16Data* data, FlightData* flightdata, FlightData::LightBits3 bmsBit, int scBit) {
    if (flightdata->IsSet3(bmsBit)) {
        setCPBit(data, scBit);
    }
    else {
        clearCPBit(data, scBit);
    }
}


void BMSReader::setCautionLightbits(F16Data* data, FlightData* flightdata) {

    // check if MAL/IND LIGHT button is pressed and light up everything
   // 
    std::cout << "1: " << flightData->lightBits << " 2: " << flightData->lightBits2 << " 3: " << flightData->lightBits3 << "\r";
    //if ((flightData->IsSet(flightData->AllLampBitsOn)) && (flightData->IsSet2(flightData->AllLampBits2On)) && (flightData->IsSet3(flightData->AllLampBits3On))) {{
    if ( flightData->IsSet(flightData->LEFlaps)) {  // it looks like the AllLampBits check does not work, use not available bit for it
        /*data->cautionPanelLights = 0xFFFFFFFF;
        return; */
        setCPBit(data, CPLINE1);
        setCPBit(data, CPLINE2);
        setCPBit(data, CPLINE3);
        setCPBit(data, CPLINE4);
        setCPBit(data, CPLINE5);
        setCPBit(data, CPLINE6);
        setCPBit(data, CPNUCLEAR);
        setCPBit(data, CPEEC);
    }
    else {
        clearCPBit(data, CPLINE1);
        clearCPBit(data, CPLINE2);
        clearCPBit(data, CPLINE3);
        clearCPBit(data, CPLINE4);
        clearCPBit(data, CPLINE5);
        clearCPBit(data, CPLINE6);
        clearCPBit(data, CPNUCLEAR);
        clearCPBit(data, CPEEC);
    }

    // lightbits
    checkCPBit(data, flightData, flightdata->CONFIG, CPSTORESCFG);
    checkCPBit(data, flightData, flightdata->FltControlSys, CPFLCS);
    //checkCPBit(data, flightData, flightdata->LEFlaps,
    checkCPBit(data, flightData, flightdata->EngineFault, CPENGINE);
    checkCPBit(data, flightData, flightdata->Overheat, CPOVERHEAT);
    //checkCPBit(data, flightData, flightdata->FuelLow, 
    checkCPBit(data, flightData, flightdata->Avionics, CPAVIONICS);
    checkCPBit(data, flightData, flightdata->RadarAlt, CPRADARALT);
    checkCPBit(data, flightData, flightdata->IFF, CPIFF);
    //checkCPBit(data, flightData, flightdata->ECM, 
    checkCPBit(data, flightData, flightdata->Hook, CPHOOK);
    checkCPBit(data, flightData, flightdata->NWSFail, CPNWS);
    checkCPBit(data, flightData, flightdata->CabinPress, CPCABIN);
    checkCPBit(data, flightData, flightdata->EQUIP_HOT, CPEQUIPHOT); 
    //lightbits2
    
    checkCPBit(data, flightData, flightdata->FwdFuelLow, CPFWDFUEL);
    checkCPBit(data, flightData, flightdata->AftFuelLow, CPAFTFUEL);
    checkCPBit(data, flightData, flightdata->SEC, CPSEC);
    checkCPBit(data, flightData, flightdata->OXY_LOW, CPOXYLOW);
    checkCPBit(data, flightData, flightdata->PROBEHEAT, CPPROBEHEAT);
    checkCPBit(data, flightData, flightdata->SEAT_ARM, CPSEAT);
    checkCPBit(data, flightData, flightdata->BUC, CPBUC);
    checkCPBit(data, flightData, flightdata->FUEL_OIL_HOT, CPFUELOIL);
    checkCPBit(data, flightData, flightdata->ANTI_SKID, CPANTISKID);
    
    // lightbits3
    checkCPBit(data, flightData, flightdata->Elec_Fault, CPELECSYS);
    //checkCPBit(data, flightData, flightdata->Lef_Fault, 
    checkCPBit(data, flightData, flightdata->cadc, CPCADC);
    checkCPBit(data, flightData, flightdata->ATF_Not_Engaged, CPATF);
    checkCPBit(data, flightData, flightdata->Inlet_Icing, CPINLET);
       
    return;
}

void BMSReader::readF16Data(F16Data* data) {
    // std::cout << "Reading from BMS!\n";

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

    // CautionPanel
    setCautionLightbits(data, flightData);
    
}
