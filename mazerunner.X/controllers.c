#include "controllers.h"
#include "motorEncoders.h"
#include "myPWM.h"
#include <stdbool.h> // for bool type
#include <math.h> // for fabs() function

//#include math.h

/***
 * A simple PI-Controller to drive the motor at a targetVelocity via PWM
***/
float pi_vel_controller_left(float targetVelocity, float currentVelocity, bool forward)
{
    // bool forward = true;
    // if (targetVelocity < 0)
    // {
    //     forward = false;
    // }
    targetVelocity = fabs(targetVelocity);
    currentVelocity = fabs(currentVelocity);
    // TODO: What happens when we switch to forward drive while driving backwards ? Weirted control behavior ?

    //float currentVelocity = getVelocityInCountsPerSample_1(); // The control loop does not work if the function is called within the function
    float error = targetVelocity - currentVelocity; // -20
    float dc = 0;
    float kp = 0.5;
    float ki = 0.1;
    static float integral = 0;
    // float dt = 0.01;
    float maxDC = 0.5;
    float minDC = 0.0;
    // float minDC = -100; // Should reverse motor direction for active breaking ?
    float maxIntegral = 4;
    float minIntegral = -4;
    
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
    
    dc = kp * error + ki * integral; // -200
    
    if (dc > maxDC)
    {
        dc = maxDC;
    }
    else if (dc < minDC)
    {
        dc = minDC;
    }
    

    if (forward)
    {
        set_DC_and_motor_state_left(dc, "forward_slow_decay");
    }
    else
    {
        set_DC_and_motor_state_left(dc, "reverse_slow_decay");
    }
    // set_DC_and_motor_state_left(dc, "forward_slow_decay");
    return dc;
}


/***
 * A simple PI-Controller to drive the motor at a targetVelocity via PWM
***/
float pi_vel_controller_right(float targetVelocity, float currentVelocity, bool forward)
{
    // bool forward = true;
    // if (targetVelocity < 0)
    // {
    //     forward = false;
    // }
    targetVelocity = fabs(targetVelocity);
    currentVelocity = fabs(currentVelocity);

    //float currentVelocity = getVelocityInCountsPerSample_1(); // The control loop does not work if the function is called within the function
    float error = targetVelocity - currentVelocity; // -20
    float dc = 0;
    float kp = 0.5;
    float ki = 0.1;
    static float integral = 0;
    // float dt = 0.01;
    float maxDC = 0.5;
    float minDC = 0.0;
    // float minDC = -100; // Should reverse motor direction for active breaking ?
    float maxIntegral = 4;
    float minIntegral = -4;
    
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
    
    dc = kp * error + ki * integral; // -200
    
    if (dc > maxDC)
    {
        dc = maxDC;
    }
    else if (dc < minDC)
    {
        dc = minDC;
    }
    

    if (forward)
    {
        set_DC_and_motor_state_right(dc, "forward_slow_decay");
    }
    else
    {
        set_DC_and_motor_state_right(dc, "reverse_slow_decay");
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
    // float maxDC = 1.0;
    // float minDC = 0.0;
    
    result.vel_left = vel_base - kp * error;
    result.vel_right = vel_base + kp * error;
    
    // if (dc > maxDC)
    // {
    //     dc = maxDC;
    // }
    // else if (dc < minDC)
    // {
    //     dc = minDC;
    // }
    
    // return both velocities
    return result;
}




/* P-Distance-To-Goal Controller */
// Ensures that the robot slows down when it approaches the goal distance

float p_goal_distance_controller(float distance_to_goal, float vel_cruise)
{
    
    float error = distance_to_goal;
    float kp = 6;
    
    float vel_base = kp*error;

    // Limit the vel_base to the vel_cruise as a max value
    if (fabs(vel_base) > fabs(vel_cruise))
    {
        // Conserve the sign of the velocity base gained from the sign of the distance_to_goal
        vel_base = (vel_base/fabs(vel_base)) * vel_cruise;
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
    
    float error = fabs(angle_to_goal);
    // float error = angle_to_goal;
    float kp = 0.03;
    
    float vel_turn_base = kp*error;

    if (vel_turn_base > vel_turn_cruise)
    {
        vel_turn_base = vel_turn_cruise;
    }

    // Determine the sign of the turn base velocity for the left wheel
    float sign = angle_to_goal/fabs(angle_to_goal);

    return sign * vel_turn_base;
    // return vel_turn_base;
}
