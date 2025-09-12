// settings and functions to drive servo motors (directly connected to the arduino)
// V1.3.7 26.09.2021

//target= reference link to the line of the servodata table of this module
//ref2= not used
//ref3= not used
//ref4= not used
//ref5= not used

#include <Servo.h>

#define SERVODELAY 80
#define SERVOSLEEPTIME 20000

typedef struct
{
byte pIN;           //PIN the motor is connected to
byte p_ug;          //min arc for the motor (must not be less than 0!)
byte p_og;          //max arc for the motor (must not be more than 180!)
unsigned int a_ug;           //min absolut value that might be displayed (i.e.   0 for a speed indicator)
unsigned int a_og;           //max absolut value that might be displayed (i.e. 800 for a speed indicator)
unsigned int last;           //previous value
unsigned long lu;   //last update
} Servodata;


Servodata servodata[] =
{// pIN  p_ug  p_og  a_ug  a_og  last  lu
	{40,   3,   157,   0,   4000,    0,   0}, 	// HYDPRESS A
	{41,   12,   168,   0,   4000,    0,   0}, 	// HYDPRESS B
	{37,   10,   160,   4200,   0,    0,   0},  	// FUEL QTY Fwd
	{36,   10,   180,   4200,   0,    0,   0}  	// FUEL QTY Aft
};
const int SERVOZAHL = sizeof(servodata)/sizeof(servodata[0]);
Servo servo[SERVOZAHL];

void SetupServo()
{
  for (byte lauf=0;lauf<SERVOZAHL;lauf++)
  {
	
    servo[lauf].attach(servodata[lauf].pIN);
    servo[lauf].write(servodata[lauf].p_ug);
    delay(500);
    servo[lauf].write(servodata[lauf].p_og);
    delay(500);
    /*servo[lauf].write(90);
    delay(500); */
	servo[lauf].write(servodata[lauf].p_ug);
	delay(300);	
    servo[lauf].detach();
	servodata[lauf].lu = millis();  
  }
}

void Servo_FastUpdate()
{}

void Servo_Zeroize(bool mode)
{
  for (byte lauf=0;lauf<SERVOZAHL;lauf++)
  {
    if (!servo[lauf].attached())
        {servo[lauf].attach(servodata[lauf].pIN);}  //reactivate servo
    servo[lauf].write(servodata[lauf].p_ug);
    if (mode)
    {
      delay(500);
      servo[lauf].write(servodata[lauf].p_og);
      delay(500);
      servo[lauf].write(servodata[lauf].p_og - servodata[lauf].p_ug);
      delay(500);
    }
	servo[lauf].write(servodata[lauf].p_ug);
	delay(300);
    servo[lauf].detach();
  }
}


void UpdateServo(byte p)
{
  
  sendReadBackString("UpdateServo");
  if (servodata[vars[p]->modIndex].lu + SERVOSLEEPTIME < millis())
  {
	//if (debugmode){SendMessage("detaching Servo",1);}
    servo[vars[p]->modIndex].detach();                        //disable servo if no new signal for more than 5 seconds
	servodata[vars[p]->modIndex].lu = millis();
  }
else if (servodata[vars[p]->modIndex].lu + SERVODELAY < millis())
  {
    servodata[vars[p]->modIndex].lu = millis();  //pause servo

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
  
  sprintf(rbMsg, "updateServo, longVal: %u", longVal);
  sendReadBackString(rbMsg);
	/*if (vars[p].format == 'f') {
		// float tmpVal = atof(vars[p].wert);
		newValue = 10 * atof(vars[p].wert); //tmpVal * 10;
	} else {
		newValue = atol(vars[p].wert);
	}*/
	
    if (servodata[vars[p]->modIndex].last != longVal)
    {
	  uint16_t winkel;
      servodata[vars[p]->modIndex].last = longVal;
      
	  if (!servo[vars[p]->modIndex].attached()) servo[vars[p]->modIndex].attach(servodata[vars[p]->modIndex].pIN);  //reactivate servo
	  
	  winkel = map(servodata[vars[p]->modIndex].last, servodata[vars[p]->modIndex].a_ug, servodata[vars[p]->modIndex].a_og, servodata[vars[p]->modIndex].p_ug, servodata[vars[p]->modIndex].p_og);
	  sprintf(rbMsg, "updateServo, Winkel: %u", winkel);
  sendReadBackString(rbMsg);
    servo[vars[p]->modIndex].write(winkel);
    }
  }
}
