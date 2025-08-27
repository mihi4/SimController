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

    // Iteriere �ber die Bytes im Vektor  
    for (char byte : updateString)  {
        
        oss << std::hex << std::setw(2) << std::setfill('0') << (static_cast<unsigned int>(byte) & 0xFF) << " ";
    }
   
    debugString = oss.str();

    // Optional: Entferne das letzte Leerzeichen  
    if (!debugString.empty()) {
        debugString.pop_back();
    }   
    // Ausgabe  
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
    for (int i = 0; i < splitValues.size(); i++) {
        updateString.push_back(splitValues[i]);
    }
    buildVarStringEnd(updateString);
}

// create the updateString to the Arduino from a var with an UnsignedInt value
void Controller::buildVarString(int varNum, unsigned int value, std::vector<char>& updateString) {    
    buildVarStringBegin(varNum, sizeof(value), updateString);
    std::vector<unsigned char> splitValues = splitValue(value, sizeof(value));
    for (int i = 0; i < splitValues.size(); i++) {
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
    case FUELAFT:
        if (data->fuelAFT != prevData->fuelAFT) {
            buildVarString(varNum, data->fuelAFT, updateString);
        }
    case FUELTOTAL:
        if (data->fuelTotal != prevData->fuelTotal) {            
            buildVarString(varNum, data->fuelTotal, updateString);
        }
    case HYDA:
        if (data->hydA != prevData->hydA) {
            buildVarString(varNum, data->hydA, updateString);
        }
    case HYDB:        
        if (data->hydB != prevData->hydB) {        
            buildVarString(varNum, data->hydB, updateString);
        }
    case EPUFUEL:        
        if (data->epuFuel != prevData->epuFuel) {        
            buildVarString(varNum, (unsigned short) util.map(data->epuFuel, 0, 10000, 0, 65535), updateString);
        }
    case CABINPRESS:        
        if (data->cabinPress != prevData->cabinPress) {        
            buildVarString(varNum, (unsigned short) util.map(data->cabinPress, 0, 50000, 0, 65535), updateString);
        }
    case CAUTIONPANELLIGHTS:        
        if (data->cautionPanelLights != prevData->cautionPanelLights) {        
            buildVarString(varNum, data->cautionPanelLights, updateString);
        }
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
    // CENTER CONSOLE INSTRUMENTS
        break;
    case INSTRUMENTBITS:
        if (data->instrumentBits != prevData->instrumentBits) {
            buildVarString(varNum, data->instrumentBits, updateString);
        }
        break;
    // LEFT CONSOLE


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
            if (updateString.size() > 3) debugUpdateString(updateString);
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

