/* 
 * File:   dma.h
 * Author: 
 *
 * Created on June 13, 2019, 11:41 AM
 */

#ifndef DMA_H
#define	DMA_H


#include <xc.h>
    
extern unsigned int adcData[32]__attribute__((space(dma)));

void initDmaChannel4(void);

//add some defines to make accessing data more readable

#define IO_1 adcData[0] //AN0
#define TEST_SENSOR adcData[1]  //AN5   Test sensor is connected to AN5 therefor it gets sampled by channel 5 and channel five is positioned in the adc after channel 0 (as in order of conversion)
// #define TEST_SENSOR adcData[0]  //AN5   Test sensor is connected to AN5 therefor it gets sampled by channel 5 and channel five is positioned in the adc after channel 0 (as in order of conversion)





#endif	/* DMA_H */

