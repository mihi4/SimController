#pragma once
#include "lib/f16Arduino.h"

class F16Data
{
public:

	// **********************
	// common sim data
	// **********************
	char powerStates; // goes with PowerBits and in Flightdata.h

	// **********************
	// RAUX data
	// **********************

	// ---- FQTI ----
	unsigned short fuelFWD;  // 0..65535, pointer position
	unsigned short fuelAFT;  // 0..65535, pointer position
	unsigned char fuelTotal; // 0...255, in hundreds of pounds, 22 = 2200 total fuel

	// ---- HYD PRESS ----
	unsigned short hydA;  // 0..65535, pointer position
	unsigned short hydB;  // 0..65535, pointer position

	// ---- EPU FUEL ----
	unsigned short epuFuel; // 0..65535, pointer position
	
	// ---- CABIN PRESSURE ALT ----
	unsigned short cabinPress; // 0..65535, pointer position

	// ---- CAUTION PANEL ----
	unsigned long cautionPanelLights; // 32 bits, one bit per lamp
    
	// ---- PFD ----
	char pfdLine1[25];
	char pfdLine2[25];
	char pfdLine3[25];
	char pfdLine4[25];
	char pfdLine5[25];

	// **********************
	// INSTRUMENT PANEL data
	// **********************

	
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

