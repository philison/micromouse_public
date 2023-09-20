/* 
 * File:   controllers.h
 * Author: Noah & Phil / Algorithm: Noah
 *
 * Created on June 27, 2023, 5:59 PM
 */


#include <stdbool.h> // for bool type
#include <stdio.h> // for the print function
#include <stdlib.h> // for malloc for the print function

#include "distanceSensors.h"
#include "newTypes.h"


#ifndef MAZESOLVER_H
#define	MAZESOLVER_H

#define MAZE_SIZE 6
#define STACK_SIZE 256



int checkGoal(int x, int y);
int checkStart(int x, int y);

void updateWalls(int x, int y, int orientation, struct CellData walls[MAZE_SIZE][MAZE_SIZE]);

// TODO: Add the remaining function prototypes. Necessary if functions are not used outside ?

void initMazeSolver();
void explorationToCenter();
void explorationToStart();
void finalRun();

void mazeSolver();

void printString2UART(const char *str); // TODO: Untested function

// State Machine:
enum MazeSolverStates {
    EXPLORATION_TO_CENTER,
    EXPLORATION_TO_START,
    FINAL_RUN,
    TURN_TO_LOWEST_DISTANCE,
    MOVE_FORWARD,
};

struct MazeSolverState {
    enum MazeSolverStates curr_state;
    enum MazeSolverStates prev_state;
    bool just_switched_state;
    bool just_startet_execution;
}

struct MazeSolverState maze_solver_state;


// Needed flags:
// just_startet_execution

// States:
// EXPLORATION_TO_CENTER
// EXPLORATION_TO_START
// FINAL_RUN

// Caution;
// Only when standing at the cell center should the following functions be executed:
// checkGoal()  // to ensure that the robot does not end its run prematurely while driving to the center, however can be avoided if we only update the robot position after it has executed the movement function
// checkStart() 
// updateWalls()


// From initMazeSolver()
struct CellData walls[MAZE_SIZE][MAZE_SIZE];
int orientation = 16;
int x = 0;
int y = 0;
int distance[MAZE_SIZE][MAZE_SIZE];
int distance_Open_Neighbours[4];
int lowestNeighbour = 6;
Stack *currentLevel = createStack(STACK_SIZE); // current level is needed for the floodfill function
Stack *nextLevel = createStack(STACK_SIZE);    // next level is needed for the floodfill function

#endif	/* MAZESOLVER_H */