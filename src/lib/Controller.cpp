#include "Controller.h"


Controller::Controller(std::string name, std::string comPort, long baudrate, unsigned short dataVars[]) {
    ceSerial serialPort(comPort,baudrate, 8,'N', 1);
    controllerName = name;
    int varCount = sizeof(dataVars);
    datafields = new unsigned short[varCount];
    for (int i = 0; i < varCount; i++) {
        datafields[i] = dataVars[i];
    }
    
}

Controller::~Controller() {
    // close serial port
    delete[] datafields;
    serialPort.Close();
}

std::string Controller::getName() { return controllerName;  }