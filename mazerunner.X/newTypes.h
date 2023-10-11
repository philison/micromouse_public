/* 
 * File:   abstractControl.h
 *
 * Created on September 18, 2023, 08:10 PM
 */

#include <stdbool.h> // for bool type
#include "IOconfig.h"
#include "xc.h"

#ifndef __NEWTYPES_H__
#define	__NEWTYPES_H__

// State that selects the correct motor control loop in the motor controller
enum MovementPrimitiveTypes {
    DRIVING_STRAIGHT,
    DRIVING_STRAIGHT_FOREVER,
    TURNING,
    PARKING,
};

// TODO: Add a velocity to the movement primitive, and use it in the motor controller !!!
struct MovementPrimitive {
    enum MovementPrimitiveTypes type;
    float value;
    float vel_cruise; // For both Driving and Turning
};

void updateMovementPrimitiveParameters(enum MovementPrimitiveTypes type, float value_as_distance_or_angle, float velocity);

// Struct to hold the Encoder Values in Counts for the Right and Left Motor
struct EncoderCounts {
    long right;
    long left;
};


struct MovementControlParameters {
    struct MovementPrimitive movementPrimitive;
    // float vel_cruise;
    // float vel_turn_cruise;
    struct EncoderCounts goalPositionInEncoderCounts;
    bool is_movement_goal_reached;
};

// This is the global variable that holds the current movement control parameters for the motor controller
extern struct MovementControlParameters currMovementControlParameters;

typedef enum States {
    IDLE,
    DELAY_BEFORE_START,
    // EXECUTE_MOTION_PRIMITIVE,
    EXECUTE,
    STOP,
    EMERGENCY,
} States;

typedef struct RobotState {
    States state;
    States previous_state;
    bool just_switched_state;
    // Inputs to the state machine
    bool button1_pressed;
    bool timer2_expired;
    bool execution_finished;
    // Others
    bool start_new_timer2;
    int timer2_value;
} RobotState;

// This is the global variable that holds the current state of the robot 
// (the main switch-case statement in main.c)
extern RobotState robot_state;

void switchRobotStateTo(States new_state);
void timeWithTimer2(int timer2_value_in_ms);
void turnOnStateLED();

#endif	/* __NEWTYPES_H__ */