#include "Controller.h"


Controller::Controller(std::string name, std::string comPort, long baudrate, unsigned short dataVars[]) {
    ceSerial serialPort(comPort,baudrate, 8,'N', 1);
}

Controller::~Controller() {
    // close serial port
    serialPort.Close();
}