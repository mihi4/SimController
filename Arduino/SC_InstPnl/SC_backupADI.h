// DCS F-16 - Standby Attitude Indicator (SAI) display
// in use: ESP32S3 chipset and round 1.28 inch GC9A01 SPI display

// SAI pitch trim -> potentiometer still tbd during physical design
#include <TFT_eSPI.h>	// Library has to be adjusted for the display driver and resolution in use !!!
#include <SPI.h>
#include "SAI_graphics.h"	// self made bitmaps for the SAI: ColorDepth 16bit

// Easily to remember names for the bitmaps in use
  #define BAND SAI_band		// whole SAI scale
  #define BANK_B SAI_bank_b	// bank indication (black stripe bottom of the scale)
  #define BANK_T SAI_bank_t	// bank indication (blue stripe top of the scale) 
  #define WING SAI_wing		// indication of the horizontal bar (aircraft symbol)
  #define FLAG SAI_flag		// OFF flag

/*/ Easily to remember names for the encoder
#define pitchTrimA 37
#define pitchTrimB 36
#define sadiCage 35
*/ 

// Definition of the instrument Layers (Sprites)
  TFT_eSPI tft=TFT_eSPI();
  TFT_eSprite gaugeBack = TFT_eSprite(&tft);
  TFT_eSprite band = TFT_eSprite(&tft);
  TFT_eSprite bank_b = TFT_eSprite(&tft);
  TFT_eSprite bank_t = TFT_eSprite(&tft);
  TFT_eSprite wing = TFT_eSprite(&tft);
  TFT_eSprite flag = TFT_eSprite(&tft);

// Startup values to center the SAI display
  int16_t  SaiPitch = 421;  // set SAI band to center: 1012(bitmap height) - 170(Sprite height) = 842 -> 842 / 2 = 421 px (y-direction)
  int16_t  SaiBank = 0;  // level: 0°
  int16_t  SaiPitchAdj = 115;  // set adjustment to center: 115 px
  int16_t  SaiWarningFlag = 0; // set warning flag: ON (65535 = rotation 0°); OFF (0 = rotation -25°)

// setup PWM for the Backlighting
  const int pwmFreq = 12000;		// works best with 12000 Hz
  const int pwmResolution = 8;  	// 8bit PWM signal from 0 to 255
  const int pwmLedChannelTFT = 0;	// PWM Channel 1
  int brightness = 100;			// startup brightness value


// ---------- FUNCTIONS declaration ----------




void createGauge(){
  gaugeBack.setColorDepth(16);
  gaugeBack.createSprite(240, 240);
  gaugeBack.fillSprite(TFT_BLACK);
}

void createBand(int16_t SaiPitch){
  band.setColorDepth(16);
  band.deleteSprite();
  band.createSprite(240, 170); 
  band.setPivot(120, 85);
  band.fillSprite(TFT_BLACK);
  band.pushImage(20, -SaiPitch, 200, 1012, BAND);
}

void createBank_B(){
  bank_b.setColorDepth(16);
  bank_b.deleteSprite();
  bank_b.createSprite(240, 35); 
  bank_b.setPivot(120, -85);
  bank_b.pushImage(0, 0, 240, 35, BANK_B);
}

void createBank_T(){
  bank_t.setColorDepth(16);
  bank_t.deleteSprite();
  bank_t.createSprite(240, 35); 
  bank_t.setPivot(120, 120);
  bank_t.pushImage(0, 0, 240, 35, BANK_T);
}

void createWing(int16_t SaiPitchAdj){
  wing.setColorDepth(16);
  wing.deleteSprite();
  wing.createSprite(240, 30); 
  wing.setPivot(120, SaiPitchAdj - 110);
  wing.fillSprite(TFT_TRANSPARENT);
  wing.pushImage(0, 0, 240, 30, WING);
}

void createFlag(int16_t SaiWarningFlag){
  flag.setColorDepth(16);
  flag.deleteSprite();
  flag.createSprite(62, 80);
  flag.setPivot(0, 80);
  flag.fillSprite(0x0000);
  flag.pushImage(0, 0, 62, 80, FLAG);
  gaugeBack.setPivot(0, 125);
  flag.pushRotated(&gaugeBack, SaiWarningFlag, 0x0000);
  gaugeBack.setPivot(120, 120);
}


void createBitmaps(int16_t SaiPitch, int16_t SaiPitchAdj, int16_t SaiWarningFlag){
  createGauge();
  createBand(SaiPitch);
  createBank_B();
  createBank_T();
  createWing(SaiPitchAdj);
}

void plotGauge(int16_t SaiBank, int16_t SaiWarningFlag){
  
  bank_b.pushRotated(&gaugeBack, SaiBank);
  bank_t.pushRotated(&gaugeBack, SaiBank);
  band.pushRotated(&gaugeBack, SaiBank);
  wing.pushRotated(&gaugeBack, 0, 0x0000);
  if (SaiWarningFlag > -20)	// show OFF-Flag only if it's within the round display area ( 0° to -20°)
    {
    createFlag(SaiWarningFlag);
    }
  gaugeBack.pushSprite(0 , 0);
}




// ---------- DCS.BIOS stuff here ----------
// --- SAI Bank -> rotating the band left / right
void onSaiBankChange(unsigned int newValue) {
  SaiBank = map(newValue, 0, 65535, 180, -180); // bank angle from -180 to +180 degrees
  createBitmaps(SaiPitch, SaiPitchAdj, SaiWarningFlag);
  plotGauge(SaiBank, SaiWarningFlag);
}
// DcsBios::IntegerBuffer saiBankBuffer(0x44b0, 0xffff, 0, onSaiBankChange);


// --- SAI Pitch -> moving the band up / down
void onSaiPitchChange(unsigned int newValue) {
  SaiPitch = map(newValue, 0, 65535, 850, 4); // 1012 (bitmap height) - 170 (Sprite height) = 842 px (plus 8 px for adjustment)
  // ->> to be corrected by the real instrument height (240px) instead of the Sprite height (170px)
  createBitmaps(SaiPitch, SaiPitchAdj, SaiWarningFlag);
  plotGauge(SaiBank, SaiWarningFlag);
}
// DcsBios::IntegerBuffer saiPitchBuffer(0x44ae, 0xffff, 0, onSaiPitchChange);


// --- SAI pitch adjustment -> moving the wing up / down
void onSaiPitchTrimChange(unsigned int newValue) {
  SaiPitchAdj = map(newValue, 0, 65535, 75, 155); // 120 (tft center) - 5 (midpoint) = 115 -> 115 + 40 (bitmap height) and 115 - 40 (bitmap height) 
  createBitmaps(SaiPitch, SaiPitchAdj, SaiWarningFlag);
  plotGauge(SaiBank, SaiWarningFlag);
}
// DcsBios::IntegerBuffer saiPitchTrimBuffer(0x446c, 0xffff, 0, onSaiPitchTrimChange);

/*/ --- input for the SAI pitch adjustment (adjust aircraft reference symbol)
DcsBios::RotaryEncoder saiPitchTrim("SAI_PITCH_TRIM", "+3200", "-3200", pitchTrimA, pitchTrimB);

// --- input for the SAI cage knob (pull to cage)
DcsBios::Switch2Pos saiCage("SAI_CAGE", sadiCage);
*/
// --- show the SAI OFF flag
void onSaiOffFlagChange(unsigned int newValue) {
  SaiWarningFlag = map(newValue, 0, 65535, -25, 0); // set warning flag: ON (0° = 65535); OFF (-25° = 0)
  createBitmaps(SaiPitch, SaiPitchAdj, SaiWarningFlag);
  plotGauge(SaiBank, SaiWarningFlag);
}
// DcsBios::IntegerBuffer saiOffFlagBuffer(0x44b2, 0xffff, 0, onSaiOffFlagChange);


// ---------- Flight Instrument Lights --------------
//void onPriInstPnlBrtKnbChange(unsigned int newValue) {  // Min. Value 0 - Max. Value: 65535
//  brightness = (newValue/256);                     // for an 8 bit PWM signal
//  brightness = constrain(brightness, 30, 255);     // limit range of brightness between 30 and 255
//  ledcWrite(pwmLedChannelTFT, brightness);         // write PWM signal (0 - 255) to PWM Channel 1
//}
//DcsBios::IntegerBuffer priInstPnlBrtKnbBuffer(0x4410, 0xffff, 0, onPriInstPnlBrtKnbChange);


void setupBackupADI() {
    
// setup the display
  tft.init();
  tft.setRotation(0);
  tft.fillScreen(0x0000); 	// clear the whole screen area
  tft.setPivot(120, 120);	// midpoint of the resolution (240x240)

// control LCD brightness
//  ledcSetup(pwmLedChannelTFT, pwmFreq, pwmResolution);
// ledcAttachPin(TFT_BL, pwmLedChannelTFT);	// see library configuration for the pin setup (TFT_BL)
//  ledcWrite(pwmLedChannelTFT, 90);

// setup of graphics (bitmaps) and initiate the master functions (no need to call them within the LOOP-section)
  createBitmaps(SaiPitch, SaiPitchAdj, SaiWarningFlag);
  plotGauge(SaiBank, SaiWarningFlag);
  
}

// ---------- Main Loop (if possible, keep emty) ----------
void UpdateBackupADI(byte p) {
  
}
// ---------- END of Main Loop ----------

