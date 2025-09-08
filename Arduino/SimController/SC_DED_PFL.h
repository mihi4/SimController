  // V2.3  5.4.22
 // Display of the DED or the PFL on a 256x64 OLED display
 // inspired by the DEDuino coding by Uri_ba (https://pit.uriba.org/tag/deduino/)

  #include <U8g2lib.h>
  #include "FalconDEDFont.h"   //load font


// Declare screen Object
#if defined(DUE) || defined(DUE_NATIVE) || defined(MEGA)
  //arduino board with enough memory will use the unbuffered mode
    U8G2_SSD1322_NHD_256X64_F_4W_SW_SPI displayDED(U8G2_R0, /* clock=*/ 12, /*  data=*/ 14, /* cs=*/ 16, /* dc=*/ 13 /*, reset=  14 */ );
   //U8G2_SSD1322_NHD_256X64_F_4W_HW_SPI displayDED(U8G2_R0, /* CS=*/ 4, /* DC=*/ 2);
#else
  //arduino board with low memory will have to use the buffered mode   
	U8G2_SSD1322_NHD_256X64_F_4W_SW_SPI displayDED(U8G2_R0, /* clock=*/ 12, /*  data=*/ 14, /* cs=*/ 16, /* dc=*/ 13 /*, reset=  14 */ );
   //U8G2_SSD1322_NHD_256X64_1_4W_HW_SPI displayDED(U8G2_R0, /* CS=*/ 4, /* DC=*/ 2);
#endif

// How many milliseconds go before the displays reset after initillizing (for user inspection)
  #define PRE_BOOT_PAUSE 500
  #define POST_BOOT_PAUSE 1500
  #define DED_DRAW_PAUSE 200
  #define DEDINDEX 8  // index of first var to store the ded/pfd data
  #define DIMPIN A1
  #define DEDUPATEINTERVALL 330 
  
unsigned long DED_last_comm = millis();
unsigned long startRequest = 0;
bool powerOn=false;
bool sw=false;
byte lastBrightnessValue = 0;

void ClearDED();
void RealDED();

///draw empty display
void ClearDED()
{
//SendMessage("ClearDED start",1);
//debugTime('a');
  //char emptyLine[]="                        ";
  //String emptyLine="                        ";
  for (byte linie=DEDINDEX;linie<DEDINDEX+5;linie++)  
  {
    String search = (vars[DEDINDEX]->value.valString)->substring(0,vars[DEDINDEX]->value.valString->length()-1);
    vars[DEDINDEX]->value.valString->replace(search, "                        ");// memmove(DEDLines[linie],emptyLine,sizeof(emptyLine));
  }
//SendMessage("ClearDED end",1);
//debugTime('a');
}

///draw DED data from SharedMem
void RealDED()
{	//SendMessage("RealDED",1);
    #if defined(DUE) || defined(DUE_NATIVE) //|| defined(MEGA)
      displayDED.clearBuffer();     //strong arduinos: command for full buffered mode
    #else
      displayDED.firstPage(); //weak arduinos: command for paged mode
      do {
    #endif

      for (byte line = DEDINDEX; line < DEDINDEX+5; line++)
      {
        char tempDedLine[25];
        (vars[line+DEDINDEX]->value.valString)->toCharArray(tempDedLine, 25);
        displayDED.drawStr(DED_H_CONST, line * DED_CHAR_H + DED_V_CONST, tempDedLine);
      }
    #if defined(DUE) || defined(DUE_NATIVE) //|| defined(MEGA)
      displayDED.sendBuffer(); //strong arduinos: command for full buffered mode
    #else
      } while ( displayDED.nextPage() ); //weak arduinos: command for paged mode
    #endif
}


///draw a test picture
void TestDED()
{
	//SendMessage("TESTDed",1);
  #if defined(DUE) || defined(DUE_NATIVE) //|| defined(MEGA)
    // SendMessage("clearingBufferStrong",1);
    displayDED.clearBuffer();     //strong arduinos: command for full buffered mode
  #else
    displayDED.firstPage(); //weak arduinos: command for paged mode
    do {
  #endif
      //SendMessage("settingStrings",1);
	  displayDED.drawStr(23,15, "MICHIS F-16 SIMULATOR");	  	  
	  displayDED.drawStr(37, 35, "PILOT FAULT DISPLAY");
      /*displayDED.drawFrame(1,1,255,63);
      displayDED.drawFrame(32,16,195,32);
      displayDED.drawStr(30+DED_H_CONST, DED_V_CONST+11+DED_CHAR_H , "PILOT FAULT DISPLAY");
      /*displayDED.drawLine(128,1,128,16);
      displayDED.drawLine(128,48,128,63);
      displayDED.drawLine(1,32,32,32);
      displayDED.drawLine(227,32,255,32); */

  #if defined(DUE) || defined(DUE_NATIVE) // || defined(MEGA)
    (//SendMessage("sendingBuffer",1);
    displayDED.sendBuffer(); //strong arduinos: command for full buffered mode
  #else
    } while ( displayDED.nextPage() ); //weak arduinos: command for paged mode
  #endif
}

void setDEDBrightness()
{
 int potVal = analogRead(DIMPIN);
 int brightnessValue = map(potVal, 0, 1023, 0, 16);
 if (brightnessValue != lastBrightnessValue) 
 {
    displayDED.sendF("C7h", brightnessValue);
 }
}

//setup the OLED
void SetupDED()
{
  //pinMode(DIMPIN,INPUT);
  //SendMessage("Setting up DED",1);
  //setDEDBrightness();
  delay(PRE_BOOT_PAUSE);
  displayDED.begin();
  displayDED.setFont(FalconDED);
  displayDED.setFontPosTop();
  TestDED();
  delay(POST_BOOT_PAUSE);
  ClearDED();
  //SERIALCOM.setTimeout(500);
}


///updates the output of the DED on the OLED
void DrawDED(bool mode)
{
  if (mode) {
    RealDED();
  } else {
	 //SendMessage("callingTestDED",1); 
     TestDED();
  }
}



///update the DED
void UpdateDED()
{
  //SendMessage("updatingDED",1);
  /*  FIXXXME - remove comment if everything works
  if ((millis()-lastInput)>10000) //if no data was recieved within 10 seconds, shut down display
  {
    if (!debugmode) //in testmode, display remains on regardless of data transfer status
    {
      if (powerOn)
      {
        ClearDED();
        displayDED.setPowerSave(1); //show nothing
        powerOn=false;
      }
    }
    return;
  }
  if (!powerOn)
  {
    displayDED.setPowerSave(0);
    powerOn=true;
  }
  */
  
  /*if (DEDLines[0][0]!='T')  //PC is not in 3D
  { SendMessage("2D",1); DrawDED(false);}          //show testscreen
  else if (DEDLines[1][0]!='T')
  {
    ClearDED();               //PC is in 3D, Avionics are off
    DrawDED(true);
	SendMessage("3D AV off",1);
  }
  else

  { SendMessage("3D AV on",1); DrawDED(true); }          //PC is in 3D, Avionics are on
   */
   DrawDED(true);
}
