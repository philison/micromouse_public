/* 
 * File:   main.c
 * Author: Alexander Lenz
 *
 * Created on 27 Nov 2020, 09:36
 */


/// Configuration Bits---------------------------

// FBS
#pragma config BWRP = WRPROTECT_OFF     // Boot Segment Write Protect (Boot Segment may be written)
#pragma config BSS = NO_FLASH           // Boot Segment Program Flash Code Protection (No Boot program Flash segment)

// FGS
#pragma config GWRP = OFF               // General Code Segment Write Protect (User program memory is not write-protected)
#pragma config GSS = OFF                // General Segment Code Protection (User program memory is not code-protected)

// FOSCSEL
#pragma config FNOSC = FRC                 // Start with Internal RC Oscillator
#pragma config IESO = OFF                // Internal External Switch Over Mode (Start-up device with FRC, then automatically switch to user-selected oscillator source when ready)

// FOSC
#pragma config POSCMD = HS             // Primary Oscillator Source (HS Oscillator Mode)
#pragma config OSCIOFNC = OFF           // OSC2 Pin Function (OSC2 pin has clock out function)
#pragma config IOL1WAY = OFF            // Peripheral Pin Select Configuration (Allow Multiple Re-configurations)
#pragma config FCKSM = CSECMD           // Clock Switching and Monitor (Both Clock Switching and Fail-Safe Clock Monitor are enabled)

// FWDT
#pragma config WDTPOST = PS1            // Watchdog Timer Postscaler (1:1)
#pragma config WDTPRE = PR128           // WDT Prescaler (1:128)
#pragma config WINDIS = OFF             // Watchdog Timer Window (Watchdog Timer in Non-Window mode)
#pragma config FWDTEN = OFF             // Watchdog Timer Enable (Watchdog timer enabled/disabled by user software)

// FPOR
#pragma config FPWRT = PWR128           // POR Timer Value (128ms)
#pragma config ALTI2C = ON             // Alternate I2C  pins (I2C mapped to SDA1/SCL1 pins)
#pragma config LPOL = ON                // Motor Control PWM Low Side Polarity bit (PWM module low side output pins have active-high output polarity)
#pragma config HPOL = ON                // Motor Control PWM High Side Polarity bit (PWM module high side output pins have active-high output polarity)
#pragma config PWMPIN = ON              // Motor Control PWM Module Pin Mode bit (PWM module pins controlled by PORT register at device Reset)

// FICD
#pragma config ICS = PGD1               // Comm Channel Select (Communicate on PGC1/EMUC1 and PGD1/EMUD1)
#pragma config JTAGEN = OFF             // JTAG Port Enable (JTAG is Disabled)


/// Include headers-------------------------------
#include "xc.h"
#include "IOconfig.h"
#include "myTimers.h"
#include "myPWM.h"
#include "serialComms.h"
#include "adc.h"
#include "dma.h"
#include <stdlib.h>


/// Defines----------------------------
#define SEVEN_MEG_OSC 1//set to 1 if we use slow (7.3728 MHz) oscillator and not 16 MHz

/*
 * 
 */
int main() 
{
    int pinStatus;
#if (SEVEN_MEG_OSC == 0) 
     /*** oscillator setup --------------------------------------------------
     * The external oscillator runs at 16MHz
     * PLL is used to generate 53.3 MHz clock (FOSC)
     * The relationship between oscillator and cycle frequency: FCY = FOSC/2
     * Have a look at "PLL Configuration" paragraph in the mcu manual
    
     * Result: FCY = 0.5 * (16MHz*20/(3*2)) = 26.666 MIPS, Tcycle=37.5nsec
    ---------------------------------------------------------------------***/
    PLLFBDbits.PLLDIV = 18;                      //set PPL to M=20 (18+2)
    CLKDIVbits.PLLPRE = 1;            //N1 = input/3
    CLKDIVbits.PLLPOST = 0;           //N2 = output/2
    
    
    
#else //Below the 7.3728 Setup 
    
         /*** oscillator setup --------------------------------------------------
     * The external oscillator runs at 7.3728 MHz
     * PLL is used to generate 53.3 MHz clock (FOSC)
     * The relationship between oscillator and cycle frequency: FCY = FOSC/2
     * Have a look at "PLL Configuration" paragraph in the mcu manual
    
     * Result: FCY = 0.5 * (7.3728 MHz*29/(2*2)) = 26.73 MIPS, which is 
          * not exactl Tcycle=37.5nsec, but close enough for our purposes
    ---------------------------------------------------------------------***/
    PLLFBDbits.PLLDIV = 27;                      //set PPL to M=29 (27+2)
    CLKDIVbits.PLLPRE = 0;            //N1 = input/2
    CLKDIVbits.PLLPOST = 0;           //N2 = output/2
#endif //SEVEN_MEG_OSC == 0
    
    
        /* Clock switch to incorporate PLL*/
    __builtin_write_OSCCONH( 0x03 );            // Initiate Clock Switch to Primary

    // Oscillator with PLL (NOSC=0b011)
    __builtin_write_OSCCONL( OSCCON || 0x01 );  // Start clock switching
    
    while( OSCCONbits.COSC != 0b011 );
    
    // In reality, give some time to the PLL to lock
    while (OSCCONbits.LOCK != 1); //Wait for PPL to lock
 
    setupIO(); //configures inputs and outputs
    //initTimer1(4166); //creates a 10ms timer interrupt
    initTimer1inMS(1.0);
    setupUART1();
    // unsigned int 
    initQEI1(0);

    startTimer1();
    
    // LED4 = 1; //switches off
    // LED5 = LEDOFF;
    // LED6 = LEDOFF;
    // LED7 = LEDOFF;

    // Switch off all leds at once in the beginning
    LED4 = LED5 = LED6 = LED7 = LEDOFF;
    
    //setupUART2();
    
    // setupPWM(); DEBUG

    // setPWM_DCpercentage(&P1DC1, 0.1);
    LATAbits.LATA0 = 0; // Set Motor direction to forward
    
    //setupButtons();
    
    // setupADC1(); DEBUG
    // startADC1();DEBUG
    // initDmaChannel4();DEBUG
    


    while(1)
    {
        

    };
 
    
    return 0;
}

