#pragma once
#include <cmath>

// bits for airplane power data and sim status
#define BUSBAT					1    // true if at least the battery bus is powered
#define BUSEMER					2     // true if at least the emergency bus is powered
#define BUSESSENTIAL	 		4    // true if at least the essential bus is powered
#define BUSNONESSENTIAL			8    // true if at least the non-essential bus is powered
#define MAINGENON               16
#define STBYGENON               32
#define PLANEFLYING	            64   // true, if player is attached to plane, FLYING in enum FlyStates

// CAUTION PANEL lights bit order
// different order because of soldering on hardware
// set those to "original" values - topleft to bottomright - if necessary

#define CPSEAT      pow(2,0)
#define CPNWS       pow(2,1)  
#define CPANTISKID  pow(2,2)
#define CPHOOK      pow(2,3)
#define CPAVIONICS  pow(2,4)
#define CPEQUIPHOT  pow(2,5)
#define CPRADARALT  pow(2,6)
#define CPIFF       pow(2,7)
#define CPNUCLEAR   pow(2,8)
#define CPENGINE    pow(2,9)
#define CPSEC       pow(2,10)
#define CPFUELOIL   pow(2,11)
#define CPINLET     pow(2,12)
#define CPOVERHEAT  pow(2,13)
#define CPFLCS      pow(2,14)
#define CPELECSYS   pow(2,15)
#define CPPROBEHEAT pow(2,16)
#define CPCADC      pow(2,17)
#define CPAFTFUEL   pow(2,18)
#define CPFWDFUEL   pow(2,19)
#define CPATF       pow(2,20)
#define CPSTORESCFG pow(2,21)
#define CPLINE1     pow(2,22)
#define CPBUC       pow(2,23)
#define CPEEC       pow(2,24)
#define CPOVERHEAT  pow(2,25)
#define CPLINE2     pow(2,26)
#define CPLINE3     pow(2,27)
#define CPLINE4     pow(2,28)
#define CPLINE5     pow(2,29)
#define CPLINE6     pow(2,30)
#define CPCABIN     pow(2,31)




// variable numbers to use to configure data to get from F16Data.h
#define POWERSTATES         1

// **********************
// RAUX data
// **********************

// ---- FQTI ----
#define FUELFWD             10
#define FUELAFT             20
#define FUELTOTAL           30

// ---- HYD PRESS ----
#define HYDA                40
#define HYDB                50

// ---- EPU FUEL ----
#define EPUFUEL             60

// ---- CABIN PRESSURE ALT ----
#define CABINPRESS          70

// ---- CAUTION PANEL ----
#define CAUTIONPANELLIGHTS  80

// ---- PFD ----
#define PFDLINE1            100
#define PFDLINE2            101
#define PFDLINE3            102
#define PFDLINE4            103
#define PFDLINE5            104

