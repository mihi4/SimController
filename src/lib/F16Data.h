#pragma once
#include "f16Arduino.h"

class F16Data
{
public:

	// **********************
	// common sim data
	// **********************
	char powerStates = 0; // goes with PowerBits and in Flightdata.h

	// **********************
	// RAUX data
	// **********************

	// ---- FQTI ----
	unsigned short fuelFWD = 0;  // 0..4200
	unsigned short fuelAFT = 0;  // 0..00
	unsigned char fuelTotal = 0; // 0...255, in hundreds of pounds, 22 = 2200 total fuel

	// ---- HYD PRESS ----
	unsigned short hydA = 0;  // 0..4000
	unsigned short hydB = 0;  // 0..4000

	// ---- EPU FUEL ----
	unsigned short epuFuel = 0; // 0..65535, pointer position
	
	// ---- CABIN PRESSURE ALT ----
	unsigned short cabinPress = 0; // 0..65535, pointer position

	// ---- CAUTION PANEL ----
	unsigned long cautionPanelLights = 0; // 32 bits, one bit per lamp
    
	// ---- PFD ----
	char pfdLine1[25];
	char pfdLine2[25];
	char pfdLine3[25];
	char pfdLine4[25];
	char pfdLine5[25];

	// **********************
	// INSTRUMENT PANEL data
	// **********************

	unsigned char hudLights = 0; // AOA and NWS indexers

	
	// **********************
	// CENTER CONSOLE data
	// **********************

	
	// **********************
	// LAUX data
	// **********************


	// **********************
	// LCONS data
	// **********************

};

