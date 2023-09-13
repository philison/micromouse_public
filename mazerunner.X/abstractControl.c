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

void driveStraightForNMeters(float nMeters, float vel_cruise) {
    float initial_distance_to_goal = nMeters; // in meters

    float distance_to_goal = getDistanceToGoalInMeters(initial_distance_to_goal, false);
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
}

void driveStraightForNCells(int nCells, float vel_cruise) {
    driveStraightForNMeters(nCells * MAZE_CELL_LENGTH, vel_cruise);
}

void turnForNDegrees(float nDegrees, float vel_turn_cruise) {
    float initial_angle_to_goal = nDegrees; // in degrees, with pos values for right turn and neg values for left turn (clockwise)

    float angle_to_goal = getAngleToGoalInDegrees(initial_angle_to_goal, false);
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
}

void turn90DegreesRight(float vel_turn_cruise) {
    turnForNDegrees(90.0, vel_turn_cruise);
}

void turn90DegreesLeft(float vel_turn_cruise) {
    turnForNDegrees(-90.0, vel_turn_cruise);
}

void turn180DegreesRight(float vel_turn_cruise) {
    turnForNDegrees(180.0, vel_turn_cruise);
}

void turn180DegreesLeft(float vel_turn_cruise) {
    turnForNDegrees(-180.0, vel_turn_cruise);
}