#pragma once

// defines for the gauge conversion vector
#define GAUGERPM    0
#define GAUGEFTIT   1
#define GAUGEASI    2

struct
{
    unsigned short absolutwert_u;
    unsigned short absolutwert_o;
    unsigned short ticks_u;
    unsigned short ticks_o;
    unsigned short ticks_delta;
} GaugeConversion;



class miUtility {
public:
    unsigned short map(unsigned short x, unsigned short in_min, unsigned short in_max, unsigned short out_min, unsigned short out_max) {
        return ((unsigned long) ((x - in_min) * (out_max - out_min)) / (in_max - in_min) + out_min);
    }

    std::string getBinaryRep(int n) {
        std::string ans = "";
        // Check for each bit.
        for (int i = 31; i >= 0; i--) {
            // If i'th bit is set 
            if (n & (1 << i)) ans += '1';
            else ans += '0';
        }
        return ans;
    }

    void outputHexString(std::string miString) {
        for (int i = 0; i < miString.size(); i++) {
            std::cout << std::hex << (int)miString.at(i) << " ";
        }
        std::cout << std::endl;
    }

    void outputHexString(char miString[]) {
        const int size = sizeof(miString);
        for (int i = 0; i < size; i++) {
            std::cout << std::hex << (int)miString[i] << " ";
        }
        std::cout << std::endl;
    }

    // conversions for nonlinear gauges: FTIT, ASI, RPM


};

