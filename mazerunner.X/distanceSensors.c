#include "distanceSensors.h"
#include "dma.h" // for SENSOR_LEFT, SENSOR_FRONT, SENSOR_RIGHT
// #include <stdbool.h> // for bool type


// float distanceSensorLeft(void)
// {
//     return SENSOR_LEFT;
// }

// float distanceSensorFront(void)
// {
//     return SENSOR_FRONT;
// }

// float distanceSensorRight(void)
// {
//     return SENSOR_RIGHT;
// }

float distanceSensorInPercentLeft(void)
{
    return (float) SENSOR_LEFT / 4096.0 * 100.0; // 12 bit ADC therfore 2^12 = 4096 
}

float distanceSensorInPercentFront(void)
{
    return (float) SENSOR_FRONT / 4096.0 * 100.0; // 12 bit ADC therfore 2^12 = 4096 
}

float distanceSensorInPercentRight(void)
{
    return (float) SENSOR_RIGHT / 4096.0 * 100.0; // 12 bit ADC therfore 2^12 = 4096 
}


struct DistanceSensorsReadingsInPercent distanceSensorsReadingsInPercent(void)
{
    struct DistanceSensorsReadingsInPercent currentDistanceSensorsReadingsInPercent;
    currentDistanceSensorsReadingsInPercent.left = distanceSensorInPercentLeft();
    currentDistanceSensorsReadingsInPercent.front = distanceSensorInPercentFront();
    currentDistanceSensorsReadingsInPercent.right = distanceSensorInPercentRight();
    return currentDistanceSensorsReadingsInPercent;
}


bool isWallLeft(void)
{
    return distanceSensorInPercentLeft() > WALL_DETECTION_THRESHOLD;
}

bool isWallFront(void)
{
    return distanceSensorInPercentFront() > WALL_DETECTION_THRESHOLD;
}

bool isWallRight(void)
{
    return distanceSensorInPercentRight() > WALL_DETECTION_THRESHOLD;
}


enum lateralControlMode getLateralControlMode(void)
{
    if (isWallLeft() && isWallRight())
    {
        return TWO_WALL_CENTERING;
    }
    else if (isWallLeft() && !isWallRight())
    {
        return ONE_WALL_FOLLOWING_LEFT;
    }
    else if (!isWallLeft() && isWallRight())
    {
        return ONE_WALL_FOLLOWING_RIGHT;
    }
    else
    {
        return ZERO_WALL_DRIVING;
    }
}

// enum lateralControlMode currentLateralControlMode;
// currentLateralControlMode = TWO_WALL_CENTERING;
// enum lateralControlMode getLateralControlMode(void)
// {
//     if (isWallLeft() && isWallRight())
//     {
//         currentLateralControlMode = TWO_WALL_CENTERING;
//     }
//     else if (isWallLeft() && !isWallRight())
//     {
//         currentLateralControlMode = ONE_WALL_FOLLOWING_LEFT;
//     }
//     else if (!isWallLeft() && isWallRight())
//     {
//         currentLateralControlMode = ONE_WALL_FOLLOWING_RIGHT;
//     }
//     else
//     {
//         currentLateralControlMode = ZERO_WALL_DRIVING;
//     }

//     return currentLateralControlMode;
// }


const char* getLateralControlModeName(enum lateralControlMode lateralControlMode) 
{
   switch (lateralControlMode) 
   {
    case TWO_WALL_CENTERING: return "TWO_WALL_CENTERING";
    case ONE_WALL_FOLLOWING_RIGHT: return "ONE_WALL_FOLLOWING_RIGHT";
    case ONE_WALL_FOLLOWING_LEFT: return "ONE_WALL_FOLLOWING_LEFT";
    case ZERO_WALL_DRIVING: return "ZERO_WALL_DRIVING";
    default: return "Unknown Control Mode";
   }
}