#include <Arduino.h>
#include <OneButton.h>
#include "f16common.h"

/*
Variable model
==============
number: number of variable as configured in f16common.h
module: number of the hardware modul to use, also configured in f16common.h
modIndex: if more modules of same hardware are used, they will be configured in an array, this configures the index position in it
valIndex: can be used as single bit value to change in a LED module... let's see, how we're going to use it :-)
Type: one of CHAR, INT, LONG, STRING
*/
struct f16var {  
    unsigned char number;
    unsigned char module;
    unsigned char modIndex;
    unsigned char valIndex;
    //unsigned char type;

	enum Type {	
		CHAR =    0,
    INT =     1,  
    LONG =    2,  
    STRING =  3  
  } type; 

	 // Union for different data types
    union Value {  
      unsigned char valC;
      unsigned int valI;  
      unsigned long valL;  
      String* valString;  

    } value;
	
    // constructor
    f16var(unsigned char number, unsigned char module, unsigned char modIndex, unsigned char valIndex, Type type) : number(number), module(module), modIndex(modIndex), valIndex(valIndex), type(type) {  }
      
    
}; 

struct f16varC : public f16var {
//  unsigned char value;
    
  f16varC(unsigned char number, unsigned char module, unsigned char modIndex, unsigned char valIndex, unsigned char value) : f16var(number, module, modIndex, valIndex, CHAR) {
	  this->value.valC = value;
	}
};

struct f16varI : public f16var {  // using I as in integer for Short, since both are 2 bytes on arduino
  
  f16varI(unsigned char number, unsigned char module, unsigned char modIndex, unsigned char valIndex, unsigned char value) : f16var(number, module, modIndex, valIndex, INT)  {
	  
	  this->value.valI = value;
	  
  }
};

struct f16varL : public f16var {
  
  f16varL(unsigned char number, unsigned char module, unsigned char modIndex, unsigned char valIndex, unsigned char value) : f16var(number, module, modIndex, valIndex, LONG) {
	  this->value.valL = value;	  
  }
};

struct f16varS : public f16var {
  
  f16varS(unsigned char number, unsigned char module, unsigned char modIndex, unsigned char valIndex, unsigned char value) : f16var(number, module, modIndex, valIndex, STRING) {
	  this->value.valString = new String(value);
  }
  
      // Destructor  
    ~f16varS() {  
        delete value.valString; 
    }  
};

//////////////////////////////
// function definitions
//////////////////////////////

bool checkBit(unsigned long var, unsigned long bit);
void setupModules();
void fastUpdate();

unsigned int power(unsigned int base, unsigned int exp) {
	int retVal = 1;
	for (int x=0; x<exp; x++) {
		retVal *= base;
	}
	return retVal;
}

///////////////////////////////
//       global vars
///////////////////////////////

unsigned long lastDEDUpdate = 0;
boolean varsChanged = false;
boolean debugmode = false;

char lastParsedVar;
bool configmode = false;
int altimeterOffset = 0;

// only for debugging
unsigned long time =0;
unsigned long gap = 0;

//////////////////////////////
//        includes
//////////////////////////////

#include "SC_UserConfig.h"
#include "SCComms.h"

#ifdef DED_PFL
	#include "SC_DED_PFL.h"
#endif
#ifdef LED_MM5451
	#include "SC_LED_MM5451.h"
#endif
#ifdef SSegMAX7219
	#include "SC_SSegMAX7219.h"
#endif
 #ifdef ServoMotor
  #include "SC_Servo.h"    
 #endif
 #ifdef StepperX27
  #include "SC_StepperX27.h"    
 #endif
//Steppermotor ControllerBoard settings
#ifdef StepperVID                   
  #include "SC_StepperVID.h"
#endif  

 //7Segment settings TM1637 tube
#ifdef SSegTM1637                            
  #include "SC_SSegTM1637.h"
#endif  
//PWM Shield for Servos settings
#ifdef ServoPWM                       
  #include "SC_ServoPWMShield.h"               
#endif  
//Steppermotor settings
#ifdef StepperBYJ                  
  #include "SC_Stepper28BYJ48.h"
#endif                                

//Compass setting
#ifdef CompassX27                  
  #include "SC_CompassX27.h"
#endif  

//Altimeter setting
#ifdef AltimeterX27                  
  #include "SC_AltimeterX27.h"
  #include "AltimeterInternal.h"
#endif  
//motor poti settings
#ifdef MotorPoti 
  #include "SC_MotorPoti.h"
#endif 
#ifdef OLED
  #include "SC_OLED.h"
#endif
#ifdef SpeedBrake                             
  #include "SC_SBI.h"
#endif
#ifdef FuelFlowIndicator
  #include "SC_FF.h"
#endif

//rotary encoders
#ifdef RotEncoder               
  #include "SC_Encoder.h"
#endif  


 

//////////////////////////////
// function imlementations
//////////////////////////////
bool checkBit(unsigned long var, unsigned long bit) {
  if (var & bit) return true;
  return false;
}

void setupModules() {
  #ifdef LED_MM5451
    SetupLED_MM5451();
  #endif
  #ifdef SSegMAX7219
    SetupMax7219();
  #endif
  #ifdef ServoMotor
    SetupServo();
  #endif
  #ifdef StepperX27
    SetupStepperX27();
  #endif
  #ifdef DED_PFL
    SetupDED();
  #endif
  #ifdef LCD                            //LCD setup begin
   SetupLCD();
  #endif                                //LCD setup end
  #ifdef SSegTM1637                     //7-segment-display TM1637 setup begin
   SetupTM1637();
  #endif                                //7-segment-display TM1637 setup end
  #ifdef ServoPWM                       //servo PWM shield setup begin
    SetupServoPWM();          
  #endif                                //servo PWM shield setup end 
  #ifdef StepperBYJ                     //stepper setup begin
   //SetupStepperBYJ();
  #endif                                //stepper setup end
  #ifdef StepperVID                     //stepper on controller board setup begin
   SetupStepperVID();
  #endif                                //stepper setup end
  #ifdef CompassX27                     //Compass setup start
   SetupCompassX27();
  #endif                                //Compass setup end
  #ifdef AltimeterX27                   //Altimeter setup start
   SetupAltimeterX27();
   SetupAltimeterKnob();
  #endif                                //Altimeter setup end
  #ifdef MotorPoti                      //MotorPoti setup begin
    SetupMotorPoti();
  #endif                                //MotorPoti setup end
  #ifdef OLED                           //OLED setup begin
    SetupOLED();
  #endif                                //OLED setup end
  #ifdef SpeedBrake                     //SBI setup begin
    SetupSBI();
  #endif                                //SBI setup end
  #ifdef FuelFlowIndicator              //FFI setup begin
   SetupFFI();
  #endif                                //FFI setup end
  #ifdef RotEncoder                     //Rotary encoder setup begin
   SetupEncoder();
  #endif                                //Rotary encoder setup end
  
}

///Call for another update of the motors to allow for fast, fluent movement
void fastUpdate()
{
  #ifdef ServoMotor                       
    Servo_FastUpdate();            
  #endif                            

  #ifdef ServoPWM                       
    ServoPWM_FastUpdate();              
  #endif 

  #ifdef StepperVID
    StepperVID_FastUpdate();   
  #endif
   
  #ifdef StepperX27
    StepperX27_FastUpdate();  
  #endif
   
  #ifdef Stepper28BYJ48
    Stepper28BYJ48_FastUpdate();  
  #endif 

  #ifdef CompassX27
    CompassX27_FastUpdate();
  #endif      

  #ifdef AltimeterX27
    AltimeterX27_FastUpdate();
  #endif      

  #ifdef MotorPoti
    MotorPoti_FastUpdate(); 
  #endif  
}


void outputVars() {
	
  int i = lastParsedVar;  
  switch (vars[i]->module) {
    #ifdef LED_MM5451
    case MODMM5451:            
      UpdateLED_MM5451(i);
      break;
    #endif
    #ifdef ServoMotor
    case MODSERVO:
      UpdateServo(i);
      break;
    #endif
    #ifdef SSegMAX7219
    case MODMAX7219:
      UpdateMAX7219(i);
      break;
    #endif
    #ifdef StepperX27
    case MODX27:
      UpdateStepperX27(i);
      break;
    #endif
    #ifdef DED_PFL  
    case MODDED:
      if (millis() > lastDEDUpdate + DEDUPATEINTERVALL) {
          UpdateDED();  
          lastDEDUpdate = millis();
      }  
      break;
    #endif
    #ifdef SSegTM1637
    case MODTM1637:   //7-Segment display TM1637
      UpdateTM1637(i);
      break;
    #endif
    #ifdef ServoPWM  
      case MODSERVOPWM: //Servo on PWM Shield
        UpdateServoPWM(i);
        break;
    #endif    
    #ifdef StepperBYJ 
      case MODSTEPBYJ: //Stepper Motor
        UpdateStepperBYJ(i);
        break;
    #endif 
    #ifdef HDSP253X 
      case MODHDSP253X: //Stepper Motor
        UpdateHDSP253X(i);
        break;
    #endif 
    #ifdef StepperVID
      case MODX27VID: //one to four Servos on a VID6606 board
        UpdateStepperVID(i);
        break;
    #endif
    #ifdef CompassX27
      case MODCOMPASS: //compass driven by X-class stepper motor
        UpdateCompassX27(i);
        break;
    #endif	  
	  #ifdef AltimeterX27
      case MODALTIMETER: //altimeter needle driven by X-class stepper motor
        UpdateAltimeterX27(i);
        break;
    #endif
    #ifdef MotorPoti
      case MODMOTORPOTI: //MotorPoti
        UpdateMotorPoti(i);
        break;      
    #endif

    #ifdef OLED
      case MODOLED: //generic OLED
        UpdateOLED(i);
        break;      
    #endif
    
    #ifdef SpeedBrake
      case MODSPEEDBRK: //Speedbrake OLED
        UpdateSBI(i);
        break;      
    #endif
    
    #ifdef FuelFlowIndicator
      case MODFFI: //FFI OLED
        UpdateFFI(i);
        break;      
    #endif
    
    
    
  }
  varsChanged = false;
      
}

void UpdateInput(bool all)
{
  
  #ifdef RotEncoder              
   CheckEncoder();            //check encoders and initiate commands if they were moved
  #endif
  
  #ifdef AltimeterX27
	UpdateAltimeterKnob();
  #endif
 
}

//////////////////////////////
// setup and loop
//////////////////////////////

void setup() {
  // put your setup code here, to run once:
	SERIALCOM.begin(BAUDRATE);
	while (!SERIALCOM) {}
	SERIALCOM.println("Simcontroller starting");

	setupModules();

	//SERIALCOM.println("Setup Done");
  sendConnectReply();
}

void loop() {

  fastUpdate();
  ReadSerial();
  fastUpdate();
  // showNewData();
  parseSerialCommand();
  fastUpdate();
  if (varsChanged) outputVars();	
  fastUpdate();
  UpdateInput(false);
  delay(5);
  
}
