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
    return (float) SENSOR_LEFT / 4096.0 * 100.0;
}

float distanceSensorInPercentFront(void)
{
    return (float) SENSOR_FRONT / 4096.0 * 100.0;
}

float distanceSensorInPercentRight(void)
{
    return (float) SENSOR_RIGHT / 4096.0 * 100.0;
}


bool isWallLeft(void)
{
    return distanceSensorInPercentLeft() > 12.0;
}

bool isWallFront(void)
{
    return distanceSensorInPercentFront() > 12.0;
}

bool isWallRight(void)
{
    return distanceSensorInPercentRight() > 12.0;
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