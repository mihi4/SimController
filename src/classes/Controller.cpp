#include "Controller.h"

/*Controller::Controller(std::string name, unsigned char portNum, long baudrate = 115200, std::vector<unsigned char> datafieldsIn = {0})
    : baudrate(baudrate), controllerName(name), portNum(portNum), datafields(datafieldsIn) {
    
    //datafields = datafieldsIn; // new unsigned char[datafieldCount];
    //int varCount = datafields.size();   
    std::cout << "my contructor, datafieldcount:--" << datafields.size() << "--\n";
}*/

Controller::Controller(std::string name, WindowsSerial* serPort, long baudrate = 115200)
    : baudrate(baudrate), controllerName(name) {
    
    datafields = {}; // new unsigned char[datafieldCount];    
    //int varCount = datafields.size();
    std::cout << "my contructor, datafieldcount:--" << datafields.size() << "--\n";
}

/*Controller::Controller() {
    std::cout << "standard contructor\n";
}*/

Controller::~Controller() {
    //delete[] datafields;
    //delete  serial port
    //serialPort->close();
    //delete serialPort;
    
    
}

////////////////////////////////////////
// internal helper functions
////////////////////////////////////////

void debugUpdateString(std::vector<char> updateString) {
    std::string debugString = "";
    for (int i = 0; i < updateString.size(); i++) {
        if (updateString[i] < 32 || updateString[i]>127) {
            if (updateString[i] == 0) break;
            debugString += ".";
            debugString += std::to_string(updateString[i]);
            debugString += "_";
        }
        else {
            char buf = updateString.at(i);
            debugString += buf;
        }
    }
    std::cout << "debugString: " << debugString << "\n";
}

std::vector<unsigned char> Controller::splitValue(int value, int size) {
    std::vector<unsigned char> bytes;
    for (size_t i = 0; i < size; ++i) {
        // Maskieren und Verschieben, um das i-te Byte zu erhalten
        unsigned char byte = (value >> (8 * i)) & 0xFF;
        bytes.push_back(byte);
    }
    return bytes;
}

///////////////////////////////////////////////
//      Serial communications
///////////////////////////////////////////////

void Controller::readFromPort() {
    static boolean recvInProgress = false;
    static byte ndx = 0;
    char startMarker = CMDSTART;
    char endMarker = CMDEND;
    byte rb;

    byte escapeBytenum = -2;  // escape "counter", shows the index, where escape character (dec 27, hex 0x1B) was found; -1 is bad, sind index 0-1 is -1 :-D

    while (Serial1.available() > 0 && newData == false) {
        rb = Serial1.read();

        if (recvInProgress) {
            if (rb != endMarker) {
                if (rb == CMDESC) {
                    //Serial1.print("escape found, ndx=");Serial1.println(ndx, DEC);
                    //msg.concat("esc1.");

                    if (escapeBytenum == (ndx - 1)) {
                        //Serial1.print("2nd escape found, escBytenum=");Serial1.println(escapeBytenum, DEC);
                        //msg.concat("esc2.");
                        receivedBytes[escapeBytenum] = rb;
                        escapeBytenum = -2; // reset escape "counter"
                    }
                    else {
                        escapeBytenum = ndx;
                        //Serial1.print("escapeBytenum: ");Serial1.println(escapeBytenum);
                        ndx++;
                    }
                }
                else {
                    receivedBytes[ndx] = rb;
                    ndx++;
                }
                if (ndx >= numBytes) {
                    ndx = numBytes - 1;
                }
            }
            else {
                //Serial1.println("endmarker found");
                if (escapeBytenum == (ndx - 1)) {  // was previous byte escaping byte?	
                  //Serial1.println("prev byte was escape, setting prev index to rb");
                    receivedBytes[escapeBytenum] = rb;
                }
                else {
                    //Serial1.println("final delimiter found");
                    receivedBytes[ndx] = '\0'; // terminate the string
                    recvInProgress = false;
                    numReceived = ndx;  // save the number for use when printing
                    ndx = 0;
                    newData = true;
                }
                escapeBytenum = -2;
            }
        }
        else if (rb == startMarker) {
            recvInProgress = true;
            numReceived = 0;
        }
    }
}

bool Controller::parseControllerInfo() {
    
    byte count = 0;
    byte startIndex = 1;
    
    while (true) {
        count++;
        if (receivedBytes[count] == ';') break;        
        if (count == numBytes - 1) {
            std::cout << "ERROR parsing Controller name, end of received bytes reached!";
            return false;
        }
    }
    std::string tempName(receivedBytes + startIndex, count-1);    

    startIndex += count;
    byte varNum = receivedBytes[startIndex];

    startIndex += 1;
    // now add the variable numbers to the datafields array;
    for (int i = 0; i < varNum; i++) {
        datafields.push_back(receivedBytes[startIndex + i]);
    }

    std::cout << controllerName << " on port COM" << std::dec << (int)portNum << " identifies as " << tempName << ", variabe count is " << std::dec << (int)varNum << std::endl;
    std::cout << "Datafields: ";
    for (int i = 0; i < datafields.size(); i++) {
        std::cout << std::dec << (int)datafields.at(i) << " ";
    }
    std::cout << std::endl;
}

bool Controller::parseSerialInput() {
    if (newData == true) {

        /*Serial1.print("received Bytes: ");Serial1.println(numReceived, DEC);
        for (int i=0;i<numReceived;i++) {
          Serial1.print(receivedBytes[i], HEX);Serial1.print("-");
        }
        Serial1.println();*/
        switch (receivedBytes[0]) {
        case 'I':  // answer to connect command
            std::cout << controllerName << ":connection string received!\n";
            connected = true;
            if (!parseControllerInfo()) {
                return false;
            }
            break;
        case 'Y':  // answer to heartbeat command
            std::cout << controllerName << " is still here\n";
            connected = true;            
            break;  
        case 'R':  // answer to heartbeat command
            std::cout << controllerName << "rB: " << receivedBytes << std::endl;
            break;
        default:        
            std::cout << "ERROR parsing, no known command found\n";
            return false;
            break;
        }
        newData = false;
        return true;
    }


    //if (receivedBytes[0] == 'C') Serial1.println("Connect command!");

}

void Controller::readSerial() {
    readFromPort();
    if (numReceived) {
        if (!parseSerialInput()) {
            std::cout << "ERROR parsing serial input";
        }
    }
    numReceived = 0;

}

void Controller::sendDataUpdate(std::vector<char> updateString) {
    debugUpdateString(updateString);
    
}


bool Controller::initialize() {
    Serial1.begin(115200, portNum);
    return true;
}

void Controller::connect()
{
    std::cout << "connecting to " << controllerName << " on port COM" << std::dec << (int)portNum << std::endl;
    if (Serial1.connected()) {
        Serial1.println("<C>");
    }
    
    
    connected = true;
}
void Controller::disconnect()
{
    //std::cout << "stopping serial port thread" << std::endl;
    //serialPort->threadStop();
    std::cout << "closing serial port" << std::endl;
    Serial1.end();
}

void Controller::addByteToUpdateString(std::vector<char>* updateString, char byte) {
   // FIXXXME not sure, if needed

}



void Controller::buildVarStringBegin(int varNum, unsigned char size, std::vector<char>& updateString) {
    updateString.push_back(CMDSTART);
    updateString.push_back(CMDUPDATE);

    updateString.push_back(varNum);
    updateString.push_back(size);
}
void Controller::buildVarStringEnd(std::vector<char>& updateString) {
    updateString.push_back(CMDEND);
    updateString.push_back('\0');

}

void Controller::buildVarString(int varNum, unsigned char value, std::vector<char>& updateString) {
    // std::cout << "buildChar. valsize: " << sizeof(value) << " strSize: " << updateString.size() << std::endl;
    buildVarStringBegin(varNum, sizeof(value), updateString);
    updateString.push_back(value);
    buildVarStringEnd(updateString); 
}

void Controller::buildVarString(int varNum, unsigned short value, std::vector<char>& updateString) {
    // std::cout << "buildShort. valsize: " << sizeof(value) << " strSize: " << updateString.size() << std::endl;
    updateString.push_back(varNum);
    updateString.push_back(sizeof(value));
    std::vector<unsigned char> splitValues = splitValue(value, sizeof(value));
    for (int i = 0; i < splitValues.size(); i++) {
        updateString.push_back(splitValues[i]);
    }
}
void Controller::buildVarString(int varNum, unsigned int value, std::vector<char>& updateString) {
    std::cout << "buildIsnt. valsize: " << sizeof(value) << " strSize: " << updateString.size() << std::endl;
    updateString.push_back(varNum);
    updateString.push_back(sizeof(value));    
}

void Controller::addVarDataToUpdateString(int varNum, std::vector<char> &updateString, F16Data* data, F16Data* prevData) {
    
    //std::cout << "comparing var " << varNum << ". ";
    
    switch (varNum) {
    case POWERSTATES:
        if (data->powerStates != prevData->powerStates) {
            // std::cout << "powerstates, var = " << varNum << ", size: " << sizeof(data->powerStates) << ".\n";
            buildVarString(varNum, data->powerStates, updateString);
        }
        //prevData->powerStates = data->powerStates;
        break;
    case FUELFWD:
        //std::cout << "fuel prev: " << prevData->fuelFWD << " fuel now " << data->fuelFWD;
        if (data->fuelFWD != prevData->fuelFWD) {
            // std::cout << "fuelFWD, var = " << varNum << ", size: " << sizeof(data->fuelFWD) << ".\n";
            buildVarString(varNum, data->fuelFWD, updateString);
        }
        //prevData->fuelFWD = data->fuelFWD;
    case FUELTOTAL:
        //std::cout << "fuel prev: " << prevData->fuelFWD << " fuel now " << data->fuelFWD;
        if (data->fuelTotal != prevData->fuelTotal) {            
            //std::cout << "fueltotal, var = " << varNum << ", size: " << sizeof(data->fuelTotal) << ". value: " << std::to_string(data->fuelTotal) << "\n";
            buildVarString(varNum, data->fuelTotal, updateString);
        }
        //prevData->fuelTotal = data->fuelTotal;
    default:
        break;
    }     
    return;
}

void Controller::updateController(F16Data* data, F16Data* prevData) {
    //std::cout << "updating " << controllerName << "\n";    
#
        std::vector<char> updateString;

        // this and lower part removed to create separate update commands for each variable. 
        // better to do it all in one update command?  FIXXXME

        //updateString.push_back(CMDSTART);
        //updateString.push_back(CMDUPDATE);

        for (int i = 0; i < datafields.size(); i++) {
            addVarDataToUpdateString(datafields[i], updateString, data, prevData);
            if (updateString.size() > 2) sendDataUpdate(updateString);
        }
        //addVarDataToUpdateString(6, updateString, data, prevData);

        //updateString.push_back('>');
        //updateString.push_back('\0');

}

//////////////////////////////////////
// simple getter and setter methods
//////////////////////////////////////

void Controller::setDataField(int pos, int value) {
    //std::cout << "set datafield " << pos << " to " << value << "\n";
    datafields[pos] = value;
}

void Controller::addDataField(unsigned char value) {
    datafields.push_back(value);
}

std::vector<unsigned char> Controller::getDatafields() {
    return datafields;
}

std::string Controller::getName() { return controllerName; }

void Controller::setConnected(bool status) { connected = status; }

bool Controller::isConnected() { return connected; }
