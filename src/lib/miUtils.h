#pragma once

class miUtility {
public:
    long map(long x, long in_min, long in_max, long out_min, long out_max) {
        return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
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
};

