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

void BMSReader::setSimVersion(F16Data* data, FlightData2* flightData2) {
    data->simVersion = "Version ";
    char buf[20];
    _itoa_s(flightData2->BMSVersionMajor, buf, 10);
    data->simVersion.append(buf);
    data->simVersion.append(".");
    _itoa_s(flightData2->BMSVersionMinor, buf, 10);
    data->simVersion.append(buf);
    data->simVersion.append(".");
    _itoa_s(flightData2->BMSVersionMicro, buf, 10);
    data->simVersion.append(buf);
}

void BMSReader::setCPBit(F16Data* data, unsigned long bit) {
    data->cautionPanelLights |= bit;
}
void BMSReader::clearCPBit(F16Data* data, unsigned long bit) {
    data->cautionPanelLights &= ~bit;
}
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
    //std::cout << "1: " << flightData->lightBits << " 2: " << flightData->lightBits2 << " 3: " << flightData->lightBits3 << "\r";
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

void BMSReader::checkPowerbit(F16Data* data, FlightData2* flightData2, FlightData2::PowerBits bmsBit, int scBit) {
    if (flightData2->IsSetPower(bmsBit)) {
        data->powerStates |= scBit;
    }
    else {
        data->powerStates &= ~scBit;
    }
}
void BMSReader::setPowerbits(F16Data* data, FlightData2* flightData2) {
    
    checkPowerbit(data, flightData2, flightData2->MainGenerator, MAINGENON);
    checkPowerbit(data, flightData2, flightData2->StandbyGenerator, STBYGENON);
    checkPowerbit(data, flightData2, flightData2->BusPowerBattery, BUSBAT);
    checkPowerbit(data, flightData2, flightData2->BusPowerEmergency, BUSEMER);
    checkPowerbit(data, flightData2, flightData2->BusPowerEssential, BUSESSENTIAL);
    checkPowerbit(data, flightData2, flightData2->BusPowerNonEssential, BUSNONESSENTIAL);
   
}

void BMSReader::setDatabit(unsigned int &var,  unsigned int bit) {
    var |= bit;
}
void BMSReader::setDatabit(unsigned char &var, unsigned int bit) {
    var |= bit;
}
void BMSReader::clearDatabit(unsigned int&var, unsigned int bit) {
    var &= bit;
}
void BMSReader::clearDatabit(unsigned char &var, unsigned int bit) {
    var &= bit;
}

std::string BMSReader::trimPFL(std::string pfl, bool firsthalf)
{
    if (pfl.empty()) return "";

    char PFLcompiled[12] = "";
    //code by Uri_ba (https://pit.uriba.org/uriba/ded-adventures-part-3/)

    byte offset = 0;
    if (!firsthalf)
    {
        offset = 12;
    }

    for (byte ctr = 0; ctr < 12; ctr++)
    {
        if (pfl[ctr + offset] == 1)           //cursor
        {
            PFLcompiled[ctr] = (char)36;
        }
        else if (pfl[ctr + offset] == 2)      //stars (inverted)
        {
            PFLcompiled[ctr] = (char)37;
        }
        else if (pfl[ctr + offset] == 3)      //??
        {
            PFLcompiled[ctr] = '_';
        }
        else if (pfl[ctr + offset] == '~')    //Pfeil nach unten
        {
            PFLcompiled[ctr] = (char)29;
        }
        else if (pfl[ctr + offset] == 94)     // °
        {
            PFLcompiled[ctr] = (char)63;
        }
        else if (pfl[ctr + offset] == 125)    // }
        {
            PFLcompiled[ctr] = (char)30;
        }
        else if (pfl[ctr + offset] == 123)    // {
        {
            PFLcompiled[ctr] = (char)31;
        }
        else
        {
            PFLcompiled[ctr] = pfl[ctr + offset];
        }
    }

    std::string ergebnis(PFLcompiled);
    return ergebnis;
}


void BMSReader::readF16Data(F16Data* data) {
    // std::cout << "Reading from BMS!\n";    

    flightData = (FlightData*)gSharedMemPtr;
    flightData2 = (FlightData2*)gSharedMemPtr2;
    
    setSimVersion(data, flightData2);

    /////////////////////////////////////////////////////////////////
    //      SIM Bits
    /////////////////////////////////////////////////////////////////
    // use PLANEFLYING to see, if cockpit is active
    if (flightData->IsSetHsi(flightData->Flying)) setDatabit(data->simStates, SIMPLANEFLYING); else clearDatabit(data->simStates, SIMPLANEFLYING);
    // FIXXXME: implement setting the SIMTESTLIGHTS bit to lightup everything during MAL/IND test
   
    /////////////////////////////////////////////////////////////////
    //      Power bits  
    /////////////////////////////////////////////////////////////////
    if (flightData2->IsSetPower(flightData2->MainGenerator)) setDatabit(data->powerStates, MAINGENON); else clearDatabit(data->powerStates, MAINGENON);
    if (flightData2->IsSetPower(flightData2->StandbyGenerator)) setDatabit(data->powerStates, STBYGENON); else clearDatabit(data->powerStates, STBYGENON);
    if (flightData2->IsSetPower(flightData2->BusPowerBattery)) setDatabit(data->powerStates, BUSBAT); else clearDatabit(data->powerStates, BUSBAT);
    if (flightData2->IsSetPower(flightData2->BusPowerEmergency)) setDatabit(data->powerStates, BUSEMER); else clearDatabit(data->powerStates, BUSEMER);
    if (flightData2->IsSetPower(flightData2->BusPowerEssential)) setDatabit(data->powerStates, BUSESSENTIAL); else clearDatabit(data->powerStates, BUSESSENTIAL);
    if (flightData2->IsSetPower(flightData2->BusPowerNonEssential)) setDatabit(data->powerStates, BUSNONESSENTIAL); else clearDatabit(data->powerStates, BUSNONESSENTIAL);

    /////////////////////////////////////////////////////////////////
    //      Relay bits  
    /////////////////////////////////////////////////////////////////
    if (flightData->IsSet3(flightData->OnGround)) setDatabit(data->relayStates, RLYWEIGHTONWHEELS); else clearDatabit(data->relayStates, RLYWEIGHTONWHEELS);
    if (flightData->IsSet(flightData->AutoPilotOn)) setDatabit(data->relayStates, RLYAUTOPILOT); else clearDatabit(data->relayStates, RLYAUTOPILOT);
    if (flightData->IsSet3(flightData->ParkBrakeOn)) setDatabit(data->relayStates, RLYPARKINGBRAKE); else clearDatabit(data->relayStates, RLYPARKINGBRAKE);
    if (flightData2->IsSetPower(flightData2->JetFuelStarter)) setDatabit(data->relayStates, RLYJFSTARTER); else clearDatabit(data->relayStates, RLYJFSTARTER);
    if (flightData->IsSet3(flightData->FlcsBitRun)) setDatabit(data->relayStates, RLYFLTCTLBIT); else clearDatabit(data->relayStates, RLYFLTCTLBIT);
    if (flightData->IsSet3(flightData->SpeedBrake)) setDatabit(data->relayStates, RLYSPEEDBRAKE); else clearDatabit(data->relayStates, RLYSPEEDBRAKE);
    

    // setPowerbits(data, flightData2);
    /////////////////////////////////////////////////////////////////
    //   Right AUX
    ///////////////////////////////////////////////////////////////// 
    // fuel data    
    data->fuelFWD = (unsigned short)flightData->fwd; // util.map((long)flightData->fwd, 0, 42000, 0, 65535);
    data->fuelAFT = (unsigned short)flightData->aft;  // util.map((long)flightData->aft, 0, 42000, 0, 65535);
    data->fuelTotal = ((unsigned short)flightData->total)/100;
    // HYD PRESS
    data->hydA = (unsigned short)flightData2->hydPressureA;
    data->hydB = (unsigned short)flightData2->hydPressureB;   
    data->epuFuel = (unsigned short) util.map((long)flightData->epuFuel*100, 0, 10000, 0, 65535);
    data->cabinPress = (unsigned short)flightData2->cabinAlt;
    // CautionPanel
    setCautionLightbits(data, flightData);


    
    

}
