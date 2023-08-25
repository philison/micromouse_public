
#include "myPWM.h"
#include <xc.h>
#include "IOconfig.h"

#define _USE_MATH_DEFINES
#include <math.h>
#ifdef M_PI
    #define M_PI 3.14159265358979323846
#endif


/* Mazerunner Config:
* Left Motor: PWM1H1 (Pin: RB14), PWM1L1 (Pin: RB15) (PWM Pin pair 1 of the PWM Module 1)
* Right Motor: PWM1H2 (Pin: RB12), PWM1L2 (Pin: RB13) (PWM Pin pair 2 of the PWM Module 1)
* 
* // Datasheet H-Bridge
* PWM1Lx goes to the H-Bridge input xIN1
* PWM1Hx goes to the H-Bridge input xIN2
*/


void setupPWM()
{
        /* PWM1H1 *, configured to 1kHz, based on fcyc = 26.666 MIPS, Tcycle=37.5nsec/
         * 1ms/37.5nsec = 26666.666 ==> 26666 (fits in 15 bits)
         * of course, we could use a pre-scaler and end up somewhere else
         */
    P1TCONbits.PTEN = 0; // Switch off PWM generator
    P1TCONbits.PTCKPS = 0b00; // Sets prescaler, available are 1(00),4(01),16(10) or 64(11)
    P1TPER = MYPWM_MAX/2; //15 bit register
    PWM1CON1bits.PMOD1 = 1; // Left Motor PWM Pin pair/ set PWM unit 1 to independent mode: sets the PWM output pin pair such that both pins are either active high or low -> not complementary / Complementary mode would be 0
    PWM1CON1bits.PMOD2 = 1; // Right Motor PWM Pin pair


    // TODO: Check if Needed for Output Override Mode ?  
    // PWM1CON1bits.OSYNC = 1 // Output overrides via the PxOVDCON register are synchronized to the PWM time base

    // 0 = disable PWM driver, I/O pin becomes general purpose I/O, Value can be written to it via the Latch
    // 1 = enable PWM driver, PWM output appears on the I/O pin

    /*  For the Motors:
    *   The two PWM outputs 1H1 and 1H2 are used to control the H-bridge inputs AIN2 and BIN2
    *
    *   1H1 <--> AIN2
    *   1H2 <--> BIN2
    * 
    *   1L1 <--> AIN1
    *   1L2 <--> BIN1
    */

    /*
        PWM Control of Motor Speed

        RB15/RB13 | RB14/RB12 | FUNCTION
        -------------------------------
        PWM       | 0         | Forward PWM, fast decay
        1         | PWM       | Forward PWM, slow decay
        0         | PWM       | Reverse PWM, fast decay
        PWM       | 1         | Reverse PWM, slow decay
    */

    // // Left Motor:
    // // Reverse & fast decay OR Forward & slow decay
    // PWM1CON1bits.PEN1H = 1; // enable  PWM driver PWM1H1
    // PWM1CON1bits.PEN1L = 0; // disable PWM driver, I/O pin becomes general purpose I/O 
    // // Forward & fast decay OR Reverse & slow decay
    // // PWM1CON1bits.PEN1H = 0; // disable PWM driver PWM1H1
    // // PWM1CON1bits.PEN1L = 1; // enable PWM driver 


    // // Right Motor:
    // // Reverse & fast decay OR Forward & slow decay
    // PWM1CON1bits.PEN2H = 1; // enable PWM driver, 
    // PWM1CON1bits.PEN2L = 0; // disable PWM driver, I/O pin becomes general purpose I/O
    // // Forward & fast decay OR Reverse & slow decay
    // // PWM1CON1bits.PEN2H = 0; // disable PWM driver
    // // PWM1CON1bits.PEN2L = 1; // enable PWM driver

    // Motor Control via OVERRIDE CONTROL REGISTER

    // Enable PWM drivers for all 4 Motor PWM Channels
    PWM1CON1bits.PEN1H = 1;
    PWM1CON1bits.PEN1L = 1;

    PWM1CON1bits.PEN2H = 1;
    PWM1CON1bits.PEN2L = 1;

    // Set forward / reverse and slow / fast decay Mode
    // forward_slow_decay_left();
    // forward_slow_decay_right();
    forward_fast_decay_left();
    forward_fast_decay_right();

    // Third PWM Generator is not needed
    PWM1CON1bits.PEN3H = 0; // disable PWM driver
    PWM1CON1bits.PEN3L = 0; // disable PWM driver


    P1TCONbits.PTEN = 1; // Switch on PWM generator

    // Set the PWM Duty Cycle Registers for the 3 PWM pairs of PWM module 1
    set_DC_and_motor_state_left(0.0f, "forward_slow_decay");
    set_DC_and_motor_state_right(0.0f, "forward_slow_decay");


    // P1DC1 = .01*MYPWM_MAX; //to get 100% DC, you need to write twice the PER Value (2*26666), PWM Duty Cycle 1 Register
    // P1DC1 = .0*MYPWM_MAX_MAZE_MOTOR; // Limit the Motor PWM max duty cycle to 66% for mazerunner to limit the motor voltage to 6V (from 9V H-Bridge Supply) 
    // P1DC2 = .0*MYPWM_MAX_MAZE_MOTOR;
    P1DC3 = 0;


    // RB15 Pin26: LED4 - PWM1L1 
    // RB14 Pin25: LED5 - PWM1H1
    // RB13 Pin24: LED6 - PWM1L2
    // RB12 Pin23: LED7 - PWM1H2


    // Motor Control via OVERRIDE CONTROL REGISTER
    // P1OVDCONbits.POVD1H = 1 // = 1 Output on PWMx I/O pin is controlled by the PWM generator
    // P1OVDCONbits.POVD1H = 0 // = 0 Output on PWMx I/O pin is controlled by the value in the corresponding POUTxH:POUTxL bit
    // P1OVDCONbits.POUT1H = 1 // = 1 PWMx I/O pin is driven active when the corresponding POVDxH:POVDxL bit is cleared
    // P1OVDCONbits.POUT1H = 0 // = 0 PWMx I/O pin is driven inactive when the corresponding POVDxH:POVDxL bit is cleared

}

// Set PWM Duty Cycle in Percent and set the Motor Drive Direction and Decay Mode (Fast or Slow Decay)
// dc percentage as a float and the motor state as a string
void set_DC_and_motor_state_left(float dc, char *motor_state)
{

    // Set Motor Direction and Decay Mode
    if (strcmp(motor_state, "forward_slow_decay") == 0)
    {
        forward_slow_decay_left();
        // In Slow Decay Mode: The H-Bridge supplies the motor with 9V during the PWM off time = 1-dc
        // P1DC1 = (1-dc)*MYPWM_MAX_MAZE_MOTOR;
        setPWM_DCpercentage_Motor_inverted(&P1DC1, dc);
    }
    else if (strcmp(motor_state, "forward_fast_decay") == 0)
    {
        forward_fast_decay_left();
        // P1DC1 = dc*MYPWM_MAX_MAZE_MOTOR;
        setPWM_DCpercentage_Motor(&P1DC1, dc);
    }
    else if (strcmp(motor_state, "reverse_slow_decay") == 0)
    {
        reverse_slow_decay_left();
        // P1DC1 = (1-dc)*MYPWM_MAX_MAZE_MOTOR;
        setPWM_DCpercentage_Motor_inverted(&P1DC1, dc);
    }
    else if (strcmp(motor_state, "reverse_fast_decay") == 0)
    {
        reverse_fast_decay_left();
        // P1DC1 = dc*MYPWM_MAX_MAZE_MOTOR;
        setPWM_DCpercentage_Motor(&P1DC1, dc);
    }
    else
    {
        // Do nothing
    }
    
}

void set_DC_and_motor_state_right(float dc, char *motor_state)
{

    // Set Motor Direction and Decay Mode
    if (strcmp(motor_state, "forward_slow_decay") == 0)
    {
        forward_slow_decay_right();
        // In Slow Decay Mode: The H-Bridge supplies the motor with 9V during the PWM off time = 1-dc
        // setPWM_DCpercentage_Motor(&P1DC2, (1-dc));
        setPWM_DCpercentage_Motor_inverted(&P1DC2, dc);
    }
    else if (strcmp(motor_state, "forward_fast_decay") == 0)
    {
        forward_fast_decay_right();
        // P1DC1 = dc*MYPWM_MAX_MAZE_MOTOR;
        setPWM_DCpercentage_Motor(&P1DC2, dc);
    }
    else if (strcmp(motor_state, "reverse_slow_decay") == 0)
    {
        reverse_slow_decay_right();
        // P1DC1 = (1-dc)*MYPWM_MAX_MAZE_MOTOR;
        setPWM_DCpercentage_Motor_inverted(&P1DC2, dc);
    }
    else if (strcmp(motor_state, "reverse_fast_decay") == 0)
    {
        reverse_fast_decay_right();
        // P1DC1 = dc*MYPWM_MAX_MAZE_MOTOR;
        setPWM_DCpercentage_Motor(&P1DC2, dc);
    }
    else
    {
        LED4 = LEDON;
    }
    
}

   




/* Slow Decay Motor Mode
*   - active braking
*/

void reverse_slow_decay_left() {
    P1OVDCONbits.POVD1H = 1; 
    P1OVDCONbits.POVD1L = 0; // Output Override Enabled

    P1OVDCONbits.POUT1L = 1; // Override Vlaue (values that is set instead of the PWM)
}

void forward_slow_decay_right() {
    P1OVDCONbits.POVD2H = 1; 
    P1OVDCONbits.POVD2L = 0; // Output Override Enabled

    P1OVDCONbits.POUT2L = 1; // Override Vlaue (values that is set instead of the PWM)
}

void forward_slow_decay_left() {
    P1OVDCONbits.POVD1H = 0; // Output Override Enabled
    P1OVDCONbits.POVD1L = 1; 

    P1OVDCONbits.POUT1H = 1; // Override Vlaue (values that is set instead of the PWM)
}

void reverse_slow_decay_right() {
    P1OVDCONbits.POVD2H = 0; // Output Override Enabled
    P1OVDCONbits.POVD2L = 1; 

    P1OVDCONbits.POUT2H = 1; // Override Vlaue (values that is set instead of the PWM)
}

/* Fast Decay Motor Mode
*   - coasting
*/

void reverse_fast_decay_left() {
    P1OVDCONbits.POVD1H = 0; // Output Override Enabled
    P1OVDCONbits.POVD1L = 1; 

    P1OVDCONbits.POUT1H = 0; // Override Vlaue (values that is set instead of the PWM)
}

void forward_fast_decay_right() {
    P1OVDCONbits.POVD2H = 0; // O;utput Override Enabled
    P1OVDCONbits.POVD2L = 1; 

    P1OVDCONbits.POUT2H = 0; // Override Vlaue (values that is set instead of the PWM)
}

void forward_fast_decay_left() {
    P1OVDCONbits.POVD1H = 1; 
    P1OVDCONbits.POVD1L = 0; // Output Override Enabled

    P1OVDCONbits.POUT1L = 0; // Override Vlaue (values that is set instead of the PWM)
}

void reverse_fast_decay_right() {
    P1OVDCONbits.POVD2H = 1; 
    P1OVDCONbits.POVD2L = 0; // Output Override Enabled

    P1OVDCONbits.POUT2L = 0; // Override Vlaue (values that is set instead of the PWM)
}



void setPWM_DCpercentage(uint16_t *pwmDutyCycleRegister, float percentage)
{
    // *pwmDutyCycleRegister = (uint16_t) percentage * MYPWM_MAX;
    *pwmDutyCycleRegister = percentage * MYPWM_MAX;
}


void setPWM_DCpercentage_Motor(uint16_t *pwmDutyCycleRegister, float percentage)
{
    // Add error handling to prefent a duty cycle higher than 100% -> percentage > 1.0f
    if (percentage <= 1.0f)
    {
        *pwmDutyCycleRegister = percentage * MYPWM_MAX_MAZE_MOTOR;
    }
    
}

void setPWM_DCpercentage_Motor_inverted(uint16_t *pwmDutyCycleRegister, float percentage)
{
    // Add error handling to prefent a duty cycle higher than 100% -> percentage > 1.0f
    if (percentage <= 1.0f && percentage >= 0.0f)
    {
        // float inverted_dc = MYPWM_MAX_MAZE_MOTOR - (percentage * MYPWM_MAX_MAZE_MOTOR);
        // float inverted_dc = MYPWM_MAX_MAZE_MOTOR * (1 - percentage);

        // Inverts the PWM Signal to correct for the H-Bridge characteristic in slow decay mode
        // And does ensure that the max time that the motor is supplied with current is limited to 66% of the PWM Period
        float inverted_dc = MYPWM_MAX - (percentage * MYPWM_MAX_MAZE_MOTOR); // float inverted_dc = MYPWM_MAX * ( 1 - 2.0/3.0 * percentage);
        *pwmDutyCycleRegister = inverted_dc;
    }
    
}

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


// Ex 5.4.6
void modulatePWMwithSINE(uint16_t *pwmDutyCycleRegister)
{
    /**
     * Modulate LED4 with a 1Hz sine wave. The sine-function should be called
     * in your 10ms Timer1 ISR you created last week. Remember that the
     * output of sine function is a floating point value in the range [-1.0 .. 1.0]
     * and you need to adapt this to the range of your PWM duty cycle register.
    */
    static float time = 0.0f; 

    time += 0.01f;

    *pwmDutyCycleRegister = (0.5f + 0.5f * sinf(2.0f * M_PI * 1.0f * time)) * MYPWM_MAX; 

}//