
void sendConnectReply(){
	//std::string 
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
	//std::string 	
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

        // FIXXXME remove after testing
        byteCount=2;
		
        SERIALCOM.print("byteCount: ");SERIALCOM.println(byteCount);
        
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
		
	//if (receivedBytes[0] == 'C') SERIALCOM.println("Connect command!");
	
}
