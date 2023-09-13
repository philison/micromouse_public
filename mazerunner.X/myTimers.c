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
//             // TODO: Create a Dist sensor file that contains a function to read the values in percentage etc.
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
//             float distance_to_goal = getDistanceToGoalInMeters(initial_distance_to_goal);
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

//             bool shouldTurn = false;
//             // float dc_left = pi_vel_controller_left(vel_desired.vel_left , motor_vel_left);
//             // float dc_right = pi_vel_controller_right(vel_desired.vel_right, motor_vel_right, shouldTurn);

//             float dc_left = pi_vel_controller_left(vel_base, motor_vel_left);
//             float dc_right = pi_vel_controller_right(vel_base, motor_vel_right, shouldTurn);

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



/* Control Distance Driven and detect and USE the correct Wall Following Control Mode*/
void __attribute__((__interrupt__, auto_psv)) _T1Interrupt(void)
{
    IFS0bits.T1IF = 0;           // reset Timer 1 interrupt flag 
    static int myCount=0;

    if (mazi_running == 1){

        static int waitBeforeDriveCounter=0;
        static enum lateralControlMode oldLateralControlMode;

        if (waitBeforeDriveCounter == 0) {
            // UART Buffer
            char buffer[30];

            // Print Sensor Values
            // sprintf(buffer, "SLP \tSFP \tSRP \tm \tvL \tvR \tcMode\n\r");    
            sprintf(buffer, "SLP;SFP;SRP;m;vL;vR;cMode\n\r");    

            // oldLateralControlMode = getLateralControlMode();
            // const char* lateralControlModeString = getLateralControlModeName(oldLateralControlMode);
            // sprintf(buffer, "%s\n\r", lateralControlModeString);
            putsUART1(buffer);
        }


        if (waitBeforeDriveCounter >= 1000){
            LED1 = ~LED1;
            // waitBeforeDriveCounter=0;
        
            float vel_cruise = 0.3;
            float initial_distance_to_goal = 5 * 0.18; // in meters

            // float distance_to_goal = (float)SENSOR_FRONT / 4096.0f * 100.0; // 12 bit ADC therfore 2^12 = 4096
            float distance_to_goal = getDistanceToGoalInMeters(initial_distance_to_goal);
            // float distance_driven = getDistanceDrivenInMeters();

            float vel_base = p_goal_distance_controller(distance_to_goal, vel_cruise);

            // Convert the value of the potentiometer to a percentage for the PWM
            float sensor_left_distance_in_percentage =distanceSensorInPercentLeft(); 
            float sensor_front_distance_in_percentage = distanceSensorInPercentFront(); 
            float sensor_right_distance_in_percentage = distanceSensorInPercentRight(); 

            // Switch Case to execute the correct Laterall Control Mode
            struct Velocities vel_desired;
            switch (getLateralControlMode())
            {
                case ONE_WALL_FOLLOWING_RIGHT:
                    vel_desired = p_one_wall_following_right(sensor_right_distance_in_percentage, vel_base);
                    break;
                case ONE_WALL_FOLLOWING_LEFT:
                    vel_desired = p_one_wall_following_left(sensor_left_distance_in_percentage, vel_base);
                    break;
                case TWO_WALL_CENTERING:
                    vel_desired = p_wall_centering_controller( sensor_left_distance_in_percentage, sensor_right_distance_in_percentage, vel_base);
                    break;
                case ZERO_WALL_DRIVING:
                    vel_desired = (struct Velocities){vel_base, vel_base}; // Assign left and right velocities, (struct Velocities){vel_base, vel_base} creates a temporary struct with the specified values and assigns it to vel_desired
                    // Alternative to the compact professional code line above
                    // vel_desired.left_velocity = vel_base; // Assign the left wheel velocity
                    // vel_desired.right_velocity = vel_base; // Assign the right wheel velocity
                    break;
                default:
                    break;
            }

            // Current Motor Velocities from Encoders
            float motor_vel_left = getVelocityInRoundsPerSecond_Left();
            float motor_vel_right = getVelocityInRoundsPerSecond_Right();

            bool shouldTurn = false;
            float dc_left = pi_vel_controller_left(vel_desired.vel_left , motor_vel_left);
            float dc_right = pi_vel_controller_right(vel_desired.vel_right, motor_vel_right, shouldTurn);

            // float dc_left = pi_vel_controller_left(vel_base, motor_vel_left);
            // float dc_right = pi_vel_controller_right(vel_base, motor_vel_right, shouldTurn);

            if (myCount >= 100){
                LED1 = ~LED1;
                // char buffer[10];
                // sprintf(buffer, "%2.3f\r\n", distance);
                float distance = getTotalDrivenDistanceInMeters();
                // float inter_distance = getDrivenDistanceInMeters2();
                // sprintf(buffer, "%2.4f\r\n", distance);
                // sprintf(buffer, "%2.2f %2.2f\r\n", distance, inter_distance);
                // putsUART1(buffer);

                enum lateralControlMode currenLateralControlMode = getLateralControlMode();

                // UART Buffer
                char buffer[40];
                // Print Sensor Values
                // sprintf(buffer, "SLP: %3.2f,\tSFP: %3.2f,\tSRP: %3.2f\n\r", (sensor_left_distance_in_percentage*100), (sensor_front_distance_in_percentage*100), (sensor_right_distance_in_percentage*100));    
                // sprintf(buffer, "%2.2f \t%2.2f \t%2.2f \t%2.3f \t%2.2f \t%2.2f \t%i\n\r", (sensor_left_distance_in_percentage), (sensor_front_distance_in_percentage), (sensor_right_distance_in_percentage), distance, vel_desired.vel_left, vel_desired.vel_right, currenLateralControlMode);
                sprintf(buffer, "%2.2f;%2.2f;%2.2f;%2.3f;%2.2f;%2.2f;%i;\n\r", (sensor_left_distance_in_percentage), (sensor_front_distance_in_percentage), (sensor_right_distance_in_percentage), distance, vel_desired.vel_left, vel_desired.vel_right, currenLateralControlMode);
                
                // // Print the result of getLateralControlMode()
                // enum lateralControlMode LCM = getLateralControlMode();
                // const char* lateralControlModeString = getLateralControlModeName(LCM);
                // sprintf(buffer, "%s\n\r", lateralControlModeString);

                putsUART1(buffer);
                myCount=0;
            }

            enum lateralControlMode currenLateralControlMode = getLateralControlMode();

            // if (currenLateralControlMode != oldLateralControlMode) {
            //     char buffer[50];
            //     const char* lateralControlModeString = getLateralControlModeName(currenLateralControlMode);
            //     sprintf(buffer, "%s\n\r", lateralControlModeString);

            //     putsUART1(buffer);
            //     oldLateralControlMode = currenLateralControlMode;
            // }

            myCount++;

            if (getTotalDrivenDistanceInMeters() >= initial_distance_to_goal){
                waitBeforeDriveCounter = 0;
            }
        }

    waitBeforeDriveCounter++;

    } else {
        set_DC_and_motor_state_left(0.0, "forward_slow_decay");
        set_DC_and_motor_state_right(0.0, "forward_slow_decay");
    }
}