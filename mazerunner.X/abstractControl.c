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


// TODO: It can only register a goal reached if it is called often enough to detect the goal reached state, 
// or if the robot staies in the goal reached state for a long enough time
bool isDistanceGoalReached(float distance_to_goal) {
    return fabs(distance_to_goal) <= GOAL_REACHED_THRESHOLD_DISTANCE;
}
// bool isDistanceGoalReached(float total_driven_distance, float distance_to_goal) {
//     return fabs(total_driven_distance - distance_to_goal) <= GOAL_REACHED_THRESHOLD_DISTANCE;
// }

// TODO: It can only register a goal reached if it is called often enough to detect the goal reached state, 
// or if the robot staies in the goal reached state for a long enough time
bool isAngleGoalReached(float distance_to_goal) {
    return fabs(distance_to_goal) <= GOAL_REACHED_THRESHOLD_ANGLE;
}

float driveStraightForNMeters(float nMeters, float vel_cruise, bool start_new_motion_primitive) {
    float initial_distance_to_goal = nMeters; // in meters

    // If it is the start of a new motion primitive, reset/init the position and distance variables
    // This is done in the getDistanceToGoalInMeters function if the init_starting_position parameter is true
    float distance_to_goal = getDistanceToGoalInMeters(initial_distance_to_goal, start_new_motion_primitive);
    
    // Check if the goal is reached
    if (isDistanceGoalReached(distance_to_goal)) {
        return 0;
    }

    // The base velocity (vel_base) becomes zero when the goal is reached
    float vel_base = p_goal_distance_controller(distance_to_goal, vel_cruise);

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

    return distance_to_goal;
}

float driveStraightForNCells(int nCells, float vel_cruise, bool start_new_motion_primitive) {
    return driveStraightForNMeters(nCells * MAZE_CELL_LENGTH, vel_cruise, start_new_motion_primitive);
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