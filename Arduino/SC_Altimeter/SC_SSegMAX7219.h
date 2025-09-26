// settings and functions to display numbers on 7-segment-displays
// varIndex is used as starting position on display for the string

#include <LedControl.h>
LedControl Max7219_display[]={
    LedControl(0,0,0, 1)     //empty call of the LEDControl class
    //,LedControl(0,0,0, 1)  //uncomment this if you use a second Max7219 on one arduino. copy this line if you use more than 2.
};
const byte max7219anz = sizeof(Max7219_display)/sizeof(Max7219_display[0]); 

#define MAX_CLK 9   //PIN "Clock" for the SPI connection of the 7-Segment Tube 
#define MAX_CS 8    //PIN "Cable Select" for the SPI connection of the 7-Segment Tube
#define MAX_DIN 10  //PIN "Data In" for the SPI connection of the 7-Segment Tube 
#define MAX_BRIGHTNESS 8

byte currentIntensity[max7219anz] = { 0 } ;
  
void SetupMax7219()
{
  Max7219_display[0]=LedControl(MAX_DIN, MAX_CLK, MAX_CS, 1);  //actual call of the LEDControl class with the correct PINs 
  //Max7219_display[1]=LedControl(/*DIN*/,/*CLK*/, /*CS*/, 1);  //uncomment this if you use a second Max7219 on one arduino. copy this line if you use more than 2. Make sure to enter the correct PINs the additional devices are connected to
  for (byte x=0;x<max7219anz;x++)
  {
    Max7219_display[x].shutdown(0,false);
    Max7219_display[x].setIntensity(0,0); 
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
  delay(1000);
  for (byte x=0;x<max7219anz;x++)
  {
    Max7219_display[x].shutdown(0,false);
    Max7219_display[x].setIntensity(0,0); 
    Max7219_display[x].setDigit(0,0,0,false);
    Max7219_display[x].setDigit(0,1,1,false);
    Max7219_display[x].setDigit(0,2,2,false);
    Max7219_display[x].setDigit(0,3,3,false);
    Max7219_display[x].setDigit(0,4,4,false);
    Max7219_display[x].setDigit(0,5,5,false);
    Max7219_display[x].setDigit(0,6,6,false);
    Max7219_display[x].setDigit(0,7,7,false);
  }
  delay(1000);
  for (byte x=0;x<max7219anz;x++)
    {Max7219_display[x].clearDisplay(0);}
}

void decreaseIntensity(byte x) {
	currentIntensity[x] -= 1;
	if (currentIntensity[x] < 0) { currentIntensity[x] = 0; }
	Max7219_display[x].setIntensity(0,currentIntensity[x]);
}

void increaseIntensity(byte x) {
	currentIntensity[x] += 1;
	if (currentIntensity[x] > MAX_BRIGHTNESS) { currentIntensity[x] = MAX_BRIGHTNESS; }
	Max7219_display[x].setIntensity(0,currentIntensity[x]);
}


void printOffset(byte p) {  
	int v = altimeterOffset;
    int ones;  
    int tens;  
    int hundreds; 

    boolean negative=false;

    if(v < -999 || v > 999)  
        return;  
    if(v<0) {  
        negative=true; 
        v=v*-1;  
    }
    ones=v%10;  
    v=v/10;  
    tens=v%10;  
    v=v/10; hundreds=v;  
    if(negative) {  
        //print character '-' in the leftmost column  
        Max7219_display[vars[p]->modIndex].setChar(0,7,'-',false);  } 
    else {
        //print a blank in the sign column  
        Max7219_display[vars[p]->modIndex].setChar(0,7,' ',false);  
    }  
    //Now print the number digit by digit 
    Max7219_display[vars[p]->modIndex].setDigit(0,6,(byte)hundreds,false);
    Max7219_display[vars[p]->modIndex].setDigit(0,5,(byte)tens,false); 
    Max7219_display[vars[p]->modIndex].setDigit(0,4,(byte)ones,false); 
}



void UpdateMAX7219(byte p)
{
  bool dp=false;
  char chars[8]=""; // store 8 characters (max one 7291 can display);

  if (configmode) {
    for (byte x=0; x<3; x++) {
    Max7219_display[vars[p]->modIndex].setDigit(0,x,8,true);	
    }
    printOffset(p);
    return;
  }
  
  // longVal = atol((*vars[p]->value.valString).c_str());  
     
  int dataLength = (*vars[p]->value.valString).length();

  char Wert[dataLength]="";

  memcpy(Wert, (*vars[p]->value.valString).c_str(), dataLength);
  
  byte stelle = 7-vars[p]->valIndex;
  for (byte x=0 ; x<dataLength; x++)
  {
    if (dp){dp=false;}
    
  if (Wert[x] == 'G') // BMSAIT sends a special character 'G', if altitude is <10000. In the real altimeter, the stripes are shown. We will show a blank
  {
    // outputSpecial0(p, stelle, dp);
    Max7219_display[vars[p]->modIndex].setChar(0,stelle,' ',dp);
  } else {	
    Max7219_display[vars[p]->modIndex].setChar(0,stelle,Wert[x],dp);
  }
    stelle--;
  }          
  

}
