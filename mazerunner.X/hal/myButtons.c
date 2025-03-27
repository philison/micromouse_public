#include "myButtons.h"
#include <xc.h>
#include "IOconfig.h"

#include "myPWM.h"



/**
 * All port pins have three registers directly associated
 * with their operation as digital I/O. The data direction
 * register (TRISx) determines whether the pin is an input
 * or an output. If the data direction bit is a ‘1’, then the pin
 * is an input. All port pins are defined as inputs after a
 * Reset. Reads from the latch (LATx), read the latch.
 * Writes to the latch, write the latch. Reads from the port
 * (PORTx), read the port pins, while writes to the port
 * pins, write the latch.
*/


// void setupButtons() {
//     // Configure the pins connected to the buttons, to be used as digital inputs, this happens in IOconfig.h
//     // SW1 is connected to Pin14,RB5

//     // INT1IF: External Interrupt 1 Flag Status bit, 1 = Interrupt request has occurred, 0 = Interrupt request has not occurred
//     // INT1EP: External Interrupt 1 Edge Detect Polarity Select bit, 1 = Interrupt on negative edge, 0 = Interrupt on positive edge
//     // INT1IE: External Interrupt 1 Enable bit, 1 = Enables external interrupt, 0 = Disables external interrupt
//     // INT1IP<2:0>: External Interrupt 1 Priority bits, 111 = Interrupt is high priority, 000 = Interrupt is low priority

//     // // Configure the switch interrupt
//     // INTCON2bits.INT1EP = 0;  // Interrupt on positive edge
//     // IPC5bits.INT1IP = 3;     // Set interrupt priority (example: priority 3)
//     // IFS1bits.INT1IF = 0;     // Clear the interrupt flag
//     // IEC1bits.INT1IE = 1;     // Enable the interrupt


//     // Configure the switch interrupt using IOC (Interrupt-on-Change)
//     CNPU2bits.CN27PUE = 1;  // Enable internal pull-up resistor for RB5

//     IPC4bits.CNIP = 3;      // Set interrupt priority (example: priority 3)
//     IFS1bits.CNIF = 0;      // Clear the interrupt flag
//     IEC1bits.CNIE = 1;      // Enable the interrupt

//     CNEN2bits.CN27IE = 1;   // Enable CN27 (RB5) change notification interrupt
// }

// Mazerunner
void setupButtons() {
    // Configure the pins connected to the buttons, to be used as digital inputs, this happens in IOconfig.h
    // SW1 is connected to RP21,RC5, Pin 38

    // INT1IF: External Interrupt 1 Flag Status bit, 1 = Interrupt request has occurred, 0 = Interrupt request has not occurred
    // INT1EP: External Interrupt 1 Edge Detect Polarity Select bit, 1 = Interrupt on negative edge, 0 = Interrupt on positive edge
    // INT1IE: External Interrupt 1 Enable bit, 1 = Enables external interrupt, 0 = Disables external interrupt
    // INT1IP<2:0>: External Interrupt 1 Priority bits, 111 = Interrupt is high priority, 000 = Interrupt is low priority

    // // Configure the switch interrupt
    // INTCON2bits.INT1EP = 0;  // Interrupt on positive edge
    // IPC5bits.INT1IP = 3;     // Set interrupt priority (example: priority 3)
    // IFS1bits.INT1IF = 0;     // Clear the interrupt flag
    // IEC1bits.INT1IE = 1;     // Enable the interrupt


    // Configure the switch interrupt using IOC (Interrupt-on-Change)
    CNPU2bits.CN26PUE = 1;  // Enable internal pull-up resistor for RC5 (Look up the CNx name in datasheet for the pin)

    IPC4bits.CNIP = 3;      // Set interrupt priority (example: priority 3)
    IFS1bits.CNIF = 0;      // Clear the interrupt flag
    IEC1bits.CNIE = 1;      // Enable the interrupt

    CNEN2bits.CN26IE = 1;   // Enable CN27 (RC5) change notification interrupt
}


// // Ex 5.4.7
// void __attribute__((__interrupt__, auto_psv)) _INT1Interrupt(void)
// {
//     IFS1bits.INT1IF = 0;     // Clear the interrupt flag
 
//     static int myCountPress=0;
//     static float percentagesDC[] = {0.1f, 0.25f, 0.5f, 0.75f};

//     if (myCountPress >= sizeof(percentagesDC) / sizeof(percentagesDC[0])){
//         myCountPress = 0;
//     };

//     // setPWM_DCpercentage(&P1DC1, percentagesDC[myCountPress]);
//     LED5=~LED5;

//     myCountPress++;

// }//


// Ex 5.4.7
// void __attribute__((interrupt, no_auto_psv)) _CNInterrupt(void) {
//     IFS1bits.CNIF = 0;      // Clear the interrupt flag
 
//     static int myCountPress=0;
//     static float percentagesDC[] = {0.1f, 0.25f, 0.5f, 0.75f};

//     if (myCountPress >= sizeof(percentagesDC) / sizeof(percentagesDC[0])){
//         myCountPress = 0;
//     };

//     setPWM_DCpercentage(&P1DC1, percentagesDC[myCountPress]);
//     // LED5=~LED5;

//     myCountPress++;
// }


// Toggel LED 
void __attribute__((interrupt, no_auto_psv)) _CNInterrupt(void) {
    IFS1bits.CNIF = 0;      // Clear the interrupt flag

    static int flankCouter = 0;

    // Toggel LED on every second flank, since the interupt gets triggered
    // on the rising and falling edge of a single button press
    if (flankCouter % 2 == 0){
        // LED2 = ~LED2;
        // mazi_running = !mazi_running;
        // Set the button1_pressed flag to true in the global robot_state struct
        robot_state.button1_pressed = true;
    }

    flankCouter++;
}