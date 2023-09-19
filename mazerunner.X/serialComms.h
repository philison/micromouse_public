/* Microchip Technology Inc. and its subsidiaries.  You may use this software 
 * and any derivatives exclusively with Microchip products. 
 * 
 * THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS".  NO WARRANTIES, WHETHER 
 * EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED 
 * WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A 
 * PARTICULAR PURPOSE, OR ITS INTERACTION WITH MICROCHIP PRODUCTS, COMBINATION 
 * WITH ANY OTHER PRODUCTS, OR USE IN ANY APPLICATION. 
 *
 * IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, 
 * INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND 
 * WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS 
 * BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE.  TO THE 
 * FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS 
 * IN ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF 
 * ANY, THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
 *
 * MICROCHIP PROVIDES THIS SOFTWARE CONDITIONALLY UPON YOUR ACCEPTANCE OF THESE 
 * TERMS. 
 */

/* 
 * File:   
 * Author: 
 * Comments:
 * Revision history: 
 */

// This is a guard condition so that contents of this file are not included
// more than once.  
#ifndef SERIALCOMMS_H
#define	SERIALCOMMS_H

#include <xc.h> // include processor files - each processor file is guarded.
#include <stdio.h> // For string handling functions
#include <stdbool.h> // For bool type

// Enum to define command codes
enum Commands {
    CMD_NONE,   // No command
    CMD_VEL,    // Velocity command
    CMD_TURN,   // Turn command
    CMD_DRIVE,  // Drive command
    CMD_STOP,   // Stop command
    CMD_EMERGENCY, // Emergency stop command
    CMD_PARK,   // Park command
    CMD_START,  // Start command
    // Add more command codes as needed
};

// Struct to store command information
struct UARTCommand {
    enum Commands command_code;
    char name[20];
    union {
        float fval;
        int ival;
    } command_value;
    bool new_command;
};

extern volatile struct UARTCommand currentUARTCommand;

void setupUART1(void);
void putsUART1(char *buffer);
void putsUART2(char *buffer);

struct UARTCommand parseCommand(const char* commandString);
void handleNewUARTCommand();
void sendParsedValues();

#endif	/* SERIALCOMMS_H */

