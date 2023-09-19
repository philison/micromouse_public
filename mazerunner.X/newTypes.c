#include "newTypes.h"

// Initialize the global variables
struct MovementControlParameters currMovementControlParameters = {PARKING, 0.0, 0.0, {0.0, 0.0}, false};

RobotState robot_state = {IDLE, IDLE, false, false, false, false, 32000};


// void updateRobotState(bool button1_pressed) {
//     // Update the robot state
//     robot_state.previous_state = robot_state.state;

//     switch (robot_state.state)
//     {
//     case IDLE:
//         if (button1_pressed) {
//             robot_state.state = DELAY_BEFORE_START;
//         }
//         break;
    
//     case DELAY_BEFORE_START:
//         robot_state.state = EXECUTE;
//         break;

//     case EXECUTE:
//         if (currMovementControlParameters.is_movement_goal_reached) {
//             robot_state.state = STOP;
//         }
//         break;

//     case STOP:
//         robot_state.state = IDLE;
//         break;

//     default:
//         break;
//     }
// }

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