#include "newTypes.h"

// Initialize the global variables
// struct MovementControlParameters currMovementControlParameters = {PARKING, 0.0, 0.0, {0.0, 0.0}, false};
// struct MovementControlParameters currMovementControlParameters = {{PARKING, 0.0}, 0.0, 0.0, {0.0, 0.0}, false};
struct MovementControlParameters currMovementControlParameters = {
    {PARKING, 0.0, 0.0}, // movementPrimitive
    {0.0, 0.0}, // goalPositionInEncoderCounts
    false // is_movement_goal_reached
};

/*
 * RobotState Initialization:
 * The `robot_state` variable is a global structure that tracks the current state and 
 * operational parameters of the robot. It is used to manage the robot's behavior, 
 * timing, and emergency conditions.
 *
 * 1. `state` (initial value: IDLE):
 *    - Represents the current operational state of the robot.
 *    - Possible states include: IDLE, DELAY_BEFORE_START, EXECUTE, STOP, EMERGENCY
 *
 * 2. `previous_state` (initial value: IDLE):
 *    - Stores the robot's previous state before transitioning to the current state.
 *
 * 3. `just_switched_state` (initial value: false):
 *    - A flag indicating whether the robot has just transitioned to a new state.
 *
 * 4. `start_new_timer2` (initial value: false):
 *    - A flag indicating whether Timer 2 should be started.
 *
 * 5. `timer2_expired` (initial value: false):
 *    - A flag indicating whether Timer 2 has expired.
 *
 * 6. `timer2_running` (initial value: false):
 *    - A flag indicating whether Timer 2 is currently running.
 *
 * 7. `emergency_stop` (initial value: false):
 *    - A flag indicating whether the robot is in an emergency stop condition.
 *
 * 8. `timer2_value` (initial value: 32000):
 *    - Stores the value for Timer 2
 *
 * The `robot_state` structure is essential for coordinating the robot's behavior, 
 * managing state transitions, and ensuring safe and predictable operation.
 */
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