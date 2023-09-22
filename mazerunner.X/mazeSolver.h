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
#define STACK_SIZE 36

// *** Start of Existing Functions ***
// define values for each cell
enum CellValue
{
    UNKNOWN,
    WALL,
    WAY,
    EXPLORED
};

// Define a structure to hold the four values
struct CellData
{
    enum CellValue north;
    enum CellValue east;
    enum CellValue south;
    enum CellValue west;
    enum CellValue center;
};

int checkGoal(int x, int y);
int checkStart(int x, int y);

void updateWalls(int x, int y, int orientation, struct CellData walls[MAZE_SIZE][MAZE_SIZE]);


typedef struct
{
    int x, y;
} Point;

// Stack data structure
typedef struct
{
    Point *items;
    int top;
    int capacity;
} Stack;

// Initialize a stack with a given capacity
Stack *createStack(int capacity);

void push(Stack *stack, Point item);
Point pop(Stack *stack);
void swapStacks(Stack **stack1, Stack **stack2);
void floodFill(Stack *currentLevel, Stack *nextLevel, int distance[MAZE_SIZE][MAZE_SIZE], struct CellData walls[MAZE_SIZE][MAZE_SIZE], int goal);
void final_floodFill(Stack *currentLevel, Stack *nextLevel, int distance[MAZE_SIZE][MAZE_SIZE], struct CellData walls[MAZE_SIZE][MAZE_SIZE]);
int findlowestDistance(int arr[]);
int turn_to_lowest_distance(int lowestNeighbour, int orientation);
void openNeighbours(int x, int y, struct CellData walls[MAZE_SIZE][MAZE_SIZE], int distance_Open_Neighbours[4], int distance[MAZE_SIZE][MAZE_SIZE]);
// *** End of Existing Functions ***

// TODO: Add the remaining function prototypes. Necessary if functions are not used outside ?

void initMazeSolver(Stack *currentLevel, Stack *nextLevel);
// void explorationToCenter();
// void explorationToStart();
// void finalRun();

void mazeSolver(Stack *currentLevel, Stack *nextLevel);

void printString2UARTmalloc(const char *str); // To use malloc: the heap size has to be set in the project settings
void printString2UARTmax60(const char *str); // Does work
void printString2UARTmax10(const char *str); // Does work

const char *getOrientationString(int orientation);

// State Machine:
// TODO: The first three states could be combined into one state, however this would make the code less readable ? 
// and some lines are not executed in all three phases, and every phase has a different goal condition to check for
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
    enum MazeSolverStates curr_phase;
    bool just_switched_state;
    bool just_startet_execution;
    bool already_aligned_with_lowest_distance;
};

static struct MazeSolverState maze_solver_state;



// // From initMazeSolver()
// // TODO: do they have to be declared as extern variables to be visible in the scope of the different states of the state machine ?
// // Or in one global struct ?
// struct CellData walls[MAZE_SIZE][MAZE_SIZE];
// int orientation = 16;
// int x = 0;
// int y = 0;
// int distance[MAZE_SIZE][MAZE_SIZE];
// int distance_Open_Neighbours[4];
// int lowestNeighbour = 6;
// Stack *currentLevel = createStack(STACK_SIZE); // current level is needed for the floodfill function
// Stack *nextLevel = createStack(STACK_SIZE);    // next level is needed for the floodfill function

// Stack *currentLevel; // Declare currentLevel as a global variable
// Stack *nextLevel;    // Declare nextLevel as a global variable

// DEBUGGING
// // Declare global variables here
// extern struct CellData walls[MAZE_SIZE][MAZE_SIZE];
// extern int orientation;
// extern int x;
// extern int y;
// extern int distance[MAZE_SIZE][MAZE_SIZE];
// extern int distance_Open_Neighbours[4];
// extern int lowestNeighbour;

// // Declare global pointers to Stack
// extern Stack *currentLevel;
// extern Stack *nextLevel;
// DEBUGGING



#endif	/* MAZESOLVER_H */