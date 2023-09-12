/* 
 * File:   controllers.h
 * Author: Roman & Phil
 *
 * Created on June 27, 2023, 5:59 PM
 */

#include <stdbool.h> // for bool type

#ifndef CONTROLLERS_H
#define	CONTROLLERS_H

// #ifdef	__cplusplus
// extern "C" {
// #endif




// #ifdef	__cplusplus
// }
// #endif


// Define a struct to hold the Motor Velocities
struct Velocities {
    float vel_right;
    float vel_left;
};

// float pi_vel_controller(float targetVelocity, float currentVelocity);

float pi_vel_controller_left(float targetVelocity, float currentVelocity);
float pi_vel_controller_right(float targetVelocity, float currentVelocity, bool shouldTurn);

struct Velocities p_wall_centering_controller(float distance_left, float distance_right, float vel_base);
float p_goal_distance_controller(float distance_to_goal, float vel_cruise);

float p_goal_angle_controller(float angle_to_goal, float vel_turn_cruise);

#endif	/* CONTROLLERS_H */

