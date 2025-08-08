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

void Controller::sendDataUpdate(std::vector<char> updateString) {
    const char* buf = updateString.data();
    std::cout << "dataUpdate: " << buf << "\n";
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

void Controller::addVarDataToUpdateString(int varNum, std::vector<char> &updateString, F16Data* data, F16Data* prevData) {
    
    std::cout << "adding " << varNum << "; ";
    updateString.push_back(varNum);
    switch (varNum) {
    case POWERSTATES:
        std::cout << "power " << data->powerStates;
        if (data->powerStates != prevData->powerStates) {
            updateString.push_back(sizeof(prevData->powerStates));  
            updateString.push_back(data->powerStates);
        }
        break;
    case FUELFWD:
        if (data->fuelFWD != prevData->fuelFWD) {
            updateString.push_back(sizeof(prevData->powerStates));
            updateString.push_back(1);
            updateString.push_back(2);
        }
    default:
        break;
    }     
    return;
}

void testme(std::vector<char> &me) {    
    const char* buf = me.data();
    std::cout << "testme:" << buf << std::endl;
    me.push_back('m'); me.push_back('E');
}

void Controller::updateController(F16Data* data, F16Data* prevData) {
    //std::cout << "updating " << controllerName << "\n";
    unsigned short fieldCount = datafields.size();
#
    std::vector<char> updateString;

    updateString.push_back(CMDSTART);
    updateString.push_back(CMDUPDATE);
  
    for (int i = 0; i < fieldCount; i++) {
        addVarDataToUpdateString(datafields[i], updateString, data, prevData);
    } 
    addVarDataToUpdateString(6, updateString, data, prevData);

    updateString.push_back('>');
    updateString.push_back('\0');

    const char* buf = updateString.data();
    std::cout << "Command:" << buf << std::endl;

}
