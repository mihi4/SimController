#ifndef MM5451_h
#define MM5451_h
#endif
#include <Arduino.h>

class MM5451
{
    public:
        void pulseCLK();
        void outputDataBits();
        void setOutput(unsigned char pin, bool state);
		bool getOutput(unsigned char pin);
        void clearAll();
        void lightAll();
        void setClockPin(unsigned char pinNumber);
        void setDataPin(unsigned char pinNumber);
		
    
        MM5451(unsigned char clockpin, unsigned char datapin); 
 
    private:
        void sendStartBit();

        unsigned char CLK;
        unsigned char DATA;  // pins to connect to
        bool databits[35];

};