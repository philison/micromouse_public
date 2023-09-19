
#include <xc.h>
//#include "gpio.h"
#include "motorEncoders.h"
//file global



 long rotationCount1;
 long rotationCount2;
//long currentEncoderPosition;

#include <math.h>

//****************************************************************INITIALISE QEI************************

void initQEI1( unsigned int  startPos)
{


    QEI1CONbits.QEISIDL = 1; // discontinue module operation in idle mode
    QEI1CONbits.QEIM = 0b111;// Quadrature Encoder Interface enabled (x4mode) with position counter reset by match (MAX1CNT)
    QEI1CONbits.SWPAB = 0; // Phase A and B not swapped
    QEI1CONbits.PCDOUT = 0; // disable position counter direction pin
    QEI1CONbits.TQGATE = 0; // timer gated time acc disabled
    QEI1CONbits.POSRES = 0; // index does not reset position counter
    QEI1CONbits.TQCS = 0; // internal clock source (Tcy))
    QEI1CONbits.UPDN_SRC = 0; // direction of position counter determined using internal logic

    MAXCNT = 0xffff;
  
    POSCNT =  startPos;
    rotationCount1=0;

    IFS3bits.QEI1IF = 0; // clear interrupt flag
    IEC3bits.QEI1IE = 1; // enable interrupt
    IPC14bits.QEI1IP = 5;
}

void initQEI2( unsigned int  startPos)
{


    QEI2CONbits.QEISIDL = 1; // discontinue module operation in idle mode
    QEI2CONbits.QEIM = 0b111;// Quadrature Encoder Interface enabled (x4mode) with position counter reset by match (MAX1CNT)
    QEI2CONbits.SWPAB = 1; // Phase A and B  swapped
    QEI2CONbits.PCDOUT = 0; // disable position counter direction pin
    QEI2CONbits.TQGATE = 0; // timer gated time acc disabled
    QEI2CONbits.POSRES = 0; // index does not reset position counter
    QEI2CONbits.TQCS = 0; // internal clock source (Tcy))
    QEI2CONbits.UPDN_SRC = 0; // direction of position counter determined using internal logic

    MAX2CNT = 0xffff;

    POS2CNT =  startPos;
    rotationCount2=0;
    IFS4bits.QEI2IF = 0; // clear interrupt flag
    IEC4bits.QEI2IE = 1; // enable interrupt
    IPC18bits.QEI2IP = 5;
}


void __attribute__((__interrupt__, auto_psv)) _QEI1Interrupt(void)
{
    // Interrupt generated by QEI roll over/under
    IFS3bits.QEI1IF = 0; // clear interrupt
    //LED1=~LED1;


    if (POSCNT < 32768)
    {
        rotationCount1=rotationCount1+ (long)  0x10000; //we had a positive roll-over
    }
    else
    {
        rotationCount1=rotationCount1- (long) 0x10000;//we had a negative roll-over
    }


}


void __attribute__((__interrupt__, auto_psv)) _QEI2Interrupt(void)
{
    // Interrupt generated by QEI roll over/under
    IFS4bits.QEI2IF = 0; // clear interrupt
    //LED1=~LED1;
    if (POS2CNT < 32768)
    {
        rotationCount2=rotationCount2+ (long)  0x10000; //we had a positive roll-over
    }
    else
    {
        rotationCount2=rotationCount2- (long) 0x10000;//we had a negative roll-over
    }





}


float getPositionInRad()
{
    long currentEncoderPosition;
        //disable interrupts to make sure we have consistent data
    _NSTDIS=1;
    GET_ENCODER_1 (currentEncoderPosition);
        //disable interrupts to make sure we have consistent data
    _NSTDIS=0;
    return 3.141592*2*currentEncoderPosition/(16*4*33);
}

long getPositionInCounts_1()
{
        long currentEncoderPosition;
        GET_ENCODER_1 (currentEncoderPosition);
        return currentEncoderPosition; 
}

int getVelocityInCountsPerSample_1()
{
    static long oldPosition;
    long currentPosition;
    int velocity;

        //disable interrupts to make sure we have consistent data
    _NSTDIS=1;
    GET_ENCODER_1 (currentPosition);
    _NSTDIS=0;
    velocity=(currentPosition-oldPosition);

    oldPosition=currentPosition;
    return velocity;


}


long getPositionInCounts_2()
{
        long currentEncoderPosition;
        GET_ENCODER_2 (currentEncoderPosition);
        return currentEncoderPosition; 
}

int getVelocityInCountsPerSample_2()
{
    static long oldPosition;
    long currentPosition;
    int velocity;

        //disable interrupts to make sure we have consistent data
    _NSTDIS=1;
    GET_ENCODER_2 (currentPosition);
    _NSTDIS=0;
    velocity=(currentPosition-oldPosition);

    oldPosition=currentPosition;
    return velocity;


}

float getVelocityInRadPerSecond()
{


    static long oldPosition;
    float velocity;
    long currentPosition;

        //disable interrupts to make sure we have consistent data
    _NSTDIS=1;
    GET_ENCODER_1 (currentPosition);
    _NSTDIS=0;
    velocity=3.141592 *2* ((currentPosition-oldPosition) * timer_time * 1000.0) / (33*4*16);

    oldPosition=currentPosition;
    return velocity;
}



/** Mazerunner Functions, some based on the functions provided by bauma above **/

// Get current Position in Counts since initialization (Provided by the Encoders for the left and right motor)
float getPositionInCounts_Right()
{
    long currentEncoderPosition;
    //disable interrupts to make sure we have consistent data
    _NSTDIS=1;
    GET_ENCODER_1 (currentEncoderPosition);
    _NSTDIS=0;
    return (float) currentEncoderPosition; 
}


float getPositionInCounts_Left()
{
    long currentEncoderPosition;
    //disable interrupts to make sure we have consistent data
    _NSTDIS=1;
    GET_ENCODER_2 (currentEncoderPosition);
    _NSTDIS=0;
    return (float) currentEncoderPosition; 
}


// Get 
float getVelocityInRadPerSecond_Right()
{

    static long oldPosition;
    float velocity;
    long currentPosition;

    //disable interrupts to make sure we have consistent data
    _NSTDIS=1;
    GET_ENCODER_1 (currentPosition);
    _NSTDIS=0;
    velocity=3.141592 *2* ((currentPosition-oldPosition) * timer_time * 1000.0) / (33*4*16);

    oldPosition=currentPosition;
    return velocity;

}


float getVelocityInRadPerSecond_Left()
{

    static long oldPosition;
    float velocity;
    long currentPosition;

    //disable interrupts to make sure we have consistent data
    _NSTDIS=1;
    GET_ENCODER_2 (currentPosition);
    _NSTDIS=0;
    velocity=3.141592 *2* ((currentPosition-oldPosition) * timer_time * 1000.0) / (33*4*16);

    oldPosition=currentPosition;
    return velocity;

}

float getVelocityInRoundsPerSecond_Left() {
    // Calc: 1/(2*3.141592) = 0.159154
   return 0.159154 * getVelocityInRadPerSecond_Left();
}

float getVelocityInRoundsPerSecond_Right() {
    // Calc: 1/(2*3.141592) = 0.159154
   return 0.159154 * getVelocityInRadPerSecond_Right();
}

struct Velocities getVelocitiesInRoundsPerSecond() {
    struct Velocities velocitiesInRoundsPerSecond;
    velocitiesInRoundsPerSecond.vel_left = getVelocityInRoundsPerSecond_Left();
    velocitiesInRoundsPerSecond.vel_right = getVelocityInRoundsPerSecond_Right();
    return velocitiesInRoundsPerSecond;
}

float getVelocityInRoundsPerMinutes_Left() {
   return 60 * getVelocityInRoundsPerSecond_Left();
}

float getVelocityInRoundsPerMinutes_Right() {
   return 60 * getVelocityInRoundsPerSecond_Right();
}





// float getVelocityInRoundsPerMinute_Right() {
//     static long oldPosition;
//     float velocity;
//     long currentPosition;

//     //disable interrupts to make sure we have consistent data
//     _NSTDIS=1;
//     GET_ENCODER_2 (currentPosition);
//     _NSTDIS=0;
//     float nbr_of_rounds_per_second = (currentPosition-oldPosition) / (33.0*4.0*16.0) * 1000.0;
//     velocity=60.0 * nbr_of_rounds_per_second;

//     oldPosition=currentPosition;
//     return velocity;
// }

float getFlanksPerSecond_Right() {
    static long oldFLankCount;
    float flanksPerSecond;
    long currentFLankCount;

    //disable interrupts to make sure we have consistent data
    _NSTDIS=1;
    GET_ENCODER_1 (currentFLankCount);
    _NSTDIS=0;

    flanksPerSecond = (currentFLankCount-oldFLankCount) * timer_time * 1000; // timer_time * 1000 = timer time in seconds

    oldFLankCount=currentFLankCount;
    return flanksPerSecond;
}

// !!! returns flanks per second
// float getRPM_Right() {
//     static long oldFLankCount;
//     float flanksPerSecond;
//     long currentFLankCount;

//     //disable interrupts to make sure we have consistent data
//     _NSTDIS=1;
//     GET_ENCODER_1 (currentFLankCount);
//     _NSTDIS=0;

//     flanksPerSecond = (currentFLankCount-oldFLankCount) * timer_time * 1000; // timer_time * 1000 = timer time in seconds

//     oldFLankCount=currentFLankCount;
//     return flanksPerSecond;
// }


// float getVelocityInDegreePerSecond_Left()
// {
//     static long oldPosition;
//     float velocity;
//     long currentPosition;

//         //disable interrupts to make sure we have consistent data
//     _NSTDIS=1;
//     GET_ENCODER_1 (currentPosition);
//     _NSTDIS=0;
//     velocity=360 *2* ((currentPosition-oldPosition)*0.01) / (33*4*16);
    
//     oldPosition=currentPosition;
//     return velocity;
// }


// Works
// float getTotalDrivenDistanceInMeters()
// {
//     static long oldPosition;
//     static float distance = 0;
//     // float distance;
//     long currentPosition;

//     float wheel_diameter = 0.06;
//     float wheel_cirumference = wheel_diameter * 3.141592;

//         //disable interrupts to make sure we have consistent data
//     _NSTDIS=1;
//     GET_ENCODER_1 (currentPosition);
//     _NSTDIS=0;
//     // distance += (currentPosition-oldPosition) / (33*4*16) * wheel_cirumference;
//     distance += (float)(currentPosition-oldPosition) / (33*4*16) * wheel_cirumference;
//     // distance = (float)(currentPosition-oldPosition) / (33*4*16) * wheel_cirumference;
//     // distance = (float)(currentPosition-oldPosition);
//     // distance = (float)(currentPosition);

//     oldPosition=currentPosition;
//     return distance;
// }

/*
* This function returns the total driven distance in meters since the last reset/init of the starting position
* The starting position is the position where the robot was when the function was called with the init_starting_position parameter set to true
*/
float getTotalDrivenDistanceInMeters(bool init_starting_position)
{
    static float oldPosition = 0;
    static float distance = 0;
    // float distance;
    float currentPosition;

    currentPosition = getPositionInCounts_Left();

    if (init_starting_position) {
        oldPosition = currentPosition;
        distance = 0;

        // This return will end the function here 
        // and not execute the rest of the code
        return 1;
    }

    // float wheel_diameter = 0.06;
    // float wheel_cirumference = wheel_diameter * 3.141592;

    // distance += (currentPosition-oldPosition) / (33*4*16) * wheel_cirumference;
    // WORKS: // distance += (float)(currentPosition-oldPosition) / (33*4*16) * wheel_cirumference;
    distance += convertCountsToDistanceInMeters(currentPosition-oldPosition);
    // distance = (float)(currentPosition-oldPosition) / (33*4*16) * wheel_cirumference;
    // distance = (float)(currentPosition-oldPosition);
    // distance = (float)(currentPosition);

    oldPosition=currentPosition;
    return distance;
}

// Each invocation of the getDrivenDistanceInMeters function influences the next invocation of this function
// Therefore if the getDrivenDistanceInMeters function gets called from different independent functions the results of this function will be wrong
// TODO: Each function than needs to have its own getDrivenDistanceInMeters (or find another solution like a function specific key variable that is passed to the getDrivenDistanceInMeters function)
// Returns the distance since last invocation of this function
// Works
// float getDrivenDistanceInMeters()
// {
//     static long oldPosition;
//     float distance;
//     // float distance;
//     long currentPosition;

//     float wheel_diameter = 0.06;
//     float wheel_cirumference = wheel_diameter * 3.141592;

//         //disable interrupts to make sure we have consistent data
//     _NSTDIS=1;
//     GET_ENCODER_1 (currentPosition);
//     _NSTDIS=0;
//     // distance += (currentPosition-oldPosition) / (33*4*16) * wheel_cirumference;
//     distance = (float)(currentPosition-oldPosition) / (33*4*16) * wheel_cirumference;
//     // distance = (float)(currentPosition-oldPosition) / (33*4*16) * wheel_cirumference;
//     // distance = (float)(currentPosition-oldPosition);
//     // distance = (float)(currentPosition);

//     oldPosition=currentPosition;
//     return distance;
// }

// float getDrivenDistanceInMetersSinceLastInvocation()
float getDrivenDistanceInMeters()
{
    static float oldPosition;
    float currentPosition;
    float distance;

    currentPosition = getPositionInCounts_Right();
    distance = convertCountsToDistanceInMeters(currentPosition-oldPosition);

    oldPosition=currentPosition;
    return distance;
}

float convertCountsToDistanceInMeters(float counts)
{
    float distance;

    float wheel_diameter = 0.06;
    float wheel_cirumference = wheel_diameter * 3.141592;

    distance = (float)(counts) / (33*4*16) * wheel_cirumference;

    return distance;
}


// Each invocation of the getDrivenDistanceInMeters function influences the next invocation of this function
// Therefore if the getDrivenDistanceInMeters function gets called from different independent functions the results of this function will be wrong
// Each function than needs to have its own getDrivenDistanceInMeters (or find another solution like a function specific key variable that is passed to the getDrivenDistanceInMeters function)
float getDrivenDistanceInMeters2()
{
    static long oldPosition;
    float distance;
    // float distance;
    long currentPosition;

    //disable interrupts to make sure we have consistent data
    currentPosition = getPositionInCounts_Right();
    // distance += (currentPosition-oldPosition) / (33*4*16) * wheel_cirumference;
    distance = convertCountsToDistanceInMeters(currentPosition-oldPosition);
    // distance = (float)(currentPosition-oldPosition) / (33*4*16) * wheel_cirumference;
    // distance = (float)(currentPosition-oldPosition);
    // distance = (float)(currentPosition);

    oldPosition=currentPosition;
    return distance;
}




// TODO: currently the traveled distance is only meassured on the right (most recently only on the left) wheel !!!
// and not a mean between the two wheels
// float getDistanceToGoalInMeters(float initial_distance_to_goal){

//     float wheel_diameter = 0.06;
//     float wheel_cirumference = wheel_diameter * 3.141592;

//     static float distance_driven = 0;
//     static float old_position = 0;
//     float current_position = getPositionInCounts_1();
//     float distance_driven_in_counts = current_position - old_position;
//     float distance_driven_in_meters = distance_driven_in_counts / (33.0*4.0*16.0) * wheel_cirumference;
//     distance_driven += distance_driven_in_meters;
//     old_position = current_position;

//     return initial_distance_to_goal - distance_driven;
// }

// Simplified version from above
// float getDistanceToGoalInMeters(float initial_distance_to_goal){

//     static float distance_driven = 0;
//     float distance_driven_in_meters = getDrivenDistanceInMeters();
//     distance_driven += distance_driven_in_meters;

//     return initial_distance_to_goal - distance_driven;
// }

// Working
// float getDistanceToGoalInMeters(float initial_distance_to_goal, bool init_starting_position){

//     static float oldPosition;
//     static float distance_driven;
//     float currentPosition;

//     currentPosition = getPositionInCounts_Left();

//     if (init_starting_position) {
//         oldPosition = currentPosition;
//         distance_driven = 0;

//         // This return will end the function here
//         // and not execute the rest of the code
//         return initial_distance_to_goal;
//     }

//     // currentPosition = getPositionInCounts_Right();
//     distance_driven += convertCountsToDistanceInMeters(currentPosition-oldPosition);

//     oldPosition=currentPosition;

//     return initial_distance_to_goal - distance_driven;
// }

// Get the angle difference between the current robot angle and the goal angle
// TODO: Only the left wheel is used to calculate the angle driven !!!!
// TODO: angle_driven will not be reseted after the goal is reached !!!!
// float getAngleToGoalInDegrees(float initial_angle_to_goal) {
    
//     float wheel_diameter = 0.06;
//     float wheel_cirumference = wheel_diameter * 3.141592;

//     static float angle_driven = 0;
//     static float old_position = 0;

//     // Calculate distance driven
//     float current_position = getPositionInCounts_2();
//     float distance_driven_in_counts = current_position - old_position;
//     float distance_driven_in_meters = distance_driven_in_counts / (33.0*4.0*16.0) * wheel_cirumference;
    
//     // Calculate angle driven from distance of right wheel
//     // a  = d * 360 / (2 * pi * r) with a = angle_driven_in_degrees and r in meters
//     float angle_driven_in_degrees = distance_driven_in_meters * 3000.0 / 3.141592;
//     angle_driven += angle_driven_in_degrees;    
//     old_position = current_position;

//     return initial_angle_to_goal - angle_driven;

//     // float current_angle = getPositionInRad();
//     // float angle_difference = initial_angle_to_goal - current_angle;
//     // return angle_difference * 180.0 / 3.141592;
// }

/* Working
* This function returns the angle difference between the current robot angle and the goal angle.
* The static variables are reseted when the init_starting_position parameter is set to true
* This should be done before each motion primitive is started, like turning for 90 degrees left
*/
float getAngleToGoalInDegrees(float initial_angle_to_goal, bool init_starting_position) {

    static float angle_driven = 0;
    static float old_position = 0;
    float distance_driven_in_meters;

    // Calculate distance driven
    float current_position;

    // Read distance from the Left wheel to get a positive distance when turning right (clockwise)
    current_position = getPositionInCounts_Left();

    if (init_starting_position) {
        old_position = current_position;
        angle_driven = 0;

        // This return will end the function here
        // and not execute the rest of the code
        return initial_angle_to_goal;
    }

    // } else if (reset_static_variables) {
    //     old_position = current_position;
    //     angle_driven = 0;

    //     // This return will end the function here
    //     // and not execute the rest of the code
    //     return 1;
    // }
    
    distance_driven_in_meters = convertCountsToDistanceInMeters(current_position-old_position);
    
    // Calculate angle driven from distance of right wheel
    angle_driven += calculateAngleInDegreesFromArcLengthInMetersAndTurnRadius(distance_driven_in_meters,TURN_RADIUS);    
    old_position = current_position;

    return initial_angle_to_goal - angle_driven;

    // float current_angle = getPositionInRad();
    // float angle_difference = initial_angle_to_goal - current_angle;
    // return angle_difference * 180.0 / 3.141592;
}



// float calculateAngleInDegreesFromArcLengthInMetersAndTurnRadius(float arc_length_in_meters, float turn_radius_in_meters) {
//     // a  = d * 360 / (2 * pi * r) with a = angle_driven_in_degrees and r in meters
//     float angle_in_degrees = arc_length_in_meters * 360.0 / (2 * 3.141592 *turn_radius_in_meters);
//     return angle_in_degrees;
// }

float calculateAngleInDegreesFromArcLengthInMetersAndTurnRadius(float arc_length_in_meters, float turn_radius_in_meters) {
    // a  = d * 360 / (2 * pi * r) with a = angle_driven_in_degrees and r in meters
    float angle_in_degrees = arc_length_in_meters * 360.0 / (2 * 3.141592 * turn_radius_in_meters);
    return angle_in_degrees;
}


// float getTotalDrivenAngleInDegrees() {
    
//     float wheel_diameter = 0.06;
//     float wheel_cirumference = wheel_diameter * 3.141592;

//     static float angle_driven = 0;
//     static float old_position = 0;

//     // Calculate distance driven
//     float current_position = getPositionInCounts_2();
//     float distance_driven_in_counts = (float) current_position - old_position;
//     float distance_driven_in_meters = distance_driven_in_counts / (33.0*4.0*16.0) * wheel_cirumference;
    
//     // Calculate angle driven from distance of right wheel
//     // a  = d * 360 / (2 * pi * r) with a = angle_driven_in_degrees and r in meters
//     float angle_driven_in_degrees = distance_driven_in_meters * 3000.0 / 3.141592;
//     angle_driven += angle_driven_in_degrees;    
//     old_position = current_position;

//     // return angle_driven;
//     return distance_driven_in_meters;
// }

/*
*  This function returns the total angle driven since the last reset/init of the static variables.
*  The static variables are reseted when the init_starting_position parameter is set to true (similar to the init_starting_position parameter in the getAngleToGoalInDegrees function)
*/
float getTotalDrivenAngleInDegrees(bool init_starting_position) {

    static float angle_driven = 0;
    static float old_position = 0;
    float distance_driven_in_meters;

    float current_position;
    current_position = getPositionInCounts_Left();

    if (init_starting_position) {
        old_position = current_position;
        angle_driven = 0;

        // This return will end the function here
        // and not execute the rest of the code
        return 1;
    }

    distance_driven_in_meters = convertCountsToDistanceInMeters(current_position-old_position);
    
    // Calculate angle driven from distance of right wheel
    float angle_driven_in_degrees = calculateAngleInDegreesFromArcLengthInMetersAndTurnRadius(distance_driven_in_meters, TURN_RADIUS);
    angle_driven += angle_driven_in_degrees;    

    old_position = current_position;
    return angle_driven;
}


/*
    // Calc: 1/(2*3.141592) = 0.159154
    float distance_driven_left = 0.159154 * getVelocityInRadPerSecond_Left() * timer_time * 1000.0;
    float distance_driven_right = 0.159154 * getVelocityInRadPerSecond_Right() * timer_time * 1000.0;

    distance_driven += (distance_driven_left + distance_driven_right) / 2.0;
*/




// NEW

long getPositionInCountsLong_Left() {
    return getPositionInCounts_2();
}
long getPositionInCountsLong_Right() {
    return getPositionInCounts_1();
}

long convertDistanceInMetersToCounts(float distance_to_goal_in_meters) {
    long counts;

    // float wheel_diameter = 0.06;
    // float wheel_cirumference = wheel_diameter * 3.141592;

    // TODO: check if the sign is correct after converting to long
    // counts = (long) ( distance_to_goal_in_meters / wheel_cirumference * ENCODER_COUNTS_PER_REVOLUTION );
    counts = (long) ( distance_to_goal_in_meters / WHEEL_CIRCUMFERENCE * ENCODER_COUNTS_PER_REVOLUTION );

    return counts;
}

struct EncoderCounts getGoalPositionInEncoderCounts(float initial_distance_to_goal_in_meters) {
    struct EncoderCounts goal_position_in_encoder_counts;

    long initial_distance_to_goal_in_counts = convertDistanceInMetersToCounts(initial_distance_to_goal_in_meters);
    long current_position_in_counts = getPositionInCountsLong_Left();
    long goal_position_in_counts = current_position_in_counts + initial_distance_to_goal_in_counts;
    
    goal_position_in_encoder_counts.left = goal_position_in_counts;
    goal_position_in_encoder_counts.right = goal_position_in_counts;

    return goal_position_in_encoder_counts;
}

// TODO: It can only register a goal reached if it is called often enough to detect the goal reached state, 
// or if the robot staies in the goal reached state for a long enough time
bool isDistanceGoalReached(float distance_to_goal) {
    return fabs(distance_to_goal) <= GOAL_REACHED_THRESHOLD_DISTANCE;
}

// TODO: It can only register a goal reached if it is called often enough to detect the goal reached state, 
// or if the robot staies in the goal reached state for a long enough time
bool isAngleGoalReached(float angle_to_goal) {
    return fabs(angle_to_goal) <= GOAL_REACHED_THRESHOLD_ANGLE;
}

float getDistanceToGoalInMeters(){

    long current_position = getPositionInCountsLong_Left();
    // TODO: Check if the sign is correct
    long distance_to_goal_in_counts = currMovementControlParameters.goalPositionInEncoderCounts.left - current_position;

    return convertCountsToDistanceInMeters(distance_to_goal_in_counts);
}

bool isMovementGoalReached() {
    switch (currMovementControlParameters.movementPrimitive.type)
    {
    case DRIVING_STRAIGHT: {
        // Currently check only the left encoder for completion as currently everything is based on the left wheel
        float distance_to_goal_in_meters = getDistanceToGoalInMeters(); 
        return isDistanceGoalReached(distance_to_goal_in_meters);
        break;
    }
    case TURNING: {
        // Currently check only the left encoder for completion as currently everything is based on the left wheel
        float distance_to_goal_in_meters = getDistanceToGoalInMeters();
        // TODO: Replace by new getAngleToGoalInDegrees function
        float angle_to_goal_in_degrees = calculateAngleInDegreesFromArcLengthInMetersAndTurnRadius(distance_to_goal_in_meters, TURN_RADIUS);
        return isAngleGoalReached(angle_to_goal_in_degrees);
        break;
    }
    default:
        break;
    }
}


float calculateArcLengthInMeters(float angle_to_turn_in_degrees) {
    // a  = d * 360 / (2 * pi * r) with a = angle_driven_in_degrees and r in meters, d = arc_length_in_meters
    float arc_length_in_meters = angle_to_turn_in_degrees * (2 * 3.141592 * TURN_RADIUS) / 360.0;
    return arc_length_in_meters;
}

// float getAngleToGoalInDegrees(float initial_angle_to_goal, bool init_starting_position) {

// }

// ENDNEW