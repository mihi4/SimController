#include "Controller.h"

Controller::Controller(std::string name, int comPortNum, long baudrate = 115200)
    : controllerName(name), baudrate(baudrate), comPortNum(comPortNum) {
    datafields = {}; // new unsigned char[datafieldCount];    
    //std::cout << "Controller constructor, " << controllerName << std::endl;
}

Controller::~Controller() {
    //std::cout << "Controller " << controllerName << " deconstructor, deleting serial port\n";   
    //delete serialPort;
}

////////////////////////////////////////
// internal helper functions
////////////////////////////////////////

void debugUpdateString(std::vector<char> updateString) {
    std::string debugString;    
    std::ostringstream oss;

    // Iteriere über die Bytes im Vektor  
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

void Controller::buildVarString(int varNum, unsigned char value, std::vector<char>& updateString) {
    // std::cout << "buildChar. valsize: " << sizeof(value) << " strSize: " << updateString.size() << std::endl;
    buildVarStringBegin(varNum, sizeof(value), updateString);
    updateString.push_back(value);
    buildVarStringEnd(updateString); 
}

void Controller::buildVarString(int varNum, unsigned short value, std::vector<char>& updateString) {
    // std::cout << "buildShort. valsize: " << sizeof(value) << " strSize: " << updateString.size() << std::endl;
    //updateString.push_back(varNum);
    //updateString.push_back(sizeof(value));
    buildVarStringBegin(varNum, sizeof(value), updateString);
    std::vector<unsigned char> splitValues = splitValue(value, sizeof(value));
    for (int i = 0; i < splitValues.size(); i++) {
        updateString.push_back(splitValues[i]);
    }
    buildVarStringEnd(updateString);
}

void Controller::buildVarString(int varNum, unsigned int value, std::vector<char>& updateString) {
    //std::cout << "buildIsnt. valsize: " << sizeof(value) << " strSize: " << updateString.size() << std::endl;
    //updateString.push_back(varNum);
    //updateString.push_back(sizeof(value));
    buildVarStringBegin(varNum, sizeof(value), updateString);
    std::vector<unsigned char> splitValues = splitValue(value, sizeof(value));
    for (int i = 0; i < splitValues.size(); i++) {
        updateString.push_back(splitValues[i]);
    }
    buildVarStringEnd(updateString);
}

void Controller::buildVarString(int varNum, int value, std::vector<char>& updateString) {
    //std::cout << "buildIsnt. valsize: " << sizeof(value) << " strSize: " << updateString.size() << std::endl;
    
    //updateString.push_back(varNum);
    //updateString.push_back(sizeof(value));
    buildVarStringBegin(varNum, sizeof(value), updateString);
    std::vector<unsigned char> splitValues = splitValue(value, sizeof(value));
    for (int i = 0; i < splitValues.size(); i++) {
        updateString.push_back(splitValues[i]);
    }
    buildVarStringEnd(updateString);
}

void Controller::buildVarString(unsigned char varNum, std::string valueString, std::vector<char>& updateString) {
    /*updateString.push_back(CMDSTART);
    updateString.push_back(CMDUPDATE);
    updateString.push_back(varNum);
    updateString.push_back(VARSTRING);*/
    buildVarStringBegin(varNum, VARSTRING, updateString);
    updateString.insert(updateString.end(), valueString.begin(), valueString.end());
    buildVarStringEnd(updateString);
}

void Controller::addVarDataToUpdateString(unsigned char varNum, std::vector<char> &updateString, F16Data* data, F16Data* prevData) {
    
    switch (varNum) {
    case SIMSTATES:
        if (data->simStates != prevData->simStates) {
            // std::cout << "powerstates, var = " << varNum << ", size: " << sizeof(data->powerStates) << ".\n";
            buildVarString(varNum, data->simStates, updateString);
        }        
        break;
    case POWERSTATES:
        if (data->powerStates != prevData->powerStates) {
            // std::cout << "powerstates, var = " << varNum << ", size: " << sizeof(data->powerStates) << ".\n";
            buildVarString(varNum, data->powerStates, updateString);
        }
        //prevData->powerStates = data->powerStates;
        break;
    case FUELFWD:
        //std::cout << "fuel prev: " << prevData->fuelFWD << " fuel now " << data->fuelFWD;
        if (data->fuelFWD != prevData->fuelFWD) {
            // std::cout << "fuelFWD, var = " << varNum << ", size: " << sizeof(data->fuelFWD) << ".\n";
            buildVarString(varNum, data->fuelFWD, updateString);
        }
    case FUELAFT:
        //std::cout << "fuel prev: " << prevData->fuelFWD << " fuel now " << data->fuelFWD;
        if (data->fuelAFT != prevData->fuelAFT) {
            // std::cout << "fuelFWD, var = " << varNum << ", size: " << sizeof(data->fuelFWD) << ".\n";
            buildVarString(varNum, data->fuelAFT, updateString);
        }
        //prevData->fuelFWD = data->fuelFWD;
    case FUELTOTAL:
        //std::cout << "fuel prev: " << prevData->fuelFWD << " fuel now " << data->fuelFWD;
        if (data->fuelTotal != prevData->fuelTotal) {            
            //std::cout << "fueltotal, var = " << varNum << ", size: " << sizeof(data->fuelTotal) << ". value: " << std::to_string(data->fuelTotal) << "\n";
            buildVarString(varNum, data->fuelTotal, updateString);
        }
        //prevData->fuelTotal = data->fuelTotal;
    case HYDA:
        //std::cout << "fuel prev: " << prevData->fuelFWD << " fuel now " << data->fuelFWD;
        if (data->hydA != prevData->hydA) {
            // std::cout << "fuelFWD, var = " << varNum << ", size: " << sizeof(data->fuelFWD) << ".\n";
            buildVarString(varNum, data->hydA, updateString);
        }
    case HYDB:
        //std::cout << "fuel prev: " << prevData->fuelFWD << " fuel now " << data->fuelFWD;
        if (data->hydB != prevData->hydB) {
            // std::cout << "fuelFWD, var = " << varNum << ", size: " << sizeof(data->fuelFWD) << ".\n";
            buildVarString(varNum, data->hydB, updateString);
        }
    case EPUFUEL:
        //std::cout << "fuel prev: " << prevData->fuelFWD << " fuel now " << data->fuelFWD;
        if (data->epuFuel != prevData->epuFuel) {
            // std::cout << "fuelFWD, var = " << varNum << ", size: " << sizeof(data->fuelFWD) << ".\n";
            buildVarString(varNum, data->epuFuel, updateString);
        }
    case CABINPRESS:
        //std::cout << "fuel prev: " << prevData->fuelFWD << " fuel now " << data->fuelFWD;
        if (data->cabinPress != prevData->cabinPress) {
            // std::cout << "fuelFWD, var = " << varNum << ", size: " << sizeof(data->fuelFWD) << ".\n";
            buildVarString(varNum, data->cabinPress, updateString);
        }
    case CAUTIONPANELLIGHTS:
        //std::cout << "fuel prev: " << prevData->fuelFWD << " fuel now " << data->fuelFWD;
        if (data->cautionPanelLights != prevData->cautionPanelLights) {
            // std::cout << "fuelFWD, var = " << varNum << ", size: " << sizeof(data->fuelFWD) << ".\n";
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
    default:
        break;
    }     
    return;
}

void Controller::updateController(F16Data* data, F16Data* prevData) {
    //std::cout << "updating " << controllerName << "\n";    
#
        std::vector<char> updateString;

        // this and lower part removed to create separate update commands for each variable. 
        // better to do it all in one update command?  

        //updateString.push_back(CMDSTART);
        //updateString.push_back(CMDUPDATE);

        for (int i = 0; i < datafields.size(); i++) {
            updateString.clear();
            addVarDataToUpdateString(datafields[i], updateString, data, prevData);            
            if (updateString.size() > 3) debugUpdateString(updateString);// serialHandler.sendDataUpdate(updateString);
        }
        //addVarDataToUpdateString(6, updateString, data, prevData);

        //updateString.push_back('>');
        //updateString.push_back('\0');
        

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

