#pragma once

#include <iostream>
#include <vector>
#include <SFML/Graphics.hpp>

#define NOMINMAX    
#include <Windows.h>


#include "lib/miUtils.h"

#include "classes/BMSReader.h"
#include "classes/DCSReader.h"
#include "classes/MSFSReader.h"

#include "classes/ControllerHandler.h"

#define BMS 1
#define DCS 2 // reader not implemented yet
#define MSFS 3 // reader not implemented yet

#define CONFIGFILE "simcontrol.conf"

bool checkParameter(int argNum);
char parseSimParameter(char* argv[]);
std::unique_ptr<DataReader> createDataReader(short selectedSim);