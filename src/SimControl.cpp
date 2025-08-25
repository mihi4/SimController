// SimControl.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include "SimControl.h"

bool checkParameter(int argNum) {
    if (argNum == 2) return false;
    return true;
}

char parseSimParameter(char* argv[]) {
    // std::cout << "Parsing " << argv[1] << std::endl;
    if (strcmp(argv[1], "BMS") == 0) {
        // std::cout << "parsed BMS\n";
        return BMS;
    }
    else if (strcmp(argv[1], "DCS") == 0) {
        // std::cout << "parsed DCS\n";
        return DCS;
    }
    else if (strcmp(argv[1], "MSFS") == 0) {
        // std::cout << "parsed MSFS\n";
        return MSFS;
    }
    else {
        return NULL;
    }

}
 
std::unique_ptr<DataReader> createDataReader(short selectedSim) {
    switch (selectedSim) {
    case BMS:
        return std::make_unique<BMSReader>();
        break;
    case DCS:
        return std::make_unique<DCSReader>();
        break;
    case MSFS:
        return std::make_unique<MSFSReader>();
        break;
    default:
        return nullptr;
        break;
    }    
}

int main(int argc, char* argv[])
{   
    std::cout << "*************************************************************\n";
    std::cout << "*                                                           *\n";
    std::cout << "* Hello, Pilot! Press LSHIFT/LCTRL/LALT + BACKSPACE to quit *\n";
    std::cout << "*                                                           *\n";
    std::cout << "*************************************************************\n";

    /*********************************************
       check if commandline parameter fit 
       if not, print message and quit
    **********************************************/
    // 
    if (checkParameter(argc)) {
        std::cout << "Wrong parameter! Usage: SimControl.exe BMS|DCS|MSFS";
        return 0;
    }

    short selectedSim = parseSimParameter(argv);
    if (!selectedSim) {
        std::cout << "Sim parameter not supported, Usage: SimControl.exe BMS|DCS|MSFS";
        return 0;
    }

    auto reader = createDataReader(selectedSim);

    if (!reader) {
        std::cout << "!!!!   ERROR creating DataReader   !!!!";
        return 0;
    }
    
    /*********************************************
      setup the needed variables and objects
    **********************************************/

    F16Data data;
    F16Data prevData;
    miUtility util;
    
    bool simConnected = false;

    std::cout << "------ Setting up Controllers ------\n";    
    
    ControllerHandler cHandler("dummyFilename");  // later read config from file, maybe

    cHandler.setupControllers();
    cHandler.showControllers();
    Sleep(2000); // let arduinos reboot
    cHandler.readControllerComms();
    Sleep(500);

    std::cout << "------ Controller Setup done  ------\n";
    std::cout << std::endl;
    /****************************************
     
                    main loop
    
    *****************************************/    
    
    while (true) {
        if (!simConnected) {
            std::cout << "connecting to sim...\r";
            if (reader->connectToSim()) {
                simConnected = true;
                reader->readF16Data(&data);
                std::cout << "\nConnected to " << argv[1] << data.simVersion << "\n";
            }
        }
                        
        if (reader->connectToSim()) {            
            reader->readF16Data(&data);         
            if (!prevData.isSameAs(data)) {  // only send data if anything has changed 
                cHandler.updateControllers(&data, &prevData);
            }
            prevData = data;
            //std::cout << "mapping" << util.map(data.fuelFWD, 0, 42000, 0, 65534) << "\r";
        } else {
            simConnected = false; // try again next run
        }        
        
        //std::cout << "altPointer: " << data.altPointer << " slip: " << data.adiSideslip << " pitch: " << data.adiPitch << " roll: " << data.adiRoll << " ilsHor: " << data.adiIlsHorPos << " ilsVer: " << data.adiIlsVerPos << "\n";
    
        cHandler.readControllerComms();
        
        // check for quit keycommand LCTRL+LSHIFT+LALT+BACKSPACE
        if ((GetKeyState(VK_LCONTROL) & 0x8000) && (GetKeyState(VK_LSHIFT) & 0x8000) && (GetKeyState(VK_LMENU) & 0x8000) && (GetKeyState(VK_BACK) & 0x8000)) { break; }
        
        Sleep(300);
        
    }

    std::cout << "\n\nquitting!\n";   

    return 0;
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
