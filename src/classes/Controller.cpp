#include "Controller.h"

Controller::Controller(std::string name, int comPortNum, long baudrate = 115200)
    : controllerName(name), baudrate(baudrate), comPortNum(comPortNum) {
    datafields = {}; // new unsigned char[datafieldCount];    
    //std::cout << "Controller constructor, " << controllerName << std::endl;
}

Controller::~Controller() {
    std::cout << "Controller " << controllerName << " deconstructor.\n"; 
    //delete serialPort;
}

////////////////////////////////////////
// internal helper functions
////////////////////////////////////////

void debugUpdateString(std::vector<char> updateString) {
    std::string debugString;    
    std::ostringstream oss;

    
    for (char byte : updateString)  {        
        oss << std::hex << std::setw(2) << std::setfill('0') << (static_cast<unsigned int>(byte) & 0xFF) << " ";
    }   
    debugString = oss.str();

    // remove last space
    if (!debugString.empty()) {
        debugString.pop_back();
    }   
    std::cout << "Debug String: " << debugString << std::endl;
}

std::vector<unsigned char> Controller::splitValue(int value, int size) {
    std::vector<unsigned char> bytes;
    for (size_t i = 0; i < size; ++i) {
        // Maskieren und Verschieben, um das i-te Byte zu erhalten
        unsigned char byte = (value >> (8 * i)) & 0xFF;        
        bytes.push_back(byte);
    }
    return bytes;
}

// just calls the readSerial() function from the serialHandler
void Controller::readSerial() {
    serialHandler.readSerial(datafields);
}

void Controller::connect()
{
    if (serialHandler.initializeComport(comPortNum, baudrate, controllerName)) {        
        std::cout << controllerName << " sending connect command\n";
        serialHandler.sendCommand("<C>");
    }
    else {
        std::cout << controllerName << " Cannot initialize Comport " << std::dec << comPortNum << std::endl;
    }
}

void Controller::disconnect()
{
    //std::cout << "stopping serial port thread" << std::endl;
    //serialPort->threadStop();
      
}

void Controller::addByteToUpdateString(std::vector<char>& updateString, char byte) {
   // FIXXXME not sure, if needed

}

// add the first part of the updatestring, usually <U$varnum$varsize
void Controller::buildVarStringBegin(int varNum, unsigned char size, std::vector<char>& updateString) {
    //updateString.clear();
    updateString.push_back(CMDSTART);
    updateString.push_back(CMDUPDATE);

    updateString.push_back(varNum);
    updateString.push_back(size);
}
void Controller::buildVarStringEnd(std::vector<char>& updateString) {
    updateString.push_back(CMDEND);
    //updateString.push_back('\0');
}

// create the updateString to the Arduino from a var with an Char value
void Controller::buildVarString(int varNum, unsigned char value, std::vector<char>& updateString) {
    // std::cout << "buildChar. valsize: " << sizeof(value) << " strSize: " << updateString.size() << std::endl;
    buildVarStringBegin(varNum, sizeof(value), updateString);
    updateString.push_back(value);
    buildVarStringEnd(updateString); 
}

// create the updateString to the Arduino from a var with a Short value
void Controller::buildVarString(int varNum, unsigned short value, std::vector<char>& updateString) {    
    buildVarStringBegin(varNum, sizeof(value), updateString);
    std::vector<unsigned char> splitValues = splitValue(value, sizeof(value));
    std::cout << "SHORTvar " << std::dec << varNum << ", value " << std::dec << value << std::endl;
    for (int i = 0; i < splitValues.size(); i++) { 
        //std::cout << "SHORTvar " << std::dec << varNum << ", byte " << std::dec << i << ": " << std::hex << (int)splitValues[i] << std::endl;
        updateString.push_back(splitValues[i]);
    }
    buildVarStringEnd(updateString);    
}

// create the updateString to the Arduino from a var with an UnsignedInt value
void Controller::buildVarString(int varNum, unsigned int value, std::vector<char>& updateString) {    
    buildVarStringBegin(varNum, sizeof(value), updateString);
    std::vector<unsigned char> splitValues = splitValue(value, sizeof(value));
    //std::cout << "INTvar " << std::dec << varNum << ", value " << std::dec << value << std::endl;
    for (int i = 0; i < splitValues.size(); i++) {
        //std::cout << "iNTvar " << std::dec << varNum << ", byte " << std::dec << i << ": " << std::hex << (int)splitValues[i] << std::endl;
        updateString.push_back(splitValues[i]);
    }
    buildVarStringEnd(updateString);
}

// create the updateString to the Arduino from a var with an Int value
void Controller::buildVarString(int varNum, int value, std::vector<char>& updateString) {
    buildVarStringBegin(varNum, sizeof(value), updateString);
    std::vector<unsigned char> splitValues = splitValue(value, sizeof(value));
    for (int i = 0; i < splitValues.size(); i++) {
        updateString.push_back(splitValues[i]);
    }
    buildVarStringEnd(updateString);
}

// create the updateString to the Arduino from a var with String value
void Controller::buildVarString(unsigned char varNum, std::string valueString, std::vector<char>& updateString) {    
    buildVarStringBegin(varNum, VARSTRING, updateString);
    updateString.insert(updateString.end(), valueString.begin(), valueString.end());
    buildVarStringEnd(updateString);
}

// get the correct data for varNum from the internal F-16 data and call the corresponding "buildVarString" function 
void Controller::addVarDataToUpdateString(unsigned char varNum, std::vector<char> &updateString, F16Data* data, F16Data* prevData) {
    
    switch (varNum) {
    // states
    case SIMSTATES:
        if (data->simStates != prevData->simStates) {
            buildVarString(varNum, data->simStates, updateString);
        }        
        break;
    case POWERSTATES:
        if (data->powerStates != prevData->powerStates) {
            buildVarString(varNum, data->powerStates, updateString);
        }
        break;
    case RELAYSTATES:
        if (data->relayStates != prevData->relayStates) {
            buildVarString(varNum, data->relayStates, updateString);
        }
        break;
    // RIGHT AUX
    case FUELFWD:
        if (data->fuelFWD != prevData->fuelFWD) {                 
            buildVarString(varNum, data->fuelFWD, updateString);
        }
        break;
    case FUELAFT:
        if (data->fuelAFT != prevData->fuelAFT) {            
            buildVarString(varNum, data->fuelAFT, updateString);
        }
        break;
    case FUELTOTAL:
        if (data->fuelTotal != prevData->fuelTotal) {            
            buildVarString(varNum, data->fuelTotal, updateString);
        }
        break;
    case HYDA:
        if (data->hydA != prevData->hydA) {
            buildVarString(varNum, data->hydA, updateString);
        }
        break;
    case HYDB:        
        if (data->hydB != prevData->hydB) {        
            buildVarString(varNum, data->hydB, updateString);
        }
        break;
    case EPUFUEL:        
        if (data->epuFuel != prevData->epuFuel) {        
            buildVarString(varNum, data->epuFuel, updateString);
        }
        break;
    case CABINPRESS:        
        if (data->cabinPress != prevData->cabinPress) {        
            buildVarString(varNum, data->cabinPress, updateString);
        }
        break;
    case CAUTIONPANELLIGHTS:        
        if (data->cautionPanelLights != prevData->cautionPanelLights) {        
            buildVarString(varNum, data->cautionPanelLights, updateString);
        }
        break;
    case PFDLINE1:
        if (data->pfdLine1 != prevData->pfdLine1) {
            buildVarString(PFDLINE1, data->pfdLine1, updateString);            
        }        
        break;
    case PFDLINE2:
        if (data->pfdLine2 != prevData->pfdLine2) {
            buildVarString(PFDLINE2, data->pfdLine2, updateString);
        }
        break;
    case PFDLINE3:
        if (data->pfdLine3 != prevData->pfdLine3) {
            buildVarString(PFDLINE3, data->pfdLine3, updateString);
        }
        break;
    case PFDLINE4:
        if (data->pfdLine4 != prevData->pfdLine4) {
            buildVarString(PFDLINE4, data->pfdLine4, updateString);
        }
        break;
    case PFDLINE5:
        if (data->pfdLine5 != prevData->pfdLine5) {
            buildVarString(PFDLINE5, data->pfdLine5, updateString);
        }
        break;
    // INSTRUMENT PANEL
    case DEDLINE1:
        if (data->dedLine1 != prevData->dedLine1) {
            buildVarString(DEDLINE1, data->dedLine1, updateString);
        }
        break;
    case DEDLINE2:
        if (data->dedLine2 != prevData->dedLine2) {
            buildVarString(DEDLINE2, data->dedLine2, updateString);
        }
        break;
    case DEDLINE3:
        if (data->dedLine3 != prevData->dedLine3) {
            buildVarString(DEDLINE3, data->dedLine3, updateString);
        }
        break;
    case DEDLINE4:
        if (data->dedLine4 != prevData->dedLine4) {
            buildVarString(DEDLINE4, data->dedLine4, updateString);
        }
        break;
    case DEDLINE5:
        if (data->dedLine5 != prevData->dedLine5) {
            buildVarString(DEDLINE5, data->dedLine5, updateString);
        }
        break;
    case OILPRESS:
        if (data->oilPressure != prevData->oilPressure) {
            buildVarString(varNum, data->oilPressure, updateString);
        }
        break;
    case NOZZLEPOS:
        if (data->nozzlePos != prevData->nozzlePos) {
            buildVarString(varNum, data->nozzlePos, updateString);
        }
        break;
    case RPM:
        if (data->rpm != prevData->rpm) {
            buildVarString(varNum, data->rpm, updateString);
        }
        break;
    case FTIT:
        if (data->ftit != prevData->ftit) {
            buildVarString(varNum, data->ftit, updateString);
        }
        break;
    case FUELFLOW:
        if (data->fuelFlow != prevData->fuelFlow) {
            buildVarString(varNum, data->fuelFlow, updateString);
        }
        break;
    case INSTPANELLIGHTS:
        if (data->instPanelLights != prevData->instPanelLights) {
            buildVarString(varNum, data->instPanelLights, updateString);
        }    
        break;
    // CENTER CONSOLE INSTRUMENTS
    case KIAS:
        if (data->kias != prevData->kias) {
            buildVarString(varNum, data->kias, updateString);
        }
        break;
    case MACH:
        if (data->mach != prevData->mach) {
            buildVarString(varNum, data->mach, updateString);
        }
        break;
    case ALTTHOUSANDS:
        if (data->altThousands != prevData->altThousands) {
            buildVarString(varNum, data->altThousands, updateString);
        }
        break;
    case ALTPOINTER:
        if (data->altPointer != prevData->altPointer) {
            buildVarString(varNum, data->altPointer, updateString);
        }
        break;
    case ALTITUDE:
        if (data->altitude != prevData->altitude) {
            buildVarString(varNum, data->altitude, updateString);
        }
        break;
    case ALTCAL:
        if (data->altCalibration != prevData->altCalibration) {
            buildVarString(varNum, data->altCalibration, updateString);
        }
        break;
    case VVIVAL:
        if (data->vvi != prevData->vvi) {
            buildVarString(varNum, data->vvi, updateString);
        }
        break;
    case AOAVAL:
        if (data->aoa != prevData->aoa) {
            buildVarString(varNum, data->aoa, updateString);
        }
        break;
    case ADIILSHORPOS:
        if (data->adiIlsHorPos != prevData->adiIlsHorPos) {
            buildVarString(varNum, data->adiIlsHorPos, updateString);
        }
        break;
    case ADIILSVERPOS:
        if (data->adiIlsVerPos != prevData->adiIlsVerPos) {
            buildVarString(varNum, data->adiIlsVerPos, updateString);
        }
        break;
    case ADIPITCH:
        if (data->adiPitch != prevData->adiPitch) {
            buildVarString(varNum, data->adiPitch, updateString);
        }
        break;
    case ADIROLL:
        if (data->adiRoll != prevData->adiRoll) {
            buildVarString(varNum, data->adiRoll, updateString);
        }
        break;
    case ADISIDESLIP:
        if (data->adiSideslip != prevData->adiSideslip) {
            buildVarString(varNum, data->adiSideslip, updateString);
        }
        break;
    case HSIAIRBASEX:
        if (data->hsiAirbaseX != prevData->hsiAirbaseX) {
            buildVarString(varNum, data->hsiAirbaseX, updateString);
        }
        break;
    case HSIAIRBASEY:
        if (data->hsiAirbaseY != prevData->hsiAirbaseY) {
            buildVarString(varNum, data->hsiAirbaseY, updateString);
        }
        break;
    case HSIBEARINGTOBEACON:
        if (data->hsiBearingToBeacon != prevData->hsiBearingToBeacon) {
            buildVarString(varNum, data->hsiBearingToBeacon, updateString);
        }
        break;
    case HSICOURSEDEVIATION :
        if (data->hsiCourseDeviation != prevData->hsiCourseDeviation) {
            buildVarString(varNum, data->hsiCourseDeviation, updateString);
        }
        break;
    case HSICURRENTHEADING:
        if (data->hsiCurrentHeading != prevData->hsiCurrentHeading) {
            buildVarString(varNum, data->hsiCurrentHeading, updateString);
        }
        break;
    case HSIDESIREDCOURSE:
        if (data->hsiDesiredCourse != prevData->hsiDesiredCourse) {
            buildVarString(varNum, data->hsiDesiredCourse, updateString);
        }
        break;
    case HSIDESIREDHEADING:
        if (data->hsiDesiredHeading != prevData->hsiDesiredHeading) {
            buildVarString(varNum, data->hsiDesiredHeading, updateString);
        }
        break;
    case HSIDEVIATIONLIMIT:
        if (data->hsiDeviationLimit != prevData->hsiDeviationLimit) {
            buildVarString(varNum, data->hsiDeviationLimit, updateString);
        }
        break;
    case HSIDISTANCETOBEACON:
        if (data->hsiDistanceToBeacon != prevData->hsiDistanceToBeacon) {
            buildVarString(varNum, data->hsiDistanceToBeacon, updateString);
        }
        break;
    case HSIHALFDEVIATIONLIMIT:
        if (data->hsiHalfDeviationLimit != prevData->hsiHalfDeviationLimit) {
            buildVarString(varNum, data->hsiHalfDeviationLimit, updateString);
        }
        break;
    case HSILOCALIZERCOURSE:
        if (data->hsiLocalizerCourse != prevData->hsiLocalizerCourse) {
            buildVarString(varNum, data->hsiLocalizerCourse, updateString);
        }
        break;

    case INSTRUMENTBITS:
        if (data->instrumentBits != prevData->instrumentBits) {
            buildVarString(varNum, data->instrumentBits, updateString);
        }
        break;
    /////////////////////////////        
    // LEFT CONSOLE
    /////////////////////////////
    case LEFTCONSLIGHTS:
        if (data->leftConsLights != prevData->leftConsLights) {
            buildVarString(varNum, data->leftConsLights, updateString);
        }
        break;
    case ECMLIGHTS:
        if (data->ecmLights != prevData->ecmLights) {
            buildVarString(varNum, data->ecmLights, updateString);
        }
        break;
    case UHFCHAN:
        if (data->uhfChannel != prevData->uhfChannel) {
            buildVarString(varNum, data->uhfChannel, updateString);
        }
        break;
    case UHFFREQ:
        if (data->uhfFrequency != prevData->uhfFrequency) {
            buildVarString(varNum, data->uhfFrequency, updateString);
        }
        break;
    case IFFDISP:
        if (data->iffDisplay != prevData->iffDisplay) {
            buildVarString(varNum, data->iffDisplay, updateString);
        }
        break;
    case AUXDISP:
        if (data->auxDisplay != prevData->auxDisplay) {
            buildVarString(varNum, data->auxDisplay, updateString);
        }
        break;
    case TRIMROLL:
        if (data->trimRoll != prevData->trimRoll) {
            buildVarString(varNum, data->trimRoll, updateString);
        }
        break;
    case TRIMPITCH:
        if (data->trimPitch != prevData->trimPitch) {
            buildVarString(varNum, data->trimPitch, updateString);
        }
        break;
    case CMDSBITS:
        if (data->cmdsBits != prevData->cmdsBits) {
            buildVarString(varNum, data->cmdsBits, updateString);
        }
        break;
    case CMDS01STR:
        if (data->cmds01Str != prevData->cmds01Str) {
            buildVarString(varNum, data->cmds01Str, updateString);
        }
        break;
    case CMDS02STR:
        if (data->cmds02Str != prevData->cmds02Str) {
            buildVarString(varNum, data->cmds02Str, updateString);
        }
        break;
    case CMDSCHAFFSTR:
        if (data->cmdsChaffStr != prevData->cmdsChaffStr) {
            buildVarString(varNum, data->cmdsChaffStr, updateString);
        }
        break;
    case CMDSFLARESTR:
        if (data->cmdsFlareStr != prevData->cmdsFlareStr) {
            buildVarString(varNum, data->cmdsFlareStr, updateString);
        }
        break;


    default:
        break;
    }     
    return;
}

// iterates through all vars defined in datafields, get updateString for it and send it to Arduino
void Controller::updateController(F16Data* data, F16Data* prevData) {

        std::vector<char> updateString;

        for (int i = 0; i < datafields.size(); i++) {
            updateString.clear();                        
            addVarDataToUpdateString(datafields[i], updateString, data, prevData); 
            // if (updateString.size() > 3) debugUpdateString(updateString);
            if (updateString.size() > 3) serialHandler.sendDataUpdate(updateString);
            readSerial();
            //Sleep(10);
        }
}

void Controller::initController(F16Data* data, F16Data* prevData) {
    std::cout << "initializing controller variables" << std::endl;
    std::vector<char> updateString;

    for (int i = 0; i < datafields.size(); i++) {
        updateString.clear();
        std::cout << "init var " << std::dec << (int)datafields[i] << std::endl;
        addVarDataToUpdateString(datafields[i], updateString, data, prevData);
        if (updateString.size() > 3) serialHandler.sendDataUpdate(updateString);
    }
}

//////////////////////////////////////
// simple getter and setter methods
//////////////////////////////////////

void Controller::setDataField(int pos, int value) {
    //std::cout << "set datafield " << pos << " to " << value << "\n";
    datafields[pos] = value;
}

void Controller::addDataField(unsigned char value) {
    datafields.push_back(value);
}

std::vector<unsigned char> Controller::getDatafields() {
    return datafields;
}

std::string Controller::getName() { return controllerName; }
int Controller::getComport() { return comPortNum; }
int Controller::getBaudrate() { return baudrate; }


