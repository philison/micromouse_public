
/*! \file   adc.c
 * Author: Alexander Lenz
 *
 * Created on 10 Oct 2018, 16:53
 */

#include "adc.h"
#include <xc.h>
#include "serialComms.h"
#include "IOconfig.h"




void startADC1(void)
{
        AD1CON1bits.ADON=1; //set on-bit
        AD1CON1bits.ASAM=1;
}



void setupADC1()
{
    
    AD1CON1bits.ADON = 0; // disable ADC1 module
   
    

    AD1CON1bits.ADSIDL=0;//no sampling in idle mode
    AD1CON1bits.ADDMABM=1; //DMA channels are written in order of conversion
    AD1CON1bits.AD12B=1;// 12-bit operation
    AD1CON1bits.SIMSAM=1; // simultaneous sampling in 10 bit mode unimplemented in 12bit mode
    AD1CON1bits.FORM=0b00; //format is unsigned integer
    AD1CON1bits.SSRC=0b111; //see comment below for options
            
             //   SSRC (Sample Clock Source Select bits) options:
             //   111 = Internal counter ends sampling and starts conversion (auto-convert)
             //   110 = Reserved
             //   101 = Motor Control PWM2 interval ends sampling and starts conversion
             //   100 = GP timer (Timer5 for ADC1) compare ends sampling and starts conversion
             //   011 = Motor Control PWM1 interval ends sampling and starts conversion
             //   010 = GP timer (Timer3 for ADC1) compare ends sampling and starts conversion
             //   001 = Active transition on INT0 pin ends sampling and starts conversion
             //   000 = Clearing sample bit ends sampling and starts conversion

             
    
    AD1CON1bits.ASAM=0; //sampling starts not immediately after last conversion but when SAMP bit is set
    AD1CON1bits.SAMP=0; //bit is automatically set if ASAM=1 (auto-sampling)
    //AD1CON1bist.DONE=x; //bit set by hardware when conversion done


    //ADCON2

    AD1CON2bits.VCFG=0b000; //select Vref+ and Vref- as internal reference voltage
    AD1CON2bits.CSCNA=1; //enable analog input SCAN on channel 0
    AD1CON2bits.CHPS=0b11; // important for 10 bit mode //unimplemented in 12-bit mode
    //AD1CON2bits.BUFS=x; //indicates which buffer is currently written (only if BUFM=1)
    AD1CON2bits.SMPI=1;//!!!CHANGE HERE!!! Selects Increment Rate for DMA Addresses bits or number of sample/conversion operations per interrupt
                       // update, now only set  to 1 because we scan 2 channels
    // AD1CON2bits.SMPI=0;//!!!CHANGE HERE!!! Selects Increment Rate for DMA Addresses bits or number of sample/conversion operations per interrupt
    AD1CON2bits.BUFM=0; //always fill buffer starting at address 0x00
    AD1CON2bits.ALTS=0; //always use channel A and do not alternate

    //ADCON3
    AD1CON3bits.ADRC=0;// use internal clock source
    AD1CON3bits.SAMC=0x05; //auto sample time bits, number of Tad   SAMC = SampleCount
                           //only valid when SSRC=0b111
    AD1CON3bits.ADCS=0x06 ;//8-bits to derive ADC clock
                           //Tad= TCY ? (ADCS<7:0> + 1)

    //ADCON4
    AD1CON4bits.DMABL=0b000;//<2:0>: Selects Number of DMA Buffer Locations per Analog Input bits


    //AD1CSSL (input scan select register)
    AD1CSSL= 0b0000000000100001; //select the analog channel 0 and 5 !!!CHANGE HERE!!! Select channels to sample always starts with the LSB and this is also the order the data is put in the buffer
    // AD1CSSL= 0b0000000000100000; //select the analog channel 0 and 5 !!!CHANGE HERE!!! Select channels to sample always starts with the LSB and this is also the order the data is put in the buffer


    AD1CHS123bits.CH123NA = 0b00; //negative input for S/H 123 is Vref -
    AD1CHS123bits.CH123SA = 0b1 ; //sample channel 4 and 5  on S/h 2 and 3
    //interrupt configuration

    IFS0bits.AD1IF = 0;
    IEC0bits.AD1IE = 0;
    IPC3bits.AD1IP = 5;

}

// void __attribute__((interrupt, no_auto_psv)) _ADCInterrupt(void)
// {
//     unsigned int temperature;
//     IFS0bits.AD1IF=0; //reset the interrupt flag
//     AD1CON1bits.ASAM=0; //stop the sampling
//     //now read the data from the buffers ADCBUF0, ADCBUF1 etc.
//     potentiometer=ADC1BUF0; //get the result
//     //temperature=ADC1BUF1;
//     //do something with the data
    
//     U1TXREG='D';
//     LED4=~LED4;
// }