// settings and functions to display numbers on 7-segment-displays
//target= device ID (if multiple displays are connected
//ref2= not used
//ref3= number of figures to display
//ref4= offset position 
//ref5= decimal point

#include <LedControl.h>
LedControl Max7219_display[]={
    LedControl(0,0,0, 1)     //empty call of the LEDControl class
    //,LedControl(0,0,0, 1)  //uncomment this if you use a second Max7219 on one arduino. copy this line if you use more than 2.
};
const byte max7219anz = sizeof(Max7219_display)/sizeof(Max7219_display[0]); 

#define MAX_CLK 33   //PIN "Clock" for the SPI connection of the 7-Segment Tube 
#define MAX_CS 34    //PIN "Cable Select" for the SPI connection of the 7-Segment Tube
#define MAX_DIN 35  //PIN "Data In" for the SPI connection of the 7-Segment Tube 
#define MAX_BRIGHTNESS 5
  
void SetupMax7219()
{
  Max7219_display[0]=LedControl(MAX_DIN, MAX_CLK, MAX_CS, 1);  //actual call of the LEDControl class with the correct PINs 
  //Max7219_display[1]=LedControl(/*DIN*/,/*CLK*/, /*CS*/, 1);  //uncomment this if you use a second Max7219 on one arduino. copy this line if you use more than 2. Make sure to enter the correct PINs the additional devices are connected to
  for (byte x=0;x<max7219anz;x++)
  {
    Max7219_display[x].shutdown(0,false);
    Max7219_display[x].setIntensity(0,MAX_BRIGHTNESS);
    Max7219_display[x].setDigit(0,0,8,false);
    Max7219_display[x].setDigit(0,1,8,false);
    Max7219_display[x].setDigit(0,2,8,false);
    Max7219_display[x].setDigit(0,3,8,false);
    Max7219_display[x].setDigit(0,4,8,false);
    Max7219_display[x].setDigit(0,5,8,false);
    Max7219_display[x].setDigit(0,6,8,false);
    Max7219_display[x].setDigit(0,7,8,false);
	Max7219_display[x].setIntensity(0,0); 
  }
  delay(2000);
  for (byte x=0;x<max7219anz;x++)
  {
    Max7219_display[x].shutdown(0,false);
    Max7219_display[x].setIntensity(0,MAX_BRIGHTNESS);
    Max7219_display[x].setDigit(0,0,0,false);
    Max7219_display[x].setDigit(0,1,1,false);
    Max7219_display[x].setDigit(0,2,2,false);
    Max7219_display[x].setDigit(0,3,3,false);
    Max7219_display[x].setDigit(0,4,4,false);
    Max7219_display[x].setDigit(0,5,5,false);
    Max7219_display[x].setDigit(0,6,6,false);
    Max7219_display[x].setDigit(0,7,7,false);
  }
  delay(500);
  for (byte x=0;x<max7219anz;x++)
    {Max7219_display[x].clearDisplay(0);}
}


void UpdateMAX7219(byte p)
{
  bool dp=false;
  char chars[8]=""; // store 8 characters (max one 7291 can display);

  //memcpy(Wert, datenfeld[p].wert, DATENLAENGE);
 
	/*if (debugmode){SendMessage("updating MAX",1);}
	if (debugmode){SendMessage(datenfeld[p].ID,1);}
	if (debugmode){SendMessage(datenfeld[p].wert,1);}*/
 
 
  
  unsigned long longVal = 0;
  switch (vars[p]->type) {  
    case f16var::INT:  
        longVal = vars[p]->value.valI;  
        break;  
    case f16var::CHAR:  
        longVal = vars[p]->value.valC;  
        break;  
    case f16var::STRING:  
        longVal = atol((*vars[p]->value.valString).c_str());  
        break;  
    case f16var::LONG:  
        longVal = vars[p]->value.valL;  
        break;  
  } 
  
  // for fqti only, remove for others
  longVal *= 100;
  char digits[8];
  sprintf(digits, "%d", longVal);
  char digitLength = strlen(digits);
  
  char startPos = 5 - digitLength;
  if (startPos>0) Max7219_display[vars[p]->modIndex].setChar(0,0,' ',0); // remove leading number from display, if less than 10000 lbs remaining
  for (int i=0; i<digitLength; i++) {
    Max7219_display[vars[p]->modIndex].setChar(0,i+startPos,digits[i],0);
  }
  
  
  
  
  
  /*
  //byte stelle = 7-datenfeld[p].ref4;
  for (byte x=0 ; x<datenfeld[p].ref3 ; x++)
  {
    if (dp){dp=false;}
    if ((x==datenfeld[p].ref5-1)&&(Wert[x]!=' ')){dp=true;}    //set decimal point 
    Max7219_display[datenfeld[p].target].setChar(0,x+datenfeld[p].ref4,Wert[x],dp);
    //stelle--;
  }          
    */
}
