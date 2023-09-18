/* 
 * File:   abstractControl.h
 * Author: Philipp
 *
 * Created on September 18, 2023, 08:10 PM
 */

#include <stdbool.h> // for bool type

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

#endif	/* __NEWTYPES_H__ */