#pragma once

// bits for airplane power data and sim status
#define BUSBAT					1    // true if at least the battery bus is powered
#define BUSEMER					2     // true if at least the emergency bus is powered
#define BUSESSENTIAL	 		4    // true if at least the essential bus is powered
#define BUSNONESSENTIAL			8    // true if at least the non-essential bus is powered
#define MAINGENON               16
#define STBYGENON               32
#define PLANEFLYING	            64   // true, if player is attached to plane, FLYING in enum FlyStates


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

