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

#define CPSEAT      0x00000001
#define CPNWS       0x00000002 
#define CPANTISKID  0x00000004
#define CPHOOK      0x00000008
#define CPAVIONICS  0x00000010
#define CPEQUIPHOT  0x00000020
#define CPRADARALT  0x00000040
#define CPIFF       0x00000080
#define CPENGINE    0x00000100
#define CPSEC       0x00000200
#define CPFUELOIL   0x00000400
#define CPINLET     0x00000800
#define CPFLCS      0x00001000
#define CPELECSYS   0x00002000
#define CPPROBEHEAT 0x00004000
#define CPCADC      0x00008000
#define CPAFTFUEL   0x00010000
#define CPFWDFUEL   0x00020000
#define CPATF       0x00040000
#define CPSTORESCFG 0x00080000
#define CPLINE1     0x00100000
#define CPBUC       0x00200000
#define CPEEC       0x00400000
#define CPOVERHEAT  0x00800000
#define CPLINE2     0x01000000
#define CPLINE3     0x02000000
#define CPLINE4     0x04000000
#define CPNUCLEAR   0x08000000
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

