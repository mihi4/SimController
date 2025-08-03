#pragma once

#include <iostream>
#include <Windows.h>
#include "lib/miUtils.h"

#include "lib/BMSReader.h"
#include "lib/DCSReader.h"
#include "lib/MSFSReader.h"


#define BMS 1
#define DCS 2 // reader not implemented ye
#define MSFS 3 // reader not implemented ye

bool checkParameter(int argNum);
char parseSimParameter(char* argv[]);
std::unique_ptr<DataReader> createDataReader(short selectedSim);