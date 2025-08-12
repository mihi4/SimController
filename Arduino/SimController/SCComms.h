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
     SERIALCOM.println("shortFromBytes");
    return (bytes[0] << 8) | bytes[1];
}

unsigned long longFromBytes(char bytes[4]) {     
    for (int i=0; i<4; i++) {
        SERIALCOM.print(bytes[i], HEX);SERIALCOM.print(" ");
    }
    SERIALCOM.println();
    unsigned long retVal = ((unsigned long) bytes[0] << 24)
                        | ((unsigned long) bytes[1] << 16)
                        | ((unsigned long) bytes[2] << 8)
                        | (unsigned long) bytes[3];    
    SERIALCOM.print("retVal: ");SERIALCOM.println(retVal, DEC);
    return retVal;
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

void sendReadBackString(String message) {
    String msg = "<R";
    msg.concat(message);
    msg.concat(">");
    SERIALCOM.println(msg);
}

void sendCheckReply(){
		
	SERIALCOM.println("<OK>");	
}

void parseUpateCommand() {
	char varNumber = receivedBytes[1];
	
	char varIndex = -1;
	for (char i = 0; i<varCount; i++) {
		if (vars[i].number == varNumber) varIndex = i;
	}
	if (varIndex > -1) {
		SERIALCOM.println("Var found");
		char byteCount = receivedBytes[2];
        
        //FIXXXME 
        byteCount = vars[varIndex].type;        
        
        if (byteCount == vars[varIndex].type) {  // type = number of bytes, except VARSTRING
            
            unsigned long dataValue = 0;
            SERIALCOM.println(dataValue);
            if (byteCount == VARCHAR) {
                SERIALCOM.println("varchar");
                dataValue = receivedBytes[3];
            } else if (byteCount == VARSHORT) {
                SERIALCOM.println("varshort");
                char shortBytes[VARSHORT] = {0};
                for (int i=0; i<VARSHORT; i++) {
                    shortBytes[i] = receivedBytes[3+i];
                }
                dataValue = shortFromBytes(shortBytes);
            } else if (byteCount == VARLONG) {
                SERIALCOM.println("calling longfromBytes");
                char bytes[VARINT] = {0};
                for (int i=0; i<VARLONG; i++) {
                    bytes[i] = receivedBytes[3+i];
                }
                dataValue = longFromBytes(bytes);
            } else {
                SERIALCOM.println("default");
            }
            
            /*
            switch (byteCount) {
                case 4:
                    SERIALCOM.println("calling ifromBytes");
                    char bytes[VARINT] = {0};
                    for (int i=0; i<VARINT; i++) {
                        bytes[i] = receivedBytes[3+i];
                    }
                    dataValue = intFromBytes(bytes);
                    break;
                case 1:
                    SERIALCOM.println("varchar");
                    dataValue = receivedBytes[3];
                    break;
                case 2:
                    SERIALCOM.println("varshort");
                    char shortBytes[VARSHORT] = {0};
                    for (int i=0; i<VARSHORT; i++) {
                        shortBytes[i] = receivedBytes[3+i];
                    }
                    dataValue = shortFromBytes(shortBytes);
                    break;                
                default:
                    SERIALCOM.println("default");
                    break;
            } */           
        
            SERIALCOM.print("byteCount: ");SERIALCOM.println(byteCount, DEC);
            SERIALCOM.print("varData - number: ");
            SERIALCOM.print(vars[varIndex].number);SERIALCOM.print(" mod: ");
            SERIALCOM.print(vars[varIndex].module);SERIALCOM.print(" index: ");
            SERIALCOM.print(vars[varIndex].index);SERIALCOM.print(" type: ");
            SERIALCOM.println(vars[varIndex].type);
            SERIALCOM.print(" value: ");SERIALCOM.print(dataValue, BIN);SERIALCOM.print(",");SERIALCOM.println(dataValue, DEC);
        }

        
        
        
        
		// Add part to read in bytes, creat the short/int/long and call update in specific module
		// Maybe store value in a special struct or class array of all values with appropriat module (and additional parameter)
		
		
	} else {
		SERIALCOM.println("Var not found in List");		
	}
		
	return;	
}

void resetController () {  // this function should reset the arduino
	
	//FIXXXME 
    sendReadBackString("reset start");
    char bytes[4] = { 'A', 'B', 'C', 'D' };
    unsigned long test = longFromBytes(bytes);
    
    SERIALCOM.print("testval: ");SERIALCOM.println(test, DEC);
    
    
    sendReadBackString("reset end");
    
   

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

