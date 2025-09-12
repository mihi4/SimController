#pragma once
#include <iostream>
#include <vector>
#include <bit>
#include <string>
#include <functional>
#include <iostream>
#include <fstream>
#include "../lib/F16Data.h"

#include "Controller.h"

#define CONFIGFILE "simcontrol.conf"

class ControllerHandler {

public:
    ControllerHandler(std::string cfgFileName);
    // Controller();
    ~ControllerHandler();

    void showControllers();
    void setupControllers();
    void updateControllers(F16Data *data, F16Data *prevData);
    void initControllers(F16Data* data, F16Data* prevData);
    void readControllerComms();

private:
    std::vector<std::vector<std::string>> readConfig(const std::string& filename);

    std::vector<Controller> allControllers;
    std::vector<WindowsSerial> allComports;

    int contNumber = 0;

    // functions
    void createControllerVector();

};