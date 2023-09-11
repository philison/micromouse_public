#include "controllers.h"
#include "motorEncoders.h"
#include "myPWM.h"

//#include math.h

/***
 * A simple PI-Controller to drive the motor at a targetVelocity via PWM
***/
float pi_vel_controller_left(float targetVelocity, float currentVelocity)
{
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
    
    set_DC_and_motor_state_left(dc, "forward_slow_decay");
    return dc;
}


/***
 * A simple PI-Controller to drive the motor at a targetVelocity via PWM
***/
float pi_vel_controller_right(float targetVelocity, float currentVelocity)
{
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
    
    set_DC_and_motor_state_right(dc, "forward_slow_decay");
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
    float kp = 0.05;
    
    float vel_base = kp*error;

    if (vel_base > vel_cruise)
    {
        vel_base = vel_cruise;
    }
    // else if (vel_base < 0)
    // {
    //     vel_base = 0;
    // }

    return vel_base;
}



