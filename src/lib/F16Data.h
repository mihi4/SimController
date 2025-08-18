#pragma once
#include "../../Arduino/SimController/f16common.h"

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
			pfdLine5 == d.pfdLine5 
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
	unsigned short fuelAFT = 0;  // 0..4200
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


	/// DON'T FORGET TO ADD NEW datafields at the top for the comparator

	
	// **********************
	// Instrument Panel data
	// **********************
/*
#define OILPRESS            25
#define NOZZLEPOS           26
#define RPM                 27
#define FTIT                28

// **********************
// Right Instrument Panel 
// **********************

#define FUELFLOW            29

	std::string dedLine1 = "                    ";
	std::string dedLine2 = "                    ";
	std::string dedLine3 = "                    ";
	std::string dedLine4 = "                    ";
	std::string dedLine5 = "                    ";

// **********************
// Main Instrument Panel 
// **********************



*/

	// **********************
	// LAUX data
	// **********************


	// **********************
	// LCONS data
	// **********************

};

