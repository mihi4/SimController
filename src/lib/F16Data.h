#pragma once
#include "f16common.h"

class F16Data
{
public:
	int isSameAs(const F16Data& d) const {
		return
			powerStates == d.powerStates &&
			fuelFWD == d.fuelFWD &&
			fuelAFT == d.fuelAFT &&
			fuelTotal == d.fuelTotal &&
			hydA == d.hydA &&
			hydB == d.hydB &&
			epuFuel == d.epuFuel &&
			cabinPress == d.cabinPress &&
			cautionPanelLights == d.cautionPanelLights &&
			pfdLine1 == d.pfdLine1 &&
			pfdLine2 == d.pfdLine2 &&
			pfdLine3 == d.pfdLine3 &&
			pfdLine4 == d.pfdLine4 &&
			pfdLine5 == d.pfdLine5 &&
			hudLights == d.hudLights

			;
	}

	// **********************
	// common sim data
	// **********************
	unsigned char powerStates = 0; // goes with PowerBits and hsibits in Flightdata.h

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
	unsigned int cautionPanelLights = 0; // 32 bits, one bit per lamp
    
	// ---- PFD ----
	std::string pfdLine1 = "                    ";
	std::string pfdLine2 = "                    ";
	std::string pfdLine3 = "                    ";
	std::string pfdLine4 = "                    ";
	std::string pfdLine5 = "                    ";

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

