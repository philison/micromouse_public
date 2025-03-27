#include "myTimers.h"
#include "IOconfig.h"
#include "myPWM.h"
#include "serialComms.h"
#include "motorEncoders.h"
#include "controllers.h"
#include "dma.h"
#include "distanceSensors.h"


#define _USE_MATH_DEFINES
#include <math.h>
#ifdef M_PI
    #define M_PI 3.14159265358979323846
#endif


/************************************
 *
 *   Timer 1
 *
 *************************************/

void initTimer1(unsigned int period) 
{
    //unsigned TimerControlValue;
    
    T1CON = 0;              // ensure Timer 1 is in reset state
    //TimerControlValue=T1CON;
 
    T1CONbits.TCKPS = 0b10; // FCY divide by 64: tick = 2.4us (Tcycle=37.5ns)
    T1CONbits.TCS = 0;      // select internal FCY clock source
    T1CONbits.TGATE = 0;    // gated time accumulation disabled
    TMR1 = 0;
    PR1 = period;           // set Timer 1 period register ()
    IFS0bits.T1IF = 0;      // reset Timer 1 interrupt flag
    IPC0bits.T1IP = 4;      // set Timer1 interrupt priority level to 4
    IEC0bits.T1IE = 1;      // enable Timer 1 interrupt
    T1CONbits.TON = 0;      // leave timer disabled initially
}

/**
 * Calculates the value for the period register
 * @param timeInNS
 * @param Tcycle
 * @param prescaler
*/
unsigned int calcPeriod(float timeInNS, float Tcycle, float prescaler)
{
    return (unsigned int) (timeInNS / (Tcycle * prescaler));
}

typedef struct {
    float value[4];
    int binary[4];
} prescaler_t;


/**
 * Timer 1
 * Takes a time in ms and after selecting the suitable prescaler, sets timer1 to that value
 * @param timeInMS
*/
void initTimer1inMS(float timeInMS)
{
    // Prescaler Values: 1:1, 1:8, 1:64, 1:256
    // The Prescaler "slows" down the clock signal of 26.666 MHz (Tcylce: 37.5ns) by the given factor
    // We want to select a prescaler value that gives us the best resolution,
    // i.e. the smallest possible tick size while still being able to count up to (2^16 - 1) = 65535
    
    T1CON = 0;              // ensure Timer 1 is in reset state

    float timeInNS = timeInMS * 1000000.0f; // convert to ns
    float Tcycle = 37.5f; // ns
    float max_ticks = 65535.0f;
    unsigned int period = 1;

    prescaler_t prescaler = {
        {1.0f, 8.0f, 64.0f, 256.0f},
        {0b00, 0b01, 0b10, 0b11}
    };

    int numPrescalers = sizeof(prescaler.value) / sizeof(prescaler.value[0]);
    
    // f = ticks / time = 1 / Tcycle 
    // => ticks = f * time = time / Tcycle
    // period = (unsigned int) (timeInNS / (Tcycle * prescaler));

    // Select the optimal prescaler value
    // time = max_ticks * Tcycle * prescaler
    for (int i = 0; i < numPrescalers; i++)
    {
        if ((timeInNS < Tcycle) || (timeInNS > max_ticks * Tcycle * prescaler.value[numPrescalers-1])){
            // Not timable
            // todo: error handling
            break;
        };

        if(timeInNS <= max_ticks * Tcycle * prescaler.value[i])
        {
            T1CONbits.TCKPS = prescaler.binary[i];
            period = calcPeriod(timeInNS, Tcycle, prescaler.value[i]);
            printf("Prescaler Value: %.1f, Binary Value: 0x%X\n", prescaler.value[i], prescaler.binary[i]);
            printf("Calculated Period: %u\n\n", period);
            break;
        };
    };

    
    // T1CONbits.TCKPS = 0b10; // FCY divide by 64: tick = 2.4us (Tcycle=37.5ns)
    T1CONbits.TCS = 0;      // select internal FCY clock source
    T1CONbits.TGATE = 0;    // gated time accumulation disabled
    TMR1 = 0;
    PR1 = period;         // set Timer 1 period register ()
    IFS0bits.T1IF = 0;      // reset Timer 1 interrupt flag
    IPC0bits.T1IP = 4;      // set Timer1 interrupt priority level to 4
    IEC0bits.T1IE = 1;      // enable Timer 1 interrupt
    T1CONbits.TON = 0;      // leave timer disabled initially
}

void startTimer1(void) 
{
    T1CONbits.TON = 1; //
 
}


/* MAJOR ARCHITECTURE CHANGE */

/* Motor Control Loop */
void __attribute__((__interrupt__, auto_psv)) _T1Interrupt(void)
{
    IFS0bits.T1IF = 0;           // reset Timer 1 interrupt flag

    static int myCount=0;
    myCount++;

    // Motor Control Loop
    // Based on the the current movement primitive the correct motor control loop is executed
    // The current encoder value will be compared with the global goal encoder value
    // If the goal is reached a global goal reached flag will be set to true

    switch (currMovementControlParameters.movementPrimitive.type)
    {
    case DRIVING_STRAIGHT:
        // Make sure to check if the goal is reached and update the global flag accordingly
        drivingStraightForNMeters();
        break;

    case DRIVING_STRAIGHT_FOREVER:
        drivingStraightForever();
        break;
    
    case TURNING:
        turningForNDegrees();

        break;

    case PARKING:
        set_DC_and_motor_state_left(0.0, "forward_fast_decay");
        set_DC_and_motor_state_right(0.0, "forward_fast_decay");
        break;

    default:
        break;
    }

}



/**
 * Timer 2
 * Takes a time in ms and after selecting the suitable prescaler, sets timer1 to that value
 * @param timeInMS
*/
void initTimer2inMS(float timeInMS)
{
    // Prescaler Values: 1:1, 1:8, 1:64, 1:256
    // The Prescaler "slows" down the clock signal of 26.666 MHz (Tcylce: 37.5ns) by the given factor
    // We want to select a prescaler value that gives us the best resolution,
    // i.e. the smallest possible tick size while still being able to count up to (2^16 - 1) = 65535
    
    T2CON = 0;              // ensure Timer 2 is in reset state

    float timeInNS = timeInMS * 1000000.0f; // convert to ns
    float Tcycle = 37.5f; // ns
    float max_ticks = 65535.0f;
    unsigned int period = 1;

    prescaler_t prescaler = {
        {1.0f, 8.0f, 64.0f, 256.0f},
        {0b00, 0b01, 0b10, 0b11}
    };

    int numPrescalers = sizeof(prescaler.value) / sizeof(prescaler.value[0]);
    
    // f = ticks / time = 1 / Tcycle 
    // => ticks = f * time = time / Tcycle
    // period = (unsigned int) (timeInNS / (Tcycle * prescaler));

    // Select the optimal prescaler value
    // time = max_ticks * Tcycle * prescaler
    for (int i = 0; i < numPrescalers; i++)
    {
        if ((timeInNS < Tcycle) || (timeInNS > max_ticks * Tcycle * prescaler.value[numPrescalers-1])){
            // Not timable
            // todo: error handling
            break;
        };

        if(timeInNS <= max_ticks * Tcycle * prescaler.value[i])
        {
            T2CONbits.TCKPS = prescaler.binary[i];
            period = calcPeriod(timeInNS, Tcycle, prescaler.value[i]);
            printf("Prescaler Value: %.1f, Binary Value: 0x%X\n", prescaler.value[i], prescaler.binary[i]);
            printf("Calculated Period: %u\n\n", period);
            break;
        };
    };

    
    T2CONbits.TCS = 0;      // select internal FCY clock source
    T2CONbits.TGATE = 0;    // gated time accumulation disabled
    TMR2 = 0;
    PR2 = period;         // set Timer2 period register ()
    IFS0bits.T2IF = 0;      // reset Timer2 interrupt flag
    IPC1bits.T2IP = 4;      // set Timer2 interrupt priority level to 4
    IEC0bits.T2IE = 1;      // enable Timer2 interrupt
    T2CONbits.TON = 0;      // leave timer disabled initially
}

void startTimer2(void) 
{
    T2CONbits.TON = 1; //
 
}


void __attribute__((__interrupt__, auto_psv)) _T2Interrupt(void)
{
    IFS0bits.T2IF = 0;           // reset Timer2 interrupt flag 

    static bool timing = false;
    static int timing_counter=0;

    if (robot_state.start_new_timer2){
        timing = true;
        robot_state.start_new_timer2 = false;
        timing_counter = 0;
    }

    if (timing){
        timing_counter++;
    }

    if (timing_counter >= robot_state.timer2_value){
        // UART
        // char buffer[20];
        // sprintf(buffer, "%i >= %i", timing_counter, robot_state.timer2_value);
        // putsUART1(buffer);

        timing = false;
        robot_state.timer2_expired = true;
        timing_counter = 0;
        // LED4=~LED4;
    }

}
