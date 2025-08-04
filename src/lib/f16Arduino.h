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

#define CPSEAT      0x01
#define CPNWS       0x02 
#define CPANTISKID  0x04
#define CPHOOK      0x08
#define CPAVIONICS  0x10
#define CPEQUIPHOT  0x20
#define CPRADARALT  0x40
#define CPIFF       0x80
#define CPENGINE    0x100
#define CPSEC       0x200
#define CPFUELOIL   0x400
#define CPINLET     0x800
#define CPFLCS      0x1000
#define CPELECSYS   0x2000
#define CPPROBEHEAT 0x4000
#define CPCADC      0x8000
#define CPAFTFUEL   0x10000
#define CPFWDFUEL   0x20000
#define CPATF       0x40000
#define CPSTORESCFG 0x80000
#define CPLINE1     0x100000
#define CPBUC       0x200000
#define CPEEC       0x400000
#define CPOVERHEAT  0x800000
#define CPLINE2     0x1000000
#define CPLINE3     0x2000000
#define CPLINE4     0x4000000
#define CPNUCLEAR   0x8000000
#define CPLINE5     0x10000000
#define CPLINE6     0x20000000
#define CPCABIN     0x40000000
#define CPOXYLOW    0x80000000

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

