//BOARD SELECTION

  //#define UNO         //uncomment this if this sketch will be loaded on an UNO
  //#define NANO        //uncomment this if this sketch will be loaded on an NANO
  //#define MICRO       //uncomment this if this sketch will be loaded on an MICRO
  //#define LEONARDO    //uncomment this if this sketch will be loaded on an LEONARDO
  #define MEGA        //uncomment this if this sketch will be loaded on an MEGA
  //#define DUE         //uncomment this if this sketch will be loaded on an DUE (connected via programming port)
  //#define DUE_NATIVE  //uncomment this if this sketch will be loaded on an DUE (connected via native port)
  //#define ESP         //uncomment this if this sketch will be loaded on an ESP32 or ESP8622


 #ifdef DUE_NATIVE
    #define SERIALCOM SerialUSB   //enable communication over the native port of the DUE
  #else
    #define SERIALCOM Serial      //standard serial connection
  #endif

  //MODULE SELECTION - uncomment the modules you want to use.
   
  //#define LED               //drive LEDs
  //#define LEDMatrix         //drive LED Matrix using a MAX7219 controller
  //#define LCD               //drive LCD display
  #define LED_MM5451
  #define SSegMAX7219       //drive 7-Segment displays via MAX7219 controller
  //#define SSegTM1637        //drive 7-Segment displays via TM1367 controller
  //#define SLx2016           //drive 4-digit 5x7 dotmatrix modules
  #define ServoMotor        //drive servo motors directly connected to the arduino
  //#define ServoPWM          //drive multiple servo motors via pwm shield
  //#define StepperBYJ        //drive stepper motor 28BYJ-48
  #define StepperX27        //drive stepper motor X27.168
  //#define StepperVID        //drive multiple stepper motors X25.168 with a VID66-06 controller
  //#define CompassX27        //drive a compass with a Xxx.xxx -class stepper motor
  //#define AirCore           //drive multiple Air Core motors (via CS4192 controller chip)
  //#define MotorPoti         //motor-driven poti control
  //#define OLED              //display data on an OLED display
  //#define SpeedBrake        //Enable display of the SpeedBrake indicator on an 128x64 OLED display (DEDunino)
  //#define FuelFlowIndicator //Enable display of the FuelFlow indicator on an 128x64 OLED display (DEDunino)
  //#define OLED_Compass      //Enable display of the wet compass on an 128x32 OLED display 
  #define DED_PFL           //Enable display of DED or PFL on an 254x64 OLED display (DEDunino)
  //#define Switches          //use the arduino to read switch positions and send keyboard commands
  //#define MagSwitch         //enable magnetic held switches														   
  //#define ButtonMatrix      //use the arduino to read switch positions and send keyboard commands
  //#define RotEncoder        //use the arduino to read rotary encoders and send keyboard commands
  //#define AnalogAxis        //use the arduino to read analog resistors and sync this with a gamecontroller axis
  //#define Lighting          //software controlled backlighting
  //#define NewDevice         //placeholder. Use this line to activate your own code to drive other, specific hardware
  //#define BUPRadio          //activate special procedures to drive the Backup Radio Panel
  //#define CMDS              //activate special procedures to drive the CMDS Panel

//BASIC SETTINGS
#define BAUDRATE 115200 // 57600 //    115200       // serial connection speed
  
static const char scName[] = "RightAux";
int scNameSize = sizeof(scName)/sizeof(scName[0]);  
  
//DATA VARIABLES
/* var format:
  VarNumber, VarBytes, Module, index of modules (if more are available), Index of Value in module (depending on module), value
  value index can be used for single bits in a led module or 255 to use the full value, for example, 
*/
f16var* vars[] = { 
    new f16varC(SIMSTATES, MODNONE, 0, 0, 0)
  ,  new f16varC(POWERSTATES, MODNONE, 0, 0, 0)   // First 2 should always be configured in every module  
  ,  new f16varI(FUELTOTAL, MODMAX7219, 0, 0, 0)
  ,  new f16varI(FUELFWD, MODSERVO, 2, 0, 0)
  ,  new f16varI(FUELAFT, MODSERVO, 3, 0, 0)
  /*,  new f16varI(HYDA, MODSERVO, 0, 0, 0)
  ,  new f16varI(HYDB, MODSERVO, 1, 0, 0)  
  ,  new f16varL(CAUTIONPANELLIGHTS, MODMM5451, 0, 0, 0)  
  ,  new f16varS(PFDLINE1, MODDED, 255, 0, "                         " )
  ,  new f16varS(PFDLINE2, MODDED, 255, 0, "                         " )
  ,  new f16varS(PFDLINE3, MODDED, 255, 0, "                         " )
  ,  new f16varS(PFDLINE4, MODDED, 255, 0, "                         " )
  ,  new f16varS(PFDLINE5, MODDED, 255, 0, "                         " )
  ,  new f16varI(EPUFUEL, MODX27, 0, 0, 0)
  ,  new f16varI(CABINPRESS, MODX27, 1, 0, 0) */
};
const char varCount = sizeof(vars)/sizeof(vars[0]);




  // This is the most important part of this sketch. You need to set the data that the Arduino will have to handle
  // Make sure that you chose the definition of variableCount matches the number of entries in this table
  // Fill data as follows: 
  // 1. Short description (max 5 characters)
  // 2. Flightdata ID (check BMSAIT Variablen.csv)  as a 4-digit string (needed for PULL mode)
  // 3. data type (check BMSAIT Variablen.csv: b=bool, y=byte, 1=byte[], i=integer, f=float, s=string, 2=string[]))  (needed for PULL mode)
  // 4. Output type (10-LED, 20-LCD, 30-MAX7219, 31-TM1637, 40-Servo, 41-ServoPWM, 50-Stepper 28BYJ, 51-Stepper X27-168, 52-StepperBoard VID6606, 60-MotorPoti, 7x-OLED)
  // 5. Target - i.e. Output PIN of LEDs, a reference to an entry in a module specific data table (i.e. motors)
  // 6. Reference2 - i.e. the line on LCD displays or a link to an entry in container in motor modules
  // 7. Reference3 - i.e. number of characters to display (control display length on 7-segment or LCD displays)
  // 8  Reference4 - i.e. start position (control position of data on 7-segment or LCD displays) 
  // 9. Reference5 - i.e. decimal point (will add a decimal point on 7-segment displays after the given position)
  // 10. Initial value as string (i.e. "00")
 /** 
  
  Datenfeld datenfeld[]=
    {
      //Description ID    DT    OT    target Ref2 Ref3 Ref4 Ref5  RQ   IV
	  {"In3D",   "1800",  'b',  99,   0,    0,   0,   0,   0,     "",  "F"}          //Variable 0 - Player is in 3D
	  ,{"Fly",   "1478",  'b',  99,   0,    0,   0,   0,   0,     "",  "F"}		   //Variable 1 - Player is in Aircraft	  
	 	  // HYDPRess Servos
	  ,{"HydA",  "0281", 'i',  40,     0,   0,   0,   0,   0,     "",   0}	// 33 HYD PRESSURE A
	  ,{"HydB",  "0291", 'i',  40,     1,   0,   0,   0,   0,     "",   0}	// 34 HYD PRESSURE B
	  // FQTI Servos and max7219
      ,{"FWDFL", "0531",  'i',  40,   2,    0,   0,   0,   0,     "",   0}   // 35 FUEL QTY FWD
	  ,{"AFTFL", "0481",  'i',  40,   3,    0,   0,   0,   0,     "",   0}	// 36 FUEL QTY AFT
	  ,{"TOTFL", "0551",  's',	30,   0,    0,   5,   0,   0,     "",   0}	// 37 FUEL QTY TOTAL as String  
// Caution panel lights MM5451 (13)	  
	  ,{"ELEC",  "1591",  'b',  13,   0,   13,   0,   0,   0,     0,    0}  	// 7
	  ,{"Probe", "1565",  'b',  13,   0,   14,   0,   0,   0,     0,    0} 		// 8 Probeheat
	  ,{"Seat",  "1566",  'b',  99,   0,    0,   0,   0,   0,     0,    0} 		// 9 Seat Not Armed
	  ,{"NWS",   "1529",  'b',  99,   0,    1,   0,   0,   0,     0,    0}  	// 10 NWS Fail
	  ,{"Skid",  "1569",  'b',  99,   0,    2,   0,   0,   0,     0,    0}   	// 11 Antiskid
	  ,{"Hook",  "1528",  'b',  99,   0,    3,   0,   0,   0,     0,    0}		// 12
	  ,{"Avio",  "1524",  'b',  99,   0,    4,   0,   0,   0,     0,    0}   	// 13 Avionics Fault
	  ,{"Equip", "1504",  'b',  99,   0,    5,   0,   0,   0,     0,    0}  	// 14 Equip Hot
	  ,{"Radar", "1525",  'b',  99,   0,    6,   0,   0,   0,     0,    0} 		// 15 Radar Alt
	  ,{"IFF",   "1526",  'b',  99,   0,    7,   0,   0,   0,     0,    0}		// 16
	  ,{"Engn",  "1521",  'b',  99,   0,    8,   0,   0,   0,     0,    0}   	// 17 Engine Fault
	  ,{"SEC",   "1563",  'b',  99,   0,    9,   0,   0,   0,     0,    0}		// 18
	  ,{"Fuel",  "1568",  'b',  99,   0,   10,   0,   0,   0,     0,    0}  	// 19 Fuel Oil Hot
	  ,{"Inlet", "1609",  'b',  99,   0,   11,   0,   0,   0,     0,    0} 		// 20 Inlet Icing 
	  ,{"FLCS",  "1519",  'b',  99,   0,   12,   0,   0,   0,     0,    0}  	// 21 FLCS Fault
	  ,{"CADC",  "1602",  'b',  99,   0,   15,   0,   0,   0,     0,    0}  	// 22
	  ,{"AFuel", "1560",  'b',  99,   0,   17,   0,   0,   0,     0,    0} 		// 23 Aft Fuel low
	  ,{"FFuel", "1559",  'b',  99,   0,   18,   0,   0,   0,     0,    0} 		// 24 Fwd Fuel low
	  ,{"ATF",   "1608",  'b',  99,   0,   19,   0,   0,   0,     0,    0}   	// 25 ATF Not Enganged
	  ,{"StCfg", "1507",  'b',  99,   0,   20,   0,   0,   0,     0,    0} 		// 26 StoresConfig
	  ,{"---1",  "1612", 'b',  99,     0,    21,   0,   0,  0,   0, 0}  // not in BMS
	  ,{"BUC",   "1567",  'b',  99,   0,   22,   0,   0,   0,     0,    0}		// 27
	  ,{"EEC",   "1611",  'b',  99,   0,   23,   0,   0,   0,     0,    0}   	// 28 not in BMS, uses BUC value from winapp
	  ,{"OHeat", "1522",  'b',  99,   0,   24,   0,   0,   0,     0,    0} 		// 29 Overheat
	  ,{"---2",  "1612", 'b',  99,     0,    25,   0,   0,  0,   0, 0}  // not in BMS
	  ,{"---3",  "1612", 'b',  99,     0,    26,   0,   0,  0,   0, 0}  // not in BMS
	  ,{"---4",  "1612", 'b',  99,     0,    27,   0,   0,  0,   0, 0}  // not in BMS 
	  ,{"Nucl",  "1610", 'b',  99,     0,  28,   0,   0,   0,     0,    0}  		// 30 not in BMS, uses BUC value from winapp
	  ,{"---5",  "1612", 'b',  99,     0,    29,   0,   0,  0,   0, 0}  // not in BMS
	  ,{"---6",  "1612", 'b',  99,     0,    30,   0,   0,  0,   0, 0}  // not in BMS 
	  ,{"CabPs", "1530", 'b',  99,     0,  31,   0,   0,   0,     0,    0}  // 31 CabinPressure
	  ,{"OxLow", "1564", 'b',  99,     0,  32,   0,   0,   0,     0,    0}  // 32 OxyLow  
	  ,{"CPAll", "1619", 'b',  13,     0,  0,   0,   0,   0,     0,    0}   // All Caution Lights on, index 7!	
/* //  DED
	  ,{"DED1",   "0231", 's',  79,   0,  "",  ".1"}          //Variable 2 - DED Line 1 index 40
	  ,{"DED2",   "0232", 's',  79,   0,  "",  ".2"}          //Variable 3 - DED Line 2
	  ,{"DED3",   "0233", 's',  79,   0,  "",  ".3"}          //Variable 4 - DED Line 3
	  ,{"DED4",   "0234", 's',  79,   0,  "",  ".4"}          //Variable 5 - DED Line 4
 	  ,{"DED5",   "0235", 's',  79,   0,  "",  ".5"}          //Variable 6 - DED Line 5  
   }; 
  const byte variableCount = sizeof(datenfeld)/sizeof(datenfeld[0]); */