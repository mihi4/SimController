 
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

// global vars
static const char scName[] = "RightAux";
int scNameSize = sizeof(scName)/sizeof(scName[0]);

/* var format:
  VarName, VarBytes, Module, Index of Value in module (255 if not needed)
*/
f16var vars[] = { 
    {VARA, MODSERVO, 0, VARSHORT}, 
    {VARB, MODSERVO, 1, VARSHORT}, 
    {VARC, MODMM5451, 255, VARLONG},
    {VARD, MOD7219, 3, VARINT }
};
const char varCount = sizeof(vars)/sizeof(vars[0]);

//#include "SCSerial.h"
#include "SCComms.h"

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


