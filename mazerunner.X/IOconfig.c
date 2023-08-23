#include "xc.h"
#include "IOconfig.h"




void setupIO()
{

    int i;
    // AD1PCFGL=0xFFFF; //all pins are now digital, by default they are analogue
    // AD1PCFGL=0xFFDF; //all pins are now digital, by default they are analogue
    AD1PCFGL=0xFFDE; //all pins are now digital, by default they are analogue

    
    // // set LEDs as output
    // TRISBbits.TRISB15 = 0;
    // TRISBbits.TRISB14 = 0;
    // TRISBbits.TRISB13 = 0;
    // TRISBbits.TRISB12 = 0;

    // LED CONFIG Mazerunner
    // set LEDs as output
    TRISBbits.TRISB6 = 0;
    TRISBbits.TRISB5 = 0;
    TRISBbits.TRISB10 = 0;
    TRISBbits.TRISB11 = 0;
    
    TRISBbits.TRISB8=0;// UART1 TX

    // // Configure RB5 as an input pin to register switch presses
    // TRISBbits.TRISB5 = 1;  // Set RB5 as an input (Switch in Starter Kit)
    // __builtin_enable_interrupts();

    // Mazerunner
    // Configure RC5 as an input pin to register switch presses
    TRISCbits.TRISC5 = 1;  // Set RC5 as an input (Switch in Mazerunner)
    __builtin_enable_interrupts();

    // Configure Ponti input Pin, Apperently not needed ?????
    // TRISBbits.TRISB3 = 1;  // Set RB3 / AN5 as an input
    // TRISAbits.TRISA0 = 1;  // Set AN0 as an input


    //PIN MAPPING
    
    //before we map, we need to unlock
    __builtin_write_OSCCONL(OSCCON & 0xbf); // clear bit 6 (unlock, they are usually write protected)
    
    // PERIPHERAL receives data from which INPUT  
    RPINR18bits.U1RXR = 7; //mapped to RP7 is U1 RX, CHANGE THIS // MAZERUNNER
    // RPINR19bits.U2RXR = 9; //mapped to RP9 is U2 RX, CHANGE THIS

    //OUTPUT PIN receives data from which PERIPHERAL, 
    //see table 11-2 in datasheet to check peripheral codes 
    RPOR4bits.RP8R = 0b00011; //output bin RP8 gets data from peripheral U1 TX  // MAZERUNNER


    //PERIPHERAL QEA Encoder 1, receives data from RP10
   RPINR14bits.QEA1R = 24; // mazerunner change
    //PERIPHERAL QEB Encoder 1, receives data from RP11
   RPINR14bits.QEB1R = 25; // mazerunner change
    
    

    
    // UART to be used with programmer 
    // RPOR4bits.RP8R = 0b00101; //output Pin RP8 gets data from peripheral U2 TX 

    // PIN for Motor HBridge Control
    // Left Motor: PWM1H1 (Pin: RB14), PWM1L1 (Pin: RB15) (PWM Pin pair 1 of the PWM Module 1)
    // Right Motor: PWM1H2 (Pin: RB12), PWM1L2 (Pin: RB13) (PWM Pin pair 2 of the PWM Module 1)
    TRISBbits.TRISB15 = 0; // SET RA0 as output to send a logic high or low to the H-Bridge
    TRISBbits.TRISB13 = 0; // SET RA0 as output to send a logic high or low to the H-Bridge

    //after mapping we lock again
     __builtin_write_OSCCONL(OSCCON | 0x40); // Lock PPS registers (lock again!)
     
    for (i = 0; i < 30000; i++); // short dirty delay for changes to take effect,

    
}
