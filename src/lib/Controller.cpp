#include "Controller.h"


Controller::Controller(std::string name, std::string comPort, long baudrate = 115200, char datafieldCount = 0) {
    ceSerial serialPort(comPort,baudrate, 8,'N', 1);
    controllerName = name;    
    datafields = new unsigned char[datafieldCount];       
}

Controller::Controller() {
    // close serial port    
}

Controller::~Controller() {
    // close serial port
    delete[] datafields;
    serialPort.Close();
}

void Controller::sendDataUpdate(std::string updateString) {
    std::cout << "dataUpdate: " << updateString << "\n";
}

void Controller::setDataField(int pos, int value) {
    datafields[pos] = value;
}

unsigned char* Controller::getDatafields() {
    return datafields;
}

std::string Controller::getName() { return controllerName;  }

void Controller::connect()
{

}
