
#include <xc.h>
//#include "gpio.h"
#include "motorEncoders.h"
//file global



 long rotationCount1;
 long rotationCount2;
//long currentEncoderPosition;

#include <math.h>

//****************************************************************INITIALISE QEI************************

void initQEI1( unsigned int  startPos)
{


    QEI1CONbits.QEISIDL = 1; // discontinue module operation in idle mode
    QEI1CONbits.QEIM = 0b111;// Quadrature Encoder Interface enabled (x4mode) with position counter reset by match (MAX1CNT)
    QEI1CONbits.SWPAB = 0; // Phase A and B not swapped
    QEI1CONbits.PCDOUT = 0; // disable position counter direction pin
    QEI1CONbits.TQGATE = 0; // timer gated time acc disabled
    QEI1CONbits.POSRES = 0; // index does not reset position counter
    QEI1CONbits.TQCS = 0; // internal clock source (Tcy))
    QEI1CONbits.UPDN_SRC = 0; // direction of position counter determined using internal logic

    MAXCNT = 0xffff;
  
    POSCNT =  startPos;
    rotationCount1=0;

    IFS3bits.QEI1IF = 0; // clear interrupt flag
    IEC3bits.QEI1IE = 1; // enable interrupt
    IPC14bits.QEI1IP = 5;
}

void initQEI2( unsigned int  startPos)
{


    QEI2CONbits.QEISIDL = 1; // discontinue module operation in idle mode
    QEI2CONbits.QEIM = 0b111;// Quadrature Encoder Interface enabled (x4mode) with position counter reset by match (MAX1CNT)
    QEI2CONbits.SWPAB = 1; // Phase A and B  swapped
    QEI2CONbits.PCDOUT = 0; // disable position counter direction pin
    QEI2CONbits.TQGATE = 0; // timer gated time acc disabled
    QEI2CONbits.POSRES = 0; // index does not reset position counter
    QEI2CONbits.TQCS = 0; // internal clock source (Tcy))
    QEI2CONbits.UPDN_SRC = 0; // direction of position counter determined using internal logic

    MAX2CNT = 0xffff;

    POS2CNT =  startPos;
    rotationCount2=0;
    IFS4bits.QEI2IF = 0; // clear interrupt flag
    IEC4bits.QEI2IE = 1; // enable interrupt
    IPC18bits.QEI2IP = 5;
}


void __attribute__((__interrupt__, auto_psv)) _QEI1Interrupt(void)
{
    // Interrupt generated by QEI roll over/under
    IFS3bits.QEI1IF = 0; // clear interrupt
    //LED1=~LED1;


    if (POSCNT < 32768)
    {
        rotationCount1=rotationCount1+ (long)  0x10000; //we had a positive roll-over
    }
    else
    {
        rotationCount1=rotationCount1- (long) 0x10000;//we had a negative roll-over
    }


}


void __attribute__((__interrupt__, auto_psv)) _QEI2Interrupt(void)
{
    // Interrupt generated by QEI roll over/under
    IFS4bits.QEI2IF = 0; // clear interrupt
    //LED1=~LED1;
    if (POS2CNT < 32768)
    {
        rotationCount2=rotationCount2+ (long)  0x10000; //we had a positive roll-over
    }
    else
    {
        rotationCount2=rotationCount2- (long) 0x10000;//we had a negative roll-over
    }





}


float getPositionInRad()
{
    long currentEncoderPosition;
        //disable interrupts to make sure we have consistent data
    _NSTDIS=1;
    GET_ENCODER_1 (currentEncoderPosition);
        //disable interrupts to make sure we have consistent data
    _NSTDIS=0;
    return 3.141592*2*currentEncoderPosition/(16*4*33);
}

long getPositionInCounts_1()
{
        long currentEncoderPosition;
        GET_ENCODER_1 (currentEncoderPosition);
        return currentEncoderPosition; 
}

int getVelocityInCountsPerSample_1()
{
    static long oldPosition;
    long currentPosition;
    int velocity;

        //disable interrupts to make sure we have consistent data
    _NSTDIS=1;
    GET_ENCODER_1 (currentPosition);
    _NSTDIS=0;
    velocity=(currentPosition-oldPosition);

    oldPosition=currentPosition;
    return velocity;


}


long getPositionInCounts_2()
{
        long currentEncoderPosition;
        GET_ENCODER_2 (currentEncoderPosition);
        return currentEncoderPosition; 
}

int getVelocityInCountsPerSample_2()
{
    static long oldPosition;
    long currentPosition;
    int velocity;

        //disable interrupts to make sure we have consistent data
    _NSTDIS=1;
    GET_ENCODER_2 (currentPosition);
    _NSTDIS=0;
    velocity=(currentPosition-oldPosition);

    oldPosition=currentPosition;
    return velocity;


}

float getVelocityInRadPerSecond()
{


    static long oldPosition;
    float velocity;
    long currentPosition;

        //disable interrupts to make sure we have consistent data
    _NSTDIS=1;
    GET_ENCODER_1 (currentPosition);
    _NSTDIS=0;
    velocity=3.141592 *2* ((currentPosition-oldPosition) * timer_time * 1000.0) / (33*4*16);

    oldPosition=currentPosition;
    return velocity;
}

float getVelocityInRadPerSecond_Right()
{

    static long oldPosition;
    float velocity;
    long currentPosition;

        //disable interrupts to make sure we have consistent data
    _NSTDIS=1;
    GET_ENCODER_1 (currentPosition);
    _NSTDIS=0;
    velocity=3.141592 *2* ((currentPosition-oldPosition) * timer_time * 1000.0) / (33*4*16);

    oldPosition=currentPosition;
    return velocity;

}


float getVelocityInRadPerSecond_Left()
{

    static long oldPosition;
    float velocity;
    long currentPosition;

    //disable interrupts to make sure we have consistent data
    _NSTDIS=1;
    GET_ENCODER_2 (currentPosition);
    _NSTDIS=0;
    velocity=3.141592 *2* ((currentPosition-oldPosition) * timer_time * 1000.0) / (33*4*16);

    oldPosition=currentPosition;
    return velocity;

}

float getVelocityInRoundsPerSecond_Left() {
    // Calc: 1/(2*3.141592) = 0.159154
   return 0.159154 * getVelocityInRadPerSecond_Left();
}

float getVelocityInRoundsPerSecond_Right() {
    // Calc: 1/(2*3.141592) = 0.159154
   return 0.159154 * getVelocityInRadPerSecond_Right();
}

float getVelocityInRoundsPerMinutes_Left() {
   return 60 * getVelocityInRoundsPerSecond_Left();
}

float getVelocityInRoundsPerMinutes_Right() {
   return 60 * getVelocityInRoundsPerSecond_Right();
}





// float getVelocityInRoundsPerMinute_Right() {
//     static long oldPosition;
//     float velocity;
//     long currentPosition;

//     //disable interrupts to make sure we have consistent data
//     _NSTDIS=1;
//     GET_ENCODER_2 (currentPosition);
//     _NSTDIS=0;
//     float nbr_of_rounds_per_second = (currentPosition-oldPosition) / (33.0*4.0*16.0) * 1000.0;
//     velocity=60.0 * nbr_of_rounds_per_second;

//     oldPosition=currentPosition;
//     return velocity;
// }

float getFlanksPerSecond_Right() {
    static long oldFLankCount;
    float flanksPerSecond;
    long currentFLankCount;

    //disable interrupts to make sure we have consistent data
    _NSTDIS=1;
    GET_ENCODER_1 (currentFLankCount);
    _NSTDIS=0;

    flanksPerSecond = (currentFLankCount-oldFLankCount) * timer_time * 1000; // timer_time * 1000 = timer time in seconds

    oldFLankCount=currentFLankCount;
    return flanksPerSecond;
}

float getRPM_Right() {
    static long oldFLankCount;
    float flanksPerSecond;
    long currentFLankCount;

    //disable interrupts to make sure we have consistent data
    _NSTDIS=1;
    GET_ENCODER_1 (currentFLankCount);
    _NSTDIS=0;

    flanksPerSecond = (currentFLankCount-oldFLankCount) * timer_time * 1000; // timer_time * 1000 = timer time in seconds

    oldFLankCount=currentFLankCount;
    return flanksPerSecond;
}


// float getVelocityInDegreePerSecond_Left()
// {
//     static long oldPosition;
//     float velocity;
//     long currentPosition;

//         //disable interrupts to make sure we have consistent data
//     _NSTDIS=1;
//     GET_ENCODER_1 (currentPosition);
//     _NSTDIS=0;
//     velocity=360 *2* ((currentPosition-oldPosition)*0.01) / (33*4*16);
    
//     oldPosition=currentPosition;
//     return velocity;
// }




float getDistanceToGoalInMeters(float initial_distance_to_goal){

    float wheel_diameter = 0.06;
    float wheel_cirumference = wheel_diameter * 3.141592;

    static float distance_driven = 0;
    static float old_position = 0;
    float current_position = getPositionInCounts_1();
    float distance_driven_in_counts = current_position - old_position;
    float distance_driven_in_meters = distance_driven_in_counts / (33.0*4.0*16.0) * ;
    distance_driven += distance_driven_in_meters;
    old_position = current_position;

    return initial_distance_to_goal - distance_driven;
}



/*
    // Calc: 1/(2*3.141592) = 0.159154
    float distance_driven_left = 0.159154 * getVelocityInRadPerSecond_Left() * timer_time * 1000.0;
    float distance_driven_right = 0.159154 * getVelocityInRadPerSecond_Right() * timer_time * 1000.0;

    distance_driven += (distance_driven_left + distance_driven_right) / 2.0;
*/