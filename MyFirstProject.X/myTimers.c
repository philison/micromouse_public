#include "myTimers.h"
#include "IOconfig.h"
#include "myPWM.h"
#include "serialComms.h"

#define _USE_MATH_DEFINES
#include <math.h>
#ifdef M_PI
    #define M_PI 3.14159265358979323846
#endif



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


    // if (timeInNS < Tcycle) {
    //     // Not timable
    // }
    // else if(timeInNS > max_ticks * Tcycle) {
    //     // Prescaler: 1
    //     T1CONbits.TCKPS = 0b00;
    //     period = calcPeriod(timeInNS, Tcycle, 1f);
    // }
    // else if(timeInNS > max_ticks * Tcycle * 8f) {
    //     // Prescaler: 1
    //     T1CONbits.TCKPS = 0b01;
    //     period = calcPeriod(timeInNS, Tcycle, 8f);
    // }
    
    
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

// Original
// void __attribute__((__interrupt__, auto_psv)) _T1Interrupt(void)
// {
//     /**
//      * 
//     */
//     static int myCount=0;
//     IFS0bits.T1IF = 0;           // reset Timer 1 interrupt flag 
//     myCount++;
//     // LED6=~LED6;
// }//

// // Ex 4.4.4
// void __attribute__((__interrupt__, auto_psv)) _T1Interrupt(void)
// {
//     /**
//      * Implement a software counter in your Timer1 ISR which will allow you
//      * to toggle LED6 with exactly 1Hz while LED5 keeps on blinking with a frequency of 50Hz
//      * The ISR should be called every 1ms
//     */
//     static int myCount=0;
//     IFS0bits.T1IF = 0;           // reset Timer 1 interrupt flag 

//     myCount++;

//     if(myCount % 20 == 0)
//     {
//         LED5=~LED5;
//     }

//     if(myCount == 1000)
//     {
//         LED6=~LED6;
//         myCount=0;
//     }
// }//

// // Ex 5.4.5
// void __attribute__((__interrupt__, auto_psv)) _T1Interrupt(void)
// {
//     /**
//      * Now write a program that drives LED4 with four different pulse widths
//      * (10%, 50%, 75% and 100%) for 5 seconds each. Repeat this sequence
//      * continuously. Use the Timer1 interrupt routine (with a 10ms period) to
//      * achieve the timing. Verify this with the scope. Take scope screenshots
//      * and add them to your log-book and report.
//     */
//     static int myCountTime=0;
//     static int indexDC=0;

//     static float percentagesDC[] = {0.1f, 0.25f, 0.5f, 0.75f};

//     IFS0bits.T1IF = 0;           // reset Timer 1 interrupt flag 

//     myCountTime++;

//     // Count 5s / 10ms = 500
//     if(myCountTime == 500)
//     {
//         if (indexDC == sizeof(percentagesDC) / sizeof(percentagesDC[0])){
//             indexDC = 0;
//         };
//         // LED4=~LED4;
//         P1DC1 = percentagesDC[indexDC]*MYPWM_MAX; //to get 100% DC, you need to write twice the PER Value (2*26666), PWM Duty Cycle 1 Register
//         myCountTime=0;
//         indexDC++;
//     }
// }//


// // Ex 5.4.6
// void __attribute__((__interrupt__, auto_psv)) _T1Interrupt(void)
// {
//     /**
//      * Modulate LED4 with a 1Hz sine wave. The sine-function should be called
//      * in your 10ms Timer1 ISR you created last week. Remember that the
//      * output of sine function is a floating point value in the range [-1.0 .. 1.0]
//      * and you need to adapt this to the range of your PWM duty cycle register.
//     */
//     static float time = 0.0f; 

//     IFS0bits.T1IF = 0;           // reset Timer 1 interrupt flag 

//     time += 0.01f;

//     P1DC1 = (0.5f + 0.5f * sinf(2.0f * M_PI * 1.0f * time)) * MYPWM_MAX; 

// }//


// // Ex 5.4.6
// void __attribute__((__interrupt__, auto_psv)) _T1Interrupt(void)
// {
//     IFS0bits.T1IF = 0;           // reset Timer 1 interrupt flag 

//     modulatePWMwithSINE(&P1DC1);
// }//


// void __attribute__((__interrupt__, auto_psv)) _T1Interrupt(void)
// {
//     IFS0bits.T1IF = 0;           // reset Timer 1 interrupt flag 
//     static int myCount=0;

//     if (myCount <= 10){
//         // setPWM_DCpercentage(&P1DC1, 0.01f);
//         P1DC1 = 0.95f * MYPWM_MAX;
//         // LED5=LEDOFF;
//     }

//     myCount++;

//     if (myCount >= 100){
//         // setPWM_DCpercentage(&P1DC1, 0.95f * MYPWM_MAX);
//         P1DC1 = 0.01f * MYPWM_MAX;
//         // LED5=LEDON;
//         myCount=0;
//     }
// }//



void __attribute__((__interrupt__, auto_psv)) _T1Interrupt(void)
{
    IFS0bits.T1IF = 0;           // reset Timer 1 interrupt flag 
    static int myCount=0;

    char buffer[10];
    // sprintf(buffer, "Hello World %d\n", myCount);
    sprintf(buffer, "U");

    if (myCount >= 100){
         //putsUART2(buffer);
         myCount=0;
         LED5=~LED5;
    }
    putsUART1(buffer);

    //LED5=~LED5;
    LED4=~LED4;

    myCount++;

}//


// void __attribute__((__interrupt__, auto_psv)) _T1Interrupt(void)
// {
//     IFS0bits.T1IF = 0;           // reset Timer 1 interrupt flag 
//     static int myCount=0;

//     if (myCount >= 100){
//          //putsUART2(buffer);
//          myCount=0;
//          LED5=~LED5;
//     }

//     myCount++;

//     U1TXREG='U';

// }//

