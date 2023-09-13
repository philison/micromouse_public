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

#define MAZE_CELL_LENGTH 0.18 // in meters

// This is a guard condition so that contents of this file are not included
// more than once.  
#ifndef __ABSTRACTCONTROL_H__
#define	__ABSTRACTCONTROL_H__


struct Velocities desiredVelocitiesBasedOnCorrectLateralControlMode(float vel_base);

// Driving Straight
void driveStraightForNMeters(float nMeters, float vel_cruise);
void driveStraightForNCells(int nCells, float vel_cruise);

// Turning
void turnForNDegrees(float nDegrees, float vel_turn_cruise);
void turn90DegreesRight(float vel_turn_cruise);
void turn90DegreesLeft(float vel_turn_cruise);
void turn180DegreesRight(float vel_turn_cruise);
void turn180DegreesLeft(float vel_turn_cruise);

#endif	/* __ABSTRACTCONTROL_H__ */