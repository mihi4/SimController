#include "CommsHandler.h"

CommsHandler::CommsHandler() {
    std::cout << "CommsHandler constructor\n";
}
CommsHandler::~CommsHandler()
{
    std::cout << "CommsHandler " << controllerName << " deconstructor\n";
    if (serialPort) serialPort.end();
    //delete serialPort;
}

unsigned char CommsHandler::getComPortNum() { return portNum;  }

bool CommsHandler::isSerialConnected() { return serialConnected; }
bool CommsHandler::isArduinoConnected() { return arduinoConnected; }


bool CommsHandler::initializeComport(int comPortNum, long baudrate, std::string controllerNameIn)
{
    controllerName = controllerNameIn;
    serialPort.setPort(comPortNum);
    portNum = comPortNum;
    serialPort.begin(baudrate);
    if (serialPort.connected()) {
        Sleep(500); // wait for Arduino to boot
        return true;
    }
    // if any error during connection occurs:
    return false;
}

void CommsHandler::sendDataUpdate(std::vector<char> updateString)
{
    std::string output(updateString.begin(), updateString.end());

    std::cout << "this string goes to the serial port " << std::dec << (int)portNum << " **" << output << " **\n";
    if (serialPort && serialPort.connected()) serialPort.println(output);
}

bool CommsHandler::sendCommand(std::string cmdString)
{
    if (serialPort && serialPort.connected()) {
        serialPort.println(cmdString);
        return true;
    }
    std::cout << "ERROR: " << controllerName << " serialPort not connected!\n";
    return false;
}



void CommsHandler::readFromPort()
{
    static boolean recvInProgress = false;
    static byte ndx = 0;
    char startMarker = CMDSTART;
    char endMarker = CMDEND;
    byte rb;

    byte escapeBytenum = -2;  // escape "counter", shows the index, where escape character (dec 27, hex 0x1B) was found; -1 is bad, sind index 0-1 is -1 :-D

    while (serialPort && serialPort.available() && newData == false) {
        rb = serialPort.read();

        if (recvInProgress) {
            if (rb != endMarker) {
                if (rb == CMDESC) {
                    //serialPort->print("escape found, ndx=");serialPort->println(ndx, DEC);
                    //msg.concat("esc1.");

                    if (escapeBytenum == (ndx - 1)) {
                        //serialPort->print("2nd escape found, escBytenum=");serialPort->println(escapeBytenum, DEC);
                        //msg.concat("esc2.");
                        receivedBytes[escapeBytenum] = rb;
                        escapeBytenum = -2; // reset escape "counter"
                    }
                    else {
                        escapeBytenum = ndx;
                        //serialPort->print("escapeBytenum: ");serialPort->println(escapeBytenum);
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
                //serialPort->println("endmarker found");
                if (escapeBytenum == (ndx - 1)) {  // was previous byte escaping byte?	
                  //serialPort->println("prev byte was escape, setting prev index to rb");
                    receivedBytes[escapeBytenum] = rb;
                }
                else {
                    //serialPort->println("final delimiter found");
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

bool CommsHandler::parseControllerInfo(std::vector<unsigned char>& datafields) {

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
    std::string tempName(receivedBytes + startIndex, count - 1);

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
    arduinoConnected = true;
}

bool CommsHandler::parseSerialInput(std::vector<unsigned char>& datafields) {
    if (newData == true) {

        switch (receivedBytes[0]) {
        case 'I':  // answer to connect command
            std::cout << controllerName << ":connection string received!\n";
            arduinoConnected = true;
            if (!parseControllerInfo(datafields)) {
                return false;
            }
            break;
        case 'Y':  // answer to heartbeat command
            std::cout << controllerName << " is still here\n";
            arduinoConnected = true;
            break;
        case 'R':  // readback Data
            //std::string readBack(receivedBytes);
            memmove(receivedBytes, receivedBytes + 1, numBytes - 1);
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

}

void CommsHandler::readSerial(std::vector<unsigned char>& datafields) {
    readFromPort();
    if (numReceived) {
        if (!parseSerialInput(datafields)) {
            std::cout << "ERROR parsing serial input";
        }
    }
    numReceived = 0;

}