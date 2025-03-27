/* 
 * File:   main.c
 * Author of Congiuration Bits: Alexander Lenz
 * The rest was added.
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
#include "mazeSolver.h"


/// Defines----------------------------
// #define SEVEN_MEG_OSC 1//set to 1 if we use slow (7.3728 MHz) oscillator and not 16 MHz
#define SEVEN_MEG_OSC 1//set to 1 if we use slow (7.3728 MHz) oscillator and not 16 MHz


/// Important Global Variables-------------------
float timer_time = 10.0; // in [ms], Determines the Control Loop Period
int mazi_running = 0; // 0 = not running, 1 = running

/*
 * 
 */
int main() 
{
    int pinStatus;


     // Mazerunner Osci config:
     /*** oscillator setup --------------------------------------------------
     * The external oscillator runs at 20MHz
     * PLL is used to generate 53.3 MHz clock (FOSC)
     * The relationship between oscillator and cycle frequency: FCY = FOSC/2
     * Have a look at "PLL Configuration" paragraph in the mcu manual
    
     * Result: FCY = 0.5 * (20MHz*16/(3*2)) = 26.666 MIPS, Tcycle=37.5nsec
    ---------------------------------------------------------------------***/
    PLLFBDbits.PLLDIV = 14;           //
    CLKDIVbits.PLLPRE = 1;            //N1 = input/3
    CLKDIVbits.PLLPOST = 0;           //N2 = output/2

    
    
        /* Clock switch to incorporate PLL*/
    __builtin_write_OSCCONH( 0x03 );            // Initiate Clock Switch to Primary

    // Oscillator with PLL (NOSC=0b011)
    __builtin_write_OSCCONL( OSCCON || 0x01 );  // Start clock switching
    
    while( OSCCONbits.COSC != 0b011 );
    
    // In reality, give some time to the PLL to lock
    while (OSCCONbits.LOCK != 1); //Wait for PPL to lock
 
    setupIO(); //configures inputs and outputs
    //initTimer1(4166); //creates a 10ms timer interrupt
    initTimer1inMS(timer_time);
    initTimer2inMS(timer_time);


    /* UART or Bluetooth Module
    * Set the Baudrate in the Baudrate register (U1BRG) 
    * Bluetooth: 115200
    * UART: 115200
    */
    setupUART1(); 
    //setupUART2();

    // Init Encoders
    initQEI1(0);
    initQEI2(0);

    // Switch off all leds at once in the beginning
    LED1 = LED2 = LED3= LED4 = LEDOFF;
    

    // PWM Setup
    setupPWM();

    // Button Setup
    setupButtons();
    
    // IR-Seonsor Setup
    setupADC1();
    startADC1();
    initDmaChannel4();

    // Start the timers
    startTimer1(); // Control Loop Timer
    startTimer2(); // Delay Timer

    // Ensure that the Robot State Machien starts out in the proper state
    switchRobotStateTo(IDLE);

    // Create the stacks for the floodfill algorithm, can be moved to the maze solver files
    Stack *currentLevel = createStack(STACK_SIZE); // current level is needed for the floodfill function
    Stack *nextLevel = createStack(STACK_SIZE);    // next level is needed for the floodfill function


    while(1)
    {

        // Robot State Machine
        switch (robot_state.state)
        {
        case IDLE:
            /* Enter the State */
            if (robot_state.just_switched_state) {
                printString2UARTmax60("RS: IDLE\n");
                robot_state.just_switched_state = false;
            }

            /* Execute the State */
            currMovementControlParameters.movementPrimitive.type = PARKING;

            /* Exit the State */
            // If Button1 was pressed go to DELAY_BEFORE_START
            if (robot_state.button1_pressed) {
                switchRobotStateTo(DELAY_BEFORE_START);
                robot_state.button1_pressed = false;
            }
            break;
        
        case DELAY_BEFORE_START:
            // printString2UARTmax60("RS: DELAY_BEFORE_START\n"); // TESTING
            /* Enter the State */
            if (robot_state.just_switched_state) {
                timeWithTimer2(1000); // 1 second delay in ms
                robot_state.just_switched_state = false;
            }

            /* Execute the State */

            /* Exit the State */
            // If the delay timer2 has expired go to EXECUTE
            if (robot_state.timer2_expired) {
                switchRobotStateTo(EXECUTE);
                robot_state.timer2_expired = false;
                printString2UARTmax60("RS: Exiting DELAY_BEFORE_START\n");
            }
            break;

        case EXECUTE:
            // printString2UARTmax60("RS: EXECUTE\n"); // TESTING
            /* Enter the State */
            if (robot_state.just_switched_state) {
                // Set the just_switched_state flag to false
                robot_state.just_switched_state = false;
            }


            /* Execute the selected Execute Mode */

            // EM1 - Wall Follower
            // simpleWallFollower();
            
            // EM2 - Motion Primitive Executor
            // simpleMotionPrimitiveExecutor();

            // EM3 - Maze Solver
            mazeSolver(nextLevel, currentLevel);

            // EM4 - Remote Controlled Motion Primitive Executor
            // remoteControlledMotionPrimitiveExecutor();

            
            /* Exit the State */
            // If the EXECUTion goal is reached go to STOP, has to be set somewhere within the function called in the EXECUTE state
            if (robot_state.execution_finished) {
                printString2UARTmax60("RS: Execution finished\n");
                switchRobotStateTo(STOP);
            } else if (robot_state.button1_pressed) {
                // TODO: Implement a way to stop the robot while it is executing a motion primitive, does not work yet
                switchRobotStateTo(IDLE);
                robot_state.button1_pressed = false;
                printString2UARTmax60("RS: exEx due to btn press\n");
            }
            
            break;

        case STOP:
            printString2UARTmax60("RS: STOP\n");
            /* Enter the State */

            /* Execute the State */
            // Shut off the Motors or for no set Motor controller to PARKING
            currMovementControlParameters.movementPrimitive.type = PARKING;
            
            /* Exit the State */
            switchRobotStateTo(IDLE);
            break;

        case EMERGENCY:
            printString2UARTmax60("RS: EMERGENCY\n");
            /* Enter the State */
            
            /* Execute the State */
            // Do custome Emergency Stuff
            // Like sending a message via UART
            // TODO: Turn everything/Mc modules actually off not just in an idle model like in parking
            currMovementControlParameters.movementPrimitive.type = PARKING;
            
            /* Exit the State */

            break;

        default:
            break;
        }

    };
 
    
    return 0;
}

