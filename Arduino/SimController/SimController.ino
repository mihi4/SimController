 
#include <Arduino.h>
#include "f16common.h"


struct f16var {  
    unsigned char number;
    unsigned char module;
    unsigned char index;
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
    f16var(unsigned char number, unsigned char module, unsigned char index, Type type) : number(number), module(module), index(index), type(type) {  }
      
    // Virtuelle Methode zur Ausgabe  
    //virtual void printValue() = 0; // Reine virtuelle Methode  
}; 

struct f16varC : public f16var {
//  unsigned char value;
  
  //f16varC(unsigned char number, unsigned char module, unsigned char index, unsigned char type, unsigned char value) : f16var(number, module, index, type), value(value) {}
  f16varC(unsigned char number, unsigned char module, unsigned char index, unsigned char value) : f16var(number, module, index, CHAR) {
	  this->value.valC = value;
	}
};

struct f16varI : public f16var {  // using I as in integer for Short, since both are 2 bytes on arduino
  //unsigned int value;
  
  //f16varI(unsigned char number, unsigned char module, unsigned char index, unsigned char type, unsigned int value) : f16var(number, module, index, type), value(value) {}
  f16varI(unsigned char number, unsigned char module, unsigned char index, unsigned int value) : f16var(number, module, index, INT)  {
	  
	  this->value.valI = value;
	  
  }
};

struct f16varL : public f16var {
  //unsigned long value;
  
  //f16varL(unsigned char number, unsigned char module, unsigned char index, unsigned char type, unsigned long value) : f16var(number, module, index, type), value(value) {}
  f16varL(unsigned char number, unsigned char module, unsigned char index, unsigned long value) : f16var(number, module, index, LONG) {
	  this->value.valL = value;	  
  }
};

struct f16varS : public f16var {
  //String value;
  // f16varS(unsigned char number, unsigned char module, unsigned char index, unsigned char type, String value) : f16var(number, module, index, type), value(value) {}
  f16varS(unsigned char number, unsigned char module, unsigned char index, String value) : f16var(number, module, index, STRING) {
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

void outputVar(char varIndex, unsigned long value);

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

// only for debugging ReadSerial char input[] = { '<', 'U', 10, 2, 100, 52, '>'};


//////////////////////////////
//        includes
//////////////////////////////

#include "SC_UserConfig.h"
#include "SCComms.h"

#ifdef DED_PFS
	#include "SC_DED_PFL.h"
#endif
#ifdef LED_MM5451
	#include "SC_LED_MM5451.h"
#endif
#ifdef SSegMAX7219
	#include "SC_SSegMAX7219.h"
#endif

//////////////////////////////
// function imlementations
//////////////////////////////

void outputVars() {
	if (!varsChanged) return;
	#ifdef DED_PFL  
  if (millis() > lastDEDUpdate + DEDUPATEINTERVALL) {
    UpdateDED();  
    lastDEDUpdate = millis();
  }  
  #endif
	for (int i=0;i<varCount;i++) {
    SERIALCOM.print("var ");SERIALCOM.print(i, DEC);SERIALCOM.print(" type ");SERIALCOM.print(vars[i]->type, DEC);SERIALCOM.print(" value: ");
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
    }  
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

  varsChanged = true;
  outputVars();
  /* debugging escaping
  ReadSerial();
  showNewData();
  parseSerialCommand();
  outputVars();
  char newInput[] = { '<', 'U', 1, 1, 27, '>' , '>'};
  strcpy(input, newInput);
  ReadSerial();
  showNewData();
  parseSerialCommand();
  outputVars();*/
}

void loop() {
  // put your main code here, to run repeatedly:
	ReadSerial();	
  // showNewData();
	parseSerialCommand();
  outputVars();	
   
}
