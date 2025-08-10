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
	{40,   3,   157,   1,   65535,    0,   0}, 	// HYDPRESS A
	{41,   12,   168,   1,   65535,    0,   0}, 	// HYDPRESS B
	{37,   10,   160,   65535,   1,    0,   0},  	// FUEL QTY Fwd
	{36,   10,   180,   65535,   1,    0,   0}  	// FUEL QTY Aft*/
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


void UpdateServo(byte id)
{
	/* char buf[6];

	 if (id == 2) {
		if (debugmode){SendMessage("updatingServo",1);}
		if (debugmode){SendMessage(datenfeld[id].ID,1);}
		if (debugmode){SendMessage(datenfeld[id].wert,1);}
		ltoa(newValue, buf, 10);
		if (debugmode){SendMessage(buf,1);}
		ltoa(servodata[datenfeld[id].target].last, buf, 10);
		if (debugmode){SendMessage(buf,1);}
	}*/

  if (servodata[datenfeld[id].target].lu + SERVOSLEEPTIME < millis())
  {
	//if (debugmode){SendMessage("detaching Servo",1);}
    servo[datenfeld[id].target].detach();                        //disable servo if no new signal for more than 5 seconds
	servodata[datenfeld[id].target].lu = millis();
  }
  else if (servodata[datenfeld[id].target].lu + SERVODELAY < millis())
  {
    servodata[datenfeld[id].target].lu = millis();  //pause servo

	long newValue = 0;
	if (datenfeld[id].format == 'f') {
		// float tmpVal = atof(datenfeld[id].wert);
		newValue = 10 * atof(datenfeld[id].wert); //tmpVal * 10;
	} else {
		newValue = atol(datenfeld[id].wert);
	}
	
    if (servodata[datenfeld[id].target].last != newValue)
    {
	  uint16_t winkel;
      servodata[datenfeld[id].target].last = newValue;
      
	  if (!servo[datenfeld[id].target].attached()) servo[datenfeld[id].target].attach(servodata[datenfeld[id].target].pIN);  //reactivate servo
	  
	  winkel = map(servodata[datenfeld[id].target].last, servodata[datenfeld[id].target].a_ug, servodata[datenfeld[id].target].a_og, servodata[datenfeld[id].target].p_ug, servodata[datenfeld[id].target].p_og);
	  servo[datenfeld[id].target].write(winkel);
    }
  }
}
