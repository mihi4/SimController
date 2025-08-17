#pragma once
#include <iostream>
#include <vector>
#include <bit>
#include <string>
#include <functional>
#include "../lib/F16Data.h"
//#include "SerialPortHandler.h"
//#include "../lib/ArduSerial.h"
#include "Controller.h"

class ControllerHandler {

public:
    ControllerHandler(std::string cfgFileName);
    // Controller();
    ~ControllerHandler();

    void showControllers();
    void setupControllers();
    void updateControllers(F16Data *data, F16Data *prevData);
    void readControllerComms();

private:
    std::vector<Controller> allControllers;
    std::vector<WindowsSerial> allComports;

    int contNumber = 0;

    // functions
    void createControllerVector();

};