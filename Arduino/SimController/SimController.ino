 
#include <Arduino.h>
#include <avr/wdt.h> // to use for software reset; FIXXXME - really needed?
#include "f16common.h"

typedef struct {
  unsigned char number;
  unsigned char module;
  unsigned char index;
  unsigned char type;
} f16var;

// function definitions
void parseSerialCommand();
void outputVar(char varIndex, unsigned long value);

// global vars


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


void outputVar(char varIndex, unsigned long value) {
    switch (vars[varIndex].module) {
        case MODSERVO:
            break;
        case MODMM5451:
            break;                
    }    
}

void setup() {
  // put your setup code here, to run once:
	SERIALCOM.begin(BAUDRATE);
	while (!SERIALCOM) {}
	SERIALCOM.println("Arduino is ready");
  /* char buf[20];
  itoa(varCount, buf, 10);
  SERIALCOM.print("varCount is ");SERIALCOM.println(buf);
  for (char i=0; i<varCount; i++){
    SERIALCOM.print("varNum: ");
    SERIALCOM.print(vars[i].number);
    SERIALCOM.print(" varMod: ");SERIALCOM.print(vars[i].module);
    SERIALCOM.print(" varIndex: ");SERIALCOM.println(vars[i].index);
	}*/
	
	
}

void loop() {
  // put your main code here, to run repeatedly:
	ReadSerial();
	parseSerialCommand();
	
    //showNewData();
}
