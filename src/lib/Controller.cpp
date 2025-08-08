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

void Controller::connect()
{

}

void Controller::updateController() {
    std::cout << "updating " << controllerName << "\n";
    unsigned short fieldCount = datafields.size();

    for (int i = 0; i < fieldCount; i++) {

    }

}
