/* Microchip Technology Inc. and its subsidiaries.  You may use this software 
 * and any derivatives exclusively with Microchip products. 
 * 
 * THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS".  NO WARRANTIES, WHETHER 
 * EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED 
 * WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A 
 * PARTICULAR PURPOSE, OR ITS INTERACTION WITH MICROCHIP PRODUCTS, COMBINATION 
 * WITH ANY OTHER PRODUCTS, OR USE IN ANY APPLICATION. 
 *
 * IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, 
 * INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND 
 * WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS 
 * BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE.  TO THE 
 * FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS 
 * IN ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF 
 * ANY, THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
 *
 * MICROCHIP PROVIDES THIS SOFTWARE CONDITIONALLY UPON YOUR ACCEPTANCE OF THESE 
 * TERMS. 
 */

/* 
 * File:   
 * Author: 
 * Comments:
 * Revision history: 
 */

// This is a guard condition so that contents of this file are not included
// more than once.  
#ifndef __MYPWM_H__
#define	__MYPWM_H__

#include <xc.h> // include processor files - each processor file is guarded.  
// #define MYPWM_MAX (2*26666L)
#define MYPWM_MAX (2*527L) // For ~50.5kHz PWM frequency / or 20ys Period
// #define MYPWM_MAX_MAZE_MOTOR (35199) // 2*26666*0.66 Limit the Motor PWM max duty cycle to 66% for mazerunner to limit the motor voltage to 6V (from 9V H-Bridge Supply)
#define MYPWM_MAX_MAZE_MOTOR (MYPWM_MAX*0.66) // 2*26666*0.66 Limit the Motor PWM max duty cycle to 66% for mazerunner to limit the motor voltage to 6V (from 9V H-Bridge Supply)
void setupPWM();
void modulatePWMwithSINE(uint16_t *pwmDutyCycleRegister);
void setPWM_DCpercentage(uint16_t *pwmDutyCycleRegister, float percentage);
void setPWM_DCpercentage_Motor(uint16_t *pwmDutyCycleRegister, float percentage);
void setPWM_DCpercentage_Motor_inverted(uint16_t *pwmDutyCycleRegister, float percentage);

void forward_slow_decay_left();
void forward_slow_decay_right();
void reverse_slow_decay_left();
void reverse_slow_decay_right();

void forward_fast_decay_left();
void forward_fast_decay_right();
void reverse_fast_decay_left();
void reverse_fast_decay_right();

void set_DC_and_motor_state_left(float dc, char *motor_state);
void set_DC_and_motor_state_right(float dc, char *motor_state);

#endif	/* __MYPWM_H__ */

