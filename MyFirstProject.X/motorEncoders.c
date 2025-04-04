
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
    velocity=3.141592 *2* ((currentPosition-oldPosition)*0.01) / (33*4*16);

    oldPosition=currentPosition;
    return velocity;



}
