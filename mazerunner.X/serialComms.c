#include "serialComms.h"
#include <xc.h>
#include "IOconfig.h"


/*
*	set-up the serial port
*   here we aim to achieve a data transfer rate of 57.6 kbit/s,
*   based on Fcycle=26.6666Mhz 
*   BaudRate=Fcycle/(16*(BRG+1))
*   ==> BRG=Fcy/(16*BaudRate) - 1 = 26.666Mhz/(16*57600) - 1 = 28.23
*   ==> choose 28 ==> BaudRate= 57.474  kbit/s, which is ~ 1% off.
 * 
 * for standard communication speed of 9600 kbit/s
 * choose 173 (factor 6)
*/
void setupUART1(void)
{
	U1MODEbits.UARTEN=0; //switch the uart off during set-up
	// U1BRG=28; // baud rate register
	// U1BRG=173; // baud rate register
	// U1BRG=12; // baud rate: 128000 baude
	U1BRG=13; // baud rate: 115200 baude
	U1MODEbits.LPBACK=0; // in loopback mode for test! TODO: set to no loop-back (=0) after test 
	
	U1MODEbits.WAKE=0; //do not wake up on serial port activity

	U1MODEbits.ABAUD=0; //no auto baud rate detection
	U1MODEbits.PDSEL=0; //select 8 bits date, no parity
	U1MODEbits.STSEL=0; //one stop bit
    U1MODEbits.BRGH = 0; // No High Speed Mode


	IFS0bits.U1RXIF=0; //reset the receive interrupt flag
	IFS0bits.U1TXIF=0; //reset the transmission interrupt flag
    
	IPC2bits.U1RXIP=3; //set the RX interrupt priority
	IPC3bits.U1TXIP=5; //set the TX interrupt priority

	U1STAbits.URXISEL=0; //generate a receive interrupt as soon as a character has arrived
	U1STAbits.UTXEN=1; //enable the transmission of data

	IEC0bits.U1RXIE=1; //enable the receive interrupt
	IEC0bits.U1TXIE=0; //disable the transmit interrupt

	//FINALLY, 
	U1MODEbits.UARTEN=1; //switch the uart on

  	U1STAbits.UTXEN=1; //enable transmission
	
    
//   	U1MODE = 0x8000; /* Reset UART to 8-n-1, alt pins, and enable */
//	U1STA  = 0x0440; /* Reset status register and enable TX & RX*/
}


// Setup UART2
void setupUART2(void)
{
	U2MODEbits.UARTEN=0; //switch the uart off during set-up
	U2BRG=28; // baud rate register
	U2MODEbits.LPBACK=0; // in loopback mode for test! TODO: set to no loop-back (=0) after test 
	
	U2MODEbits.WAKE=0; //do not wake up on serial port activity

	U2MODEbits.ABAUD=0; //no auto baud rate detection
	U2MODEbits.PDSEL=0; //select 8 bits date, no parity
	U2MODEbits.STSEL=0; //one stop bit
	U2MODEbits.BRGH = 0; // No High Speed Mode

	IFS1bits.U2RXIF=0; //reset the receive interrupt flag
	IFS1bits.U2TXIF=0; //reset the transmission interrupt flag
    
	IPC7bits.U2RXIP=3; //set the RX interrupt priority
	IPC7bits.U2TXIP=5; //set the TX interrupt priority

	U2STAbits.URXISEL=0; //generate a receive interrupt as soon as a character has arrived
	U2STAbits.UTXEN=1; //enable the transmission of data

	IEC1bits.U2RXIE=0; //disable the receive interrupt
	IEC1bits.U2TXIE=0; //disable the transmit interrupt

	//FINALLY, 
	U2MODEbits.UARTEN=1; //switch the uart on

  	U2STAbits.UTXEN=1; //enable transmission
}


/************* RECEIVING *************/

/* Explanation:
1.	The '<' and '>' symbol are used as the start and end characters to define command boundaries.
2.	The _U1RXInterrupt function reads characters from the UART and stores them in commandBuffer until the end character '>' is received. 
	Once the end character is detected, the parser function parseCommand is called to analyze the content of commandBuffer.
3.	The parseCommand function uses sscanf to extract values from the received command string based on the expected format. 
	For example, it looks for patterns like "velX.Y," "turnX," and "driveX.Y" and extracts the corresponding values for velocity, angle, or distance.
4.	The state machine continuously processes the received commands and executes the appropriate actions based on the recognized command.

This should work for commands like "vel0.3," "turn90," and "drive0.2," and it can be extended to handle additional commands as needed.
*/

extern volatile struct UARTCommand currentUARTCommand = {CMD_NONE, "", 0.0, false};  // Initialize to no command

void __attribute__((interrupt, no_auto_psv)) _U1RXInterrupt(void) {
	static volatile char receivedData;  // Variable to store received data
	static volatile char commandBuffer[50]; // Buffer to store the received command
	static volatile int bufferIndex = 0;

    // TODO: Sending something from the Microcontroller to the PC via UART1 does interupt the command reading process
    //       and the command is not executed. Why is that? How can it be fixed? Is the buffer emptied when sending something? How does the command buffer work?
    // char buffer[30];
    // sprintf(buffer, "UART Interrupt \n");
    // putsUART1(buffer);


    // Clear the UART receive interrupt flag
    IFS0bits.U1RXIF = 0;

    // Read the received character from the UART receive register
    receivedData = U1RXREG;

    // Check for start character '<'
    if (receivedData == '<') {
        // Reset the buffer and buffer index
        bufferIndex = 0;
        memset(commandBuffer, 0, sizeof(commandBuffer));
    }
    // Check for end character '>'
    else if (receivedData == '>') {
        // Process the command when the end character is received
        currentUARTCommand = parseCommand(commandBuffer);

        // Set the new_command flag to true
	    currentUARTCommand.new_command = true;

		// Send back the parsed values via UART
        sendParsedValues();
        // Apply new command directly to the robot disregarding the current 
        // control strategy used in main switch case EXCECUTE 
        // (not like remoteControlledMotionPrimitiveExecutor())
        // overwriteCurrentMovementControlParametersWithUART(currentUARTCommand);

        // char buffer[30];
        // sprintf(buffer, "New UART command\n");
        // putsUART1(buffer);
    }
    // Store characters in the buffer
    else if (bufferIndex < sizeof(commandBuffer) - 1) {
        commandBuffer[bufferIndex++] = receivedData;
    }

	// clear the overflow bit if it has been set (i.e. if we were to slow to read out the fifo)
	U1STAbits.OERR=0; //we reset it all the time

}

// void __attribute__((interrupt, no_auto_psv)) _U1RXInterrupt(void)
// {	
// 	unsigned int rxData; // a local buffer to copy the data into

// 	/**Set the UART2 receiving interrupt flag to zero*/
 
// 	IFS0bits.U1RXIF=0;

//     LED4=~LED4;	


	
// 	//we should now read out the data
// 	rxData=U1RXREG;
    
//     //and copy it back out to UART
//     U1TXREG=rxData;
//     //wait until the character is gone...

// 	//we should also clear the overflow bit if it has been set (i.e. if we were to slow to read out the fifo)
// 	U1STAbits.OERR=0; //we reset it all the time
// 	//some notes on this from the data sheet
// 	/*
// 	If the FIFO is full (four characters) and a fifth character is fully received into the UxRSR register,
// 	the overrun error bit, OERR (UxSTA<1>), will be set. The word in UxRSR will be kept, but further
// 	transfers to the receive FIFO are inhibited as long as the OERR bit is set. The user must clear
// 	the OERR bit in software to allow further data to be received.
// 	If it is desired to keep the data received prior to the overrun, the user should first read all five
// 	characters, then clear the OERR bit. If the five characters can be discarded, the user can simply
// 	clear the OERR bit. This effectively resets the receive FIFO and all prior received data is lost.

// 	The data in the receive FIFO should be read prior to clearing the OERR bit. The
// 	FIFO is reset when OERR is cleared, which causes all data in the buffer to be lost.
// 	*/

// }

// void __attribute__((interrupt, no_auto_psv)) _U1TXInterrupt(void)
// {	
// 	//unsigned int rxData; // a local buffer to copy the data into
//    // long i;
// 	/**Set the UART2 receiving interrupt flag to zero*/
 
// 	IFS0bits.U1TXIF=0;
//    // LED7=0;//;
// }


/************* SENDING *************/

/***************************************************************************
* Function Name     : putsUART1                                            *
* Description       : This function puts the data string to be transmitted *
*                     into the transmit buffer (till NULL character)       *
* Parameters        : unsigned int * address of the string buffer to be    *  
*                     transmitted                                          *
* Return Value      : None                                                 *
***************************************************************************/

void putsUART1(char *buffer)
{
    char * temp_ptr = (char *) buffer;

    /* transmit till NULL character is encountered */

    if(U1MODEbits.PDSEL == 3)        /* check if TX is 8bits or 9bits */
    {
        while(*buffer != '\0') 
        {
            while(U1STAbits.UTXBF); /* wait if the buffer is full */
            U1TXREG = *buffer++;    /* transfer data word to TX reg */
        }
    }
    else
    {
        while(*temp_ptr != '\0')
        {
            while(U1STAbits.UTXBF);  /* wait if the buffer is full */
            U1TXREG = *temp_ptr++;   /* transfer data byte to TX reg */
        }
    }
}


/***************************************************************************
* Function Name     : putsUART2                                            *
* Description       : This function puts the data string to be transmitted *
*                     into the transmit buffer (till NULL character)       *
* Parameters        : unsigned int * address of the string buffer to be    *  
*                     transmitted                                          *
* Return Value      : None                                                 *
***************************************************************************/

void putsUART2(char *buffer)
{
    char * temp_ptr = (char *) buffer;

    /* transmit till NULL character is encountered */

    if(U2MODEbits.PDSEL == 3)        /* check if TX is 8bits or 9bits */
    {
        while(*buffer != '\0') 
        {
            while(U2STAbits.UTXBF); /* wait if the buffer is full */
            U2TXREG = *buffer++;    /* transfer data word to TX reg */
        }
    }
    else
    {
        while(*temp_ptr != '\0')
        {
            while(U2STAbits.UTXBF);  /* wait if the buffer is full */
            U2TXREG = *temp_ptr++;   /* transfer data byte to TX reg */
        }
    }
}






/************* HELPER FUNCTIONS *************/

// struct UARTCommand parseCommand(const char* commandString) {
//     struct UARTCommand parsedCommand;
//     parsedCommand.command_code = CMD_NONE;

// 	// Set the new_command flag to true
// 	currentUARTCommand.new_command = true;

//     // Check for velocity command "velX.Y"
//     if (sscanf(commandString, "vel%f", &parsedCommand.command_value.fval) == 1) {
//         parsedCommand.command_code = CMD_VEL;
//         strcpy(parsedCommand.name, "vel");
//     }
//     // Check for turn command "turnX"
//     else if (sscanf(commandString, "turn%d", &parsedCommand.command_value.ival) == 1) {
//         parsedCommand.command_code = CMD_TURN;
//         strcpy(parsedCommand.name, "turn");
//     }
//     // Check for drive command "driveX.Y"
//     else if (sscanf(commandString, "drive%f", &parsedCommand.command_value.fval) == 1) {
//         parsedCommand.command_code = CMD_DRIVE;
//         strcpy(parsedCommand.name, "drive");
//     }
//     // Add more command parsing logic as needed

//     return parsedCommand;
// }

struct UARTCommand parseCommand(const char* commandString) {
    struct UARTCommand parsedCommand;
    parsedCommand.command_code = CMD_NONE;

    // Check for velocity command "velX.Y"
    if (sscanf(commandString, "vel%f", &parsedCommand.command_value) == 1) {
        parsedCommand.command_code = CMD_VEL;
        strcpy(parsedCommand.name, "vel");
    }
    // Check for turn command "turnX.Y"
    else if (sscanf(commandString, "turn%f", &parsedCommand.command_value) == 1) {
        parsedCommand.command_code = CMD_TURN;
        strcpy(parsedCommand.name, "turn");
    }
    // Check for drive command "driveX.Y"
    else if (sscanf(commandString, "drive%f", &parsedCommand.command_value) == 1) {
        parsedCommand.command_code = CMD_DRIVE;
        strcpy(parsedCommand.name, "drive");
    }
    // Check for stop command "stop"
    else if (sscanf(commandString, "stop") == 1) {
        parsedCommand.command_code = CMD_STOP;
        strcpy(parsedCommand.name, "stop");
    }
    // Add more command parsing logic as needed

    return parsedCommand;
}


void handleNewUARTCommand() {
	// Process the new command
	switch (currentUARTCommand.command_code) {
		case CMD_NONE:
			// No valid command received, handle accordingly
			break;

		case CMD_VEL:
			// Handle velocity command
			// Extracted velocity is available in 'velocity' variable
			break;

		case CMD_TURN:
			// Handle turn command
			// Extracted angle is available in 'angle' variable
			break;

		case CMD_DRIVE:
			// Handle drive command
			// Extracted distance is available in 'distance' variable
			break;

		// Add cases for other commands as needed

		default:
			// Handle unknown command or error
			break;
	}
}

void sendParsedValues() {
    // Create a buffer to hold the response
    char responseBuffer[50];
    int responseLength = 0;

    // switch (currentUARTCommand.command_code) {
    //     case CMD_VEL:
    //         responseLength = sprintf(responseBuffer, "Command: %s, Value: %.2f\n", currentUARTCommand.name, currentUARTCommand.command_value);
    //         break;

    //     case CMD_TURN:
    //         responseLength = sprintf(responseBuffer, "Command: %s, Value: %.2f\n", currentUARTCommand.name, currentUARTCommand.command_value);
    //         break;

    //     case CMD_DRIVE:
    //         responseLength = sprintf(responseBuffer, "Command: %s, Value: %.2f\n", currentUARTCommand.name, currentUARTCommand.command_value);
    //         break;

    //     // Add cases for other commands as needed

    //     default:
    //         // No valid command, send an error message
    //         responseLength = sprintf(responseBuffer, "Invalid Command\n");
    //         break;
    // }

    responseLength = sprintf(responseBuffer, "Command: %s, Value: %.2f\n", currentUARTCommand.name, currentUARTCommand.command_value);


    // Send the response via UART
	putsUART1(responseBuffer);
    // for (int i = 0; i < responseLength; i++) {
    //     // Wait until the transmit buffer is empty
    //     while (!U1STAbits.TRMT);
    //     U1TXREG = responseBuffer[i];
    // }
}


// Apply new command directly to the robot disregarding the current 
// control strategy used in main switch case EXCECUTE 
// (not like remoteControlledMotionPrimitiveExecutor())
void overwriteCurrentMovementControlParametersWithUART( struct UARTCommand uartCommand ) {
    // Update the current movement control parameters
    char buffer[30];
    sprintf(buffer, "Overwriting...\n");
    putsUART1(buffer);
    switch (uartCommand.command_code) {
        case CMD_VEL: {
            currMovementControlParameters.movementPrimitive.vel_cruise = uartCommand.command_value;
            char buffer[30];
            sprintf(buffer, "vel overwritten: %f\n", currMovementControlParameters.movementPrimitive.vel_cruise);
            putsUART1(buffer);
            break;
        }
        case CMD_TURN: {
            // The velocity has to be set via the CMD_VEL command
            // TODO: Maybe we should hand over the motion primitive as a function parameter and not read the global values from within the init function
            currMovementControlParameters.movementPrimitive.type = TURNING;
            currMovementControlParameters.movementPrimitive.value = uartCommand.command_value;
            initMovementFromGlobalGoal();
            break;
        }
        case CMD_DRIVE: {
            // The velocity has to be set via the CMD_VEL command
            currMovementControlParameters.movementPrimitive.type = DRIVING_STRAIGHT;
            currMovementControlParameters.movementPrimitive.value = uartCommand.command_value;
            initMovementFromGlobalGoal();
            char buffer[30];
            sprintf(buffer, "Drive Mode activated: %f\n", currMovementControlParameters.movementPrimitive.value);
            putsUART1(buffer);
            break;
        }
        case CMD_STOP:
            currMovementControlParameters.movementPrimitive.type = PARKING;
            break;

        // Add cases for other commands as needed

        default: {
            // No valid command, send an error message
            char buffer[20];
            sprintf(buffer, "Invalid Command\n");
            putsUART1(buffer);
            break;
        }
    }
}
