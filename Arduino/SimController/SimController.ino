#include <Arduino.h>
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

	 // Union für verschiedene Datentypen  
    union Value {  
      unsigned char valC;
      unsigned int valI;  
      unsigned long valL;  
      String* valString;  

      /*Value() {} // Standardkonstruktor  
      ~Value() {} // Destruktor (wichtig für String)  */
    } value;
	
    // Konstruktor  
    f16var(unsigned char number, unsigned char module, unsigned char modIndex, unsigned char valIndex, Type type) : number(number), module(module), modIndex(modIndex), valIndex(valIndex), type(type) {  }
      
    // Virtuelle Methode zur Ausgabe  
    //virtual void printValue() = 0; // Reine virtuelle Methode  
}; 

struct f16varC : public f16var {
//  unsigned char value;
  
  //f16varC(unsigned char number, unsigned char module, unsigned char index, unsigned char type, unsigned char value) : f16var(number, module, index, type), value(value) {}
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
  
      // Destruktor  
    ~f16varS() {  
        delete value.valString; // Speicher freigeben  
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

//////////////////////////////
// function imlementations
//////////////////////////////
bool checkBit(unsigned long var, unsigned long bit) {
  if (var & bit) return true;
  return false;
}

void setupModules() {
  #ifdef DED_PFL
    SetupDED();
  #endif
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
	
/* 	#ifdef DED_PFL  
  if (millis() > lastDEDUpdate + DEDUPATEINTERVALL) {
    UpdateDED();  
    lastDEDUpdate = millis();
  }  
  #endif

 */
	//for (int i=0;i<varCount;i++) {
    /*SERIALCOM.print("var ");SERIALCOM.print(i, DEC);SERIALCOM.print(" type ");SERIALCOM.print(vars[i]->type, DEC);SERIALCOM.print(" value: ");
    switch (vars[i]->type) {  
      case f16var::INT:  
          SERIALCOM.println(vars[i]->value.valI);  
          break;  
      case f16var::CHAR:  
          SERIALCOM.println(vars[i]->value.valC);  
          break;  
      case f16var::STRING:  
          SERIALCOM.println(*vars[i]->value.valString);  
          break;  
			case f16var::LONG:  
          SERIALCOM.println(vars[i]->value.valL);  
          break;  
    } */
  int i = lastParsedVar;
  //sprintf(rbMsg, "updating Varnum %u", i);
  //sendReadBackString(rbMsg);
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

    //}

  }
  varsChanged = false;
      
}

//////////////////////////////
// setup and loop
//////////////////////////////

void setup() {
  // put your setup code here, to run once:
	SERIALCOM.begin(BAUDRATE);
	while (!SERIALCOM) {}
	 SERIALCOM.println("Arduino is ready");
/*
  varsChanged = true;
  outputVars();
  // debugging escaping
  ReadSerial();
  showNewData();
  parseSerialCommand();
  outputVars();
  char newInput[] = { '<', 'U', 1, 1, 27, '>' , '>'};
  strcpy(input, newInput);
  ReadSerial();
  showNewData();
  parseSerialCommand();
  outputVars(); */
	setupModules();

	SERIALCOM.println("Setup Done");
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
  //delay(5);
  
}
