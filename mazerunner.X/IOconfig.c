#include "xc.h"
#include "IOconfig.h"




void setupIO()
{

    int i;

    // Set all pins to be digital, except the pins used with the A2DC module (e.g. AN4, AN5, AN6)
    // define inverse of 0b0000000001110000 from AD1CSSL
    AD1PCFGL=0b1111111110001111; // Set all pins to digital except AN4, AN5, AN6 which are analogue to measure the IR-Sensor voltages
    
    // LED Config - Mazerunner
    // set LEDs as output
    TRISBbits.TRISB6 = 0;
    TRISBbits.TRISB5 = 0;
    TRISBbits.TRISB10 = 0;
    TRISBbits.TRISB11 = 0;
    
    TRISBbits.TRISB8=0;// UART1 TX


    // Button Pin Config - Mazerunner
    // Configure RC5 as an input pin to register switch presses
    TRISCbits.TRISC5 = 1;  // Set RC5 as an input (Switch in Mazerunner)
    __builtin_enable_interrupts();


    //PIN MAPPING - Mazerunner
    //before we map, we need to unlock
    __builtin_write_OSCCONL(OSCCON & 0xbf); // clear bit 6 (unlock, they are usually write protected)
    
    // PERIPHERAL receives data from which INPUT  
    RPINR18bits.U1RXR = 7; //mapped to RP7 is U1 RX, CHANGE THIS // MAZERUNNER

    //OUTPUT PIN receives data from which PERIPHERAL, 
    //see table 11-2 in datasheet to check peripheral codes 
    RPOR4bits.RP8R = 0b00011; //output bin RP8 gets data from peripheral U1 TX  // MAZERUNNER


    //PERIPHERAL QEA Encoder 1, receives data from RP24 and RP25
    RPINR14bits.QEA1R = 24; // mazerunner change
    RPINR14bits.QEB1R = 25; // mazerunner change

    //PERIPHERAL QEA Encoder 2, receives data from RP19 and RP20
    RPINR16bits.QEA2R = 19; // mazerunner change
    RPINR16bits.QEB2R = 20; // mazerunner change
    
    
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
