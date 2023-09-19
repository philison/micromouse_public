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
enum MovementPrimitive {
    DRIVING_STRAIGHT,
    TURNING,
    PARKING,
};

// Struct to hold the Encoder Values in Counts for the Right and Left Motor
struct EncoderCounts {
    long right;
    long left;
};

struct MovementControlParameters {
    enum MovementPrimitive movementPrimitive;
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
    // Others
    bool start_new_timer2;
    int timer2_value;
} RobotState;

extern RobotState robot_state;

void switchRobotStateTo(States new_state);
void timeWithTimer2(int timer2_value_in_ms);
void turnOnStateLED();

#endif	/* __NEWTYPES_H__ */