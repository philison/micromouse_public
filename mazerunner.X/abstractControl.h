/* 
 * File:   abstractControl.h
 *
 * Created on September 13, 2023, 5:18 PM
 */


#include <stdbool.h> // for bool type
#include "motorEncoders.h"
#include "distanceSensors.h"
#include "controllers.h"
#include <math.h> // for fabs
#include "newTypes.h"
#include "serialComms.h"
#include "mazeSolver.h"
#include "motorEncoders.h"


// This is a guard condition so that contents of this file are not included
// more than once.  
#ifndef __ABSTRACTCONTROL_H__
#define	__ABSTRACTCONTROL_H__

// #define MAZE_CELL_LENGTH 0.18 // in meters
#define MAZE_CELL_LENGTH 0.19 // in meters
// Works for Left-Wall Following:
// #define VEL_CRUISE 0.8
// #define VEL_TURN_CRUISE 0.8
// Final config:
#define VEL_CRUISE 0.5
#define VEL_TURN_CRUISE 0.5
#define VEL_CRUISE_FINAL_RUN 1.0
#define VEL_TURN_CRUISE_FINAL_RUN 0.5
// For Testing
// #define VEL_CRUISE 0.3
// #define VEL_TURN_CRUISE 0.3


struct Velocities desiredVelocitiesBasedOnCorrectLateralControlMode(float vel_base);

// Old Driving Straight
void driveStraightForever(float vel_cruise);

// Old Turning
float turnForNDegrees(float nDegrees, float vel_turn_cruise, bool start_new_motion_primitive);
float turn90DegreesRight(float vel_turn_cruise, bool start_new_motion_primitive);
float turn90DegreesLeft(float vel_turn_cruise, bool start_new_motion_primitive);
float turn180DegreesRight(float vel_turn_cruise, bool start_new_motion_primitive);
float turn180DegreesLeft(float vel_turn_cruise, bool start_new_motion_primitive);

// New Movement functions for the State Machine Software Structure
void initDrivingStraightForNMeters(float nMeters);
void drivingStraightForNMeters();
void drivingStraightForever();

void initTurningForNDegrees(float nDegrees);
void turningForNDegrees();

void initMovementFromGlobalGoal();


// Movement functions for the Maze Solver
void turn180();
void turnLeft();
void turnRight();
void moveForward();


/* SIMPLE WALL FOLLOWER */

enum SimpleWallFollowerStates {
    // SWF_IDLE,
    SWF_DRIVE_ONE_CELL_AHEAD,
    SWF_TURN_LEFT,
    SWF_TURN_RIGHT,
    SWF_DRIVE_STRAIGHT_AHEAD,
    SWF_DRIVE_AND_TURN_RIGHT,
    SWF_DRIVE_AND_TURN_LEFT,
    SWF_DRIVE_TO_CELL_CENTER,
    SWF_TURN_AROUND,
};

struct SimpleWallFollowerState {
    enum SimpleWallFollowerStates curr_state;
    enum SimpleWallFollowerStates prev_state;
    bool just_switched_state;
    bool seen_left_wall_after_turn;
};

extern struct SimpleWallFollowerState simple_wall_follower_state;

void switchSimpleWallFollowerStateTo(enum SimpleWallFollowerStates new_state);
void initStateSWF();
void updateSimpleWallFollowerState();

void simpleWallFollower();


/* Simple Motion Primitive Executor */
void simpleMotionPrimitiveExecutor();




#endif	/* __ABSTRACTCONTROL_H__ */