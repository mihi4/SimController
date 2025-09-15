 #ifdef DUE_NATIVE
    #define SERIALCOM SerialUSB   //enable communication over the native port of the DUE
  #else
    #define SERIALCOM Serial      //standard serial connection
  #endif

// Example 6 - Receiving binary data

const byte numBytes = 50;
byte receivedBytes[numBytes];
byte numReceived = 0;
boolean newData = false;
const byte msgLen = 50;
char rbMsg[msgLen] = ""; // readBackMessage buffer

void sendReadBackString(char message[msgLen]) {
    char timestamp[20];    
    sprintf(timestamp, "%u",millis());
    String msg = "";
    msg.concat(CMDSTART);
    msg.concat("R");
    msg.concat(timestamp);
    msg.concat(": ");
    msg.concat(message);
    msg.concat(CMDEND);
    SERIALCOM.println(msg);
}

unsigned short shortFromBytes(unsigned char bytes[2]) {     
    unsigned short retVal;// = (bytes[0] << 8) | bytes[1];
    memcpy(&retVal, bytes, sizeof(retVal));
    return retVal;// return (bytes[0] << 8) | bytes[1];
}

unsigned long longFromBytes(unsigned char bytes[4]) {             

   /* for (int i=0; i<4; i++) {
      sprintf(rbMsg, "LongFromByte %u, hexVal 0x%x", i, bytes[i]);
      sendReadBackString(rbMsg);
    } */

    unsigned long retVal = 0; // = *(unsigned long*)(bytes);
    retVal |= (unsigned long)bytes[0] << 24; // Most significant byte
    retVal |= (unsigned long)bytes[1] << 16;
    retVal |= (unsigned long)bytes[2] << 8;
    retVal |= (unsigned long)bytes[3]; // Least significant byte */
    
    //memcpy(&retVal, bytes, 4); //sizeof(unsigned long));
    /*sprintf(rbMsg, "retVal longBytes: dec %lu hex 0x%lx", retVal, retVal);
    sendReadBackString(rbMsg);*/
    return retVal;
}


void showNewData() {
    if (newData == true) {
        SERIALCOM.print("HEX values: ");
        for (byte n = 0; n < numReceived; n++) {
            SERIALCOM.print(receivedBytes[n], HEX);
            SERIALCOM.print(' ');
        }
        SERIALCOM.println();
        SERIALCOM.print("DEC values: ");
        for (byte n = 0; n < numReceived; n++) {
            SERIALCOM.print(receivedBytes[n], DEC);
            SERIALCOM.print(' ');
        }
        SERIALCOM.println();
        //newData = false;
    }
}


void sendConnectReply(){

	SERIALCOM.print(CMDSTART);	
	SERIALCOM.print("I");	
	SERIALCOM.print(scName);	
	SERIALCOM.print(";");	
	SERIALCOM.print(varCount);
	for (char i=0; i<varCount; i++) {
		SERIALCOM.print((char)vars[i]->number);	
	}
	SERIALCOM.println(CMDEND);

}

void sendCheckReply(){
	SERIALCOM.print(CMDSTART);	
	SERIALCOM.print("Y");	
  SERIALCOM.println(CMDEND);	
}

void parseUpateCommand() {
	
  /*time = millis();
  sprintf(rbMsg, "parsing update command");
  sendReadBackString(rbMsg);*/
  char varNumber = receivedBytes[1];
	
	char varIndex = -1;
  
	for (char i = 0; i<varCount; i++) {    
		if (vars[i]->number == varNumber) varIndex = i;
	}
	if (varIndex > -1) {
    lastParsedVar = varIndex;
		//SERIALCOM.println("Var found");
    
		char byteCount = receivedBytes[2];
		char typeNum = vars[varIndex]->type;
		char typeConverted=power(2,typeNum); // use enum 0,1,2,3 from f16var as exponent, so it corresponds with VARCHAR, VARSHORT and VARLONG
    
    // sprintf(rbMsg, "byteCount: %u, typeNum: %u, typeConverted: %u", byteCount, typeNum, typeConverted);
    // sendReadBackString(rbMsg);

    if (typeNum == f16var::STRING) {
      // sprintf(rbMsg, "String incoming, byteCount is %u", byteCount);
      // sendReadBackString(rbMsg);      
      String newValue = "";
      for (int i=0; i<byteCount; i++) {
        newValue.concat((char)receivedBytes[3+i]);
      }
      *vars[varIndex]->value.valString = newValue;  // String values have to be called on pointers
      varsChanged = true;
      return;
    }

    if (byteCount == typeConverted) {  // see if bytecount sent is what is expected from the var type
      unsigned long dataValue = 0;
      
      if (byteCount == VARCHAR) {                
          dataValue = receivedBytes[3];
          vars[varIndex]->value.valC = (unsigned char)dataValue;
          varsChanged = true;
      } 
      else if (byteCount == VARSHORT) {
        
        unsigned char shortBytes[VARSHORT] = {0};
          
        for (int i=0; i<VARSHORT; i++) {
              shortBytes[i] = receivedBytes[3+i];  
              sprintf(rbMsg, "SHORTbyte %u, value %x", i, shortBytes[i]);
              sendReadBackString(rbMsg); 
        }
        dataValue = shortFromBytes(shortBytes);
        sprintf(rbMsg, "SHORTvar %u datvalue %u", varIndex, dataValue);
        sendReadBackString(rbMsg);
        vars[varIndex]->value.valI = (unsigned int)dataValue;
        
        varsChanged = true;
        
      } 
      else if (byteCount == VARLONG) {
         
        unsigned char bytes[VARLONG] = {0};
       
        for (int i=0; i<VARLONG; i++) {
            bytes[i] = receivedBytes[3+i];   
           /* sprintf(rbMsg, "INTvarnum %u byte %u, value %x", varIndex, i, bytes[i]);
            sendReadBackString(rbMsg); */
        }           
        dataValue = longFromBytes(bytes);
        /* sprintf(rbMsg, "INTvar %u datvalue %lu", varIndex, dataValue);
        sendReadBackString(rbMsg); */
        vars[varIndex]->value.valL = (unsigned long)dataValue;
        
        varsChanged = true;
        
      } 
    }	else {      
      sprintf(rbMsg, "ParseERR, Var %u byteCnt %u",varNumber, byteCount);
      sendReadBackString(rbMsg);
      SERIALCOM.println(ER_BYTEMATCH);
    }
	} else {
    sprintf(rbMsg, "ParseER, Var %u not configured",varNumber);
    sendReadBackString(rbMsg);
		SERIALCOM.println(ER_WRONGVAR);  // send error message to pc
	}
  /*gap = millis() - time;
  sprintf(rbMsg, "parsing update Done, %u ms",gap);
  sendReadBackString(rbMsg);*/
	return;	
}

void resetController () {  // this function should reset the arduino
	
  sendReadBackString("reset called");
	setupModules();

}

void parseSerialCommand() {
    
  if (newData == true) {
  /*time = millis();
  sendReadBackString("parseSerial started");  */

    /*SERIALCOM.print("received Bytes: ");SERIALCOM.println(numReceived, DEC);
    for (int i=0;i<numReceived;i++) {
      SERIALCOM.print(receivedBytes[i], HEX);SERIALCOM.print("-");
    }
    SERIALCOM.println();*/
		switch (receivedBytes[0]) {
			case 'C':
				//SERIALCOM.println("Connect command!");
				sendConnectReply();
				break;
			case 'U':
				//SERIALCOM.println("Update command!");
				parseUpateCommand();
				break;
			case 'R':
				//SERIALCOM.println("Reset command!");
				resetController();
				break;
			case 'H':  // check, if Arduino is still present
				//SERIALCOM.println("CheckHere command!");
				sendCheckReply();
				break;             
			default:
				SERIALCOM.println(ER_WRONGCMD);
				break;
		}    
        newData = false;
		/*gap = millis() - time;
		sprintf(rbMsg, "ParseSerial Done, %u ms",gap);
		sendReadBackString(rbMsg);*/
    }	
}


void ReadSerial() {

  static boolean recvInProgress = false;
  static byte ndx = 0;
  char startMarker = CMDSTART;
  char endMarker = CMDEND;
  byte rb;

  byte escapeBytenum = -2;  // escape "counter", shows the index, where escape character (dec 27, hex 0x1B) was found; -1 is bad, sind index 0-1 is -1 :-D
   
  while (SERIALCOM.available() > 0 && newData == false) {
    rb = SERIALCOM.read();
    
    
    if (recvInProgress) {
      if (rb != endMarker) {
        if (rb == CMDESC) {
          //SERIALCOM.print("escape found, ndx=");SERIALCOM.println(ndx, DEC);
          //msg.concat("esc1.");
           
          if (escapeBytenum == (ndx-1)) {
            //SERIALCOM.print("2nd escape found, escBytenum=");SERIALCOM.println(escapeBytenum, DEC);
            //msg.concat("esc2.");
            receivedBytes[escapeBytenum] = rb;
            escapeBytenum = -2; // reset escape "counter"
          } else {
            escapeBytenum = ndx;
            //SERIALCOM.print("escapeBytenum: ");SERIALCOM.println(escapeBytenum);
            ndx++;
          }					
        } else {
          receivedBytes[ndx] = rb;
          ndx++;
        }
        if (ndx >= numBytes) {
            ndx = numBytes - 1;
        }
      }
      else {
        //SERIALCOM.println("endmarker found");
        if (escapeBytenum == (ndx-1)) {  // was previous byte escaping byte?	
          //SERIALCOM.println("prev byte was escape, setting prev index to rb");
          receivedBytes[escapeBytenum] = rb;
        } else {
          //SERIALCOM.println("final delimiter found");
          receivedBytes[ndx] = '\0'; // terminate the string
          recvInProgress = false;
          numReceived = ndx;  // save the number for use when printing
          ndx = 0;
          newData = true;
        }				
        escapeBytenum = -2;
      }
    } else if (rb == startMarker) {
        recvInProgress = true;
        numReceived = 0;
    }
  }
}	

