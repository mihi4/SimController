#pragma once
#include <iostream>
#include <vector>
#include <bit>
#include <string>
#include <functional>
#include <iostream>
#include <fstream>
#include <algorithm>
#include "../lib/F16Data.h"
#include "Controller.h"

class ControllerHandler {

public:
    ControllerHandler(std::string cfgFileName);
    // Controller();
    ~ControllerHandler();

    void showControllers();
    bool setupControllers();
    void updateControllers(F16Data *data, F16Data *prevData);
    void initControllers(F16Data* data, F16Data* prevData);
    void readControllerComms();

private:
    std::vector<std::vector<std::string>> readConfig(const std::string& filename);

    std::vector<Controller> allControllers;
    std::vector<WindowsSerial> allComports;

    std::string confFileName;

    int contNumber = 0;

    // functions
    void createControllerVector();

};