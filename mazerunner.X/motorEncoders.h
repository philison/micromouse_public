/*
 * File:   sensor.h
 * Author: bauma
 *
 * Created on 23. Mai 2019, 11:22
 */

#ifndef SENSOR_H
#define	SENSOR_H


#include<xc.h>
#include <stdbool.h> // for bool type
// #include "abstractControl.h"
#include "newTypes.h"

void initQEI1( unsigned int  startPos);
void initQEI2( unsigned int  startPos);
float getPositionInRad();
float getVelocityInRadPerSecond();
long getPositionInCounts_1();
int getVelocityInCountsPerSample_1();
long getPositionInCounts_2();
int getVelocityInCountsPerSample_2();

// Define a struct to hold the Motor Velocities
struct Velocities {
    float vel_right;
    float vel_left;
};

float getPositionInCounts_Right();
float getPositionInCounts_Left();

float getVelocityInRadPerSecond_Right();
float getVelocityInRadPerSecond_Left();

float getVelocityInRoundsPerSecond_Left();
float getVelocityInRoundsPerSecond_Right(); 

float getVelocityInRoundsPerMinute_Left();
float getVelocityInRoundsPerMinute_Right();
struct Velocities getVelocitiesInRoundsPerSecond();

float getFlanksPerSecond_Right();

float convertCountsToDistanceInMeters(float counts);

float getTotalDrivenDistanceInMeters(bool reset_static_variables);
float getDrivenDistanceInMeters();
float getDrivenDistanceInMeters2();
// float getDistanceToGoalInMeters(float initial_distance_to_goal, bool init_starting_position);

float getAngleToGoalInDegrees(float initial_angle_to_goal, bool init_starting_position);
float getTotalDrivenAngleInDegrees(bool reset_static_variables);

float calculateAngleInDegreesFromArcLengthInMetersAndTurnRadius(float arc_length_in_meters, float turn_radius_in_meters);

//new
#define ENCODER_COUNTS_PER_REVOLUTION (33.0 * 4.0 * 16.0)

#define GOAL_REACHED_THRESHOLD_DISTANCE 0.01 // in meters
#define GOAL_REACHED_THRESHOLD_ANGLE 1.0 // in degrees

// extern struct MovementControlParameters currMovementControlParameters;

long getPositionInCountsLong_Left();
long getPositionInCountsLong_Right();

long convertDistanceInMetersToCounts(float distance_to_goal_in_meters);

struct EncoderCounts getGoalPositionInEncoderCounts(float initial_distance_to_goal_in_meters);

bool isDistanceGoalReached(float distance_to_goal);
bool isAngleGoalReached(float distance_to_goal);
float getDistanceToGoalInMeters();

bool isMovementGoalReached();
//newend


extern long rotationCount1;
extern long rotationCount2;
extern float timer_time;

#define GET_ENCODER_1(RIGHT_ENCODER_POSITION_VALUE) (RIGHT_ENCODER_POSITION_VALUE=rotationCount1+POSCNT)
#define GET_ENCODER_2(LEFT_ENCODER_POSITION_VALUE) (LEFT_ENCODER_POSITION_VALUE=rotationCount2+POS2CNT)



#define WHEEL_ROTATIONS_PERROBOT_ROTATION 2.5
#define TICKS_PER_WHEELROTATION (64*33)
#define TICKS_PER_CENTIMETER TICKS_PER_WHEELROTATION/12.566
#define METER_PER_TICkS 0.12566/TICKS_PER_WHEELROTATION
#define DELTATICKS_90_DEGREES  (0.25* WHEEL_ROTATIONS_PERROBOT_ROTATION*TICKS_PER_WHEELROTATION) 
#define DELTATICKS_180_DEGREES (0.5 * WHEEL_ROTATIONS_PERROBOT_ROTATION*TICKS_PER_WHEELROTATION)
#define DELTATICKS_CELL_GAP (11.5*TICKS_PER_CENTIMETER)


#endif	/* SENSOR_H */