
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
	U1BRG=28; // baud rate register
	// U1BRG=173; // baud rate register
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

	IEC0bits.U1RXIE=0; //enable the receive interrupt
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



// void __attribute__((interrupt, no_auto_psv)) _U1RXInterrupt(void)
// {	
// 	unsigned int rxData; // a local buffer to copy the data into

// 	/**Set the UART2 receiving interrupt flag to zero*/
 
// 	IFS0bits.U1RXIF=0;

//     LED4=~LED4;
//     LED5=~LED5;
	


	
// 	//we should now read out the data
// 	rxData=U1RXREG;
    
//     //and copy it back out to UART
//     U1TXREG=rxData;
//         //wait until the character is gone...

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