// SimControl.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <Windows.h>

#include "lib/FalconReader.h"
#include "lib/DCSReader.h"


int main(int argc, char* argv[])
{   
    std::cout << "Hello World! Press LSHIFT/LCTRL/LALT + BACKSPACE to quit\n";
    if (argc > 1) {
        if (strcmp(argv[1], "BMS") == 0 ) {
            std::cout << "Looking for BMS\n";
        }
        if (strcmp(argv[1], "DCS") == 0) {
            std::cout << "Looking for DCS\n";
        }
    }
    for (int i = 1; i < argc; i++) {
        std::cout << "Parameter Number " << i << " Had the Value " << argv[i] << "." << std::endl;
    }
    Sleep(1400);
    std::cout << "FOUND IT!            \n";
    while (true) {  // Loop function ;-)
        std::cout << ".";
        if ( (GetKeyState(VK_LCONTROL) & 0x8000) && (GetKeyState(VK_LSHIFT) & 0x8000) && (GetKeyState(VK_LMENU) & 0x8000) && (GetKeyState(VK_BACK) & 0x8000)){ break;  }
        
        Sleep(200);
    }
    
    std::cout << "\n\nquitting!\n";

    return(0);
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
