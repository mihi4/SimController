#pragma once


/********************************************************
    common functions/data used in Arduino and PC code
*********************************************************/

// bits for airplane power data and sim status
#define BUSBAT					0x01    // true if at least the battery bus is powered
#define BUSEMER					0x02     // true if at least the emergency bus is powered
#define BUSESSENTIAL	 		0x04    // true if at least the essential bus is powered
#define BUSNONESSENTIAL			0x08    // true if at least the non-essential bus is powered
#define MAINGENON               0x10
#define STBYGENON               0x20
#define PLANEFLYING	            0x40   // true, if player is attached to plane, FLYING in enum FlyStates

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

/* Command Structure between PC - Arduino

common:
'<'<byte command>[command parameter]'>'

#### Data Update ####
<U<byte varnum><byte varlength><databyte><databyte>...<databyte>>

#### Reset and setup arduino again ####
<R>

#### Connect ####
<C>
returns <InameOfController;numberOfVars/bytes.[byte var1][byte var2]...[byte varLast]>

#### Check, if still Here ####
<H>  "You Here?" ;)
returns <Y>


*********** ERROR CODES from Arduino **********

<E1> 
WRONG VAR; data variable sent not found in local var list

<E2>
WRONG COMMAND; command sent from app not recognized

<E3>
Byte mismatch error, bytes sent does not match var datatype
*/

#define ER_WRONGVAR "<E1>"
#define ER_WRONGCMD "<E2>"
#define ER_BYTEMATCH "<E3>"


#define CMDUPDATE 'U'
#define CMDSTART '<'
#define CMDEND '>'
#define CMDESC 0x1B  // use ESC (dec 27) to escape value 62 ('>') and itself in byte order

/* module defines */
#define VARCHAR     1  
#define VARSHORT    2
#define VARINT      2
#define VARLONG     4   // unsigned long = 8 byte in win, 4 bytes on Arduino!!!
#define VARSTRING   255

#define MODNONE     255
#define MODSERVO     40
#define MODMM5451    13
#define MOD7219      30
#define MODDED       79

// INSTRUMENT PANEL light bit order


// variable numbers to use to configure data to get from F16Data.h 
// let's see if 255 values are enough ;-) (light- and statusbits are stored in 1- or 2-byte vars

#define POWERSTATES         1

// **********************
// RAUX data
// **********************

// ---- FQTI ----
#define FUELFWD             10
#define FUELAFT             11
#define FUELTOTAL           12

// ---- HYD PRESS ----
#define HYDA                13
#define HYDB                14

// ---- EPU FUEL ----
#define EPUFUEL             15

// ---- CABIN PRESSURE ALT ----
#define CABINPRESS          16

// ---- CAUTION PANEL ----
#define CAUTIONPANELLIGHTS  17

// ---- PFD ----
#define PFDLINE1            18
#define PFDLINE2            19
#define PFDLINE3            20
#define PFDLINE4            21
#define PFDLINE5            22

// REMEMBER TO LEAVE OUT 60 and 62 since those are the numbers for < and >


#define VARA	65
#define VARB	66
#define VARC	67
#define VARD	68


