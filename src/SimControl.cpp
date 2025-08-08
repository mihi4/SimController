// SimControl.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include "SimControl.h"

bool checkParameter(int argNum) {
    if (argNum == 2) return false;
    return true;
}

char parseSimParameter(char* argv[]) {
    std::cout << "Parsing " << argv[1] << std::endl;
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

    if (selectedSim == BMS) {
        return std::make_unique<BMSReader>();
    }
    else if (selectedSim == DCS) {
        return std::make_unique<DCSReader>();
    }
    else if (selectedSim == MSFS) {
        return std::make_unique<MSFSReader>();
    }
    else {
        return nullptr;
    }
}

void setupControllers(std::vector<Controller> allCs) {  // cNum is the number of controllers in the eventual config file
    std::vector<unsigned char> fields = { FUELAFT, FUELFWD, FUELTOTAL, HYDA, HYDB, EPUFUEL, CABINPRESS, CAUTIONPANELLIGHTS };   
    // short varCount = fields.size();  // sizeof(fields) / sizeof(fields[0]);
    //std::cout << "varcount: " << varCount << "\n";
    Controller c1("RightAUX", "\\\\.\\COM1", 115200, fields); 
    // std::cout << "controllername: " << c1.getName() << "\n";
    //c1.datafields = fields;
    for (int i = 0; i < fields.size(); i++) {
        c1.setDataField(i, fields[i]);
    }
    allCs[0] = c1;
    
}

void updateControllers(std::vector<Controller> allCs) {
    
    for (int i = 0; i < allCs.size(); i++) {
        allCs[i].updateController();
    }

}


int main(int argc, char* argv[])
{   
    std::cout << "*************************************************************\n";
    std::cout << "*                                                           *\n";
    std::cout << "* Hello, Pilot! Press LSHIFT/LCTRL/LALT + BACKSPACE to quit *\n";
    std::cout << "*                                                           *\n";
    std::cout << "*************************************************************\n";

    Sleep(200);
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
    miUtility util;
    
    bool simConnected = false;    

    /* 
    for first iteration create Arduinos in code, later read from config file
    */    
    char controllerNum = 1;
    std::vector<Controller> allControllers;
    //Controller * allControllers = new Controller[controllerNum];
       
    std::cout << "### Setting up Controllers\n";
    setupControllers(allControllers);
    std::cout << "--- Controller creation done\n";
    //Controller c1("RightAUX", "COM1", 115200, { FUELAFT, FUELFWD, FUELTOTAL, HYDA, HYDB, EPUFUEL, CABINPRESS, CAUTIONPANELLIGHTS });
    //srand(time(NULL)); // Seed the time
    /****************************************
     
                    main loop
    
    *****************************************/

    while (true) {
        if (!simConnected) {
            std::cout << "connecting to sim...\r";
            if (reader->connectToSim()) {
                simConnected = true;
                std::cout << "\nConnected to " << argv[1] << "\n";
            }
        }
                        
        if (reader->connectToSim()) {
            reader->readF16Data(&data);
            
            updateControllers(allControllers);
            //std::cout << "mapping" << util.map(data.fuelFWD, 0, 42000, 0, 65534) << "\r";
        }

        // check for quit keycommand LCTRL+LSHIFT+LALT+BACKSPACE
        if ((GetKeyState(VK_LCONTROL) & 0x8000) && (GetKeyState(VK_LSHIFT) & 0x8000) && (GetKeyState(VK_LMENU) & 0x8000) && (GetKeyState(VK_BACK) & 0x8000)) { break; }

        Sleep(200);
        
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
