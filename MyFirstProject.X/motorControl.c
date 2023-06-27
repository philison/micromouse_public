#include "motorControl.h"
#include "motorEncoders.h"
#include "myPWM.h"

//#include math.h

// void pd_vel_controller(float targetVelocity, float currentVelocity, float *pwm)
float pi_vel_controller(float targetVelocity, int currentVelocity)
{
    //float currentVelocity = getVelocityInCountsPerSample_1();
    float error = targetVelocity - currentVelocity; // -20
    float pwm = 0;
    float kp = 0.01;
    float ki = 0.1;
    float integral = 0;
    // float dt = 0.01;
    float maxPWM = 1.0;
    float minPWM = 0.0;
    // float minPWM = -100; // Should reverse motor direction for active breaking ?
    float maxIntegral = 100;
    float minIntegral = -100;
    
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
    
    pwm = kp * error + ki * integral; // -200
    
    if (pwm > maxPWM)
    {
        pwm = maxPWM;
    }
    else if (pwm < minPWM)
    {
        pwm = minPWM;
    }
    
    // setPWM_DCpercentage(&P1DC1, pwm * MYPWM_MAX);
    setPWM_DCpercentage(&P1DC1, pwm);
    return pwm;
}