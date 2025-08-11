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
				parseSerialCommand();
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
