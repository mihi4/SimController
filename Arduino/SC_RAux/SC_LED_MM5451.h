// settings and functions to drive LEDs through an MM5451 LED driver chip

// var.modIndex defines the MM5451 device (in an array)
// var.valIndex sets the pin of a single LED

///////////////////////////////////////////////////
//
// RIGHT AUX VERSION FOR CAUTION PANEL
//
//////////////////////////////////////////////////

#include "MM5451.h"
/*
#define LASTLEDPIN 33
#define LAMPS_STARTINDEX 8  // First index of lamps in userconfig, ignore ELEC and PROBEHEAT
#define LAMPCOUNT 30	// lamps to iterate through, exluding the 2 above 
#define BUCINDEX 27   // index of BUC lamp, used to control the "-----" lamps 20250725, not needed anymore
#define CPALLINDEX 39


unsigned long LEDTimer;
*/

#define LEDPINNUM   35
bool allLightsOn = false; // will be set, if all 3 AllLightsOn bits are set to light up the whole panel

MM5451 mm5451[]={
    MM5451(18,19)
    //MM5451(6,7)
}; // create instances of chip classes, parameters are CLK and DATA pin 

const byte mm5451anz = sizeof(mm5451)/sizeof(mm5451[0]); 
/*
void UpdateBlink()
{
  LEDTimer=millis();
  for(byte a = LAMPS_STARTINDEX; a < LAMPS_STARTINDEX+LAMPCOUNT; a++)  // for(byte a = 0; a < VARIABLENANZAHL; a++)
  {
    if (datenfeld[a].typ==13)
    {
      if (datenfeld[a].ref3==1)
      {
        datenfeld[a].ref3=0;
        if (datenfeld[a].ref4==0)
          {datenfeld[a].ref4=1;}
        else
          {datenfeld[a].ref4=0;}
      }
      else
      {
        datenfeld[a].ref3=1;
      }
    }
  }
}
*/

void LED_On(byte chipIndex, byte ledNumber) {  // this function sets the status of the appropriate output of the chip, but doesn't send it yet
    /* sprintf(rbMsg, "setting bit %u ON", ledNumber);
    sendReadBackString(rbMsg); */
    mm5451[chipIndex].setOutput(ledNumber, true);
}
void LED_Off(byte chipIndex, byte ledNumber) {
    /* sprintf(rbMsg, "setting bit %u OFF", ledNumber);
    sendReadBackString(rbMsg); */
    mm5451[chipIndex].setOutput(ledNumber, false);
}

void debugDatabits(byte x) {  // x = number of mm5451anz
	for (int i=0; i<LEDPINNUM; i++) {
		char msg[10]  = "";
		itoa(i, msg, 10);
		strcat(msg, ( (mm5451[x].getOutput(i))? "ON":"OFF" ));
		sendReadBackString(msg);
	}
}


void SetupLED_MM5451()
{
	if (debugmode) sendReadBackString("setup MM5451");
	//for (byte i=0; i<1; i++) {
		for (byte x=0; x<mm5451anz; x++) {
			mm5451[x].lightAll();	
			//debugDatabits(x);
		}	
		delay(200);
		for (byte x=0; x<mm5451anz; x++) {		
			mm5451[x].clearAll();
			//debugDatabits(x);
		}
		delay(200);
	//}
	for (byte x=0; x<mm5451anz; x++) {
        mm5451[x].lightAll();
    }	
    delay(500);
	for (byte x=0; x<mm5451anz; x++) {
        mm5451[x].clearAll();
		//debugDatabits(x);
	}		
  
  int p=7;
  for (int i=0; i<33;i++) {
    LED_On(vars[p]->modIndex, i);
    mm5451[(vars[p]->modIndex)].outputDataBits();
    delay(100);
  }
  delay(500);
  for (int i=0; i<33;i++) {
    LED_Off(vars[p]->modIndex, i);
    mm5451[(vars[p]->modIndex)].outputDataBits();
    delay(100);
  }

}

void debugTime(int x) {
  char debugmsg[20];
  ltoa(millis(),debugmsg,10);
  sendReadBackString(debugmsg);
}


void UpdateLED_MM5451(byte p)
{    
  if (checkBit(vars[0]->value.valC, SIMTESTLIGHTS ) ) // all light on bit is set
  //if ((datenfeld[CPALLINDEX].wert[0]=='T') || (datenfeld[CPALLINDEX].wert[0]=='1'))
  {
	  mm5451[0].lightAll();  // we can assume here's only one MM5451 included for the CP
	  return;
  }
 
  #include "dataConversion.h"
 
 /*  sprintf(rbMsg, "MM incoming val %lu", longVal);
  sendReadBackString(rbMsg);  */
  
  char bitNum = 0;
  switch (vars[p]->type) {  
    case f16var::INT:          
        bitNum = 16;
        break;  
    case f16var::CHAR:  
        bitNum = 8;
        break;      
    case f16var::LONG:  
        bitNum = 32;
        break;  
  } 
  if (vars[p]->valIndex == 255) { // use full varvalue to set output bits
    for (int i=0; i<bitNum; i++) {  // maximum is 4byte long (32bit) as value to send
      int j=i;
      //only for CautionPanel, it's is soldered strange, first 16 (0-15) pins are regular, but 2nd 16 leds start at pin 17, not 16
      if (i >=16) j=i+1;  // remove this line if not used with caution panel

      // if ( (longVal >> (i)) && 1) LED_On(vars[p]->modIndex, i); else LED_Off(vars[p]->modIndex, i);
      if (bitRead(longVal, i)) LED_On(vars[p]->modIndex, j); else LED_Off(vars[p]->modIndex, j);
    }
  } else {
    if (longVal) LED_On(vars[p]->modIndex, vars[p]->valIndex); else LED_Off(vars[p]->modIndex, vars[p]->valIndex); 
  }
  
  if (debugmode) debugDatabits(vars[p]->modIndex);
  mm5451[(vars[p]->modIndex)].outputDataBits();  // let MM5451 chip send out all LED data bits 
}
