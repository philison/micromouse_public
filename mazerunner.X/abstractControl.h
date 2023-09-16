/* 
 * File:   abstractControl.h
 * Author: Nicki, Philipp
 *
 * Created on September 13, 2023, 5:18 PM
 */


#include <stdbool.h> // for bool type
#include "motorEncoders.h"
#include "distanceSensors.h"
#include "controllers.h"
#include <math.h> // for fabs

#define MAZE_CELL_LENGTH 0.18 // in meters
#define GOAL_REACHED_THRESHOLD_DISTANCE 0.01 // in meters
// #define GOAL_REACHED_THRESHOLD_DISTANCE 0.02 // in meters
#define GOAL_REACHED_THRESHOLD_ANGLE 1.0 // in degrees

// This is a guard condition so that contents of this file are not included
// more than once.  
#ifndef __ABSTRACTCONTROL_H__
#define	__ABSTRACTCONTROL_H__


struct Velocities desiredVelocitiesBasedOnCorrectLateralControlMode(float vel_base);

bool isDistanceGoalReached(float distance_to_goal);
// bool isDistanceGoalReached(float total_driven_distance, float distance_to_goal);
bool isAngleGoalReached(float distance_to_goal);

// Driving Straight
float driveStraightForNMeters(float nMeters, float vel_cruise, bool start_new_motion_primitive);
float driveStraightForNCells(int nCells, float vel_cruise, bool start_new_motion_primitive);
void driveStraightForever(float vel_cruise);

// Turning
float turnForNDegrees(float nDegrees, float vel_turn_cruise, bool start_new_motion_primitive);
float turn90DegreesRight(float vel_turn_cruise, bool start_new_motion_primitive);
float turn90DegreesLeft(float vel_turn_cruise, bool start_new_motion_primitive);
float turn180DegreesRight(float vel_turn_cruise, bool start_new_motion_primitive);
float turn180DegreesLeft(float vel_turn_cruise, bool start_new_motion_primitive);

#endif	/* __ABSTRACTCONTROL_H__ */