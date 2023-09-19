/* 
 * File:   abstractControl.h
 * Author: Philipp
 *
 * Created on September 18, 2023, 08:10 PM
 */

#include <stdbool.h> // for bool type
#include "IOconfig.h"
#include "xc.h"

#ifndef __NEWTYPES_H__
#define	__NEWTYPES_H__

// State that selects the correct motor control loop in the motor controller
// enum MovementPrimitive {
//     DRIVING_STRAIGHT,
//     TURNING,
//     PARKING,
// };

enum MovementPrimitiveTypes {
    DRIVING_STRAIGHT,
    TURNING,
    PARKING,
};

// TODO: Add a velocity to the movement primitive, and use it in the motor controller !!!
struct MovementPrimitive {
    enum MovementPrimitiveTypes type;
    float value;
    float velocity;
};

void updateMovementPrimitiveParameters(enum MovementPrimitiveTypes type, float value_as_distance_or_angle, float velocity);

// Struct to hold the Encoder Values in Counts for the Right and Left Motor
struct EncoderCounts {
    long right;
    long left;
};

// struct MovementControlParameters {
//     enum MovementPrimitive movementPrimitive;
//     float vel_cruise;
//     float vel_turn_cruise;
//     struct EncoderCounts goalPositionInEncoderCounts;
//     bool is_movement_goal_reached;
// };

struct MovementControlParameters {
    struct MovementPrimitive movementPrimitive;
    float vel_cruise;
    float vel_turn_cruise;
    struct EncoderCounts goalPositionInEncoderCounts;
    bool is_movement_goal_reached;
};

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

extern RobotState robot_state;

void switchRobotStateTo(States new_state);
void timeWithTimer2(int timer2_value_in_ms);
void turnOnStateLED();

#endif	/* __NEWTYPES_H__ */