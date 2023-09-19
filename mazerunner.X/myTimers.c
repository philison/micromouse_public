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

/* Timer 2 */
/**
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

// Test Hello World Blinking Mazerunner
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

//     // if(myCount % 20 == 0)
//     // {
//     //     LED1=~LED1;
//     // }

//     LED1=~LED1;

//     // if(myCount == 1000)
//     // {
//     //     LED2=~LED2;
//     //     myCount=0;
//     // }
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


/* Write a value to the UART using the putsUART1 function - Mazerunner */
// void __attribute__((__interrupt__, auto_psv)) _T1Interrupt(void)
// {
//     IFS0bits.T1IF = 0;           // reset Timer 1 interrupt flag 
//     static int myCount=0;

//     char buffer[20];
//     // sprintf(buffer, "Hello World %d\n", myCount);
//     sprintf(buffer, "Hello Roman");

//     if (myCount >= 1000){
//         //putsUART2(buffer);
//         putsUART1(buffer);
//         myCount=0;
//         LED4=~LED4;
//     }

//     // LED4=~LED4;

//     myCount++;

// }//


/* Write a value to the UART directely by putting the message into the U1TXREG Register */
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


/* A2D Conversion of the Poti and setting the PWM DC Cyle to the read value. Additionally writing the value to the UART */
// void __attribute__((__interrupt__, auto_psv)) _T1Interrupt(void)
// {
//    /**
//     * Dim LED4 with the potentiometer. 
//     * The potentiometer value can be red from the define TEST_SENSOR
//    */

//    IFS0bits.T1IF = 0;           // reset Timer 1 interrupt flag

//    // Convert the value of the potentiometer to a percentage for the PWM
//    float potiPercentage = (float)TEST_SENSOR / 4096.0f; // 12 bit ADC therfore 2^12 = 4096 
//    P1DC1 = potiPercentage * MYPWM_MAX; //to get 100% DC, you need to write twice the PER Value (2*26666), PWM Duty Cycle 1 Register


//    // UART
//    char buffer[10];
//    // PWM
//    sprintf(buffer, "%i", TEST_SENSOR);
//    // Other Analog Pin
//    sprintf(buffer, "%i", IO_1);
//    putsUART1(buffer);

//    LED4=~LED4;
// }


// Mazerunner
/* Read Motor Encoder Values and set Motor Velocity*/
// void __attribute__((__interrupt__, auto_psv)) _T1Interrupt(void)
// {
//     IFS0bits.T1IF = 0;           // reset Timer 1 interrupt flag 
//     static int myCount=0;
//     static int myCount2=0;


//     // long motor_count = getPositionInCounts_1();
//     // int motor_vel_Right = getVelocityInCountsPerSample_1();
//     // int motor_vel_Left = getVelocityInCountsPerSample_2(); 

    
//     // float motor_vel_Left = getVelocityInRadPerSecond_Left();
//     // float motor_vel_Right = getVelocityInRadPerSecond_Right(); 

//     // float motor_vel_Left = getVelocityInRadPerSecond_Left();
//     // float motor_vel_Right = getFlanksPerSecond_Right(); 

//     float rps_right = getVelocityInRoundsPerSecond_Right();

//     // char buffer[20];
//     // sprintf(buffer, "%ld\r\n", motor_count);
//     // sprintf(buffer, "Motor_v_Left: %i\r\nMotor_v_Right: %i\r\n\n", motor_vel_Left, motor_vel_Right);
//     // sprintf(buffer, "Motor_v_Left: %.2f\r\nMotor_v_Right: %.2f\r\n\n", motor_vel_Left, motor_vel_Right);
//     // sprintf(buffer, "Motor_v_Left: %f\r\nMotor_v_Right: %f\r\n\n", motor_vel_Left, motor_vel_Right);
    
//     // sprintf(buffer, "a");

//     if (myCount >= 500){
//         char buffer[10];
//         // sprintf(buffer, "L%f R%f\n", motor_vel_Left, motor_vel_Right);
//         sprintf(buffer, "%3.2f\n\n", rps_right);
//         myCount=0;
//         putsUART1(buffer);
//         LED2=~LED2;
//     }

//     if (myCount2 >= 10000){
//         myCount2=0;
//         LED3=~LED3;
//     }

//     myCount++;
//     myCount2++;

// }//



// /* Read Motor Encoder Values */
// void __attribute__((__interrupt__, auto_psv)) _T1Interrupt(void)
// {
//     IFS0bits.T1IF = 0;           // reset Timer 1 interrupt flag 
//     static int myCount=0;

//     long motor_count = getPositionInCounts_1();
//     int motor_vel = getVelocityInCountsPerSample_1();

//     float pwm = pi_vel_controller(17.0, motor_vel);
//     // setPWM_DCpercentage(&P1DC1, pwm);
//     char buffer[40];
//     // sprintf(buffer, "%ld\r\n", motor_count);
//     sprintf(buffer, "PWM: %.2f, Vel: %i\r\n",pwm, motor_vel);
//     putsUART1(buffer);

//     // sprintf(buffer, "%i\r\n", motor_vel);
//     // putsUART1(buffer);


//     if (myCount >= 100){
//          //putsUART2(buffer);
//          myCount=0;
//          LED5=~LED5;
//     }

//     //LED5=~LED5;
//     //LED4=~LED4;

//     myCount++;

// }//



// Mazerunner 
/* A2D Conversion of the 3 IR-Sensors. Additionally writing the value to the UART */
// void __attribute__((__interrupt__, auto_psv)) _T1Interrupt(void)
// {
//     /**
//     * Dim LED4 with the potentiometer. 
//     * The potentiometer value can be red from the define TEST_SENSOR
//     */

//     IFS0bits.T1IF = 0;           // reset Timer 1 interrupt flag

//     static int myCount=0;

//     // Convert the value of the potentiometer to a percentage for the PWM
//     float sensor_left_distance_in_percentage = (float)SENSOR_LEFT / 4096.0f; // 12 bit ADC therfore 2^12 = 4096 
//     float sensor_front_distance_in_percentage = (float)SENSOR_FRONT / 4096.0f; // 12 bit ADC therfore 2^12 = 4096 
//     float sensor_right_distance_in_percentage = (float)SENSOR_RIGHT / 4096.0f; // 12 bit ADC therfore 2^12 = 4096


//     if (myCount >= 500){
//         // UART Buffer
//         char buffer[50];
//         // Print Sensor Values
//         sprintf(buffer, "SLP: %3.2f,\tSFP: %3.2f,\tSRP: %3.2f\n\r", (sensor_left_distance_in_percentage*100), (sensor_front_distance_in_percentage*100), (sensor_right_distance_in_percentage*100));    
//         putsUART1(buffer);
//         myCount=0;
//         LED1=~LED1;
//     }


//     // if (myCount >= 7000 ){
//     //     LED1=~LED1;
//     // }

//     // if (myCount >= 10000 && myCount <= 20000){
//     //     set_DC_and_motor_state_left(0.2, "forward_slow_decay");
//     //     set_DC_and_motor_state_right(0.2, "forward_slow_decay");
//     //     myCount=20000;
//     //     LED1=~LED1;
//     // }

//     myCount++;
// }



/* Control Wall Distance and Driving Forward */
// void __attribute__((__interrupt__, auto_psv)) _T1Interrupt(void)
// {
//     IFS0bits.T1IF = 0;           // reset Timer 1 interrupt flag 
//     static int myCount=0;

//     if (mazi_running == 1){

//         float vel_base = 0.3;

//         float distance_left = (float)SENSOR_LEFT / 4096.0f * 100.0; // 12 bit ADC therfore 2^12 = 4096 
//         float distance_right = (float)SENSOR_RIGHT / 4096.0f * 100.0; // 12 bit ADC therfore 2^12 = 4096 

//         struct Velocities vel_desired = p_wall_centering_controller( distance_left, distance_right, vel_base);

//         // Current Motor Velocities from Encoders
//         float motor_vel_left = getVelocityInRoundsPerSecond_Left();
//         float motor_vel_right = getVelocityInRoundsPerSecond_Right();


//         float dc_left = pi_vel_controller_left(vel_desired.vel_left , motor_vel_left);
//         float dc_right = pi_vel_controller_right(vel_desired.vel_right, motor_vel_right);

//         // if (myCount >= 500){
//         //     char buffer[30];
//         //     // sprintf(buffer, "%ld\r\n", motor_count);
//         //     sprintf(buffer, "LDC: %.3f, RDC: %.3f\r\n", dc_left, dc_right);
//         //     putsUART1(buffer);
//         //     myCount=0;
//         // }

//         if (myCount >= 100){
//             LED1 = ~LED1;
//             myCount=0;
//         }

//         myCount++;

//     } else {
//         set_DC_and_motor_state_left(0.0, "forward_slow_decay");
//         set_DC_and_motor_state_right(0.0, "forward_slow_decay");
//     }
// }



/* Read RPS */
// void __attribute__((__interrupt__, auto_psv)) _T1Interrupt(void)
// {
//     IFS0bits.T1IF = 0;           // reset Timer 1 interrupt flag 
//     static int myCount=0;

//     // Current Motor Velocities from Encoders
//     float motor_vel_left = getVelocityInRoundsPerSecond_Left();
//     // int motor_vel_right = getVelocityInRoundsPerSecond_Right();

//     if (myCount >= 500){
//         char buffer[40];
//         // sprintf(buffer, "%ld\r\n", motor_count);
//         sprintf(buffer, "VL: %.4f\r\n", motor_vel_left);
//         // sprintf(buffer, "PWM: %.2f, Vel: %i\r\n", motor_vel_left);
//         putsUART1(buffer);
//         myCount=0;
//     }

//     myCount++;

// }




/* Control Distance Driven */
// void __attribute__((__interrupt__, auto_psv)) _T1Interrupt(void)
// {
//     IFS0bits.T1IF = 0;           // reset Timer 1 interrupt flag 
//     static int myCount=0;

//     if (mazi_running == 1){

//         float vel_cruise = 0.3;
//         float initial_distance_to_goal = 0.5; // in meters

//         // float distance_to_goal = (float)SENSOR_FRONT / 4096.0f * 100.0; // 12 bit ADC therfore 2^12 = 4096
//         float distance_to_goal = getDistanceToGoalInMeters(initial_distance_to_goal);
//         // float distance_driven = getDistanceDrivenInMeters();

//         float vel_base = p_goal_distance_controller(distance_to_goal, vel_cruise);

//         float distance_left = (float)SENSOR_LEFT / 4096.0f * 100.0; // 12 bit ADC therfore 2^12 = 4096 
//         float distance_right = (float)SENSOR_RIGHT / 4096.0f * 100.0; // 12 bit ADC therfore 2^12 = 4096 

//         struct Velocities vel_desired = p_wall_centering_controller( distance_left, distance_right, vel_base);

//         // Current Motor Velocities from Encoders
//         float motor_vel_left = getVelocityInRoundsPerSecond_Left();
//         float motor_vel_right = getVelocityInRoundsPerSecond_Right();

//         bool shouldTurn = false;
//         float dc_left = pi_vel_controller_left(vel_desired.vel_left , motor_vel_left);
//         float dc_right = pi_vel_controller_right(vel_desired.vel_right, motor_vel_right, shouldTurn);

//         // if (vel_base >= 0) {
//         //     float dc_left = pi_vel_controller_left(vel_desired.vel_left, motor_vel_left, true);
//         //     float dc_right = pi_vel_controller_right(vel_desired.vel_right, motor_vel_right, true);
//         // } else if (vel_base < 0)
//         // {
//         //     float dc_left = pi_vel_controller_left(vel_desired.vel_left, motor_vel_left, false);
//         //     float dc_right = pi_vel_controller_right(vel_desired.vel_right, motor_vel_right, false);
//         // }

//         if (myCount >= 500){
//             LED1 = ~LED1;
//             char buffer[10];
//             // sprintf(buffer, "%2.3f\r\n", distance);
//             float distance = getTotalDrivenDistanceInMeters();
//             // float inter_distance = getDrivenDistanceInMeters2();
//             sprintf(buffer, "%2.4f\r\n", distance);
//             // sprintf(buffer, "%2.2f %2.2f\r\n", distance, inter_distance);
//             putsUART1(buffer);
//             myCount=0;
//         }

//         // if (myCount >= 100){
//         //     LED1 = ~LED1;
//         //     myCount=0;
//         // }

//         myCount++;

//     } else {
//         set_DC_and_motor_state_left(0.0, "forward_slow_decay");
//         set_DC_and_motor_state_right(0.0, "forward_slow_decay");
//     }
// }


/* Control Wall Distance and Driving Forward Printing the driven distance */
// void __attribute__((__interrupt__, auto_psv)) _T1Interrupt(void)
// {
//     IFS0bits.T1IF = 0;           // reset Timer 1 interrupt flag 
//     static int myCount=0;

//     if (mazi_running == 1){

//         float vel_base = 0.3;

//         float distance_left = (float)SENSOR_LEFT / 4096.0f * 100.0; // 12 bit ADC therfore 2^12 = 4096 
//         float distance_right = (float)SENSOR_RIGHT / 4096.0f * 100.0; // 12 bit ADC therfore 2^12 = 4096 

//         struct Velocities vel_desired = p_wall_centering_controller( distance_left, distance_right, vel_base);

//         // Current Motor Velocities from Encoders
//         float motor_vel_left = getVelocityInRoundsPerSecond_Left();
//         float motor_vel_right = getVelocityInRoundsPerSecond_Right();


//         float dc_left = pi_vel_controller_left(vel_desired.vel_left , motor_vel_left);
//         float dc_right = pi_vel_controller_right(vel_desired.vel_right, motor_vel_right);


//         if (myCount >= 500){
//             LED1 = ~LED1;
//             char buffer[10];
//             // sprintf(buffer, "%2.3f\r\n", distance);
//             float distance = getTotalDrivenDistanceInMeters();
//             // sprintf(buffer, "%2.4f\r\n", distance);
//             sprintf(buffer, "%6.2f\r\n", distance);
//             putsUART1(buffer);
//             myCount=0;
//         }

//         // if (myCount >= 100){
//         //     LED1 = ~LED1;
//         //     myCount=0;            
//         // }

//         myCount++;

//     } else {
//         set_DC_and_motor_state_left(0.0, "forward_slow_decay");
//         set_DC_and_motor_state_right(0.0, "forward_slow_decay");
//     }
// }




/* Control Turning by specific Angle */
// void __attribute__((__interrupt__, auto_psv)) _T1Interrupt(void)
// {
//     IFS0bits.T1IF = 0;           // reset Timer 1 interrupt flag 
//     static int myCount=0;

//     if (mazi_running == 1){

//         // float vel_cruise = 0.3;
//         float vel_turn_cruise = 0.3;
//         // float initial_distance_to_goal = 0.30;
//         float initial_angle_to_goal = -180.0; // in degrees, with pos values for right turn and neg values for left turn (clockwise)

//         // float distance_to_goal = getDistanceToGoalInMeters(initial_distance_to_goal);
//         float angle_to_goal = getAngleToGoalInDegrees(initial_angle_to_goal);

//         // float vel_base = p_goal_distance_controller(distance_to_goal, vel_cruise);
//         float vel_turn_base = p_goal_angle_controller(angle_to_goal, vel_turn_cruise);

//         // float distance_left = (float)SENSOR_LEFT / 4096.0f * 100.0; // 12 bit ADC therfore 2^12 = 4096 
//         // float distance_right = (float)SENSOR_RIGHT / 4096.0f * 100.0; // 12 bit ADC therfore 2^12 = 4096 

//         // struct Velocities vel_desired = p_wall_centering_controller( distance_left, distance_right, vel_base);

//         // Current Motor Velocities from Encoders
//         float motor_vel_left = getVelocityInRoundsPerSecond_Left();
//         float motor_vel_right = getVelocityInRoundsPerSecond_Right();

//         bool shouldTurn = true;
//         float dc_left = pi_vel_controller_left(vel_turn_base, motor_vel_left);
//         float dc_right = pi_vel_controller_right(vel_turn_base, motor_vel_right, shouldTurn);


//         if (myCount >= 500){
//             LED1 = ~LED1;
//             char buffer[20];
//             // sprintf(buffer, "%2.3f\r\n", distance);
//             // float distance = getTotalDrivenDistanceInMeters();
//             float distance = getTotalDrivenAngleInDegrees();
//             // float distance = angle_to_goal;
//             // float inter_distance = getDrivenDistanceInMeters2();
//             sprintf(buffer, "%4.4f\r\n", distance);
//             // sprintf(buffer, "%2.2f %2.2f\r\n", distance, inter_distance);
//             putsUART1(buffer);
//             myCount=0;
//         }


//         // if (myCount >= 100){
//         //     LED1 = ~LED1;
//         //     myCount=0;
//         // }

//         myCount++;

//     } else {
//         set_DC_and_motor_state_left(0.0, "forward_slow_decay");
//         set_DC_and_motor_state_right(0.0, "forward_slow_decay");
//     }
// }


/* Test angle calculation from distance traveled of left wheel while driving straight */
// void __attribute__((__interrupt__, auto_psv)) _T1Interrupt(void)
// {
//     IFS0bits.T1IF = 0;           // reset Timer 1 interrupt flag 
//     static int myCount=0;

//     if (mazi_running == 1){

//         // float vel_cruise = 0.3;
//         float vel_turn_cruise = 0.3;
//         // float initial_distance_to_goal = 0.094; // Equals a 90 degree turn in Bogenmaß
//         // float initial_distance_to_goal = -0.3;
//         float initial_angle_to_goal = -90.0; // in degrees, with pos values for right turn and neg values for left turn (clockwise)

//         // float distance_to_goal = getDistanceToGoalInMeters(initial_distance_to_goal);
//         float angle_to_goal = getAngleToGoalInDegrees(initial_angle_to_goal);

//         // float vel_base = p_goal_distance_controller(distance_to_goal, vel_cruise);
//         float vel_turn_base = p_goal_angle_controller(angle_to_goal, vel_turn_cruise);
//         float vel_base = vel_turn_base;

//         float distance_left = (float)SENSOR_LEFT / 4096.0f * 100.0; // 12 bit ADC therfore 2^12 = 4096 
//         float distance_right = (float)SENSOR_RIGHT / 4096.0f * 100.0; // 12 bit ADC therfore 2^12 = 4096 

//         // struct Velocities vel_desired = p_wall_centering_controller( distance_left, distance_right, vel_base);

//         // Current Motor Velocities from Encoders
//         float motor_vel_left = getVelocityInRoundsPerSecond_Left();
//         float motor_vel_right = getVelocityInRoundsPerSecond_Right();


//         // float dc_left = pi_vel_controller_left(vel_desired.vel_left, motor_vel_left, true);
//         // float dc_right = pi_vel_controller_right(vel_desired.vel_right, motor_vel_right, false);

//         if (vel_base >= 0) {
//             float dc_left = pi_vel_controller_left(vel_base, motor_vel_left, true);
//             float dc_right = pi_vel_controller_right(vel_base, motor_vel_right, false);
//         } else if (vel_base < 0)
//         {
//             float dc_left = pi_vel_controller_left(vel_base, motor_vel_left, false);
//             float dc_right = pi_vel_controller_right(vel_base, motor_vel_right, true);
//         }
        




//         // if (myCount >= 500){
//         //     LED1 = ~LED1;
//         //     char buffer[10];
//         //     // sprintf(buffer, "%2.3f\r\n", distance);
//         //     float distance = getTotalDrivenAngleInDegrees();
//         //     sprintf(buffer, "%4.4f\r\n", distance);
//         //     // sprintf(buffer, "%6.2f\r\n", angle_to_goal);
//         //     putsUART1(buffer);
//         //     myCount=0;
//         // }

//         if (myCount >= 500){
//             LED1 = ~LED1;
//             char buffer[20];
//             // sprintf(buffer, "%2.3f\r\n", distance);
//             // float distance = getTotalDrivenDistanceInMeters();
//             float distance = getTotalDrivenAngleInDegrees();
//             // float inter_distance = getDrivenDistanceInMeters2();
//             sprintf(buffer, "%4.4f\r\n", distance);
//             // sprintf(buffer, "L%2.2f R%2.2f\r\n", motor_vel_left, motor_vel_right);
//             // sprintf(buffer, "%2.2f %2.2f\r\n", distance, inter_distance);
//             putsUART1(buffer);
//             myCount=0;
//         }

//         // if (myCount >= 100){
//         //     LED1 = ~LED1;
//         //     myCount=0;
//         // }

//         myCount++;

//     } else {
//         set_DC_and_motor_state_left(0.0, "forward_slow_decay");
//         set_DC_and_motor_state_right(0.0, "forward_slow_decay");
//     }
// }







/* Control Distance Driven and Record Sensor Data while driving along different Obstacles */
// void __attribute__((__interrupt__, auto_psv)) _T1Interrupt(void)
// {
//     IFS0bits.T1IF = 0;           // reset Timer 1 interrupt flag 
//     static int myCount=0;

//     if (mazi_running == 1){

//         static int waitBeforeDriveCounter=0;

//         if (waitBeforeDriveCounter == 0) {
//             // UART Buffer
//             char buffer[20];
//             // Print Sensor Values
//             sprintf(buffer, "SLP \tSFP \tSRP \tm\r\n");
//             putsUART1(buffer);
//         }


//         if (waitBeforeDriveCounter >= 1000){
//             LED1 = ~LED1;
//             // waitBeforeDriveCounter=0;
        
//             float vel_cruise = 0.3;
//             float initial_distance_to_goal = 5 * 0.18; // in meters

//             // float distance_to_goal = (float)SENSOR_FRONT / 4096.0f * 100.0; // 12 bit ADC therfore 2^12 = 4096
//             float distance_to_goal = getDistanceToGoalInMeters(initial_distance_to_goal);
//             // float distance_driven = getDistanceDrivenInMeters();

//             float vel_base = p_goal_distance_controller(distance_to_goal, vel_cruise);

//             // float distance_left = (float)SENSOR_LEFT / 4096.0f * 100.0; // 12 bit ADC therfore 2^12 = 4096 
//             // float distance_right = (float)SENSOR_RIGHT / 4096.0f * 100.0; // 12 bit ADC therfore 2^12 = 4096 

//             // Convert the value of the potentiometer to a percentage for the PWM
//             float sensor_left_distance_in_percentage = (float)SENSOR_LEFT / 4096.0f; // 12 bit ADC therfore 2^12 = 4096 
//             float sensor_front_distance_in_percentage = (float)SENSOR_FRONT / 4096.0f; // 12 bit ADC therfore 2^12 = 4096 
//             float sensor_right_distance_in_percentage = (float)SENSOR_RIGHT / 4096.0f; // 12 bit ADC therfore 2^12 = 4096

//             // struct Velocities vel_desired = p_wall_centering_controller( distance_left, distance_right, vel_base);

//             // Current Motor Velocities from Encoders
//             float motor_vel_left = getVelocityInRoundsPerSecond_Left();
//             float motor_vel_right = getVelocityInRoundsPerSecond_Right();

//             bool shouldTurn = false;
//             // float dc_left = pi_vel_controller_left(vel_desired.vel_left , motor_vel_left);
//             // float dc_right = pi_vel_controller_right(vel_desired.vel_right, motor_vel_right, shouldTurn);

//             float dc_left = pi_vel_controller_left(vel_base, motor_vel_left);
//             float dc_right = pi_vel_controller_right(vel_base, motor_vel_right, shouldTurn);

//             if (myCount >= 500){
//                 LED1 = ~LED1;
//                 // char buffer[10];
//                 // sprintf(buffer, "%2.3f\r\n", distance);
//                 float distance = getTotalDrivenDistanceInMeters();
//                 // float inter_distance = getDrivenDistanceInMeters2();
//                 // sprintf(buffer, "%2.4f\r\n", distance);
//                 // sprintf(buffer, "%2.2f %2.2f\r\n", distance, inter_distance);
//                 // putsUART1(buffer);

//                 // UART Buffer
//                 char buffer[50];
//                 // Print Sensor Values
//                 // sprintf(buffer, "SLP: %3.2f,\tSFP: %3.2f,\tSRP: %3.2f\n\r", (sensor_left_distance_in_percentage*100), (sensor_front_distance_in_percentage*100), (sensor_right_distance_in_percentage*100));    
//                 sprintf(buffer, "%3.2f \t%3.2f \t%3.2f \t%1.3f\n\r", (sensor_left_distance_in_percentage*100), (sensor_front_distance_in_percentage*100), (sensor_right_distance_in_percentage*100), distance);    
//                 putsUART1(buffer);
//                 myCount=0;
//             }

//             // if (myCount >= 100){
//             //     LED1 = ~LED1;
//             //     myCount=0;
//             // }

//             myCount++;

//             if (getTotalDrivenDistanceInMeters() >= initial_distance_to_goal){
//                 waitBeforeDriveCounter = 0;
//             }
//         }
    
//     waitBeforeDriveCounter++;

//     } else {
//         set_DC_and_motor_state_left(0.0, "forward_slow_decay");
//         set_DC_and_motor_state_right(0.0, "forward_slow_decay");
//     }
// }





/* Control Distance Driven and Wall Following Right and Left*/
// Left: 25.5 as wall distance value when driving in the middle
// Right: 27.0 as wall distance value when driving in the middle (left and right differ when as the wheels aren't completely pushed in to the same depth --> therefore weither the wheel center is in the middle or the PCB with the sensors)
// void __attribute__((__interrupt__, auto_psv)) _T1Interrupt(void)
// {
//     IFS0bits.T1IF = 0;           // reset Timer 1 interrupt flag 
//     static int myCount=0;

//     if (mazi_running == 1){

//         static int waitBeforeDriveCounter=0;

//         if (waitBeforeDriveCounter == 0) {
//             // UART Buffer
//             char buffer[20];
//             // Print Sensor Values
//             sprintf(buffer, "SLP \tSFP \tSRP \tm\n\r");    
//             putsUART1(buffer);
//         }


//         if (waitBeforeDriveCounter >= 1000){
//             LED1 = ~LED1;
//             // waitBeforeDriveCounter=0;
        
//             float vel_cruise = 0.3;
//             float initial_distance_to_goal = 5 * 0.18; // in meters

//             // float distance_to_goal = (float)SENSOR_FRONT / 4096.0f * 100.0; // 12 bit ADC therfore 2^12 = 4096
//             float distance_to_goal = getDistanceToGoalInMeters(initial_distance_to_goal);
//             // float distance_driven = getDistanceDrivenInMeters();

//             float vel_base = p_goal_distance_controller(distance_to_goal, vel_cruise);

//             // float distance_left = (float)SENSOR_LEFT / 4096.0f * 100.0; // 12 bit ADC therfore 2^12 = 4096 
//             // float distance_right = (float)SENSOR_RIGHT / 4096.0f * 100.0; // 12 bit ADC therfore 2^12 = 4096 

//             // Convert the value of the potentiometer to a percentage for the PWM
//             float sensor_left_distance_in_percentage = (float)SENSOR_LEFT / 4096.0f * 100.0; // 12 bit ADC therfore 2^12 = 4096 
//             float sensor_front_distance_in_percentage = (float)SENSOR_FRONT / 4096.0f * 100.0; // 12 bit ADC therfore 2^12 = 4096 
//             float sensor_right_distance_in_percentage = (float)SENSOR_RIGHT / 4096.0f * 100.0; // 12 bit ADC therfore 2^12 = 4096

//             // struct Velocities vel_desired = p_wall_centering_controller( distance_left, distance_right, vel_base);
//             struct Velocities vel_desired = p_one_wall_following_right(sensor_right_distance_in_percentage, vel_base);
//             // struct Velocities vel_desired = p_one_wall_following_left(sensor_left_distance_in_percentage, vel_base);

//             // Current Motor Velocities from Encoders
//             float motor_vel_left = getVelocityInRoundsPerSecond_Left();
//             float motor_vel_right = getVelocityInRoundsPerSecond_Right();

//             bool shouldTurn = false;
//             float dc_left = pi_vel_controller_left(vel_desired.vel_left , motor_vel_left);
//             float dc_right = pi_vel_controller_right(vel_desired.vel_right, motor_vel_right, shouldTurn);

//             // float dc_left = pi_vel_controller_left(vel_base, motor_vel_left);
//             // float dc_right = pi_vel_controller_right(vel_base, motor_vel_right, shouldTurn);

//             if (myCount >= 500){
//                 LED1 = ~LED1;
//                 // char buffer[10];
//                 // sprintf(buffer, "%2.3f\r\n", distance);
//                 float distance = getTotalDrivenDistanceInMeters();
//                 // float inter_distance = getDrivenDistanceInMeters2();
//                 // sprintf(buffer, "%2.4f\r\n", distance);
//                 // sprintf(buffer, "%2.2f %2.2f\r\n", distance, inter_distance);
//                 // putsUART1(buffer);

//                 // UART Buffer
//                 char buffer[50];
//                 // Print Sensor Values
//                 // sprintf(buffer, "SLP: %3.2f,\tSFP: %3.2f,\tSRP: %3.2f\n\r", (sensor_left_distance_in_percentage*100), (sensor_front_distance_in_percentage*100), (sensor_right_distance_in_percentage*100));    
//                 sprintf(buffer, "%3.2f \t%3.2f \t%3.2f \t%1.3f\n\r", (sensor_left_distance_in_percentage), (sensor_front_distance_in_percentage), (sensor_right_distance_in_percentage), distance);    
//                 putsUART1(buffer);
//                 myCount=0;
//             }

//             // if (myCount >= 100){
//             //     LED1 = ~LED1;
//             //     myCount=0;
//             // }

//             myCount++;

//             if (getTotalDrivenDistanceInMeters() >= initial_distance_to_goal){
//                 waitBeforeDriveCounter = 0;
//             }
//         }

//     waitBeforeDriveCounter++;

//     } else {
//         set_DC_and_motor_state_left(0.0, "forward_slow_decay");
//         set_DC_and_motor_state_right(0.0, "forward_slow_decay");
//     }
// }






/* Control Distance Driven and Wall Following Left and than turn when reaching front facing wall */
// void __attribute__((__interrupt__, auto_psv)) _T1Interrupt(void)
// {
// }




/* Control Distance Driven and detect which Wall Following Control Mode to Use */
// Wall Detection Threshold Left / Right / Front: 12% values over this threshold will be detected as a wall
// void __attribute__((__interrupt__, auto_psv)) _T1Interrupt(void)
// {
//     IFS0bits.T1IF = 0;           // reset Timer 1 interrupt flag 
//     static int myCount=0;

//     if (mazi_running == 1){

//         static int waitBeforeDriveCounter=0;
//         static enum lateralControlMode oldLateralControlMode;

//         if (waitBeforeDriveCounter == 0) {
//             // UART Buffer
//             char buffer[20];
//             // // Print Sensor Values
//             // sprintf(buffer, "SLP \tSFP \tSRP \tm\n\r");    

//             oldLateralControlMode = getLateralControlMode();
//             const char* lateralControlModeString = getLateralControlModeName(oldLateralControlMode);
//             sprintf(buffer, "%s\n\r", lateralControlModeString);
//             putsUART1(buffer);
//         }


//         if (waitBeforeDriveCounter >= 1000){
//             LED1 = ~LED1;
//             // waitBeforeDriveCounter=0;
        
//             float vel_cruise = 0.3;
//             float initial_distance_to_goal = 5 * 0.18; // in meters

//             // float distance_to_goal = (float)SENSOR_FRONT / 4096.0f * 100.0; // 12 bit ADC therfore 2^12 = 4096
//             float distance_to_goal = getDistanceToGoalInMeters(initial_distance_to_goal, false);
//             // float distance_driven = getDistanceDrivenInMeters();

//             float vel_base = p_goal_distance_controller(distance_to_goal, vel_cruise);

//             // float distance_left = (float)SENSOR_LEFT / 4096.0f * 100.0; // 12 bit ADC therfore 2^12 = 4096 
//             // float distance_right = (float)SENSOR_RIGHT / 4096.0f * 100.0; // 12 bit ADC therfore 2^12 = 4096 

//             // Convert the value of the potentiometer to a percentage for the PWM
//             // float sensor_left_distance_in_percentage = (float)SENSOR_LEFT / 4096.0f * 100.0; // 12 bit ADC therfore 2^12 = 4096 
//             // float sensor_front_distance_in_percentage = (float)SENSOR_FRONT / 4096.0f * 100.0; // 12 bit ADC therfore 2^12 = 4096 
//             // float sensor_right_distance_in_percentage = (float)SENSOR_RIGHT / 4096.0f * 100.0; // 12 bit ADC therfore 2^12 = 4096

//             // struct Velocities vel_desired = p_wall_centering_controller( distance_left, distance_right, vel_base);
//             // struct Velocities vel_desired = p_one_wall_following_right(sensor_right_distance_in_percentage, vel_base);
//             // struct Velocities vel_desired = p_one_wall_following_left(sensor_left_distance_in_percentage, vel_base);

//             // Current Motor Velocities from Encoders
//             float motor_vel_left = getVelocityInRoundsPerSecond_Left();
//             float motor_vel_right = getVelocityInRoundsPerSecond_Right();

//             // bool shouldTurn = false;
//             // float dc_left = pi_vel_controller_left(vel_desired.vel_left , motor_vel_left);
//             // float dc_right = pi_vel_controller_right(vel_desired.vel_right, motor_vel_right, shouldTurn);

//             float dc_left = pi_vel_controller_left(vel_base, motor_vel_left);
//             float dc_right = pi_vel_controller_right(vel_base, motor_vel_right);

//             // if (myCount >= 500){
//             //     LED1 = ~LED1;
//             //     // char buffer[10];
//             //     // sprintf(buffer, "%2.3f\r\n", distance);
//             //     // float distance = getTotalDrivenDistanceInMeters();
//             //     // float inter_distance = getDrivenDistanceInMeters2();
//             //     // sprintf(buffer, "%2.4f\r\n", distance);
//             //     // sprintf(buffer, "%2.2f %2.2f\r\n", distance, inter_distance);
//             //     // putsUART1(buffer);

//             //     // UART Buffer
//             //     char buffer[50];
//             //     // Print Sensor Values
//             //     // sprintf(buffer, "SLP: %3.2f,\tSFP: %3.2f,\tSRP: %3.2f\n\r", (sensor_left_distance_in_percentage*100), (sensor_front_distance_in_percentage*100), (sensor_right_distance_in_percentage*100));    
//             //     // sprintf(buffer, "%3.2f \t%3.2f \t%3.2f \t%1.3f\n\r", (sensor_left_distance_in_percentage), (sensor_front_distance_in_percentage), (sensor_right_distance_in_percentage), distance);
                
//             //     // Print the result of getLateralControlMode()
//             //     enum lateralControlMode LCM = getLateralControlMode();
//             //     const char* lateralControlModeString = getLateralControlModeName(LCM);
//             //     sprintf(buffer, "%s\n\r", lateralControlModeString);

//             //     putsUART1(buffer);
//             //     myCount=0;
//             // }

//             enum lateralControlMode currenLateralControlMode = getLateralControlMode();

//             if (currenLateralControlMode != oldLateralControlMode) {
//                 char buffer[50];
//                 const char* lateralControlModeString = getLateralControlModeName(currenLateralControlMode);
//                 sprintf(buffer, "%s\n\r", lateralControlModeString);

//                 putsUART1(buffer);
//                 oldLateralControlMode = currenLateralControlMode;
//             }



//             // if (myCount >= 100){
//             //     LED1 = ~LED1;
//             //     myCount=0;
//             // }

//             myCount++;

//             if (getTotalDrivenDistanceInMeters(false) >= initial_distance_to_goal){
//                 waitBeforeDriveCounter = 0;
//             }
//         }

//     waitBeforeDriveCounter++;

//     } else {
//         set_DC_and_motor_state_left(0.0, "forward_slow_decay");
//         set_DC_and_motor_state_right(0.0, "forward_slow_decay");
//     }
// }



/* Control Distance Driven and detect and USE the correct Wall Following Control Mode*/
// void __attribute__((__interrupt__, auto_psv)) _T1Interrupt(void)
// {
//     IFS0bits.T1IF = 0;           // reset Timer 1 interrupt flag 
//     static int myCount=0;

//     if (mazi_running == 1){

//         static int waitBeforeDriveCounter=0;
//         static enum lateralControlMode oldLateralControlMode;

//         if (waitBeforeDriveCounter == 0) {
//             // UART Buffer
//             char buffer[30];

//             // Print Sensor Values
//             // sprintf(buffer, "SLP \tSFP \tSRP \tm \tvL \tvR \tcMode\n\r");    
//             sprintf(buffer, "SLP;SFP;SRP;m;vL;vR;cMode\n\r");    

//             // oldLateralControlMode = getLateralControlMode();
//             // const char* lateralControlModeString = getLateralControlModeName(oldLateralControlMode);
//             // sprintf(buffer, "%s\n\r", lateralControlModeString);
//             putsUART1(buffer);
//         }


//         if (waitBeforeDriveCounter >= 1000){
//             LED1 = ~LED1;
//             // waitBeforeDriveCounter=0;
        
//             float vel_cruise = 0.3;
//             float initial_distance_to_goal = 5 * 0.18; // in meters

//             // float distance_to_goal = (float)SENSOR_FRONT / 4096.0f * 100.0; // 12 bit ADC therfore 2^12 = 4096
//             float distance_to_goal = getDistanceToGoalInMeters(initial_distance_to_goal, false);
//             // float distance_driven = getDistanceDrivenInMeters();

//             float vel_base = p_goal_distance_controller(distance_to_goal, vel_cruise);

//             // Convert the value of the potentiometer to a percentage for the PWM
//             float sensor_left_distance_in_percentage = distanceSensorInPercentLeft(); 
//             float sensor_front_distance_in_percentage = distanceSensorInPercentFront(); 
//             float sensor_right_distance_in_percentage = distanceSensorInPercentRight(); 

//             // Switch Case to execute the correct Laterall Control Mode
//             struct Velocities vel_desired;
//             switch (getLateralControlMode())
//             {
//                 case ONE_WALL_FOLLOWING_RIGHT:
//                     vel_desired = p_one_wall_following_right(sensor_right_distance_in_percentage, vel_base);
//                     break;
//                 case ONE_WALL_FOLLOWING_LEFT:
//                     vel_desired = p_one_wall_following_left(sensor_left_distance_in_percentage, vel_base);
//                     break;
//                 case TWO_WALL_CENTERING:
//                     vel_desired = p_wall_centering_controller( sensor_left_distance_in_percentage, sensor_right_distance_in_percentage, vel_base);
//                     break;
//                 case ZERO_WALL_DRIVING:
//                     vel_desired = (struct Velocities){vel_base, vel_base}; // Assign left and right velocities, (struct Velocities){vel_base, vel_base} creates a temporary struct with the specified values and assigns it to vel_desired
//                     // Alternative to the compact professional code line above
//                     // vel_desired.left_velocity = vel_base; // Assign the left wheel velocity
//                     // vel_desired.right_velocity = vel_base; // Assign the right wheel velocity
//                     break;
//                 default:
//                     break;
//             }

//             // Current Motor Velocities from Encoders
//             float motor_vel_left = getVelocityInRoundsPerSecond_Left();
//             float motor_vel_right = getVelocityInRoundsPerSecond_Right();

//             bool shouldTurn = false;
//             float dc_left = pi_vel_controller_left(vel_desired.vel_left , motor_vel_left);
//             float dc_right = pi_vel_controller_right(vel_desired.vel_right, motor_vel_right);

//             // float dc_left = pi_vel_controller_left(vel_base, motor_vel_left);
//             // float dc_right = pi_vel_controller_right(vel_base, motor_vel_right, shouldTurn);

//             if (myCount >= 100){
//                 LED1 = ~LED1;
//                 // char buffer[10];
//                 // sprintf(buffer, "%2.3f\r\n", distance);
//                 float distance = getTotalDrivenDistanceInMeters(false);
//                 // float inter_distance = getDrivenDistanceInMeters2();
//                 // sprintf(buffer, "%2.4f\r\n", distance);
//                 // sprintf(buffer, "%2.2f %2.2f\r\n", distance, inter_distance);
//                 // putsUART1(buffer);

//                 enum lateralControlMode currenLateralControlMode = getLateralControlMode();

//                 // UART Buffer
//                 char buffer[40];
//                 // Print Sensor Values
//                 // sprintf(buffer, "SLP: %3.2f,\tSFP: %3.2f,\tSRP: %3.2f\n\r", (sensor_left_distance_in_percentage*100), (sensor_front_distance_in_percentage*100), (sensor_right_distance_in_percentage*100));    
//                 // sprintf(buffer, "%2.2f \t%2.2f \t%2.2f \t%2.3f \t%2.2f \t%2.2f \t%i\n\r", (sensor_left_distance_in_percentage), (sensor_front_distance_in_percentage), (sensor_right_distance_in_percentage), distance, vel_desired.vel_left, vel_desired.vel_right, currenLateralControlMode);
//                 sprintf(buffer, "%2.2f;%2.2f;%2.2f;%2.3f;%2.2f;%2.2f;%i;\n\r", (sensor_left_distance_in_percentage), (sensor_front_distance_in_percentage), (sensor_right_distance_in_percentage), distance, vel_desired.vel_left, vel_desired.vel_right, currenLateralControlMode);
                
//                 // // Print the result of getLateralControlMode()
//                 // enum lateralControlMode LCM = getLateralControlMode();
//                 // const char* lateralControlModeString = getLateralControlModeName(LCM);
//                 // sprintf(buffer, "%s\n\r", lateralControlModeString);

//                 putsUART1(buffer);
//                 myCount=0;
//             }

//             enum lateralControlMode currenLateralControlMode = getLateralControlMode();

//             // if (currenLateralControlMode != oldLateralControlMode) {
//             //     char buffer[50];
//             //     const char* lateralControlModeString = getLateralControlModeName(currenLateralControlMode);
//             //     sprintf(buffer, "%s\n\r", lateralControlModeString);

//             //     putsUART1(buffer);
//             //     oldLateralControlMode = currenLateralControlMode;
//             // }

//             myCount++;

//             if (getTotalDrivenDistanceInMeters(false) >= initial_distance_to_goal){
//                 waitBeforeDriveCounter = 0;
//             }
//         }

//     waitBeforeDriveCounter++;

//     } else {
//         set_DC_and_motor_state_left(0.0, "forward_slow_decay");
//         set_DC_and_motor_state_right(0.0, "forward_slow_decay");
//     }
// }



/* Control Distance Driven and detect and USE the correct Wall Following Control Mode
*   But testing right wheel control while turning and driving straight without the shouldTurn variable as a controller function parameter: instead switching the vel sign for the right wheel when turning
*/
// void __attribute__((__interrupt__, auto_psv)) _T1Interrupt(void)
// {
//     IFS0bits.T1IF = 0;           // reset Timer 1 interrupt flag 
//     static int myCount=0;

//     if (mazi_running == 1){

//         static int waitBeforeDriveCounter=0;
//         static enum lateralControlMode oldLateralControlMode;

//         if (waitBeforeDriveCounter == 0) {
//             // UART Buffer
//             char buffer[30];   
//             sprintf(buffer, "SLP;SFP;SRP;m;vL;vR;cMode\n\r");    
//             putsUART1(buffer);
//         }


//         if (waitBeforeDriveCounter >= 1000){
//             LED1 = ~LED1;
//             // waitBeforeDriveCounter=0;
        
//             float vel_cruise = - 0.3;
//             float initial_distance_to_goal = 5 * 0.18; // in meters

//             // float distance_to_goal = (float)SENSOR_FRONT / 4096.0f * 100.0; // 12 bit ADC therfore 2^12 = 4096
//             float distance_to_goal = getDistanceToGoalInMeters(initial_distance_to_goal);
//             // float distance_driven = getDistanceDrivenInMeters();

//             float vel_base = p_goal_distance_controller(distance_to_goal, vel_cruise);

//             // // Convert the value of the potentiometer to a percentage for the PWM
//             // float sensor_left_distance_in_percentage = distanceSensorInPercentLeft(); 
//             // float sensor_front_distance_in_percentage = distanceSensorInPercentFront(); 
//             // float sensor_right_distance_in_percentage = distanceSensorInPercentRight(); 

//             // // Switch Case to execute the correct Laterall Control Mode
//             // struct Velocities vel_desired;
//             // switch (getLateralControlMode())
//             // {
//             //     case ONE_WALL_FOLLOWING_RIGHT:
//             //         vel_desired = p_one_wall_following_right(sensor_right_distance_in_percentage, vel_base);
//             //         break;
//             //     case ONE_WALL_FOLLOWING_LEFT:
//             //         vel_desired = p_one_wall_following_left(sensor_left_distance_in_percentage, vel_base);
//             //         break;
//             //     case TWO_WALL_CENTERING:
//             //         vel_desired = p_wall_centering_controller( sensor_left_distance_in_percentage, sensor_right_distance_in_percentage, vel_base);
//             //         break;
//             //     case ZERO_WALL_DRIVING:
//             //         vel_desired = (struct Velocities){vel_base, vel_base}; // Assign left and right velocities, (struct Velocities){vel_base, vel_base} creates a temporary struct with the specified values and assigns it to vel_desired
//             //         // Alternative to the compact professional code line above
//             //         // vel_desired.left_velocity = vel_base; // Assign the left wheel velocity
//             //         // vel_desired.right_velocity = vel_base; // Assign the right wheel velocity
//             //         break;
//             //     default:
//             //         break;
//             // }

//             // Current Motor Velocities from Encoders
//             float motor_vel_left = getVelocityInRoundsPerSecond_Left();
//             float motor_vel_right = getVelocityInRoundsPerSecond_Right();

//             // bool shouldTurn = false;
//             // float dc_left = pi_vel_controller_left(vel_desired.vel_left , motor_vel_left);
//             // float dc_right = pi_vel_controller_right(vel_desired.vel_right, motor_vel_right, shouldTurn);


//             bool shouldTurn = true;
//             float dc_left = pi_vel_controller_left(vel_base, motor_vel_left);
//             // The vel_desired.vel_right for the right Motor has to be negated when the robot should turn as the vel sign is derived from the left wheel "coordinates"
//             if (shouldTurn) {
//                 float dc_right = pi_vel_controller_right(-vel_base, motor_vel_right);
//             } else {
//                 float dc_right = pi_vel_controller_right(vel_base, motor_vel_right);
//             }
            

//             // float dc_left = pi_vel_controller_left(vel_base, motor_vel_left);
//             // float dc_right = pi_vel_controller_right(vel_base, motor_vel_right, shouldTurn);

//             // if (myCount >= 100){
//             //     LED1 = ~LED1;
//             //     float distance = getTotalDrivenDistanceInMeters();
//             //     enum lateralControlMode currenLateralControlMode = getLateralControlMode();

//             //     // UART Buffer
//             //     char buffer[40];
//             //     sprintf(buffer, "%2.2f;%2.2f;%2.2f;%2.3f;%2.2f;%2.2f;%i;\n\r", (sensor_left_distance_in_percentage), (sensor_front_distance_in_percentage), (sensor_right_distance_in_percentage), distance, vel_desired.vel_left, vel_desired.vel_right, currenLateralControlMode);

//             //     putsUART1(buffer);
//             //     myCount=0;
//             // }

//             enum lateralControlMode currenLateralControlMode = getLateralControlMode();

//             myCount++;

//             if (getTotalDrivenDistanceInMeters() >= initial_distance_to_goal){
//                 waitBeforeDriveCounter = 0;
//             }
//         }

//     waitBeforeDriveCounter++;

//     } else {
//         set_DC_and_motor_state_left(0.0, "forward_slow_decay");
//         set_DC_and_motor_state_right(0.0, "forward_slow_decay");
//     }
// }



/* Control Distance Driven and detect and USE the correct Wall Following Control Mode 
*  Using the new abstractControl functions
*/
// void __attribute__((__interrupt__, auto_psv)) _T1Interrupt(void)
// {
//     IFS0bits.T1IF = 0;           // reset Timer 1 interrupt flag 
//     static int myCount=0;

//     if (mazi_running == 1){

//         static int waitBeforeDriveCounter=0;

//         if (waitBeforeDriveCounter >= 1000){
//             LED1 = ~LED1;
        
//             float vel_cruise = 0.3;
//             int cells_to_drive = 5;
//             driveStraightForNCells(5, vel_cruise);

//             if ( fabs(getTotalDrivenDistanceInMeters() - cells_to_drive * MAZE_CELL_LENGTH) <= GOAL_REACHED_THRESHOLD_DISTANCE ){
//                 waitBeforeDriveCounter = 0;
//                 LED4 = LEDON;
//             }
//         }

//     waitBeforeDriveCounter++;

//     } else {
//         set_DC_and_motor_state_left(0.0, "forward_slow_decay");
//         set_DC_and_motor_state_right(0.0, "forward_slow_decay");
//     }
// }

/* Control Distance Turned and detect and USE the correct Wall Following Control Mode 
*  Using the new abstractControl functions
*/
// void __attribute__((__interrupt__, auto_psv)) _T1Interrupt(void)
// {
//     IFS0bits.T1IF = 0;           // reset Timer 1 interrupt flag 
//     static int myCount=0;

//     if (mazi_running == 1){

//         static int waitBeforeDriveCounter=0;

//         if (waitBeforeDriveCounter >= 1000){
//             LED1 = ~LED1;
        
//             float vel_turn_cruise = 0.3;
//             float degrees_to_turn = 90;
//             turn90DegreesRight(vel_turn_cruise);

//             if ( fabs(getTotalDrivenAngleInDegrees() - degrees_to_turn) <= GOAL_REACHED_THRESHOLD_ANGLE ){
//                 waitBeforeDriveCounter = 0;
//                 LED4 = LEDON;
//             }
//         }

//     waitBeforeDriveCounter++;

//     } else {
//         set_DC_and_motor_state_left(0.0, "forward_slow_decay");
//         set_DC_and_motor_state_right(0.0, "forward_slow_decay");
//     }
// }



/* 
* Test Drive: moving straight - and moving straight
* without state machine and improvised goal reached cascade
*/
// void __attribute__((__interrupt__, auto_psv)) _T1Interrupt(void)
// {
//     IFS0bits.T1IF = 0;           // reset Timer 1 interrupt flag 
//     static int myCount=0;

//     static bool done = false;

//     if (mazi_running == 1 && !done){

//         static int waitBeforeDriveCounter=0;
//         // Improvised local state to keep track of which driving instruction to execute
//         static int driving_instruction_step = 0;

//         if (waitBeforeDriveCounter >= 1000){
//             LED1 = ~LED1;

//             // UART Buffer
//             char buffer[5];
//             sprintf(buffer, "%i\n\r", driving_instruction_step);
//             myCount++;

//             // if (myCount >= 100){
//             //     putsUART1(buffer);
//             //     myCount=0;
//             // }

//             switch (driving_instruction_step)
//             {
//             case 0: {
//                 float vel_cruise = 0.3;
//                 int cells_to_drive = 1;
//                 driveStraightForNCells(cells_to_drive, vel_cruise);
//                 if (isDistanceGoalReached(getTotalDrivenDistanceInMeters(false), cells_to_drive * MAZE_CELL_LENGTH)) {
//                     driving_instruction_step++;
//                     bool reset_static_variables = true;
//                     getTotalDrivenDistanceInMeters(reset_static_variables);
//                     getDistanceToGoalInMeters(0.0, reset_static_variables);
//                 }
//                 // sprintf(buffer, "%i %0.2f\n\r", driving_instruction_step, getTotalDrivenDistanceInMeters(false));
//                 putsUART1(buffer);
//                 break;
//             }
//             case 1: {
//                 float vel_cruise = 0.3;
//                 int cells_to_drive = 1;
//                 driveStraightForNCells(cells_to_drive, vel_cruise);
//                 if (isDistanceGoalReached(getTotalDrivenDistanceInMeters(false), cells_to_drive * MAZE_CELL_LENGTH)) {
//                     driving_instruction_step++;
//                     bool reset_static_variables = true;
//                     getTotalDrivenDistanceInMeters(reset_static_variables);
//                     getDistanceToGoalInMeters(0.0, reset_static_variables);
//                 }
//                 putsUART1(buffer);
//                 break;
//             }
//             case 2:
//                 // All instructions executed
//                 waitBeforeDriveCounter = 0;
//                 LED4 = LEDON;
//                 putsUART1(buffer);
//                 done = true;
//                 break;
            
//             default:
//                 break;
//             }
//         }

//     waitBeforeDriveCounter++;

//     } else {
//         set_DC_and_motor_state_left(0.0, "forward_slow_decay");
//         set_DC_and_motor_state_right(0.0, "forward_slow_decay");
//     }
// }


/* 
* Test Drive: moving straight - turning - and moving straight
* without state machine and improvised goal reached cascade
*/
// void __attribute__((__interrupt__, auto_psv)) _T1Interrupt(void)
// {
//     IFS0bits.T1IF = 0;           // reset Timer 1 interrupt flag 
//     static int myCount=0;

//     static bool done = false;

//     if (mazi_running == 1 && !done){

//         static int waitBeforeDriveCounter=0;
//         // Improvised local state to keep track of which driving instruction to execute
//         static int driving_instruction_step = 0;

//         if (waitBeforeDriveCounter >= 1000){
//             LED1 = ~LED1;

//             // UART Buffer
//             char buffer[5];
//             sprintf(buffer, "%i\n\r", driving_instruction_step);
//             myCount++;

//             // if (myCount >= 100){
//             //     putsUART1(buffer);
//             //     myCount=0;
//             // }

//             switch (driving_instruction_step)
//             {
//             case 0: {
//                 float vel_cruise = 0.3;
//                 int cells_to_drive = 1;
//                 driveStraightForNCells(cells_to_drive, vel_cruise);
//                 if (isDistanceGoalReached(getTotalDrivenDistanceInMeters(false), cells_to_drive * MAZE_CELL_LENGTH)) {
//                     driving_instruction_step++;
//                     bool reset_static_variables = true;
//                     getTotalDrivenDistanceInMeters(reset_static_variables);
//                     getDistanceToGoalInMeters(0.0, reset_static_variables);
//                 }
//                 // sprintf(buffer, "%i %0.2f\n\r", driving_instruction_step, getTotalDrivenDistanceInMeters(false));
//                 putsUART1(buffer);
//                 break;
//             }
//             case 1: {
//                 float vel_turn_cruise = 0.3;
//                 float degrees_to_turn = 90;
//                 turn90DegreesRight(vel_turn_cruise);
//                 if (isAngleGoalReached(getTotalDrivenAngleInDegrees(false), degrees_to_turn)) {
//                     driving_instruction_step++;
//                     bool reset_static_variables = true;
//                     getTotalDrivenAngleInDegrees(reset_static_variables);
//                     getAngleToGoalInDegrees(0.0, reset_static_variables);
//                 }
//                 putsUART1(buffer);
//                 break;
//             }
//             case 2: {
//                 float vel_cruise = 0.3;
//                 int cells_to_drive = 1;
//                 driveStraightForNCells(cells_to_drive, vel_cruise);
//                 if (isDistanceGoalReached(getTotalDrivenDistanceInMeters(false), cells_to_drive * MAZE_CELL_LENGTH)) {
//                     driving_instruction_step++;
//                     bool reset_static_variables = true;
//                     getTotalDrivenDistanceInMeters(reset_static_variables);
//                     getDistanceToGoalInMeters(0.0, reset_static_variables);
//                 }
//                 putsUART1(buffer);
//                 break;
//             }
//             case 3:
//                 // All instructions executed
//                 waitBeforeDriveCounter = 0;
//                 LED4 = LEDON;
//                 putsUART1(buffer);
//                 done = true;
//                 break;
            
//             default:
//                 break;
//             }
//         }

//     waitBeforeDriveCounter++;

//     } else {
//         set_DC_and_motor_state_left(0.0, "forward_slow_decay");
//         set_DC_and_motor_state_right(0.0, "forward_slow_decay");
//     }
// }


/**
* Test if the tuning and driving straight functions do all work as intended (here without using the reset functionality)
* Test scenarios:
*   - drive straight for 1 cell [DONE]
*   - turn 90 degrees right [DONE]
*   - turn 90 degrees left [DONE]
*   - turn 180 degrees right [DONE]
*   - turn 180 degrees left [DONE]
*/
// void __attribute__((__interrupt__, auto_psv)) _T1Interrupt(void)
// {
//     IFS0bits.T1IF = 0;           // reset Timer 1 interrupt flag 
//     static int myCount=0;

//     static bool done = false;

//     if (mazi_running == 1 && !done){

//         static int waitBeforeDriveCounter=0;
//         // Improvised local state to keep track of which driving instruction to execute

//         if (waitBeforeDriveCounter >= 1000){
//             LED1 = ~LED1;

//             // UART Buffer
//             // char buffer[5];
//             // sprintf(buffer, "%i\n\r", driving_instruction_step);
//             // putsUART1(buffer);
//             myCount++;

//             // if (myCount >= 100){
//             //     putsUART1(buffer);
//             //     myCount=0;
//             // }

//             // Drive straight for 1 cell [DONE]
//             // float vel_cruise = 0.3;
//             // int cells_to_drive = 1;
//             // driveStraightForNCells(cells_to_drive, vel_cruise);
            
//             // Turn 90 degrees right [DONE]
//             // float vel_turn_cruise = 0.3;
//             // float degrees_to_turn = 90;
//             // turn90DegreesRight(vel_turn_cruise);

//             // Turn 90 degrees left [DONE]
//             // float vel_turn_cruise = 0.3;
//             // float degrees_to_turn = 90;
//             // turn90DegreesLeft(vel_turn_cruise);

//             // Turn 180 degrees right [DONE]
//             // float vel_turn_cruise = 0.3;
//             // float degrees_to_turn = 180;
//             // turn180DegreesRight(vel_turn_cruise);

//             // Turn 180 degrees left
//             // float vel_turn_cruise = 0.3;
//             // float degrees_to_turn = 180;
//             // turn180DegreesLeft(vel_turn_cruise);

//             // Drive straight backwards for 1 cell
//             float vel_cruise = 0.3;
//             int cells_to_drive = -1;
//             float distance_to_goal;

//             distance_to_goal = driveStraightForNCells(cells_to_drive, vel_cruise, false);

//             // UART Buffer
//             char buffer[5];
//             sprintf(buffer, "%1.2F\n\r", distance_to_goal);
//             putsUART1(buffer);

//         }

//     waitBeforeDriveCounter++;

//     } else {
//         set_DC_and_motor_state_left(0.0, "forward_slow_decay");
//         set_DC_and_motor_state_right(0.0, "forward_slow_decay");
//     }
// }


/**
* Test if the tuning and driving straight functions do all work as intended when using the reset functionality
* Test scenarios:
*   - Drive straight for 1 cell -&- Drive straight for 1 cell [DONE]
*   - Drive straight for 1 cell -&- Turn 90 degrees right [DONE]
*/
// void __attribute__((__interrupt__, auto_psv)) _T1Interrupt(void)
// {
//     IFS0bits.T1IF = 0;           // reset Timer 1 interrupt flag 
//     static int myCount=0;

//     static bool done = false;

//     if (mazi_running == 1 && !done){

//         static int waitBeforeDriveCounter=0;
//         // Improvised local state to keep track of which driving instruction to execute
//         static int driving_instruction_step = 0;

//         if (waitBeforeDriveCounter >= 1000){
//             static bool start_new_motion_primitive = true;
//             LED1 = ~LED1;

//             // UART Buffer
//             char buffer[5];
//             sprintf(buffer, "%i\n\r", driving_instruction_step);
//             myCount++;

//             // if (myCount >= 100){
//             //     putsUART1(buffer);
//             //     myCount=0;
//             // }

//             switch (driving_instruction_step)
//             {
//             case 0: {
//                 float vel_cruise = 0.3;
//                 int cells_to_drive = 1;
//                 float distance_to_goal;

//                 putsUART1(buffer);
                
//                 distance_to_goal = driveStraightForNCells(cells_to_drive, vel_cruise, start_new_motion_primitive);

//                 // This could also be handled in the driveStraightForNMeters function itself by checking if the goal is reached
//                 // However this might be less flexible e.g. if the goal is not reached before the motion primitive is changed (interrupted), 
//                 // than the flag will not be reseted automatically and the next motion primitive might be executed wrongly
//                 if (start_new_motion_primitive) {
//                     start_new_motion_primitive = false;
//                 }
                
//                 if (distance_to_goal == 0) {
//                     driving_instruction_step++;
//                     start_new_motion_primitive = true;
//                 }

//                 break;
//             }
//             case 1: {
//                 float vel_turn_cruise = 0.3;
//                 float degrees_to_turn = 90;
//                 float angle_to_goal;

//                 putsUART1(buffer);

//                 angle_to_goal = turn90DegreesLeft(vel_turn_cruise, start_new_motion_primitive);

//                 // This could also be handled in the turnForNDegrees function itself by checking if the goal is reached
//                 // However this might be less flexible e.g. if the goal is not reached before the motion primitive is changed (interrupted),
//                 // than the flag will not be reseted automatically and the next motion primitive might be executed wrongly
//                 if (start_new_motion_primitive) {
//                     start_new_motion_primitive = false;
//                 }

//                 if (angle_to_goal == 0) {
//                     driving_instruction_step++;
//                     start_new_motion_primitive = true;
//                 }

//                 break;
//             }
//             case 2:
//                 // All instructions executed
//                 waitBeforeDriveCounter = 0;
//                 LED4 = LEDON;
//                 putsUART1(buffer);
//                 done = true;
//                 break;
            
//             default:
//                 break;
//             }
//         }

//     waitBeforeDriveCounter++;

//     } else {
//         set_DC_and_motor_state_left(0.0, "forward_slow_decay");
//         set_DC_and_motor_state_right(0.0, "forward_slow_decay");
//     }
// }


/**
* Test if the tuning and driving straight functions do all work as intended when using the reset functionality
* Mega long test scenario with all motion primitives
* Test scenaio: [SUCCESS]
*   - 0: straight for 1 cell
*   - 1: turn 90 degrees left
*   - 2: straight for 1 cell
*   - 3: turn 90 degrees right
*   - 4: straight for 1 cell
*   - 5: turn 90 degrees left
*   - 6: turn 180 degrees left
*   - 7: turn 360 degrees right
*   - 8: straight for 2 cells
*   - 9: straight backwards for 1 cell
*/
// void __attribute__((__interrupt__, auto_psv)) _T1Interrupt(void)
// {
//     IFS0bits.T1IF = 0;           // reset Timer 1 interrupt flag 
//     static int myCount=0;
//     myCount++;

//     static bool done = false;

//     if (mazi_running == 1 && !done){

//         static int waitBeforeDriveCounter=0;
//         static bool start_driving = false;
//         // Improvised local state to keep track of which driving instruction to execute
//         static int driving_instruction_step = 0;

//         // if (myCount >= 100){
//         //     char buffer[3];
//         //     sprintf(buffer, "R\n\r");
//         //     putsUART1(buffer);
//         //     myCount=0;
//         // }

//         // Has to be done like this as max int value is 32.767 and the waitBeforeDriveCounter is an int
//         // Therefore after 32.767 the waitBeforeDriveCounter will overflow and start from 0 again, At least this is what I think
//         // But just setting a flag once, I don't have to worry about the overflow
//         if (waitBeforeDriveCounter >= 1000){
//             start_driving = true;
//         }

//         if (start_driving){
//             static bool start_new_motion_primitive = true;
//             LED1 = ~LED1;

//             // UART Buffer
//             char buffer[5];
//             sprintf(buffer, "%i\n\r", driving_instruction_step);
//             // myCount++;

//             // if (myCount >= 100){
//             //     char buffer[4];
//             //     sprintf(buffer, "RD\n\r");
//             //     putsUART1(buffer);
//             //     // myCount=0;
//             // }

//             switch (driving_instruction_step)
//             {
//             case 0: {
//                 // Drive straight for 1 cell
//                 float vel_cruise = 0.3;
//                 int cells_to_drive = 1;
//                 float distance_to_goal;
                
//                 distance_to_goal = driveStraightForNCells(cells_to_drive, vel_cruise, start_new_motion_primitive);

//                 // This could also be handled in the driveStraightForNMeters function itself by checking if the goal is reached
//                 // However this might be less flexible e.g. if the goal is not reached before the motion primitive is changed (interrupted), 
//                 // than the flag will not be reseted automatically and the next motion primitive might be executed wrongly
//                 if (start_new_motion_primitive) {
//                     putsUART1(buffer);
//                     start_new_motion_primitive = false;
//                 }
                
//                 if (distance_to_goal == 0) {
//                     driving_instruction_step++;
//                     start_new_motion_primitive = true;
//                 }

//                 break;
//             }
//             case 1: {
//                 // Turn 90 degrees left
//                 float vel_turn_cruise = 0.3;
//                 // float degrees_to_turn = 90;
//                 float angle_to_goal;

//                 angle_to_goal = turn90DegreesLeft(vel_turn_cruise, start_new_motion_primitive);

//                 // This could also be handled in the turnForNDegrees function itself by checking if the goal is reached
//                 // However this might be less flexible e.g. if the goal is not reached before the motion primitive is changed (interrupted),
//                 // than the flag will not be reseted automatically and the next motion primitive might be executed wrongly
//                 if (start_new_motion_primitive) {
//                     putsUART1(buffer);
//                     start_new_motion_primitive = false;
//                 }

//                 if (angle_to_goal == 0) {
//                     driving_instruction_step++;
//                     start_new_motion_primitive = true;
//                 }

//                 break;
//             }
//             case 2: {
//                 // Drive straight for 1 cell
//                 float vel_cruise = 0.3;
//                 int cells_to_drive = 1;
//                 float distance_to_goal;

//                 distance_to_goal = driveStraightForNCells(cells_to_drive, vel_cruise, start_new_motion_primitive);

//                 // This could also be handled in the driveStraightForNMeters function itself by checking if the goal is reached
//                 // However this might be less flexible e.g. if the goal is not reached before the motion primitive is changed (interrupted), 
//                 // than the flag will not be reseted automatically and the next motion primitive might be executed wrongly
//                 if (start_new_motion_primitive) {
//                     putsUART1(buffer);
//                     start_new_motion_primitive = false;
//                 }
                
//                 if (distance_to_goal == 0) {
//                     driving_instruction_step++;
//                     start_new_motion_primitive = true;
//                 }

//                 break;
//             }
//             case 3: {
//                 // Turn 90 degrees right
//                 float vel_turn_cruise = 0.3;
//                 // float degrees_to_turn = 90;
//                 float angle_to_goal;

//                 angle_to_goal = turn90DegreesRight(vel_turn_cruise, start_new_motion_primitive);

//                 // This could also be handled in the turnForNDegrees function itself by checking if the goal is reached
//                 // However this might be less flexible e.g. if the goal is not reached before the motion primitive is changed (interrupted),
//                 // than the flag will not be reseted automatically and the next motion primitive might be executed wrongly
//                 if (start_new_motion_primitive) {
//                     putsUART1(buffer);
//                     start_new_motion_primitive = false;
//                 }

//                 if (angle_to_goal == 0) {
//                     driving_instruction_step++;
//                     start_new_motion_primitive = true;
//                 }

//                 break;
//             }
//             case 4: {
//                 // Drive straight for 1 cell
//                 float vel_cruise = 0.3;
//                 int cells_to_drive = 1;
//                 float distance_to_goal;

//                 distance_to_goal = driveStraightForNCells(cells_to_drive, vel_cruise, start_new_motion_primitive);

//                 // This could also be handled in the driveStraightForNMeters function itself by checking if the goal is reached
//                 // However this might be less flexible e.g. if the goal is not reached before the motion primitive is changed (interrupted), 
//                 // than the flag will not be reseted automatically and the next motion primitive might be executed wrongly
//                 if (start_new_motion_primitive) {
//                     putsUART1(buffer);
//                     start_new_motion_primitive = false;
//                 }
                
//                 if (distance_to_goal == 0) {
//                     driving_instruction_step++;
//                     start_new_motion_primitive = true;
//                 }

//                 break;
//             }
//             case 5: {
//                 // Turn 90 degrees left
//                 float vel_turn_cruise = 0.3;
//                 // float degrees_to_turn = 90;
//                 float angle_to_goal;

//                 angle_to_goal = turn90DegreesLeft(vel_turn_cruise, start_new_motion_primitive);

//                 // This could also be handled in the turnForNDegrees function itself by checking if the goal is reached
//                 // However this might be less flexible e.g. if the goal is not reached before the motion primitive is changed (interrupted),
//                 // than the flag will not be reseted automatically and the next motion primitive might be executed wrongly
//                 if (start_new_motion_primitive) {
//                     putsUART1(buffer);
//                     start_new_motion_primitive = false;
//                 }

//                 if (angle_to_goal == 0) {
//                     driving_instruction_step++;
//                     start_new_motion_primitive = true;
//                 }

//                 break;
//             }
//             case 6: {
//                 // Turn 180 degrees left
//                 float vel_turn_cruise = 0.3;
//                 // float degrees_to_turn = 90;
//                 float angle_to_goal;

//                 angle_to_goal = turn180DegreesLeft(vel_turn_cruise, start_new_motion_primitive);

//                 // This could also be handled in the turnForNDegrees function itself by checking if the goal is reached
//                 // However this might be less flexible e.g. if the goal is not reached before the motion primitive is changed (interrupted),
//                 // than the flag will not be reseted automatically and the next motion primitive might be executed wrongly
//                 if (start_new_motion_primitive) {
//                     putsUART1(buffer);
//                     start_new_motion_primitive = false;
//                 }

//                 if (angle_to_goal == 0) {
//                     driving_instruction_step++;
//                     start_new_motion_primitive = true;
//                 }

//                 break;
//             }
//             case 7: {
//                 // Turn 360 degrees right
//                 float vel_turn_cruise = 0.3;
//                 float degrees_to_turn = 360;
//                 float angle_to_goal;

//                 angle_to_goal = turnForNDegrees(degrees_to_turn, vel_turn_cruise, start_new_motion_primitive);

//                 // This could also be handled in the turnForNDegrees function itself by checking if the goal is reached
//                 // However this might be less flexible e.g. if the goal is not reached before the motion primitive is changed (interrupted),
//                 // than the flag will not be reseted automatically and the next motion primitive might be executed wrongly
//                 if (start_new_motion_primitive) {
//                     putsUART1(buffer);
//                     start_new_motion_primitive = false;
//                 }

//                 if (angle_to_goal == 0) {
//                     driving_instruction_step++;
//                     start_new_motion_primitive = true;
//                 }

//                 break;
//             }
//             case 8: {
//                 // Drive straight for 2 cells
//                 float vel_cruise = 0.3;
//                 int cells_to_drive = 2;
//                 float distance_to_goal;

//                 distance_to_goal = driveStraightForNCells(cells_to_drive, vel_cruise, start_new_motion_primitive);

//                 // This could also be handled in the driveStraightForNMeters function itself by checking if the goal is reached
//                 // However this might be less flexible e.g. if the goal is not reached before the motion primitive is changed (interrupted), 
//                 // than the flag will not be reseted automatically and the next motion primitive might be executed wrongly
//                 if (start_new_motion_primitive) {
//                     putsUART1(buffer);
//                     start_new_motion_primitive = false;
//                 }
                
//                 if (distance_to_goal == 0) {
//                     char buffer[8];
//                     sprintf(buffer, "Goal\n\r");
//                     putsUART1(buffer);
//                     driving_instruction_step++;
//                     start_new_motion_primitive = true;
//                 }


//                 if (myCount >= 100){
//                     char buffer[8];
//                     sprintf(buffer, "%1.2f\n\r", distance_to_goal);
//                     putsUART1(buffer);
//                     myCount=0;
//                 }

//                 break;
//             }
//             case 9: {
//                 // Drive straight backwards for 1 cell
//                 float vel_cruise = 0.3;
//                 int cells_to_drive = -1;
//                 float distance_to_goal;

//                 distance_to_goal = driveStraightForNCells(cells_to_drive, vel_cruise, start_new_motion_primitive);

//                 // This could also be handled in the driveStraightForNMeters function itself by checking if the goal is reached
//                 // However this might be less flexible e.g. if the goal is not reached before the motion primitive is changed (interrupted), 
//                 // than the flag will not be reseted automatically and the next motion primitive might be executed wrongly
//                 if (start_new_motion_primitive) {
//                     putsUART1(buffer);
//                     start_new_motion_primitive = false;
//                 }
                
//                 if (distance_to_goal == 0) {
//                     char buffer[8];
//                     sprintf(buffer, "Goal\n\r");
//                     putsUART1(buffer);
//                     driving_instruction_step++;
//                     start_new_motion_primitive = true;
//                 }

//                 if (myCount >= 100){
//                     char buffer[8];
//                     sprintf(buffer, "%1.2f\n\r", distance_to_goal);
//                     putsUART1(buffer);
//                     myCount=0;
//                 }

//                 break;
//             }

//             case 10:
//                 // All instructions executed
//                 waitBeforeDriveCounter = 0;
//                 LED4 = LEDON;
//                 putsUART1(buffer);
//                 done = true;
//                 break;
            
//             default:
//                 break;
//             }
//         }

//     waitBeforeDriveCounter++;

//     } else {
//         // char buffer[3];
//         // sprintf(buffer, "S\n\r");
//         // putsUART1(buffer);
//         set_DC_and_motor_state_left(0.0, "forward_slow_decay");
//         set_DC_and_motor_state_right(0.0, "forward_slow_decay");
//     }
// }


/**
* Test if driving straight and backwars afterwards does work as intended
*/
// void __attribute__((__interrupt__, auto_psv)) _T1Interrupt(void)
// {
//     IFS0bits.T1IF = 0;           // reset Timer 1 interrupt flag 
//     static int myCount=0;

//     static bool done = false;

//     if (mazi_running == 1 && !done){

//         static int waitBeforeDriveCounter=0;
//         // Improvised local state to keep track of which driving instruction to execute
//         static int driving_instruction_step = 0;

//         if (waitBeforeDriveCounter >= 1000){
//             static bool start_new_motion_primitive = true;
//             LED1 = ~LED1;

//             // UART Buffer
//             char buffer[5];
//             sprintf(buffer, "%i\n\r", driving_instruction_step);
//             myCount++;

//             // if (myCount >= 100){
//             //     putsUART1(buffer);
//             //     myCount=0;
//             // }

//             switch (driving_instruction_step)
//             {
//             case 0: {
//                 // Drive straight for 2 cells
//                 float vel_cruise = 0.3;
//                 int cells_to_drive = 2;
//                 float distance_to_goal;
                
//                 distance_to_goal = driveStraightForNCells(cells_to_drive, vel_cruise, start_new_motion_primitive);

//                 // This could also be handled in the driveStraightForNMeters function itself by checking if the goal is reached
//                 // However this might be less flexible e.g. if the goal is not reached before the motion primitive is changed (interrupted), 
//                 // than the flag will not be reseted automatically and the next motion primitive might be executed wrongly
//                 if (start_new_motion_primitive) {
//                     putsUART1(buffer);
//                     start_new_motion_primitive = false;
//                 }
                
//                 if (distance_to_goal == 0) {
//                     char buffer[8];
//                     sprintf(buffer, "Goal\n\r");
//                     putsUART1(buffer);
//                     driving_instruction_step++;
//                     start_new_motion_primitive = true;
//                 }

//                 if (myCount >= 100){
//                     char buffer[8];
//                     sprintf(buffer, "%1.2f\n\r", distance_to_goal);
//                     putsUART1(buffer);
//                     myCount=0;
//                 }

//                 break;
//             }
//             case 1: {
//                 // Drive straight for 1 cell
//                 float vel_cruise = 0.3;
//                 int cells_to_drive = -2;
//                 float distance_to_goal;
                
//                 distance_to_goal = driveStraightForNCells(cells_to_drive, vel_cruise, start_new_motion_primitive);

//                 // This could also be handled in the driveStraightForNMeters function itself by checking if the goal is reached
//                 // However this might be less flexible e.g. if the goal is not reached before the motion primitive is changed (interrupted), 
//                 // than the flag will not be reseted automatically and the next motion primitive might be executed wrongly
//                 if (start_new_motion_primitive) {
//                     putsUART1(buffer);
//                     start_new_motion_primitive = false;
//                 }
                
//                 if (distance_to_goal == 0) {
//                     char buffer[8];
//                     sprintf(buffer, "Goal\n\r");
//                     putsUART1(buffer);
//                     driving_instruction_step++;
//                     start_new_motion_primitive = true;
//                 }

//                 if (myCount >= 100){
//                     char buffer[8];
//                     sprintf(buffer, "%1.2f\n\r", distance_to_goal);
//                     putsUART1(buffer);
//                     myCount=0;
//                 }

//                 break;
//             }
//             case 2:
//                 // All instructions executed
//                 waitBeforeDriveCounter = 0;
//                 LED4 = LEDON;
//                 sprintf(buffer, "%i\n\r", driving_instruction_step);
//                 putsUART1(buffer);
//                 done = true;
//                 break;
            
//             default:
//                 break;
//             }
//         }

//     waitBeforeDriveCounter++;

//     } else {
//         set_DC_and_motor_state_left(0.0, "forward_slow_decay");
//         set_DC_and_motor_state_right(0.0, "forward_slow_decay");
//     }
// }




/*
* Simple Wall following Algorithm
* Does not work yet, will restructure the code first
*/
// void __attribute__((__interrupt__, auto_psv)) _T1Interrupt(void)
// {
//     IFS0bits.T1IF = 0;           // reset Timer 1 interrupt flag 
//     static int myCount=0;
//     myCount++;

//     static bool done = false;

//     if (mazi_running == 1 && !done){

//         static int waitBeforeDriveCounter=0;
//         static bool start_driving = false;
//         // Improvised local state to keep track of which driving instruction to execute
//         static int driving_instruction_step = 0;

//         // Has to be done like this as max int value is 32.767 and the waitBeforeDriveCounter is an int
//         // Therefore after 32.767 the waitBeforeDriveCounter will overflow and start from 0 again, At least this is what I think
//         // But just setting a flag once, I don't have to worry about the overflow
//         if (waitBeforeDriveCounter >= 1000){
//             start_driving = true;
//         }

//         if (start_driving){

//             static bool start_new_motion_primitive = true;
//             LED1 = ~LED1;

//             float vel_cruise = 0.3;

//             /*
//             * Wall Following Algorithm
//             * Modes:
//             * 0 = driveStraightForever [DONE]
//             * 1 = turn left
//             * 2 = turn right
//             * 3 = turn around [DONE]
//             */
//             static int robot_motion_state = 0;
//             static int old_robot_motion_state = 0;
//             static bool changed_motion_state = false;
//             changed_motion_state = false;

//             if (robot_motion_state != old_robot_motion_state) {
//                 old_robot_motion_state = robot_motion_state;
//                 changed_motion_state = true;
//             }

//             // Set the current motion state / primitive of the robot, can also be done in the switch case 0 below
//             // This is only done while the robot is driving straight in the driveStraightForever Mode
//             if (robot_motion_state == 0) {
//                 if (isWallLeft() && !isWallFront()) {
//                     robot_motion_state = 0;
//                 } else if (!isWallLeft()) {
//                     robot_motion_state = 1;
//                 } else if (isWallLeft() && isWallFront() && !isWallRight()){
//                     robot_motion_state = 2;
//                 } else if (isWallLeft() && isWallFront() && isWallRight()){
//                     robot_motion_state = 3;
//                     char buffer[4];
//                     // sprintf(buffer, "S%i L%2f F%2f R%2f\n\r", robot_motion_state, distanceSensorInPercentLeft(), distanceSensorInPercentFront(), distanceSensorInPercentRight());
//                     sprintf(buffer, "T\n\r");
//                     putsUART1(buffer);
//                 }
//             }

//             if (myCount >= 500){
//                 char buffer[30];
//                 // sprintf(buffer, "S%i L%2f F%2f R%2f\n\r", robot_motion_state, distanceSensorInPercentLeft(), distanceSensorInPercentFront(), distanceSensorInPercentRight());
//                 // sprintf(buffer, "L%d F%f R%d\n\r", isWallLeft(), isWallFront(), isWallRight());
//                 sprintf(buffer, "L%d F%d R%d\n\r", false, false, false);
//                 putsUART1(buffer);
//                 myCount=0;
//             }

//             // Detect when a new motion primitive is started and reset the start_new_motion_primitive flag
//             // CAUTION! This does not detect all changes, e.g. if the state changes to 0 in the switch case but changes right back in the if else above to the respecitve switch case case, the old_robot_motion_state was never updated and therefore the change goes unnoticed
//             // --> This function has to be used before the robot_motion_state is updated in the if else above
//             if (robot_motion_state != old_robot_motion_state) {
//                 old_robot_motion_state = robot_motion_state;
//                 changed_motion_state = true;
//             }

//             if (changed_motion_state) {
//                 start_new_motion_primitive = true;

//                 char buffer[8];
//                 sprintf(buffer, "S: %i\n\r", robot_motion_state);
//                 putsUART1(buffer);
//             }

//             switch (robot_motion_state)
//             {
//                 case 0:
//                     // Drive Straight ahead
//                     driveStraightForever(vel_cruise); // Function that just drives the robot straight without a goal 
//                     start_new_motion_primitive = true;

//                     char buffer[3];
//                     sprintf(buffer, "%i\n\r", robot_motion_state);
//                     putsUART1(buffer);
//                     break;
//                 case 1: {
//                     // Drive straight for a little bit and than turn
//                     static bool done_driving_straight = false;

//                     // Reset variables for a new start of the motion primitive
//                     if (changed_motion_state) {
//                         done_driving_straight = false;
//                     }
//                     // 1. Straight
//                     if (!done_driving_straight) {
//                         float distance_to_goal = driveStraightForNMeters(0.12, vel_cruise, start_new_motion_primitive);
//                         start_new_motion_primitive = false;

//                         if (distance_to_goal == 0) {
//                             done_driving_straight = true;
//                             start_new_motion_primitive = true;
//                         }

//                         break;
//                     }

//                     // 2. Turning Left
//                     if (done_driving_straight) {
//                         float angle_to_goal = turn90DegreesLeft(vel_cruise, start_new_motion_primitive);
//                         start_new_motion_primitive = false;

//                         if (angle_to_goal == 0) {
//                             done_driving_straight = false;
//                             start_new_motion_primitive = true;
//                             robot_motion_state = 0;
//                         }
//                     }
//                     // Alternative: see if the robot just turns on its own when lateral control mode is set soly to ONE_WALL_FOLLOWING_LEFT
//                     // No, will have problems with right turns
//                     break;
//                 }
//                 case 2: {
//                     // Drive straight for a little bit and than turn
//                     static bool done_driving_straight = false;

//                     // Reset variables for a new start of the motion primitive
//                     if (changed_motion_state) {
//                         done_driving_straight = false;
//                     }
//                     // 1. Straight
//                     if (!done_driving_straight) {
//                         float distance_to_goal = driveStraightForNMeters(0.12, vel_cruise, start_new_motion_primitive);
//                         start_new_motion_primitive = false;

//                         if (distance_to_goal == 0) {
//                             done_driving_straight = true;
//                             start_new_motion_primitive = true;
//                         }

//                         break;
//                     }

//                     // 2. Turning Right
//                     if (done_driving_straight) {
//                         float angle_to_goal = turn90DegreesRight(vel_cruise, start_new_motion_primitive);
//                         start_new_motion_primitive = false;

//                         if (angle_to_goal == 0) {
//                             done_driving_straight = false;
//                             start_new_motion_primitive = true;
//                             robot_motion_state = 0;
//                         }
//                     }
//                     // Alternative: see if the robot just turns on its own when lateral control mode is set soly to ONE_WALL_FOLLOWING_LEFT
//                     // No, will have problems with right turns
//                     break;
//                 } case 3: {
//                     // Turn around and drive back
//                     float angle_to_goal = turn180DegreesLeft(vel_cruise, start_new_motion_primitive);
//                     start_new_motion_primitive = false;
//                     if (angle_to_goal == 0) {
//                         robot_motion_state = 0;
//                         // char buffer[10];
//                         // sprintf(buffer, "%f\n\r", angle_to_goal);
//                         // putsUART1(buffer);
//                     }

//                     // char buffer[8];
//                     // sprintf(buffer, "%1.2f\n\r", angle_to_goal);
//                     // putsUART1(buffer);
//                     break;
//                 } default:
//                     break;
//             }
//         }

//         waitBeforeDriveCounter++;

//     } else {
//         // char buffer[3];
//         // sprintf(buffer, "S\n\r");
//         // putsUART1(buffer);
//         set_DC_and_motor_state_left(0.0, "forward_slow_decay");
//         set_DC_and_motor_state_right(0.0, "forward_slow_decay");
//     }

// }




            // // Wall Following Algorithm
            // if (isWallLeft() && !isWallFront()) {
            //     // Drive Straight ahead
            //     driveStraightForever(vel_cruise); // Function that just drives the robot straight without a goal 
            // } else if (!isWallLeft()) {
            //     // Drive straight for a little bit and than turn
            //     // 1.
            //     float distance_to_goal = driveStraightForNMeters(0.12, vel_cruise, start_new_motion_primitive);
            //     // 2.
            //     if (distance_to_goal == 0) {
            //         // start_new_motion_primitive only when entering this if clause fot the first time
            //         // start_new_motion_primitive = true;
            //         // How to close the robots eyes while turning? Proper State machine needed ?
            //         turn90DegreesLeft(vel_cruise, start_new_motion_primitive);
            //     }
            //     // Alternative: see if the robot just turns on its own when lateral control mode is set soly to ONE_WALL_FOLLOWING_LEFT
            //     // No, will have problems with right turns
            // } else if (isWallLeft() && isWallFront() && !isWallRight()){
            //     // Drive straight for a little bit and than turn
            //     // 1.
            //     driveStraightForNMeters(0.12, vel_cruise, start_new_motion_primitive);
            //     // Turn right
            //     turn90DegreesRight(vel_cruise, start_new_motion_primitive);
            // } else if (isWallLeft() && isWallFront() && isWallRight()){
            //     // Turn around and drive back
            //     turn180DegreesLeft(vel_cruise, start_new_motion_primitive);
            // }



/*
* Drive straight Forever
*/
// void __attribute__((__interrupt__, auto_psv)) _T1Interrupt(void)
// {
//     IFS0bits.T1IF = 0;           // reset Timer 1 interrupt flag 
//     static int myCount=0;
//     myCount++;

//     static bool done = false;

//     if (mazi_running == 1 && !done){

//         static int waitBeforeDriveCounter=0;
//         static bool start_driving = false;
//         // Improvised local state to keep track of which driving instruction to execute
//         static int driving_instruction_step = 0;

//         // Has to be done like this as max int value is 32.767 and the waitBeforeDriveCounter is an int
//         // Therefore after 32.767 the waitBeforeDriveCounter will overflow and start from 0 again, At least this is what I think
//         // But just setting a flag once, I don't have to worry about the overflow
//         if (waitBeforeDriveCounter >= 1000){
//             start_driving = true;
//         }

//         if (start_driving){

//             static bool start_new_motion_primitive = true;
//             LED1 = ~LED1;

//             float vel_cruise = 0.3;

//             /*
//             * Wall Following Algorithm
//             * Modes:
//             * 0 = driveStraightForever [DONE]
//             * 1 = turn left
//             * 2 = turn right
//             * 3 = turn around [DONE]
//             */
//             static int robot_motion_state = 0;
//             static int old_robot_motion_state = 0;
//             static bool changed_motion_state = false;
//             changed_motion_state = false;

//             if (myCount >= 100){
//                 char buffer[8];
//                 sprintf(buffer, "1S: %i\n\r", robot_motion_state);
//                 putsUART1(buffer);
//                 myCount=0;
//             }

//             // Drive Straight ahead
//             driveStraightForever(vel_cruise); // Function that just drives the robot straight without a goal 
                    
            
//         }

//         waitBeforeDriveCounter++;

//     } else {
//         // char buffer[3];
//         // sprintf(buffer, "S\n\r");
//         // putsUART1(buffer);
//         set_DC_and_motor_state_left(0.0, "forward_slow_decay");
//         set_DC_and_motor_state_right(0.0, "forward_slow_decay");
//     }

// }





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

    switch (currMovementControlParameters.movementPrimitive)
    {
    case DRIVING_STRAIGHT:
        /* code */
        // Make sure to check if the goal is reached and update the global flag accordingly
        drivingStraightForNMeters();
        break;
    
    case TURNING:
        /* code */
        turningForNDegrees();
        // Write to Uart that the state was reached
        // if (myCount >= 100){
        //     char buffer[3];
        //     sprintf(buffer, "T\n\r");
        //     putsUART1(buffer);
        //     myCount=0;
        // }
        break;

    case PARKING:
        /* code */
        set_DC_and_motor_state_left(0.0, "forward_fast_decay");
        set_DC_and_motor_state_right(0.0, "forward_fast_decay");
        break;

    default:
        break;
    }

}