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


#endif	/* __ABSTRACTCONTROL_H__ */