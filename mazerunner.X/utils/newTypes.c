#include "newTypes.h"

// Initialize the global variables
// struct MovementControlParameters currMovementControlParameters = {PARKING, 0.0, 0.0, {0.0, 0.0}, false};
// struct MovementControlParameters currMovementControlParameters = {{PARKING, 0.0}, 0.0, 0.0, {0.0, 0.0}, false};
struct MovementControlParameters currMovementControlParameters = {
    {PARKING, 0.0, 0.0}, // movementPrimitive
    {0.0, 0.0}, // goalPositionInEncoderCounts
    false // is_movement_goal_reached
};

RobotState robot_state = {IDLE, IDLE, false, false, false, false, false, 32000};

void updateMovementPrimitiveParameters(enum MovementPrimitiveTypes type, float value_as_distance_or_angle, float velocity) {
    // Update the movement primitive parameters
    currMovementControlParameters.movementPrimitive.type = type;
    currMovementControlParameters.movementPrimitive.value = value_as_distance_or_angle;
    currMovementControlParameters.movementPrimitive.vel_cruise = velocity;
}

void switchRobotStateTo(States new_state) {
    // Update the robot state
    robot_state.previous_state = robot_state.state;
    robot_state.state = new_state;
    robot_state.just_switched_state = true;
    turnOnStateLED();
}

void timeWithTimer2(int timer2_value_in_ms) {
    // Set the timer2 value
    robot_state.timer2_value = timer2_value_in_ms;
    // Start the timer2
    robot_state.start_new_timer2 = true;
}

void turnOnStateLED() {
    // Turn on the LED that corresponds to the current state
    switch (robot_state.state)
    {
    case IDLE:
        LED1 = LEDON;
        LED2 = LEDOFF;
        LED3 = LEDOFF;
        LED4 = LEDOFF;
        break;
    
    case DELAY_BEFORE_START:
        LED1 = LEDOFF;
        LED2 = LEDON;
        LED3 = LEDOFF;
        LED4 = LEDOFF;
        break;

    case EXECUTE:
        LED1 = LEDOFF;
        LED2 = LEDOFF;
        LED3 = LEDON;
        LED4 = LEDOFF;
        break;

    case STOP:
        LED1 = LEDOFF;
        LED2 = LEDOFF;
        LED3 = LEDOFF;
        LED4 = LEDON;
        break;

    case EMERGENCY:
        LED1 = LEDON;
        LED2 = LEDON;
        LED3 = LEDON;
        LED4 = LEDON;
        break;
    default:
        break;
    }
}