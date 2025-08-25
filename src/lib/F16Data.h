#pragma once
#include "../../Arduino/SimController/f16common.h" 

class F16Data
{
public:
	int isSameAs(const F16Data& d) const {
		return
			simStates == d.simStates &&
			powerStates == d.powerStates &&
			relayStates == d.relayStates &&
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
			oilPressure == d.oilPressure &&
			nozzlePos == d.nozzlePos &&
			rpm == d.rpm &&
			ftit == d.ftit &&
			fuelFlow == d.fuelFlow &&
			dedLine1 == d.dedLine1 &&
			dedLine2 == d.dedLine2 &&
			dedLine3 == d.dedLine3 &&
			dedLine4 == d.dedLine4 &&
			dedLine5 == d.dedLine5 &&
			kias == d.kias &&
			mach == d.mach &&
			altPointer == d.altPointer &&
			altitude == d.altitude &&
			altCalibration == d.altCalibration &&
			vvi == d.vvi &&
			aoa == d.aoa &&
			hsiCourseDeviation == d.hsiCourseDeviation &&
			hsiDesiredCourse == d.hsiDesiredCourse &&
			hsiDistanceToBeacon == d.hsiDistanceToBeacon &&
			hsiBearingToBeacon == d.hsiBearingToBeacon &&
			hsiCurrentHeading == d.hsiCurrentHeading &&
			hsiDesiredHeading == d.hsiDesiredHeading &&
			hsiDeviationLimit == d.hsiDeviationLimit &&
			hsiHalfDeviationLimit == d.hsiHalfDeviationLimit &&
			hsiLocalizerCourse == d.hsiLocalizerCourse &&
			hsiAirbaseX == d.hsiAirbaseX &&
			hsiAirbaseY == d.hsiAirbaseY &&
			instrumentBits == d.instrumentBits &&
			instPanelLights == d.instPanelLights &&
			leftConsLights == d.leftConsLights &&
			uhfChannel == d.uhfChannel &&
			uhfFrequency == d.uhfFrequency &&
			ecmLights == d.ecmLights &&
			iffDisplay == d.iffDisplay &&
			auxDisplay == d.auxDisplay &&
			trimPitch == d.trimPitch &&
			trimRoll == d.trimRoll &&
			cmdsBits == d.cmdsBits &&
			cmds01Str == d.cmds01Str &&
			cmds02Str == d.cmds02Str &&
			cmdsChaffStr == d.cmdsChaffStr &&
			cmdsFlareStr == d.cmdsFlareStr &&
			adiPitch == d.adiPitch &&
			adiRoll == d.adiRoll &&
			adiIlsHorPos == d.adiIlsHorPos &&
			adiIlsVerPos == d.adiIlsVerPos &&
			adiSideslip == d.adiSideslip &&
			simVersion == d.simVersion
			;
	}

	// **********************
	// common sim data
	// **********************
	unsigned char simStates = 0; // simbits start with SIM
	unsigned char powerStates = 0; // goes with PowerBits and hsibits in Flightdata.h
	unsigned char relayStates = 0; // relay bits start RLY

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
	unsigned short epuFuel = 0; // 0..65535, pointer position, raw value 0-100 percent, * 100
	
	// ---- CABIN PRESSURE ALT ----
	unsigned short cabinPress = 0; // 0..65535, pointer position, raw value 0-50000;

	// ---- CAUTION PANEL ----
	unsigned int cautionPanelLights = 0; // full 32 bits, bits start with CP
    
	// ---- PFD ----
	std::string pfdLine1 = "                         ";
	std::string pfdLine2 = "                         ";
	std::string pfdLine3 = "                         ";
	std::string pfdLine4 = "                         ";
	std::string pfdLine5 = "                         ";

	/// DON'T FORGET TO ADD NEW datafields at the top for the comparator

	// **********************
	// Instrument Panel data
	// **********************

	unsigned short oilPressure = 0; // 0-10000 (0-100 * 100)
	unsigned short nozzlePos = 0; // 0-10000 (0-100 * 100)
	unsigned short rpm = 0; // 0-10300 (0-103 * 100)
	unsigned short ftit = 0; //0-65535, scale is not linear!

	unsigned int fuelFlow = 0; // 0 - 99999; let's see later, if we need a string instead
	
	std::string dedLine1 = "                         ";
	std::string dedLine2 = "                         ";
	std::string dedLine3 = "                         ";
	std::string dedLine4 = "                         ";
	std::string dedLine5 = "                         ";

	// **********************
	// Main Instrument Panel 
	// **********************

	unsigned short kias = 0; // 0-65535, scale not linear!
	unsigned short mach = 0; // see kias
	unsigned short altPointer = 0; // pointer value, 0 - 1000, one revolution of the pointer
	int altitude = 0;
	std::string altCalibration = "    "; // e.g. 2992, 1015
	std::string altThousands = "   ";
	short vvi = 0; // original zDot is float value, ft/sec -6000 to 6000
	short aoa = 0;	// original alpha is float value degrees, multiply with 100(?)

	// all hsi values are floats, multiply at least by 100
	short hsiCourseDeviation = 0;
	short hsiDesiredCourse = 0;
	short hsiDistanceToBeacon = 0;
	short hsiBearingToBeacon = 0;
	unsigned short hsiCurrentHeading = 0;
	unsigned short hsiDesiredHeading = 0;
	short hsiDeviationLimit = 0;
	short hsiHalfDeviationLimit = 0;
	unsigned short hsiLocalizerCourse = 0;
	int hsiAirbaseX = 0;
	int hsiAirbaseY = 0;

	unsigned short instrumentBits = 0;  // bits start with INST

	unsigned int instPanelLights = 0;  // lightbits of InstPanel (MISC, TWP, Indexers, Eyebrows)

	// **********************
	// Left side data
	// **********************

	unsigned int leftConsLights = 0;  // all lightbits on left Console, incl. LAUX

	unsigned char uhfChannel = 0; // 0-2x?
	std::string uhfFrequency = "      "; // frequency can also be a string to display

	unsigned int ecmLights = 0; // bits begin with ECM (ECM1 ECMEMPTY, eCMFRM, ECMSPL ,...)

	std::string iffDisplay = "    ";  // usually only digits
	std::string auxDisplay = "    "; //  tacan can be X or AA

	short trimPitch = 0; // -0.5 to +0.5, multiply with 100
	short trimRoll = 0; // -0.5 to +0.5, multiply with 100

	unsigned char cmdsBits = 0; // bits strat with CMDS

	std::string cmds01Str = "    ";
	std::string cmds02Str = "    ";
	std::string cmdsChaffStr = "    ";
	std::string cmdsFlareStr = "    ";
	
	int adiPitch = 0;
	int adiRoll = 0;
	int adiIlsHorPos = 0;
	int adiIlsVerPos = 0;
	int adiSideslip = 0;


	std::string simVersion = "";
};

