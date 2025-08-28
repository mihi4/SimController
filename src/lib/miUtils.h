#pragma once
#include <vector>

// defines for the gauge conversion vector
#define GAUGENUM    3
// index numbers of the gauges
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

std::vector<unsigned int> gaugeRpmVal = {60,70,75,80,85,90,100,105,107,110};
std::vector<unsigned int> gaugeRpmPos = {22500,26000,32250,38500,51000,55750,57250,63500,65535,65535};

std::vector<unsigned int> gaugeFtitVal = {2,7,10,12};
std::vector<unsigned int> gaugeFtitPos = {0,19660,57015,65535};

std::vector<unsigned int> gaugeAsiVal = {50,90,100,200,300,400,600,850};
std::vector<unsigned int> gaugeAsiPos = {0,2659,6269,26214,36092,43690,55087,65535};

std::vector<unsigned int> rawDatas= { gaugeRpmVal, gaugeFtitVal, gaugeAsiVal}; 
std::vector<unsigned int> calcDatas= { gaugeRpmPos, gaugeFtitPos, gaugeAsiPos}; 


std::vector<GaugeConversion> RPMGauge;
std::vector<GaugeConversion> FTITGauge;
std::vector<GaugeConversion> ASIGauge;

std::vector<GaugeConversion> allConvertions = {RPMGauge, FTITGauge, ASIGauge};

class miUtility {
public:

    void createGaugeMappings() {
        for (int i=0; i<allConvertions.size(); i++) {
            GaugeConvertion gauge = allConvertions[i];
            for (byte x = 0; x <= gauge.size(); x++)
            {
                if (x == 0)
                {
                    gauge[0].absolutwert_u = 0;
                    gauge[0].absolutwert_o = rohdatenSchwelle_o[0];
                    gauge[0].ticks_u = 0;
                    gauge[0].ticks_o = rohdatenTicks_o[0];
                    gauge[0].ticks_delta = rohdatenTicks_o[0];
                }
                else
                {
                    gauge[x].absolutwert_u = gauge[x - 1].absolutwert_o;
                    gauge[x].absolutwert_o = rohdatenSchwelle_o[x];
                    gauge[x].ticks_u = gauge[x - 1].ticks_o;
                    gauge[x].ticks_o = (UInt16)rohdatenTicks_o[x];
                    gauge[x].ticks_delta = (UInt16)(gauge[x].ticks_o - gauge[x].ticks_u);
                }
            }
        }        
    }

    unsigned int TrimGauge(float rawVal, int gaugeNum)
    {
        gauge = allConvertions.at(gaugeNum);
        if (gauge == null) return 0;
        if ((rawVal < gauge[0].absolutwert_u) || (rawVal > gauge[gauge.size()-1].absolutwert_o)) return 0;

        for (byte x = 0; x <= gauge.size()-1; x++)
        {
            if ((rawVal >= gauge[x].absolutwert_u) && (rawVal < gauge[x].absolutwert_o))
            { return (unsigned int) (gauge[x].ticks_u + map(rawVal, gauge[x].absolutwert_u, gauge[x].absolutwert_o, 0, gauge[x].ticks_delta)); }
        }
        return 65535;
    }

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

