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
#include "newTypes.h"
#include "serialComms.h"
#include "mazeSolver.h"

// #define MAZE_CELL_LENGTH 0.18 // in meters
#define MAZE_CELL_LENGTH 0.19 // in meters
//Works for Left-Wall Following:
// #define VEL_CRUISE 0.8
// #define VEL_TURN_CRUISE 0.8
#define VEL_CRUISE 0.5
#define VEL_TURN_CRUISE 0.5
#define VEL_CRUISE_FINAL_RUN 0.5
#define VEL_TURN_CRUISE_FINAL_RUN 0.5
// For Testing
// #define VEL_CRUISE 0.3
// #define VEL_TURN_CRUISE 0.3

// #define GOAL_REACHED_THRESHOLD_DISTANCE 0.01 // in meters
// // #define GOAL_REACHED_THRESHOLD_DISTANCE 0.02 // in meters
// #define GOAL_REACHED_THRESHOLD_ANGLE 1.0 // in degrees

// This is a guard condition so that contents of this file are not included
// more than once.  
#ifndef __ABSTRACTCONTROL_H__
#define	__ABSTRACTCONTROL_H__

#include "motorEncoders.h"

// extern struct MovementControlParameters currMovementControlParameters = {PARKING, 0.0, 0.0, {0.0, 0.0}, false};
// extern struct MovementControlParameters currMovementControlParameters ;

struct Velocities desiredVelocitiesBasedOnCorrectLateralControlMode(float vel_base);

// bool isDistanceGoalReached(float distance_to_goal);
// // bool isDistanceGoalReached(float total_driven_distance, float distance_to_goal);
// bool isAngleGoalReached(float distance_to_goal);

// Driving Straight
// float driveStraightForNMeters(float nMeters, float vel_cruise, bool start_new_motion_primitive);
// float driveStraightForNCells(int nCells, float vel_cruise, bool start_new_motion_primitive);
void driveStraightForever(float vel_cruise);

// NEW
void initDrivingStraightForNMeters(float nMeters);
void drivingStraightForNMeters();
void drivingStraightForever();

void initTurningForNDegrees(float nDegrees);
void turningForNDegrees();

void initMovementFromGlobalGoal();
// NEWEND

// Turning
float turnForNDegrees(float nDegrees, float vel_turn_cruise, bool start_new_motion_primitive);
float turn90DegreesRight(float vel_turn_cruise, bool start_new_motion_primitive);
float turn90DegreesLeft(float vel_turn_cruise, bool start_new_motion_primitive);
float turn180DegreesRight(float vel_turn_cruise, bool start_new_motion_primitive);
float turn180DegreesLeft(float vel_turn_cruise, bool start_new_motion_primitive);


// Movement functions for the maze solver
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

// extern struct SimpleWallFollowerState simple_wall_follower_state = {SWF_DRIVE_STRAIGHT_AHEAD, SWF_DRIVE_STRAIGHT_AHEAD, true};
extern struct SimpleWallFollowerState simple_wall_follower_state;

void switchSimpleWallFollowerStateTo(enum SimpleWallFollowerStates new_state);
void initStateSWF();
void updateSimpleWallFollowerState();

void simpleWallFollower();


/* SIMPLE MOTION PRIMITIVE EXECUTOR */

// enum SimpleMotionPrimitiveExecutorStates {
//     SMPE_TURN_90_RIGHT,
//     SMPE_TURN_90_LEFT,
//     SMPE_TURN_180_RIGHT,
//     SMPE_TURN_180_LEFT,
//     SMPE_TURN_M90_RIGHT,
//     SMPE_TURN_M90_LEFT,
//     SMPE_TURN_M180_RIGHT,
//     SMPE_TURN_M180_LEFT,
//     SMPE_DRIVE_018,
//     SMPE_DRIVE_M021,
// };

// struct SimpleMotionPrimitiveExecutorState {
//     enum SimpleMotionPrimitiveExecutorStates curr_state;
//     enum SimpleMotionPrimitiveExecutorStates prev_state;
//     bool just_switched_state;
// };

// // extern struct SimpleMotionPrimitiveExecutorState simple_motion_primitive_executor_state = {SMPE_TURNING_FOR_N_DEGREES, SMPE_TURNING_FOR_N_DEGREES, true};
// extern struct SimpleMotionPrimitiveExecutorState simple_motion_primitive_executor_state;

// void switchSimpleMotionPrimitiveExecutorStateTo(enum SimpleMotionPrimitiveExecutorStates new_state);
// // void updateSimpleMotionPrimitiveExecutorState();

void simpleMotionPrimitiveExecutor();




#endif	/* __ABSTRACTCONTROL_H__ */