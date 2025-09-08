#pragma once


/********************************************************
    common functions/data used in Arduino and PC code
*********************************************************/

/* Command Structure between PC - Arduino

common:
'<'<byte command>[command parameter]'>'

#### Data Update ####
<U<byte varnum><byte vartype><databyte>[<databyte>...<databyte>]>

#### Reset and setup arduino again ####
<R>

#### Connect ####
<C>
returns <InameOfController;numberOfVars/bytes[byte var1][byte var2]...[byte varLast]>

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

#define DEFAULTBAUDRATE 115200

#define ER_WRONGVAR "<E1>"
#define ER_WRONGCMD "<E2>"
#define ER_BYTEMATCH "<E3>"


#define CMDUPDATE 'U'
#define CMDSTART '<'
#define CMDEND '>'
#define CMDESC 0x1B  // use ESC (dec 27) to escape value 62 ('>') and itself in byte order

/* vartype defines */
#define VARCHAR     1  
#define VARSHORT    2
#define VARINT      2
#define VARLONG     4   // unsigned long = 8 byte in win, 4 bytes on Arduino!!!
#define VARSTRING   255

// defines for the hardware modules, keep same as in BMSAIT
#define MODNONE       255
#define MODSERVO      40
#define MODMM5451     13
#define MODMAX7219    30
#define MODTM1637     30
#define MODDED        79
#define MODX27        51
#define MODX27VID     52
#define MODFFI        72      

#define FLOATMULT    1000  // amount to multiply float value with to get smoother data

#define BLINKBITNUM  13
#define BLINKSLOW    600  // intervalls in milliseconds for blinking warning lights
#define BLINKFAST    250

// variable numbers to use to configure data to get from F16Data.h 
// let's see if 255 values are enough ;-) (light- and statusbits are stored in 1- or 2-byte vars
#define SIMSTATES           1
#define SIMPLANEFLYING  0x01   // true, if player is attached to plane, FLYING in enum FlyStates
#define SIMTESTLIGHTS   0x02   // true, if all lights are on (MAL&IND button pressed. Let's see, if we can create this

#define POWERSTATES         2
// bits for airplane power data and sim status
#define BUSBAT			0x01    // true if at least the battery bus is powered
#define BUSEMER			0x02     // true if at least the emergency bus is powered
#define BUSESSENTIAL	0x04    // true if at least the essential bus is powered
#define BUSNONESSENTIAL	0x08    // true if at least the non-essential bus is powered
#define MAINGENON       0x10
#define STBYGENON       0x20

#define RELAYSTATES         3
// bits for relays
#define RLYWEIGHTONWHEELS  0x01   // true, if WOW
#define RLYAUTOPILOT       0x02    
#define RLYPARKINGBRAKE    0x04
#define RLYJFSTARTER       0x08
#define RLYFLTCTLBIT       0x10
#define RLYSPEEDBRAKE      0x20


//#define MAXPOWER

////////////////////////////// RIGHT AUX //////////////////////////////////

// **********************
// Engine data
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


// **********************
// PFD and Caution Panel
// **********************

// ---- CAUTION PANEL ----
#define CAUTIONPANELLIGHTS  17
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


// ---- PFD ----
#define PFDLINE1            18
#define PFDLINE2            19
#define PFDLINE3            20
#define PFDLINE4            21
#define PFDLINE5            22

////////////////////////////// CENTER CONSOLE //////////////////////////////////

// **********************
// EngineCluster data
// **********************

#define OILPRESS            25
#define NOZZLEPOS           26
#define RPM                 27
#define FTIT                28

// **********************
// Right Instrument Panel 
// **********************

#define FUELFLOW            29

#define DEDLINE1            30
#define DEDLINE2            31
#define DEDLINE3            32
#define DEDLINE4            33
#define DEDLINE5            34

// **********************
// Main Instrument Panel 
// **********************

#define KIAS                38
#define MACH                39
#define ALTTHOUSANDS        40
#define ALTPOINTER          41   
#define ALTITUDE            42
#define ALTCAL              43
#define VVIVAL              44
#define AOAVAL              45

// HSI - not sure, if I will ever use those :-D
#define HSICOURSEDEVIATION     46 // HSI_VAL_CRS_DEVIATION
#define HSIDESIREDCOURSE       47 // HSI_VAL_DESIRED_CRS
#define HSIDISTANCETOBEACON    48 // HSI_VAL_DISTANCE_TO_BEACON
#define HSIBEARINGTOBEACON     49 // HSI_VAL_BEARING_TO_BEACON
#define HSICURRENTHEADING      50 // HSI_VAL_CURRENT_HEADING
#define HSIDESIREDHEADING      51 // HSI_VAL_DESIRED_HEADING
#define HSIDEVIATIONLIMIT      52 // HSI_VAL_DEV_LIMIT
#define HSIHALFDEVIATIONLIMIT  53 // HSI_VAL_HALF_DEV_LIMIT
#define HSILOCALIZERCOURSE     54 // HSI_VAL_LOCALIZER_CRS
#define HSIAIRBASEX            55 // HSI_VAL_AIRBASE_X
#define HSIAIRBASEY            56 // HSI_VAL_AIRBASE_Y

#define INSTRUMENTBITS         57
// bits in INSTBITS
#define INSTPNEU            0x01
#define INSTVVIFLAG         0x02
#define INSTAOAFLAG         0x04
#define INSTBUPADIOFFFLAG   0x08
#define INSTADIOFF          0x10    // ADI OFF Flag
#define INSTADIAUX          0x20    // ADI AUX Flag
#define INSTADIGS           0x40    // ADI GS FLAG
#define INSTADILOC          0x80    // ADI LOC FLAG
#define INSTHSIOFF          0x100
#define INSTHSITO           0x200
#define INSTHSIFROM         0x400
#define INSTHSIILSWARN      0x800
#define INSTHSICRSWARN      0x1000
#define INSTHSIINIT         0x2000
//#define               0x4000
//#define               0x8000

// **********************
// Left Instrument Panel
// **********************

// Nothing here to output, data for 12V relays (magnetic switches) is in separate var, all lights for MM5451 are in InstPanellights

// **********************
// All InstPanel lights 
// **********************

#define INSTPANELLIGHTS          58
// 26 lightbits, one mm5451
// Left Eyebrows
#define EBMASTERC       0x01
#define EBTFFAIL        0x02
// TWP
#define TWPHANDOFF      0x04
#define TWPLAUNCH       0x08
#define TWPPRIMODE      0x10
#define TWPUNKNOWN      0x20
#define TWPSYSTEST      0x40
#define TWPTGTSEP       0x80
// MISC
#define ECMON           0x100
#define MODEACTIVE      0x200
#define MODESTBY        0x400
// HUD indexer
#define IDXAOAABOVE     0x800
#define IDXAOAON        0x1000
#define IDXAOABELOW     0x2000
#define IDXRDY          0x4000
#define IDXARNWS        0x8000
#define IDXDISC         0x10000
// right eyebrows
#define EBENGFIRE       0x20000
#define EBENGINE        0x40000
#define EBHYDOILPRESS   0x80000
#define EBFLCS          0x100000
#define EBDBUON         0x200000
#define EBTOLDGCFG      0x400000
#define EBCANOPY        0x800000
#define EBOXYLOW        0x1000000  // not sure, if this is in all block50/52s
// CC
#define MARKERBEACON    0x2000000


// REMEMBER TO LEAVE OUT 60 and 62 since those are the numbers for < and >

////////////////////////////// LEFT CONSOLE //////////////////////////////////

#define LEFTCONSLIGHTS          65
// 28 lightbits, one MM5451
#define GEARLIGHT   0x01
#define WNOSE       0x02
#define WLEFT       0x04
#define WRIGHT      0x08
#define TWASEARCH   0x10
#define TWAACT      0x20
#define TWALOWALT   0x40
#define TWASYSPWT   0x08
#define JFSRUN      0x10
#define EPURUN      0x20
#define EPUHYD      0x40
#define EPUAIR      0x80
#define ELPMG       0x100
#define ELMAINGEN   0x200
#define ELSTDBYGEN  0x400
#define ELEPUGEN    0x800
#define ELEPUPMG    0x1000
#define ELBATTFAIL  0x2000
#define ELTOFLCS    0x4000
#define ELFLCSRLY   0x8000
#define AVTRRUN     0x10000
#define AVTRCTVS    0x20000
#define FLTCTLRUN   0x40000
#define FLTCTLFAIL  0x80000
#define TESTA       0x100000   
#define TESTB       0x200000
#define TESTC       0x400000
#define TESTD       0x800000

// UHF data
#define UHFCHAN         70
#define UHFFREQ         71

// ECM data
#define ECMLIGHTS       72
#define ECM1S       0x01    
#define ECM1A       0x02    
#define ECM1F       0x04    
#define ECM1T       0x08    
#define ECM2S       0x10    
#define ECM2A       0x20    
#define ECM2F       0x40    
#define ECM2T       0x80    
#define ECM3S       0x100    
#define ECM3A       0x200    
#define ECM3F       0x400    
#define ECM3T       0x800    
#define ECM4S       0x1000    
#define ECM4A       0x2000    
#define ECM4F       0x4000    
#define ECM4T       0x8000    
#define ECM5S       0x10000    
#define ECM5A       0x20000    
#define ECM5F       0x40000    
#define ECM5T       0x80000    
#define ECMEMPTYS   0x100000
#define ECMEMPTYA   0x200000    
#define ECMEMPTYF   0x400000    
#define ECMEMPTYT   0x800000
#define ECMFRMS     0x1000000    
#define ECMFRMA     0x2000000   
#define ECMFRMF     0x4000000    
#define ECMFRMT     0x8000000    
#define ECMSPLS     0x10000000    
#define ECMSPLA     0x20000000    
#define ECMSPLF     0x40000000    
#define ECMSPLT     0x80000000    

#define IFFDISP         73  // 4byte string to display channel/digit data
#define AUXDISP         74  // 4byte string to display channel/digit data

#define TRIMROLL        80
#define TRIMPITCH       81

// CMDS data
#define CMDSBITS        85
// bits
#define CMDSGO      0x01
#define CMDSNOGO    0x02
#define CMDSRDY     0x04
#define CMDSDEGR    0x08

#define CMDS01STR          86  // 01 String
#define CMDS02STR          87  // 02 String
#define CMDSCHAFFSTR       88  // ChaffString
#define CMDSFLARESTR       89  // Flarestring

// ADI data
#define ADIPITCH          90
#define ADIROLL           91
#define ADIILSHORPOS      92
#define ADIILSVERPOS      93
#define ADISIDESLIP       94


#define SIMVERSION          255  // at least available for BMS, let's see for DCS and MSFS
