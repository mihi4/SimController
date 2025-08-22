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
    // checkCPBit(data, flightData, flightdata->PROBEHEAT, CPPROBEHEAT);
    
    checkCPBit(data, flightData, flightdata->SEAT_ARM, CPSEAT);
    checkCPBit(data, flightData, flightdata->BUC, CPBUC);
    checkCPBit(data, flightData, flightdata->FUEL_OIL_HOT, CPFUELOIL);
    checkCPBit(data, flightData, flightdata->ANTI_SKID, CPANTISKID);
    
    // lightbits3
    // checkCPBit(data, flightData, flightdata->Elec_Fault, CPELECSYS);
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
void BMSReader::clearDatabit(unsigned int& var, unsigned int bit) {
    var &= ~bit;
}
void BMSReader::setDatabit(unsigned short& var, unsigned short bit) {
    var |= bit;
}
void BMSReader::clearDatabit(unsigned short& var, unsigned short bit) {
    var &= ~bit;
}
void BMSReader::setDatabit(unsigned char &var, unsigned char bit) {
    var |= bit;    
}
void BMSReader::clearDatabit(unsigned char &var, unsigned char bit) {
    var &= ~bit;
}

void setECMBits(F16Data* data, FlightData2* flightdata2) {
    // FIXXXME
}

std::string BMSReader::trimDED_PFD(std::string line, char inv[]) {    
    const char charNum = 25;
    if (line.empty()) return "";

    char lineCompiled[charNum] = "";
    //code by Uri_ba (https://pit.uriba.org/uriba/ded-adventures-part-3/)
    
    for (char ctr = 0; ctr < charNum; ctr++)
    {
        if (inv[ctr] == 2) {  // is char at position inverted?
            if (isalpha(line[ctr]))
            { lineCompiled[ctr] = (char)(line[ctr] + 36); }
            else if (isdigit(line[ctr]))
            { lineCompiled[ctr] = (char)(line[ctr] + 43); }
            else if (line[ctr] == 1)
            { lineCompiled[ctr] = (char)36; }
            else if (line[ctr] == 2)
            { lineCompiled[ctr] = (char)37; }
            else if (line[ctr] == 3)
            { lineCompiled[ctr] = (char)223; }
            else if (line[ctr] == 32)
            { lineCompiled[ctr] = (char)33; }
            else
            { lineCompiled[ctr] = line[ctr]; }
        }
        else {
            if (line[ctr] == 1)           //cursor
            { lineCompiled[ctr] = (char)36; }
            else if (line[ctr] == 2)      //stars (inverted)
            { lineCompiled[ctr] = (char)37; }
            else if (line[ctr] == 3)      //??
            { lineCompiled[ctr] = '_'; }
            else if (line[ctr] == '~')    //Pfeil nach unten
            { lineCompiled[ctr] = (char)29; }
            else if (line[ctr] == 94)     // °
            { lineCompiled[ctr] = (char)63; }
            else if (line[ctr] == 125)    // }
            { lineCompiled[ctr] = (char)30; }
            else if (line[ctr] == 123)    // {
            { lineCompiled[ctr] = (char)31; }
            else
            {
                lineCompiled[ctr] = line[ctr];
            }
        }
    }

    std::string ergebnis(lineCompiled);    
    return ergebnis;

}

bool BMSReader::getBlinkStatus(FlightData2* flightdata2, FlightData2::BlinkBits blinkBit) {
    auto currentTime = std::chrono::steady_clock::now();
    unsigned short intervall = 0;

    if (!flightdata2->IsSetBlink(blinkBit)) return true; // blinkbit in mem not set, so it should light normally

    double dIndex = std::log((int)blinkBit) / std::log(2);
    char vectorIndex = (char)dIndex;
    auto elapsedTime = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - lastToggleTimes[vectorIndex]).count();
    if (elapsedTime >= intervals[vectorIndex]) {
        status[vectorIndex] = !status[vectorIndex];
        lastToggleTimes[vectorIndex] = currentTime; // Aktualisiere den letzten Toggle-Zeitpunkt 
    }
    return status[vectorIndex];
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

    // setPowerbits(data, flightData2);

    /////////////////////////////////////////////////////////////////
    //      Relay bits  
    /////////////////////////////////////////////////////////////////
    if (flightData->IsSet3(flightData->OnGround)) setDatabit(data->relayStates, RLYWEIGHTONWHEELS); else clearDatabit(data->relayStates, RLYWEIGHTONWHEELS);
    if (flightData->IsSet(flightData->AutoPilotOn)) setDatabit(data->relayStates, RLYAUTOPILOT); else clearDatabit(data->relayStates, RLYAUTOPILOT);
    if (flightData->IsSet3(flightData->ParkBrakeOn)) setDatabit(data->relayStates, RLYPARKINGBRAKE); else clearDatabit(data->relayStates, RLYPARKINGBRAKE);
    if (flightData2->IsSetPower(flightData2->JetFuelStarter)) setDatabit(data->relayStates, RLYJFSTARTER); else clearDatabit(data->relayStates, RLYJFSTARTER);
    if (flightData->IsSet3(flightData->FlcsBitRun)) setDatabit(data->relayStates, RLYFLTCTLBIT); else clearDatabit(data->relayStates, RLYFLTCTLBIT);
    if (flightData->IsSet3(flightData->SpeedBrake)) setDatabit(data->relayStates, RLYSPEEDBRAKE); else clearDatabit(data->relayStates, RLYSPEEDBRAKE);
    
    
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
    data->epuFuel = (unsigned short)(flightData->epuFuel * FLOATMULT); // util.map((long)flightData->epuFuel*FLOATMULT, 0, 10000, 0, 65535);
    data->cabinPress = (unsigned short)flightData2->cabinAlt;
    // CautionPanel
    setCautionLightbits(data, flightData);
    // separat code for blinking CP lights PROBEHEAT and Elec_Fault   
    if (flightData->IsSet2(flightData->PROBEHEAT) && getBlinkStatus(flightData2, flightData2->PROBEHEAT)) setCPBit(data, CPPROBEHEAT); else clearCPBit(data, CPPROBEHEAT);
    if (flightData->IsSet3(flightData->Elec_Fault) && getBlinkStatus(flightData2, flightData2->Elec_Fault)) setCPBit(data, CPELECSYS); else clearCPBit(data, CPELECSYS);
    
    // PFD and DED
    data->pfdLine1 = trimDED_PFD(flightData->PFLLines[0], flightData->PFLInvert[0]);
    data->pfdLine2 = trimDED_PFD(flightData->PFLLines[1], flightData->PFLInvert[1]);
    data->pfdLine3 = trimDED_PFD(flightData->PFLLines[2], flightData->PFLInvert[2]);
    data->pfdLine4 = trimDED_PFD(flightData->PFLLines[3], flightData->PFLInvert[3]);
    data->pfdLine5 = trimDED_PFD(flightData->PFLLines[4], flightData->PFLInvert[4]);
        
    data->dedLine1 = trimDED_PFD(flightData->DEDLines[0], flightData->Invert[0]);
    data->dedLine2 = trimDED_PFD(flightData->DEDLines[1], flightData->Invert[1]);
    data->dedLine3 = trimDED_PFD(flightData->DEDLines[2], flightData->Invert[2]);
    data->dedLine4 = trimDED_PFD(flightData->DEDLines[3], flightData->Invert[3]); 
    data->dedLine5 = trimDED_PFD(flightData->DEDLines[4], flightData->Invert[4]);
    

    /////////////////////////////////////////////////////////////////
    //   Instrument Panel
    ///////////////////////////////////////////////////////////////// 
    
    //  Engine Cluster
    data->oilPressure = (unsigned short) (flightData->oilPressure * FLOATMULT);  // bms val is 0-100, too coarse for smooth movement
    data->nozzlePos = (unsigned short) (flightData->nozzlePos * FLOATMULT); // bms val is 0-100, too coarse for smooth movement
    data->rpm = (unsigned short) (flightData->rpm * FLOATMULT); // FIXXXME, use trim function
    data->ftit = (unsigned short) (flightData->ftit * FLOATMULT); // FIXXXME, use trim function

    data->fuelFlow = (unsigned int) (flightData->fuelFlow / 100); // only the first 3 digits change in fuelflow

    // main instruments

    data->kias = (unsigned int)(flightData->kias * FLOATMULT); //FIXXXME, use trim function
    data->mach = (unsigned short)(flightData->mach * FLOATMULT);     

    data->altitude = (int)(flightData2->AAUZ*-1);
    data->altPointer = (data->altitude) % 1000;
    data->altCalibration = std::to_string(flightData2->AltCalReading);
    data->altThousands = std::to_string((int)(data->altitude / 100));
    
    // instrument bits FIXXXME
    
    
    // instrument panel lighbits FIXXXME finish later
    // left eyebrows  
    if (flightData->IsSet(flightData->MasterCaution)) setDatabit(data->instPanelLights, EBMASTERC); else clearDatabit(data->instPanelLights, EBMASTERC);
    if (flightData->IsSet(flightData->TF)) setDatabit(data->instPanelLights, EBTFFAIL); else clearDatabit(data->instPanelLights, EBTFFAIL);
    // TWP
    if (flightData->IsSet2(flightData->HandOff)) setDatabit(data->instPanelLights, TWPHANDOFF); else clearDatabit(data->instPanelLights, TWPHANDOFF);
    if (flightData->IsSet2(flightData->Launch) && getBlinkStatus(flightData2, flightData2->Launch)) setDatabit(data->instPanelLights, TWPLAUNCH); else clearDatabit(data->instPanelLights, TWPLAUNCH);
    if (flightData->IsSet2(flightData->PriMode) && getBlinkStatus(flightData2, flightData2->PriMode)) setDatabit(data->instPanelLights, TWPPRIMODE); else clearDatabit(data->instPanelLights, TWPPRIMODE);
    if (flightData->IsSet2(flightData->Unk) && getBlinkStatus(flightData2, flightData2->Unk)) setDatabit(data->instPanelLights, TWPUNKNOWN); else clearDatabit(data->instPanelLights, TWPUNKNOWN);    
    if (flightData->IsSet3(flightData->SysTest)) setDatabit(data->instPanelLights, TWPSYSTEST); else clearDatabit(data->instPanelLights, TWPSYSTEST);
    if (flightData->IsSet2(flightData->TgtSep)) setDatabit(data->instPanelLights, TWPTGTSEP); else clearDatabit(data->instPanelLights, TWPTGTSEP);
    // ECM light
    if (flightData->IsSet2(flightData->EcmPwr) && getBlinkStatus(flightData2, flightData2->ECM_Oper)) setDatabit(data->instPanelLights, ECMON); else clearDatabit(data->instPanelLights, ECMON);
    // MISC Panel TFR light
    if (flightData->IsSet2(flightData->TFR_ENGAGED )) setDatabit(data->instPanelLights, MODEACTIVE); else clearDatabit(data->instPanelLights, MODEACTIVE);
    if (flightData->IsSet(flightData->TFR_STBY)) setDatabit(data->instPanelLights, MODESTBY); else clearDatabit(data->instPanelLights, MODESTBY);
    // HUD indexers
    if (flightData->IsSet(flightData->AOAAbove)) setDatabit(data->instPanelLights, IDXAOAABOVE); else clearDatabit(data->instPanelLights, IDXAOAABOVE);
    if (flightData->IsSet(flightData->AOAOn)) setDatabit(data->instPanelLights, IDXAOAON); else clearDatabit(data->instPanelLights, IDXAOAON);
    if (flightData->IsSet(flightData->AOABelow)) setDatabit(data->instPanelLights, IDXAOABELOW); else clearDatabit(data->instPanelLights, IDXAOABELOW);
    if (flightData->IsSet(flightData->RefuelRDY)) setDatabit(data->instPanelLights, IDXRDY); else clearDatabit(data->instPanelLights, IDXRDY);
    if (flightData->IsSet(flightData->RefuelAR)) setDatabit(data->instPanelLights, IDXARNWS); else clearDatabit(data->instPanelLights, IDXARNWS);
    if (flightData->IsSet(flightData->RefuelDSC)) setDatabit(data->instPanelLights, IDXDISC); else clearDatabit(data->instPanelLights, IDXDISC);
    // right eyebrows
    if (flightData->IsSet(flightData->ENG_FIRE)) setDatabit(data->instPanelLights, EBENGFIRE); else clearDatabit(data->instPanelLights, EBENGFIRE);
    if (flightData->IsSet2(flightData->ENGINE)) setDatabit(data->instPanelLights, EBENGINE); else clearDatabit(data->instPanelLights, EBENGINE);
    if (flightData->IsSet(flightData->HYD)) setDatabit(data->instPanelLights, EBHYDOILPRESS); else clearDatabit(data->instPanelLights, EBHYDOILPRESS);
    if (flightData->IsSet(flightData->FLCS)) setDatabit(data->instPanelLights, EBFLCS); else clearDatabit(data->instPanelLights, EBFLCS);
    if (flightData->IsSet3(flightData->DbuWarn)) setDatabit(data->instPanelLights, EBDBUON); else clearDatabit(data->instPanelLights, EBDBUON);
    if (flightData->IsSet(flightData->T_L_CFG)) setDatabit(data->instPanelLights, EBTOLDGCFG); else clearDatabit(data->instPanelLights, EBTOLDGCFG);
    // CANOPY??!?!
    if (flightData->IsSet(flightData->OXY_BROW)) setDatabit(data->instPanelLights, EBOXYLOW); else clearDatabit(data->instPanelLights, EBOXYLOW);
    // marker beacon
    if (flightData->IsSetHsi(flightData->OuterMarker)) { 
        if (getBlinkStatus(flightData2, flightData2->OuterMarker)) setDatabit(data->instPanelLights, MARKERBEACON); else clearDatabit(data->instPanelLights, MARKERBEACON);
    }
    if (flightData2->IsSetBlink(flightData2->MiddleMarker)) {
        if (getBlinkStatus(flightData2, flightData2->MiddleMarker)) setDatabit(data->instPanelLights, MARKERBEACON); else clearDatabit(data->instPanelLights, MARKERBEACON);
    }
    
    /////////////////////////////////////////////////////////////////
    //   Left Console
    ///////////////////////////////////////////////////////////////// 
   
    // CMDS
    data->cmdsChaffStr = std::to_string( (int) flightData->ChaffCount);
    data->cmdsFlareStr = std::to_string((int)flightData->FlareCount);

    if (flightData->IsSet2(flightData->Go))  setDatabit(data->cmdsBits, CMDSGO); else clearDatabit(data->cmdsBits, CMDSGO);
    if (flightData->IsSet2(flightData->NoGo))  setDatabit(data->cmdsBits, CMDSNOGO); else clearDatabit(data->cmdsBits, CMDSNOGO);
    if (flightData->IsSet2(flightData->Degr))  setDatabit(data->cmdsBits, CMDSDEGR); else clearDatabit(data->cmdsBits, CMDSDEGR);
    if (flightData->IsSet2(flightData->Rdy))  setDatabit(data->cmdsBits, CMDSRDY); else clearDatabit(data->cmdsBits, CMDSRDY);

    // UHF
    data->uhfChannel = flightData2->uhf_panel_preset;
    data->uhfFrequency = std::to_string(flightData2->uhf_panel_frequency);

    // IFF/AUXCOMM

    data->iffDisplay = flightData2->iffBackupMode1Digit1 + flightData2->iffBackupMode1Digit2 + flightData2->iffBackupMode3ADigit1 + flightData2->iffBackupMode3ADigit2;
    data->auxDisplay = std::to_string(flightData->AUXTChan);

    // MANTRIM

    data->trimPitch = flightData->TrimPitch * FLOATMULT;
    data->trimRoll = flightData->TrimRoll * FLOATMULT;

    // ECM Bits


    // Left Console Lightbits

    
}   

