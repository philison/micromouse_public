/* 
 * File:   distanceSensors.h
 * Author: Nicki, Philipp
 *
 * Created on September 12, 2023, 7:29 PM
 */


#include <stdbool.h> // for bool type


// This is a guard condition so that contents of this file are not included
// more than once.  
#ifndef __DISTANCESENSORS_H__
#define	__DISTANCESENSORS_H__

#define WALL_DETECTION_THRESHOLD 28.0 // in percent
#define WALL_DETECTION_THRESHOLD_FRONT 28.0 // TODO: select an appropriate value

/*
* It is the motion that occurs perpendicular to the primary direction of travel or orientation. 
* In the case of a two-wheeled mobile robot, lateral movement would involve the robot moving to the left or right.
*/
enum lateralControlMode {
    TWO_WALL_CENTERING,
    ONE_WALL_FOLLOWING_RIGHT,
    ONE_WALL_FOLLOWING_LEFT,
    ZERO_WALL_DRIVING
};

// Struct to hold the Distance Sensor Values 
struct DistanceSensorsReadingsInPercent {
    float left;
    float front;
    float right;
};

float distanceSensorInPercentLeft(void);
float distanceSensorInPercentFront(void);
float distanceSensorInPercentRight(void);

struct DistanceSensorsReadingsInPercent distanceSensorsReadingsInPercent(void);

enum lateralControlMode getLateralControlMode(void);
const char* getLateralControlModeName(enum lateralControlMode lateralControlMode);

#endif	/* __DISTANCESENSORS_H__*/