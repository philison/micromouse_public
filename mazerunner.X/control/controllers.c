#include "controllers.h"
#include "motorEncoders.h"
#include "myPWM.h"
#include <stdbool.h> // for bool type
#include <math.h> // for fabs() function
#include "dma.h" // for SENSOR_LEFT, SENSOR_FRONT, SENSOR_RIGHT

//#include math.h

/***
 * A simple PI-Controller to drive the motor at a targetVelocity via PWM
 * It sets the duty cycle for the PWM signal to the motor. The return value is just for debugging purposes.
***/
float pi_vel_controller_left(float targetVelocity, float currentVelocity)
{

    //float currentVelocity = getVelocityInCountsPerSample_1(); // The control loop does not work if the function is called within the function
    float error = targetVelocity - currentVelocity; 
    float dc = 0;
    float kp = 0.7;
    float ki = 0.08;
    static float integral = 0;
    // float dt = 0.01;
    float maxDC = 1.0;
    float minDC = 0.0;
    float maxIntegral = 5;
    float minIntegral = -5;
    
    // integral += error * dt;
    integral += error;
    
    if (integral > maxIntegral)
    {
        integral = maxIntegral;
    }
    else if (integral < minIntegral)
    {
        integral = minIntegral;
    }
    
    dc = kp * error + ki * integral; 

    float dc_sign = dc/fabs(dc);
    dc = fabs(dc);
    
    if (dc > maxDC)
    {
        dc = maxDC;
    }
    else if (dc < minDC)
    {
        dc = minDC;
    }

    // Handle wheel turning direction:
    if (dc_sign >= 0)
    {
        // set_DC_and_motor_state_left(dc, "forward_slow_decay");
        set_DC_and_motor_state_left(dc, "forward_fast_decay");
    }
    else if (dc_sign < 0)
    {
        // set_DC_and_motor_state_left(dc, "reverse_slow_decay");
        set_DC_and_motor_state_left(dc, "reverse_fast_decay");
    }

    return dc;
}


/***
 * A simple PI-Controller to drive the motor at a targetVelocity via PWM
 * It sets the duty cycle for the PWM signal to the motor. The return value is just for debugging purposes.
***/
float pi_vel_controller_right(float targetVelocity, float currentVelocity)
{
    float error = targetVelocity - currentVelocity; 
    float dc = 0;
    float kp = 0.7;
    float ki = 0.08;
    static float integral = 0;
    // float dt = 0.01;
    float maxDC = 1.0;
    float minDC = 0.0;
    float maxIntegral = 5;
    float minIntegral = -5;
    
    // integral += error * dt;
    integral += error;


    if (integral > maxIntegral)
    {
        integral = maxIntegral;
    }
    else if (integral < minIntegral)
    {
        integral = minIntegral;
    }
    
    dc = kp * error + ki * integral;

    float dc_sign = dc/fabs(dc);
    dc = fabs(dc);

    
    if (dc > maxDC)
    {
        dc = maxDC;
    }
    else if (dc < minDC)
    {
        dc = minDC;
    }
    
    // Handle wheel turning direction:
    if (dc_sign >= 0)
    {
        // set_DC_and_motor_state_right(dc, "forward_slow_decay");
        set_DC_and_motor_state_right(dc, "forward_fast_decay");
    }
    else if (dc_sign < 0)
    {
        // set_DC_and_motor_state_right(dc, "reverse_slow_decay");
        set_DC_and_motor_state_right(dc, "reverse_fast_decay");
    }

    // set_DC_and_motor_state_right(dc, "forward_slow_decay");
    return dc;
}


/* P-Wall-Centering Controller */
// Adds additional velocity to the left or right wheel to center the robot between two walls

struct Velocities p_wall_centering_controller(float distance_left, float distance_right, float vel_base)
{
    struct Velocities result;

    // When the robot is closer to the left wall the voltage output by the left sensor increases as the reflected light intensity increases
    // --> distance_right increases
    float error = distance_right - distance_left;
    float kp = 0.05;
    
    result.vel_left = vel_base - kp * error;
    result.vel_right = vel_base + kp * error;
    
    // return both velocities
    return result;
}



float p_goal_distance_controller(float distance_to_goal, float vel_cruise)
{
    
    float error = distance_to_goal;
    // float kp = 6;
    float kp = 25;
    
    float vel_base = kp*error;

    // Limit the vel_base to the abs value of vel_cruise, as a max value
    if (fabs(vel_base) > fabs(vel_cruise))
    {
        // Conserve the sign of the velocity base, caused by the sign of the distance_to_goal
        float sign = vel_base/fabs(vel_base);
        vel_base = sign * vel_cruise;
    }

    return vel_base;
}



/* P-Turn-To-Goal Controller */
// Ensures that the robot slows down when it approaches the goal turn angle

// Options: 
// - look at bothe wheels for the control loop
// - look at only the right or left wheel for the control loop
// Controls the turn base velocity
// needs an additional controler that controls that the two wheels achieve the same velocity druing the turn, idealy the turn base velocity

float p_goal_angle_controller(float angle_to_goal, float vel_turn_cruise)
{
    
    float error = angle_to_goal;
    // float error = angle_to_goal;
    // float kp = 0.03;
    float kp = 0.12;
    // float kp = 0.03;
    
    float vel_turn_base = kp*error;

    if (fabs(vel_turn_base) > fabs(vel_turn_cruise))
    {
        // Conserve the sign of the vel_turn_base, caused by the sign of the angle_to_goal
        float sign = vel_turn_base/fabs(vel_turn_base);
        vel_turn_base = sign * vel_turn_cruise;
    }

    return vel_turn_base;
}


/* P-Single-Wall-Following Right Controller */
// Adds additional velocity to the left or right wheel to center the robot between two walls

struct Velocities p_one_wall_following_right(float distance_right, float vel_base)
{
    struct Velocities result;
    // float distanceSensorRightWallThreshold = 25.0;

    // When the robot is closer to the right wall the voltage output by the right sensor increases as the reflected light intensity increases
    // --> distance_right increases --> right wheel should turn faster
    float error = distance_right - SENSOR_VALUE_RIGHT_WHEN_IN_CELL_CENTER;
    float kp = 0.05;
    
    result.vel_left = vel_base - kp * error;
    result.vel_right = vel_base + kp * error;
    
    // return both velocities
    return result;
}


/* P-Single-Wall-Following Left Controller */
// Adds additional velocity to the left or right wheel to center the robot between two walls

struct Velocities p_one_wall_following_left(float distance_left, float vel_base)
{
    struct Velocities result;
    // float distanceSensorLeftWallThreshold = 27.0; // Value for the left sensor when the robot is in the middle of the cell and there is a wall on the left side

    // When the robot is closer to the left wall the voltage output by the left sensor increases as the reflected light intensity increases
    // --> distance_left increases --> left wheel should turn faster
    float error = distance_left - SENSOR_VALUE_LEFT_WHEN_IN_CELL_CENTER;
    float kp = 0.05;
    
    result.vel_left = vel_base + kp * error;
    result.vel_right = vel_base - kp * error;
    
    // return both velocities
    return result;
}