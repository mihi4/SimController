 
#include <Arduino.h>
#include "f16common.h"

void parseSerialCommand();
static const char scName[] = "RightAuxController";
int scNameSize = sizeof(scName)/sizeof(scName[0]);

char vars[] = { VARA, VARB, VARC, VARD };
const char varCount = sizeof(vars)/sizeof(vars[0]);


#include "SCSerial.h"


void sendConnectReply(){
	//std::string 
	char buf[50] = { CMDSTART, 'O', 'K', '.' };
	strncat(buf, scName, scNameSize);
	char delimiter[2] = { '.', (char)varCount };	
	strncat(buf, delimiter, 2);	
	strncat(buf, vars, varCount);
	strncat(buf, ">", 1);
	
	SERIALCOM.println(buf);	
}

void parseUpateCommand() {
	char varNumber = receivedBytes[1];
	
	bool checkVarNum = false;
	for (char i = 0; i<varCount; i++) {
		if (vars[i] == varNumber) checkVarNum = true;
	}
	if (checkVarNum) {
		SERIALCOM.println("Var found");
		char byteCount = receivedBytes[2];		
		
		// Add part to read in bytes, creat the short/int/long and call update in specific module
		// Maybe store value in a special struct or class array of all values with appropriat module (and additional parameter)
		
		
	} else {
		SERIALCOM.println("Var not found in List");		
	}
	
	
	
	
	return;	
}

void resetController () {  // this function should reset the arduino
	SERIALCOM.println("resetting Arduino!");
	delay(200);
}

void parseSerialCommand() {
	switch (receivedBytes[0]) {
		case 'C':
			SERIALCOM.println("Connect command!");
			sendConnectReply();
			break;
		case 'U':
			SERIALCOM.println("Update command!");
			parseUpateCommand();
			break;
		case 'R':
			SERIALCOM.println("Reset command!");
			resetController();
			break;
		default:
			SERIALCOM.println("NO KNOWN COMMAND!");
			break;
	}
		
	//if (receivedBytes[0] == 'C') SERIALCOM.println("Connect command!");
	
}

void setup() {
  // put your setup code here, to run once:
	SERIALCOM.begin(BAUDRATE);
	while (!SERIALCOM) {}
	SERIALCOM.println("Arduino is ready");
	
	
}

void loop() {
  // put your main code here, to run repeatedly:
	ReadSerial();
	//recvBytesWithStartEndMarkers();
    showNewData();
}


