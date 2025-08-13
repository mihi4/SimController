 
#include <Arduino.h>
#include <avr/wdt.h> // to use for software reset; FIXXXME - really needed?
#include "f16common.h"


struct f16var {  
    unsigned char number;
    unsigned char module;
    unsigned char index;
    unsigned char type;
    public: 
    unsigned char value;
    // Konstruktor  
   f16var(unsigned char number, unsigned char module, unsigned char index, unsigned char type) : number(number), module(module), index(index), type(type) { value=0; }
      
    // Virtuelle Methode zur Ausgabe  
    //virtual void printValue() = 0; // Reine virtuelle Methode  
}; 

struct f16varC : public f16var {
  unsigned char value;
  
  f16varC(unsigned char number, unsigned char module, unsigned char index, unsigned char type, unsigned char value) : f16var(number, module, index, type), value(value) {}
};

struct f16varI : public f16var {  // using I as in integer for Short, since both are 2 bytes on arduino
  unsigned int value;
  
  f16varI(unsigned char number, unsigned char module, unsigned char index, unsigned char type, unsigned int value) : f16var(number, module, index, type), value(value) {}
};

struct f16varL : public f16var {
  unsigned long value;
  
  f16varL(unsigned char number, unsigned char module, unsigned char index, unsigned char type, unsigned long value) : f16var(number, module, index, type), value(value) {}
};

struct f16varS : public f16var {
  String value;
  f16varS(unsigned char number, unsigned char module, unsigned char index, unsigned char type, String value) : f16var(number, module, index, type), value(value) {}
};

// function definitions
void parseSerialCommand();
void outputVar(char varIndex, unsigned long value);

// global vars


#include "SC_UserConfig.h"
//#include "SCComms.h"

#ifdef DED_PFS
	#include "SC_DED_PFL.h"
#endif
#ifdef LED_MM5451
	#include "SC_LED_MM5451.h"
#endif
#ifdef SSegMAX7219
	#include "SC_SSegMAX7219.h"
#endif


void outputVar(char varIndex, unsigned long value) {
      
}

void printVardata(f16varC* var) {
    SERIALCOM.print("varNum: ");
		SERIALCOM.print(var->number);
		SERIALCOM.print(" varMod: ");SERIALCOM.print(var->module);
		SERIALCOM.print(" varIndex: ");SERIALCOM.println(var->index);
        SERIALCOM.print("valueC: "); SERIALCOM.println(var->value);
}

void printVardata(f16varI* var) {
    SERIALCOM.print("varNum: ");
		SERIALCOM.print(var->number);
		SERIALCOM.print(" varMod: ");SERIALCOM.print(var->module);
		SERIALCOM.print(" varIndex: ");SERIALCOM.println(var->index);
        SERIALCOM.print("valueI: "); SERIALCOM.println(var->value);
}
void printVardata(f16varL* var) {
    SERIALCOM.print("varNum: ");
		SERIALCOM.print(var->number);
		SERIALCOM.print(" varMod: ");SERIALCOM.print(var->module);
		SERIALCOM.print(" varIndex: ");SERIALCOM.println(var->index);
        SERIALCOM.print("valueL: "); SERIALCOM.println(var->value);
}
void printVardata(f16varS* var) {
    SERIALCOM.print("varNum: ");
		SERIALCOM.print(var->number);
		SERIALCOM.print(" varMod: ");SERIALCOM.print(var->module);
		SERIALCOM.print(" varIndex: ");SERIALCOM.println(var->index);
        SERIALCOM.print("valueS: "); SERIALCOM.println(var->value);
}


void setup() {
  // put your setup code here, to run once:
	SERIALCOM.begin(BAUDRATE);
	while (!SERIALCOM) {}
	delay(500);
	SERIALCOM.println("Arduino is ready");


	SERIALCOM.print("varCount is ");SERIALCOM.println(varCount, DEC);
	for (char i=0; i<varCount; i++){		
        SERIALCOM.println(vars[i]->value);
        
	}

	
}

void loop() {
  // put your main code here, to run repeatedly:
	//ReadSerial();
	
	//parseSerialCommand();
	
    //showNewData();
}
