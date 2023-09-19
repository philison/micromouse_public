#include "abstractControl.h"


/*
* Retrieves the current lateral control mode (which is based on the current distance sensor readings)
* Returns the desired motor velocities based on the vel_base and the retrieved control mode
*/
struct Velocities desiredVelocitiesBasedOnCorrectLateralControlMode(float vel_base) {

    struct Velocities desiredVelocities;
    enum lateralControlMode currentLateralControlMode = getLateralControlMode();
    struct DistanceSensorsReadingsInPercent currentDistanceSensorsReadingsInPercent = distanceSensorsReadingsInPercent();

    switch (currentLateralControlMode) {
        case ONE_WALL_FOLLOWING_RIGHT:
            desiredVelocities = p_one_wall_following_right(currentDistanceSensorsReadingsInPercent.right, vel_base);
            break;
        case ONE_WALL_FOLLOWING_LEFT:
            desiredVelocities = p_one_wall_following_left(currentDistanceSensorsReadingsInPercent.left, vel_base);
            break;
        case TWO_WALL_CENTERING:
            desiredVelocities = p_wall_centering_controller(currentDistanceSensorsReadingsInPercent.left, currentDistanceSensorsReadingsInPercent.right, vel_base);
            break;
        case ZERO_WALL_DRIVING:
            desiredVelocities = (struct Velocities){vel_base, vel_base}; // Assign left and right velocities, (struct Velocities){vel_base, vel_base} creates a temporary struct with the specified values and assigns it to desiredVelocities
            // Alternative to the compact professional code line above
            // desiredVelocities.left_velocity = vel_base; // Assign the left wheel velocity
            // desiredVelocities.right_velocity = vel_base; // Assign the right wheel velocity
            break;
        default:
            break;
    }

    return desiredVelocities;
}


// // TODO: It can only register a goal reached if it is called often enough to detect the goal reached state, 
// // or if the robot staies in the goal reached state for a long enough time
// bool isDistanceGoalReached(float distance_to_goal) {
//     return fabs(distance_to_goal) <= GOAL_REACHED_THRESHOLD_DISTANCE;
// }
// // bool isDistanceGoalReached(float total_driven_distance, float distance_to_goal) {
// //     return fabs(total_driven_distance - distance_to_goal) <= GOAL_REACHED_THRESHOLD_DISTANCE;
// // }

// // TODO: It can only register a goal reached if it is called often enough to detect the goal reached state, 
// // or if the robot staies in the goal reached state for a long enough time
// bool isAngleGoalReached(float distance_to_goal) {
//     return fabs(distance_to_goal) <= GOAL_REACHED_THRESHOLD_ANGLE;
// }

// float driveStraightForNMeters(float nMeters, float vel_cruise, bool start_new_motion_primitive) {
//     float initial_distance_to_goal = nMeters; // in meters

//     // If it is the start of a new motion primitive, reset/init the position and distance variables
//     // This is done in the getDistanceToGoalInMeters function if the init_starting_position parameter is true
//     float distance_to_goal = getDistanceToGoalInMeters(initial_distance_to_goal, start_new_motion_primitive);
    
//     // Check if the goal is reached
//     if (isDistanceGoalReached(distance_to_goal)) {
//         return 0;
//     }

//     // The base velocity (vel_base) becomes zero when the goal is reached
//     float vel_base = p_goal_distance_controller(distance_to_goal, vel_cruise);

//     // Any wheel velocity needed to correct during the wall centering or following is added to this base velocity above
//     // Resulting in the desired velocities (desiredVelocities) for the left and right wheel respectively
//     struct Velocities desiredVelocities = desiredVelocitiesBasedOnCorrectLateralControlMode(vel_base);
        
//     // Current Motor Velocities from Encoders
//     struct Velocities currentMotorVelocities = getVelocitiesInRoundsPerSecond();

//     // Hand the desired and current velocities to the PI velocity controller
//     // It sets the duty cycle for the PWM signal to the motors
//     // The return value is just for debugging purposes
//     float dc_left = pi_vel_controller_left(desiredVelocities.vel_left , currentMotorVelocities.vel_left);
//     float dc_right = pi_vel_controller_right(desiredVelocities.vel_right, currentMotorVelocities.vel_right);

//     return distance_to_goal;
// }

// This function is used to drive straight for a certain number of meters
// It will be called only once at the beginning of a new motion primitive
void initDrivingStraightForNMeters(float nMeters) {

    // Set the movement primitive to DRIVING_STRAIGHT
    currMovementControlParameters.movementPrimitive.type = DRIVING_STRAIGHT;

    // Calculate the goal encoder counts based on the number of meters to drive
    // Set the calculated goal encoder counts in the global variable
    currMovementControlParameters.goalPositionInEncoderCounts = getGoalPositionInEncoderCounts(nMeters);

    // Reset the is_movement_goal_reached flag
    currMovementControlParameters.is_movement_goal_reached = false;
}

void drivingStraightForNMeters() {

    // Check if the goal is reached
    if (isMovementGoalReached()) {
        // Set the is_movement_goal_reached flag
        currMovementControlParameters.is_movement_goal_reached = true;
        // Set the movement primitive to PARKING
        currMovementControlParameters.movementPrimitive.type = PARKING;
        return;
    }

    // The base velocity (vel_base) becomes zero when the goal is reached
    float vel_base = p_goal_distance_controller(getDistanceToGoalInMeters(), currMovementControlParameters.vel_cruise);

    // Any wheel velocity needed to correct during the wall centering or following is added to this base velocity above
    // Resulting in the desired velocities (desiredVelocities) for the left and right wheel respectively
    struct Velocities desiredVelocities = desiredVelocitiesBasedOnCorrectLateralControlMode(vel_base);
        
    // Current Motor Velocities from Encoders
    struct Velocities currentMotorVelocities = getVelocitiesInRoundsPerSecond();

    // Hand the desired and current velocities to the PI velocity controller
    // It sets the duty cycle for the PWM signal to the motors
    // The return value is just for debugging purposes
    float dc_left = pi_vel_controller_left(desiredVelocities.vel_left , currentMotorVelocities.vel_left);
    float dc_right = pi_vel_controller_right(desiredVelocities.vel_right, currentMotorVelocities.vel_right);
}


// float driveStraightForNCells(int nCells, float vel_cruise, bool start_new_motion_primitive) {
//     return driveStraightForNMeters(nCells * MAZE_CELL_LENGTH, vel_cruise, start_new_motion_primitive);
// }

void driveStraightForever(float vel_cruise) {
    float vel_base = vel_cruise;

    // Any wheel velocity needed to correct during the wall centering or following is added to this base velocity above
    // Resulting in the desired velocities (desiredVelocities) for the left and right wheel respectively
    struct Velocities desiredVelocities = desiredVelocitiesBasedOnCorrectLateralControlMode(vel_base);
        
    // Current Motor Velocities from Encoders
    struct Velocities currentMotorVelocities = getVelocitiesInRoundsPerSecond();

    // Hand the desired and current velocities to the PI velocity controller
    // It sets the duty cycle for the PWM signal to the motors
    // The return value is just for debugging purposes
    float dc_left = pi_vel_controller_left(desiredVelocities.vel_left , currentMotorVelocities.vel_left);
    float dc_right = pi_vel_controller_right(desiredVelocities.vel_right, currentMotorVelocities.vel_right);
}

float turnForNDegrees(float nDegrees, float vel_turn_cruise, bool start_new_motion_primitive) {

    // If it is the start of a new motion primitive, reset/init the position and angle variables
    // if (start_new_motion_primitive) {
    //     getAngleToGoalInDegrees(42, start_new_motion_primitive);
    //     start_new_motion_primitive = false;
    // }

    float initial_angle_to_goal = nDegrees; // in degrees, with pos values for right turn and neg values for left turn (clockwise)

    // If it is the start of a new motion primitive, reset/init the position and angle variables 
    // This is done in the getAngleToGoalInDegrees function if the init_starting_position parameter is true
    float angle_to_goal = getAngleToGoalInDegrees(initial_angle_to_goal, start_new_motion_primitive);

    // Check if the goal is reached
    if (isAngleGoalReached(angle_to_goal)) {
        // Reset the start_new_motion_primitive flag to trigger an initialization of the angle variables when a new motion primitive is started
        // TODO: Carefull !!! This might be a problem if the robot does not reach the goal but the motion primitive is interrupted otherwise without resetting the flag
        //     Maybe it is better to reset the flag in the "main loop where the function is called" when a new motion primitive is started (add this option as a parameter to the function as cuurrently implemented)
        // start_new_motion_primitive = true;
        return 0;
    }

    // The base velocity (vel_base) becomes zero when the goal is reached
    float vel_turn_base = p_goal_angle_controller(angle_to_goal, vel_turn_cruise);

    // No wall centering or following is needed during turning, some corection mechanism might be added later if necessary

    // Current Motor Velocities from Encoders
    struct Velocities currentMotorVelocities = getVelocitiesInRoundsPerSecond();

    // Hand the desired and current velocities to the PI velocity controller
    // CAUTION: In case of turning: The vel_desired.vel_right for the right Motor has to be negated when the robot should turn, since the vel sign is derived from the left wheel "coordinates"
    // It sets the duty cycle for the PWM signal to the motors
    // The return value is just for debugging purposes
    float dc_left = pi_vel_controller_left(vel_turn_base , currentMotorVelocities.vel_left);
    float dc_right = pi_vel_controller_right(-vel_turn_base, currentMotorVelocities.vel_right);

    return angle_to_goal;
}

void initTurningForNDegrees(float nDegrees) {
    // Set the movement primitive to TURNING
    currMovementControlParameters.movementPrimitive.type = TURNING;

    // Calculate the goal encoder counts based on the number of degrees to turn
    // Set the calculated goal encoder counts in the global variable
    // initial_distance_to_goal_in_meters = calculateArcLengthInMetersFromAngleInDegreesAndTurnRadius();
    float initial_distance_to_goal_in_meters = calculateArcLengthInMeters(nDegrees);
    currMovementControlParameters.goalPositionInEncoderCounts = getGoalPositionInEncoderCounts(initial_distance_to_goal_in_meters);

    // Reset the is_movement_goal_reached flag
    currMovementControlParameters.is_movement_goal_reached = false;
};

// Works for both tuning and driving straight
// The movementPrimitive.type and the movementPrimitive.value have to be set to the correct values before calling this function
void initMovementFromGlobalGoal() {
    // Reset the is_movement_goal_reached flag
    currMovementControlParameters.is_movement_goal_reached = false;

    switch (currMovementControlParameters.movementPrimitive.type)
    {
    case DRIVING_STRAIGHT: {
        // Calculate the goal encoder counts based on the number of meters to drive
        // Set the calculated goal encoder counts in the global variable
        currMovementControlParameters.goalPositionInEncoderCounts = getGoalPositionInEncoderCounts(currMovementControlParameters.movementPrimitive.value);
        break;
    }
    case TURNING: {
        // Calculate the goal encoder counts based on the number of degrees to turn
        // Set the calculated goal encoder counts in the global variable
        // initial_distance_to_goal_in_meters = calculateArcLengthInMetersFromAngleInDegreesAndTurnRadius();
        float initial_distance_to_goal_in_meters = calculateArcLengthInMeters(currMovementControlParameters.movementPrimitive.value);
        currMovementControlParameters.goalPositionInEncoderCounts = getGoalPositionInEncoderCounts(initial_distance_to_goal_in_meters);
        break;
    }
    default:
        break;
    }
};

void turningForNDegrees() {
    // Check if the goal is reached
    if (isMovementGoalReached()) {
        // Set the is_movement_goal_reached flag
        currMovementControlParameters.is_movement_goal_reached = true;
        // Set the movement primitive to PARKING
        currMovementControlParameters.movementPrimitive.type = PARKING;
        return;
    }

    // The base velocity (vel_base) becomes zero when the goal is reached
    // TODO: Replace by new getAngleToGoalInDegrees function
    float angle_to_goal = calculateAngleInDegreesFromArcLengthInMetersAndTurnRadius(getDistanceToGoalInMeters(), TURN_RADIUS);
    float vel_turn_base = p_goal_angle_controller(angle_to_goal, currMovementControlParameters.vel_turn_cruise);

    // No wall centering or following is needed during turning, some corection mechanism might be added later if necessary

    // Current Motor Velocities from Encoders
    struct Velocities currentMotorVelocities = getVelocitiesInRoundsPerSecond();

    // Hand the desired and current velocities to the PI velocity controller
    // CAUTION: In case of turning: The vel_desired.vel_right for the right Motor has to be negated when the robot should turn, since the vel sign is derived from the left wheel "coordinates"
    // It sets the duty cycle for the PWM signal to the motors
    // The return value is just for debugging purposes
    float dc_left = pi_vel_controller_left(vel_turn_base , currentMotorVelocities.vel_left);
    float dc_right = pi_vel_controller_right(-vel_turn_base, currentMotorVelocities.vel_right);
};

float turn90DegreesRight(float vel_turn_cruise, bool start_new_motion_primitive) {
    return turnForNDegrees(90.0, vel_turn_cruise, start_new_motion_primitive);
}

float turn90DegreesLeft(float vel_turn_cruise, bool start_new_motion_primitive) {
    return turnForNDegrees(-90.0, vel_turn_cruise, start_new_motion_primitive);
}

float turn180DegreesRight(float vel_turn_cruise, bool start_new_motion_primitive) {
    return turnForNDegrees(180.0, vel_turn_cruise, start_new_motion_primitive);
}

float turn180DegreesLeft(float vel_turn_cruise, bool start_new_motion_primitive) {
    return turnForNDegrees(-180.0, vel_turn_cruise, start_new_motion_primitive);
}


/* SIMPLE WALL FOLLOWER - NOT WORKING */

struct SimpleWallFollowerState simple_wall_follower_state = {SWF_DRIVE_STRAIGHT_AHEAD, SWF_DRIVE_STRAIGHT_AHEAD, true};

void switchSimpleWallFollowerStateTo(enum SimpleWallFollowerStates new_state) {
    simple_wall_follower_state.prev_state = simple_wall_follower_state.curr_state;
    simple_wall_follower_state.curr_state = new_state;
    simple_wall_follower_state.just_switched_state = true;
}

void updateSimpleWallFollowerState() {
    if (isWallLeft() && !isWallFront()) {
        switchSimpleWallFollowerStateTo(SWF_DRIVE_STRAIGHT_AHEAD);
    } else if (!isWallLeft()) {
        switchSimpleWallFollowerStateTo(SWF_DRIVE_AND_TURN_LEFT);
    } else if (isWallLeft() && isWallFront() && !isWallRight()){
        switchSimpleWallFollowerStateTo(SWF_DRIVE_AND_TURN_RIGHT);
    } else if (isWallLeft() && isWallFront() && isWallRight()){
        switchSimpleWallFollowerStateTo(SWF_TURN_AROUND);
    }
}

void simpleWallFollower() {

    // TODO: Add a safety layer that stops the robot 
    // in case a signle missread from the sensors triggers a turn,
    // although there are actually walls

    /*
    * Wall Following Algorithm
    * Modes:
    * 0 = driveStraightForever
    * 1 = turn left
    * 2 = turn right
    * 3 = turn around 
    */

    // Set the current motion state / primitive of the robot, can also be done in the switch case 0 below
    // This is only done while the robot is driving straight in the driveStraightForever Mode
    // if (simple_wall_follower_state.curr_state == SWF_DRIVE_STRAIGHT_AHEAD) {
    //     updateSimpleWallFollowerState();
    // }

    switch (simple_wall_follower_state.curr_state) {
        case SWF_DRIVE_STRAIGHT_AHEAD:
            /* Enter the State */
            if (simple_wall_follower_state.just_switched_state) {
                // Set the is_movement_goal_reached flag
                currMovementControlParameters.is_movement_goal_reached = false;
                // Reset the just_switched_state flag
                simple_wall_follower_state.just_switched_state = false;

                // Set the new motion state / primitive of the robot
                // Set the desired velocity
                currMovementControlParameters.vel_cruise = 0.3;
                // TODO: Change to DRIVING_STRAIGHT_FOREVER and implement it as a new motion primitive
                float initial_distance_to_goal_in_meters = 10.0; // in meters, 10 is like driving forever
                initDrivingStraightForNMeters(initial_distance_to_goal_in_meters);
            }
            /* Exit the State */
            updateSimpleWallFollowerState();
            break;

        case SWF_DRIVE_AND_TURN_LEFT:
            // First drive straight to the cell center and than turn left
            /* Enter the State */

            // 1. Straight Driving to the Cell Center
            if (simple_wall_follower_state.just_switched_state && simple_wall_follower_state.prev_state == SWF_DRIVE_STRAIGHT_AHEAD) {
                // Reset the just_switched_state flag
                simple_wall_follower_state.just_switched_state = false;
                switchSimpleWallFollowerStateTo(SWF_DRIVE_TO_CELL_CENTER);
            }

            // 2. Turning Left after Driving Straight to the Cell Center is completed
            if (simple_wall_follower_state.just_switched_state && simple_wall_follower_state.prev_state == SWF_DRIVE_TO_CELL_CENTER) {
                // Set the current motion state / primitive of the robot
                currMovementControlParameters.movementPrimitive.type = TURNING;
                // Set the desired velocity
                currMovementControlParameters.vel_turn_cruise = 0.3;
                // Set the is_movement_goal_reached flag
                currMovementControlParameters.is_movement_goal_reached = false;
                // Reset the just_switched_state flag
                simple_wall_follower_state.just_switched_state = false;

                float initial_angle_to_goal_in_degrees = -90.0; // in degrees, with pos values for right turn and neg values for left turn (clockwise)
                initTurningForNDegrees(initial_angle_to_goal_in_degrees);
            }

            /* Exit the State */
            // If the movement goal is reached go back to the SWF_DRIVE_STRAIGHT_AHEAD state
            if (currMovementControlParameters.is_movement_goal_reached) {
                currMovementControlParameters.is_movement_goal_reached = false;
                switchSimpleWallFollowerStateTo(SWF_DRIVE_STRAIGHT_AHEAD);
            }
            break;
        case SWF_DRIVE_AND_TURN_RIGHT:
            // First drive straight to the cell center and than turn right
            /* Enter the State */

            // 1. Straight Driving to the Cell Center
            if (simple_wall_follower_state.just_switched_state && simple_wall_follower_state.prev_state == SWF_DRIVE_STRAIGHT_AHEAD) {
                // Reset the just_switched_state flag
                simple_wall_follower_state.just_switched_state = false;
                switchSimpleWallFollowerStateTo(SWF_DRIVE_TO_CELL_CENTER);
            }

            // 2. Turning Right after Driving Straight to the Cell Center is completed
            if (simple_wall_follower_state.just_switched_state && simple_wall_follower_state.prev_state == SWF_DRIVE_TO_CELL_CENTER) {
                // Set the current motion state / primitive of the robot
                currMovementControlParameters.movementPrimitive.type = TURNING;
                // Set the desired velocity
                currMovementControlParameters.vel_turn_cruise = 0.3;
                // Set the is_movement_goal_reached flag
                currMovementControlParameters.is_movement_goal_reached = false;
                // Reset the just_switched_state flag
                simple_wall_follower_state.just_switched_state = false;

                float initial_angle_to_goal_in_degrees = 90.0; // in degrees, with pos values for right turn and neg values for left turn (clockwise)
                initTurningForNDegrees(initial_angle_to_goal_in_degrees);
            }

            /* Exit the State */
            // If the movement goal is reached go back to the SWF_DRIVE_STRAIGHT_AHEAD state
            if (currMovementControlParameters.is_movement_goal_reached) {
                currMovementControlParameters.is_movement_goal_reached = false;
                switchSimpleWallFollowerStateTo(SWF_DRIVE_STRAIGHT_AHEAD);
            }
            break;
        case SWF_DRIVE_TO_CELL_CENTER:
            /* Enter the State */
            if (simple_wall_follower_state.just_switched_state) {
                // Set the current motion state / primitive of the robot
                currMovementControlParameters.movementPrimitive.type = DRIVING_STRAIGHT;
                // Set the desired velocity
                currMovementControlParameters.vel_cruise = 0.3;
                // Set the is_movement_goal_reached flag
                currMovementControlParameters.is_movement_goal_reached = false;
                // Reset the just_switched_state flag
                simple_wall_follower_state.just_switched_state = false;

                float initial_distance_to_goal_in_meters = 0.1; // in meters, distance to the cell center from the point where one of the walls falls away
                initDrivingStraightForNMeters(initial_distance_to_goal_in_meters);
            }

            /* Exit the State */
            if (currMovementControlParameters.is_movement_goal_reached) {
                currMovementControlParameters.is_movement_goal_reached = false;
                // Return to the prev state which is either SWF_DRIVE_AND_TURN_LEFT or SWF_DRIVE_AND_TURN_RIGHT
                switchSimpleWallFollowerStateTo(simple_wall_follower_state.prev_state);
            }
            break;
        case SWF_TURN_AROUND:
            /* Enter the State */
            if (simple_wall_follower_state.just_switched_state) {
                // Set the current motion state / primitive of the robot
                currMovementControlParameters.movementPrimitive.type = TURNING;
                // Set the desired velocity
                currMovementControlParameters.vel_turn_cruise = 0.3;
                // Set the is_movement_goal_reached flag
                currMovementControlParameters.is_movement_goal_reached = false;
                // Reset the just_switched_state flag
                simple_wall_follower_state.just_switched_state = false;

                float initial_angle_to_goal_in_degrees = 180.0; // in degrees, with pos values for right turn and neg values for left turn (clockwise)
                initTurningForNDegrees(initial_angle_to_goal_in_degrees);
            }

            /* Exit the State */
            // If the movement goal is reached go back to the SWF_DRIVE_STRAIGHT_AHEAD state
            if (currMovementControlParameters.is_movement_goal_reached) {
                currMovementControlParameters.is_movement_goal_reached = false;
                switchSimpleWallFollowerStateTo(SWF_DRIVE_STRAIGHT_AHEAD);
            }
            break;
        default:
            break;
    }


}


/* SIMPLE MOTION PRIMITIVE EXECUTOR */

// struct SimpleMotionPrimitiveExecutorState simple_motion_primitive_executor_state = {SMPE_DRIVE_018, SMPE_DRIVE_018, true};

// void switchSimpleMotionPrimitiveExecutorStateTo(enum SimpleMotionPrimitiveExecutorStates new_state) {
//     simple_motion_primitive_executor_state.prev_state = simple_motion_primitive_executor_state.curr_state;
//     simple_motion_primitive_executor_state.curr_state = new_state;
//     simple_motion_primitive_executor_state.just_switched_state = true;
// }

// void initStateSMPE() {
//     // Set the is_movement_goal_reached flag
//     currMovementControlParameters.is_movement_goal_reached = false;
//     // Reset the just_switched_state flag
//     simple_motion_primitive_executor_state.just_switched_state = false;
// }

void simpleMotionPrimitiveExecutor() {
    // static enum SimpleMotionPrimitiveExecutorStates smpe_motion_array[10] = { 
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

    static bool first_invocation = true;

    // Define the movement primitives to be executed, 
    // with the resepctive type and value
    static struct MovementPrimitive smpe_motion_array[10] = { 
        {TURNING, 90.0},
        {TURNING, -90.0},
        {TURNING, 180.0},
        {TURNING, -180.0},
        {TURNING, 360.0},
        {DRIVING_STRAIGHT, 0.18},
        {DRIVING_STRAIGHT, -0.18},
    };

    static int smpe_motion_array_index = 0;

    currMovementControlParameters.vel_cruise = 0.3;
    currMovementControlParameters.vel_turn_cruise = 0.3;

    if (currMovementControlParameters.is_movement_goal_reached || first_invocation) {
        // Reset the flags
        first_invocation = false;
        currMovementControlParameters.is_movement_goal_reached = false;

        // Set global Goal to the next element in the motion array
        currMovementControlParameters.movementPrimitive = smpe_motion_array[smpe_motion_array_index];
        initMovementFromGlobalGoal();

        // UART
        char buffer[10];
        sprintf(buffer, "%i,%2.2f", currMovementControlParameters.movementPrimitive.type, currMovementControlParameters.movementPrimitive.value);
        putsUART1(buffer);

        // Increase the index to select the next element in the motion array
        smpe_motion_array_index++;
        // Ensure that the index does not exceed the array size
        if (smpe_motion_array_index >= sizeof(smpe_motion_array)/sizeof(struct MovementPrimitive)) {
            smpe_motion_array_index = 0;
        }
    }
}
