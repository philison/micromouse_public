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

