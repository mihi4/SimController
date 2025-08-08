#include "Controller.h"


Controller::Controller(std::string name, std::string comPort, long baudrate = 115200, std::vector<unsigned char> datafieldsIn = { 0 }) {
    ceSerial serialPort(comPort,baudrate, 8,'N', 1);
    controllerName = name;    
    datafields = datafieldsIn; // new unsigned char[datafieldCount];
    int varCount = datafields.size();
    std::cout << "my contructor, datafieldcount:--" << varCount << "--\n";
}

Controller::Controller() {
    // close serial port  
    std::cout << "standard contructor\n";
}

Controller::~Controller() {
    // close serial port
    //delete[] datafields;
    serialPort.Close();
}

void debugUpdateString(std::vector<char> updateString) {
    int size = updateString.size();
    std::string debugString = "";
    for (int i = 0; i < size; i++) {
        if (updateString[i] < 32 || updateString[i]>127) {                        
            if (updateString[i] == 0) break;
            debugString += ".";
            debugString += std::to_string(updateString[i]);
            debugString += "_";
        }
        else {
            char buf = updateString.at(i);
            debugString += buf;
        }
    }
    std::cout << "debugString: " << debugString << "\n";
}

void Controller::sendDataUpdate(std::vector<char> updateString) {
    debugUpdateString(updateString);
    
}

void Controller::setDataField(int pos, int value) {
    //std::cout << "set datafield " << pos << " to " << value << "\n";
    datafields[pos] = value;
}

std::vector<unsigned char> Controller::getDatafields() {
    return datafields;
}

std::string Controller::getName() { return controllerName;  }

bool Controller::connect()
{
    std::cout << "connecting to " << controllerName << std::endl;
    return false;
}

void Controller::addByteToUpdateString(std::vector<char>* updateString, char byte) {
   // FIXXXME not sure, if needed

}

std::vector<unsigned char> Controller::splitValue(int value, int size) {
    std::vector<unsigned char> bytes;
    for (size_t i = 0; i < size; ++i) {
        // Maskieren und Verschieben, um das i-te Byte zu erhalten
        unsigned char byte = (value>> (8 * i)) & 0xFF;
        bytes.push_back(byte);
    }
    return bytes;
}

void Controller::buildVarString(int varNum, unsigned char value, std::vector<char>& updateString) {
    // std::cout << "buildChar. valsize: " << sizeof(value) << " strSize: " << updateString.size() << std::endl;
    updateString.push_back(CMDSTART);
    updateString.push_back(CMDUPDATE);

    updateString.push_back(varNum);
    updateString.push_back(sizeof(value));
    updateString.push_back(value);

    updateString.push_back(CMDEND);
    updateString.push_back('\0');

}
void Controller::buildVarString(int varNum, unsigned short value, std::vector<char>& updateString) {
    // std::cout << "buildShort. valsize: " << sizeof(value) << " strSize: " << updateString.size() << std::endl;
    updateString.push_back(varNum);
    updateString.push_back(sizeof(value));
    std::vector<unsigned char> splitValues = splitValue(value, sizeof(value));
    for (int i = 0; i < splitValues.size(); i++) {
        updateString.push_back(splitValues[i]);
    }
}
void Controller::buildVarString(int varNum, unsigned int value, std::vector<char>& updateString) {
    std::cout << "buildIsnt. valsize: " << sizeof(value) << " strSize: " << updateString.size() << std::endl;
    updateString.push_back(varNum);
    updateString.push_back(sizeof(value));    
}

void Controller::addVarDataToUpdateString(int varNum, std::vector<char> &updateString, F16Data* data, F16Data* prevData) {
    
    //std::cout << "comparing var " << varNum << ". ";
    
    switch (varNum) {
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
        //prevData->fuelFWD = data->fuelFWD;
    case FUELTOTAL:
        //std::cout << "fuel prev: " << prevData->fuelFWD << " fuel now " << data->fuelFWD;
        if (data->fuelTotal != prevData->fuelTotal) {            
            //std::cout << "fueltotal, var = " << varNum << ", size: " << sizeof(data->fuelTotal) << ". value: " << std::to_string(data->fuelTotal) << "\n";
            buildVarString(varNum, data->fuelTotal, updateString);
        }
        //prevData->fuelTotal = data->fuelTotal;
    default:
        break;
    }     
    return;
}

void Controller::updateController(F16Data* data, F16Data* prevData) {
    //std::cout << "updating " << controllerName << "\n";    
        unsigned short fieldCount = datafields.size();
#
        std::vector<char> updateString;

        // this and lower part removed to create separate update commands for each variable. 
        // better to do it all in one update command?  FIXXXME

        //updateString.push_back(CMDSTART);
        //updateString.push_back(CMDUPDATE);

        for (int i = 0; i < fieldCount; i++) {
            addVarDataToUpdateString(datafields[i], updateString, data, prevData);
            if (updateString.size() > 2) sendDataUpdate(updateString);
        }
        //addVarDataToUpdateString(6, updateString, data, prevData);

        //updateString.push_back('>');
        //updateString.push_back('\0');

}
