#define BAUDRATE 115200

 #ifdef DUE_NATIVE
    #define SERIALCOM SerialUSB   //enable communication over the native port of the DUE
  #else
    #define SERIALCOM Serial      //standard serial connection
  #endif

// Example 6 - Receiving binary data

const byte numBytes = 32;
byte receivedBytes[numBytes];
byte numReceived = 0;

boolean newData = false;

unsigned short shortFromBytes(char bytes[2]) {
    return (bytes[0] << 8) | bytes[1];
}

unsigned int intFromBytes(char bytes[4]) {

}

unsigned long longFromBytes(char bytes[8]) {

}


void showNewData() {
    if (newData == true) {
        SERIALCOM.print("This just in (HEX values)... ");
        for (byte n = 0; n < numReceived; n++) {
            SERIALCOM.print(receivedBytes[n], HEX);
            SERIALCOM.print(' ');
        }
        SERIALCOM.println();
        newData = false;
    }
}


void sendConnectReply(){

	SERIALCOM.print(CMDSTART);	
	SERIALCOM.print("HI.");	
	SERIALCOM.print(scName);	
	SERIALCOM.print(".");	
	SERIALCOM.print(varCount);
	for (char i=0; i<varCount; i++) {
		SERIALCOM.print((char)vars[i].number);	
	}
	SERIALCOM.println(CMDEND);
	/*char buf[50] = { CMDSTART, 'H', 'I', '.' };
	strncat(buf, scName, scNameSize);
	char delimiter[2] = { '.', varCount };	
	strncat(buf, delimiter, 2);	
	for (char i=0; i<varCount; i++) {
		char buf2[3];
		strncat(buf, itoa(vars[i].number, buf2,10), 1);
	}
	
	strncat(buf, ">", 1);
	
	SERIALCOM.println(buf);	*/
}

void sendCheckReply(){
		
	SERIALCOM.println("<OK>");	
}

void parseUpateCommand() {
	char varNumber = receivedBytes[1];
	
	bool checkVarNum = false;
	for (char i = 0; i<varCount; i++) {
		if (vars[i].number == varNumber) checkVarNum = true;
	}
	if (checkVarNum) {
		SERIALCOM.println("Var found");
		char byteCount = receivedBytes[2];

        SERIALCOM.print("byteCount: ");SERIALCOM.println(byteCount, DEC);
		if ((char)byteCount>5) SERIALCOM.println("too many vars");
        
		// Add part to read in bytes, creat the short/int/long and call update in specific module
		// Maybe store value in a special struct or class array of all values with appropriat module (and additional parameter)
		
		
	} else {
		SERIALCOM.println("Var not found in List");		
	}
		
	return;	
}

void resetController () {  // this function should reset the arduino
	
	
    /*asm volatile ("jmp 0x7800");;
	do                          
	{                           
        wdt_enable(WDTO_15MS);  
        for(;;){}               
	} while(0);*/
}

void parseSerialCommand() {
	if (newData == true) {
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
			case 'H':  // check, if Arduino is still present
				SERIALCOM.println("CheckHere command!");
				sendCheckReply();
				break;             
			default:
				SERIALCOM.println("NO KNOWN COMMAND!");
				break;
		}    
        newData = false;
    }
	
		
	//if (receivedBytes[0] == 'C') SERIALCOM.println("Connect command!");
	
}


void ReadSerial() {
	static boolean recvInProgress = false;
    static byte ndx = 0;
    char startMarker = '<';
    char endMarker = '>';
    byte rb;
   

    while (SERIALCOM.available() > 0 && newData == false) {
        rb = SERIALCOM.read();

        if (recvInProgress) {
            if (rb != endMarker) {
                receivedBytes[ndx] = rb;
                ndx++;
                if (ndx >= numBytes) {
                    ndx = numBytes - 1;
                }
            }
            else {
                receivedBytes[ndx] = '\0'; // terminate the string
                recvInProgress = false;
                numReceived = ndx;  // save the number for use when printing
                ndx = 0;
                newData = true;
				
            }
        }
        else if (rb == startMarker) {
            recvInProgress = true;
        }
    }
}	

