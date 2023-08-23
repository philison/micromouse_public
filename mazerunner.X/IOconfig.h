/* 
 * File:   IOconfig.h
 * Author: adamp
 *
 * Created on 13 December 2019, 09:38
 */


/*
* LED CONFIG Mazerunner
* LED1 = RB6 (Remappable PIN)
* LED2 = RB5 (Remappable PIN)
* LED3 = RB10 (PWM)
* LED4 = RB11 (PWM)
*/

#ifndef IOCONFIG_H
#define	IOCONFIG_H

// LED CONFIG Mazerunner
#define LED1 LATBbits.LATB6
#define LED2 LATBbits.LATB5
#define LED3 LATBbits.LATB10
#define LED4 LATBbits.LATB11

// OLD CONFIG
// #define LED4 LATBbits.LATB15
// #define LED5 LATBbits.LATB14
// #define LED6 LATBbits.LATB13
// #define LED7 LATBbits.LATB12


// SWITCH CONFIG Mazerunner
#define SW2 PORTCbits.RC5

// OLD CONFIG
// #define SW1 !PORTBbits.RB5 


#define LEDON 0
#define LEDOFF 1

void setupIO();

#endif	/* IOCONFIG_H */

