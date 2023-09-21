#include "mazeSolver.h"


// turns false if the goal is reached
int checkGoal(int x, int y)
{
    if ((x == 2 || x == 3) && (y == 2 || y == 3))
    {
        // UART
        char buffer[40];
        sprintf(buffer, "The goal has been reached");
        putsUART1(buffer);
        return 0;
    }
    return 1;
}

// turns false if the start is reached
int checkStart(int x, int y)
{
    if (x == 0 && y == 0)
    {
        return 0;
    }
    return 1;
}

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

// Function that sets walls for the x / y position and the connecting walls of the neighbouring cells
void updateWalls(int x, int y, int orientation, struct CellData walls[MAZE_SIZE][MAZE_SIZE])
{
    walls[x][y].center = EXPLORED;

    switch (orientation % 4)
    {
    case 0:
        if (isWallLeft())
        {
            walls[x][y].west = WALL;
            if (x > 0)
            {
                walls[x - 1][y].east = WALL;
            }
        }
        if (!isWallLeft())
        {
            walls[x][y].west = WAY;
            if (x > 0)
            {
                walls[x - 1][y].east = WAY;
            }
        }

        if (isWallFront())
        {
            walls[x][y].north = WALL;
            if (y < 15)
            {
                walls[x][y + 1].south = WALL;
            }
        }
        if (!isWallFront())
        {
            walls[x][y].north = WAY;
            if (y < 15)
            {
                walls[x][y + 1].south = WAY;
            }
        }

        if (isWallRight())
        {
            walls[x][y].east = WALL;
            if (x < 15)
            {
                walls[x + 1][y].west = WALL;
            }
        }
        if (!isWallRight())
        {
            walls[x][y].east = WAY;
            if (x < 15)
            {
                walls[x + 1][y].west = WAY;
            }
        }

        break;
    case 1:
        if (isWallLeft())
        {
            walls[x][y].north = WALL;
            if (y < 15)
            {
                walls[x][y + 1].south = WALL;
            }
        }
        if (!isWallLeft())
        {
            walls[x][y].north = WAY;
            if (y < 15)
            {
                walls[x][y + 1].south = WAY;
            }
        }

        if (isWallFront())
        {
            walls[x][y].east = WALL;
            if (x < 15)
            {
                walls[x + 1][y].west = WALL;
            }
        }
        if (!isWallFront())
        {
            walls[x][y].east = WAY;
            if (x < 15)
            {
                walls[x + 1][y].west = WAY;
            }
        }

        if (isWallRight())
        {
            walls[x][y].south = WALL;
            if (y > 0)
            {
                walls[x][y - 1].north = WALL;
            }
        }
        if (!isWallRight())
        {
            walls[x][y].south = WAY;
            if (y > 0)
            {
                walls[x][y - 1].north = WAY;
            }
        }

        break;
    case 2:
        if (isWallLeft())
        {
            walls[x][y].east = WALL;
            if (x < 15)
            {
                walls[x + 1][y].west = WALL;
            }
        }
        if (!isWallLeft())
        {
            walls[x][y].east = WAY;
            if (x < 15)
            {
                walls[x + 1][y].west = WAY;
            }
        }

        if (isWallFront())
        {
            walls[x][y].south = WALL;
            if (y > 0)
            {
                walls[x][y - 1].north = WALL;
            }
        }
        if (!isWallFront())
        {
            walls[x][y].south = WAY;
            if (y > 0)
            {
                walls[x][y - 1].north = WAY;
            }
        }

        if (isWallRight())
        {
            walls[x][y].west = WALL;
            if (x > 0)
            {
                walls[x - 1][y].east = WALL;
            }
        }
        if (!isWallRight())
        {
            walls[x][y].west = WAY;
            if (x > 0)
            {
                walls[x - 1][y].east = WAY;
            }
        }

        break;
    case 3:
        if (isWallLeft())
        {
            walls[x][y].south = WALL;
            if (y > 0)
            {
                walls[x][y - 1].north = WALL;
            }
        }
        if (!isWallLeft())
        {
            walls[x][y].south = WAY;
            if (y > 0)
            {
                walls[x][y - 1].north = WAY;
            }
        }

        if (isWallFront())
        {
            walls[x][y].west = WALL;
            if (x > 0)
            {
                walls[x - 1][y].east = WALL;
            }
        }
        if (!isWallFront())
        {
            walls[x][y].west = WAY;
            if (x > 0)
            {
                walls[x - 1][y].east = WAY;
            }
        }
        if (isWallRight())
        {
            walls[x][y].north = WALL;
            if (y < 15)
            {
                walls[x][y + 1].south = WALL;
            }
        }
        if (!isWallRight())
        {
            walls[x][y].north = WAY;
            if (y < 15)
            {
                walls[x][y + 1].south = WAY;
            }
        }

        break;
    default: {
        // UART
        char buffer[40];
        sprintf(buffer, "walls were not set correctly");
        putsUART1(buffer);
        break;
    }
    }
    return;
}


/************************************
 *
 *   Non_recursive Implementation
 *      Floodfill distance update
 *
 *************************************/
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
Stack *createStack(int capacity)
{
    Stack *stack = (Stack *)malloc(sizeof(Stack));
    stack->capacity = capacity;
    stack->top = -1;
    stack->items = (Point *)malloc(sizeof(Point) * capacity);
    return stack;
}

// Push an element onto the stack
void push(Stack *stack, Point item)
{
    if (stack->top == stack->capacity - 1)
    {
        // Stack is full, handle error or resize
        return;
    }
    stack->items[++stack->top] = item;
}

// Pop an element from the stack
Point pop(Stack *stack)
{
    if (stack->top == -1)
    {
        // Stack is empty, handle error
        Point p = {-1, -1}; // Return an invalid point to indicate an error
        return p;
    }
    return stack->items[stack->top--];
}

// Swap the contents of two stacks
void swapStacks(Stack **stack1, Stack **stack2)
{
    Stack *temp = *stack1;
    *stack1 = *stack2;
    *stack2 = temp;
}



/*
 * Floodfill Function: Updates the distance array with the distance to the goal. For exploration, all unknown sides are assumed to be open.
 * This should give a distance for each cell which equal or lower than the optimal path and the mouse should find the optimal way.
 * Testing on the 16 x 16 maze in the simulator showed, that the mouse does not always find the optimal way. Multiple runs to further
 * explore would eventually lead to the optimal path.
 * FINAL_floodfill is used once for the optimal path. Its only difference to the normal floodfill function is that all unknown sides are
 * assumed to be walls. This should lead to a distance array with the optimal path at the current exploration phase
 */

void floodFill(Stack *currentLevel, Stack *nextLevel, int distance[MAZE_SIZE][MAZE_SIZE], struct CellData walls[MAZE_SIZE][MAZE_SIZE], int goal)
{
    int visited[MAZE_SIZE][MAZE_SIZE];
    for (int row = 0; row < MAZE_SIZE; row++)
    {
        for (int col = 0; col < MAZE_SIZE; col++)
        {
            distance[row][col] = 0;
            visited[row][col] = 0;
        }
    }

    // set the center cells as goal for walking towards the center
    if (goal == 0 || goal == 5)
    {
        push(currentLevel, (Point){2, 2});
        push(currentLevel, (Point){2, 3});
        push(currentLevel, (Point){3, 3});
        push(currentLevel, (Point){3, 2});
    }

    // set the start cell as goal to walk back out again
    if (goal == 1)
    {
        push(currentLevel, (Point){0, 0});
    }
    
    int newValue = 0;

    for (int i = 0; i < MAZE_SIZE * MAZE_SIZE; i++)
    {
        while (currentLevel->top >= 0)
        {
            // Pop a point from the stack
            Point current = pop(currentLevel);
            int x = current.x;
            int y = current.y;

            // Check if the current cell has been visited or already has the new value
            if (visited[x][y] == 0)
            {

                // Update the current cell with the new value
                distance[x][y] = newValue;
                // newValue++;

                // Mark the current cell as visited
                visited[x][y] = 1;
            }

            else
                continue;

            // Push neighboring cells onto the stack
            if (((walls[x][y].east == UNKNOWN) || (walls[x][y].east == WAY)) && (x + 1 < MAZE_SIZE))
            {
                push(nextLevel, (Point){x + 1, y});
            }
            if (((walls[x][y].west == UNKNOWN) || (walls[x][y].west == WAY)) && (x - 1 >= 0))
            {
                push(nextLevel, (Point){x - 1, y});
            }
            if (((walls[x][y].north == UNKNOWN) || (walls[x][y].north == WAY)) && (y + 1 < MAZE_SIZE))
            {
                push(nextLevel, (Point){x, y + 1});
            }
            if (((walls[x][y].south == UNKNOWN) || (walls[x][y].south == WAY)) && (y - 1 >= 0))
            {
                push(nextLevel, (Point){x, y - 1});
            }
        }
        swapStacks(&nextLevel, &currentLevel);
        newValue++;

        // initialize all unknown cells to maximum distance, so the mouse does not run there during final run.
        // i think this leads to an error, as the distances before are calculated based on the open cells.
        // if a cell wall is unknown, it is asumed, there is none, whcih makes sense in exploration.
        // But for the final run, only cells with known neighbours should be updates.

        // if (goal == 5)
        // {
        //     for (int row = 0; row < MAZE_SIZE; row++)
        //     {
        //         for (int col = 0; col < MAZE_SIZE; col++)
        //         {
        //             if (walls[row][col].center == UNKNOWN)
        //             {
        //                 distance[row][col] = 255;
        //             }
        //         }
        //     }
        // }
    }
}

void final_floodFill(Stack *currentLevel, Stack *nextLevel, int distance[MAZE_SIZE][MAZE_SIZE], struct CellData walls[MAZE_SIZE][MAZE_SIZE])
{
    int visited[MAZE_SIZE][MAZE_SIZE];
    for (int row = 0; row < MAZE_SIZE; row++)
    {
        for (int col = 0; col < MAZE_SIZE; col++)
        {
            distance[row][col] = 0;
            visited[row][col] = 0;
        }
    }

    // set the center cells as goal for walking towards the center
    push(currentLevel, (Point){2, 2});
    push(currentLevel, (Point){2, 3});
    push(currentLevel, (Point){3, 3});
    push(currentLevel, (Point){3, 2});

    int newValue = 0;

    for (int i = 0; i < MAZE_SIZE * MAZE_SIZE; i++)
    {
        while (currentLevel->top >= 0)
        {
            // Pop a point from the stack
            Point current = pop(currentLevel);
            int x = current.x;
            int y = current.y;

            // Check if the current cell has been visited or already has the new value
            if (visited[x][y] == 0)
            {

                // Update the current cell with the new value
                distance[x][y] = newValue;
                // newValue++;

                // Mark the current cell as visited
                visited[x][y] = 1;
            }

            else
                continue;

            // Push neighboring cells onto the stack
            // unknown sides are now treated as walls.
            if (((walls[x][y].east == WAY)) && (x + 1 < MAZE_SIZE))
            {
                push(nextLevel, (Point){x + 1, y});
            }
            if (((walls[x][y].west == WAY)) && (x - 1 >= 0))
            {
                push(nextLevel, (Point){x - 1, y});
            }
            if (((walls[x][y].north == WAY)) && (y + 1 < MAZE_SIZE))
            {
                push(nextLevel, (Point){x, y + 1});
            }
            if (((walls[x][y].south == WAY)) && (y - 1 >= 0))
            {
                push(nextLevel, (Point){x, y - 1});
            }
        }
        swapStacks(&nextLevel, &currentLevel);
        newValue++;
    }
}


// takes in the open neighbour array and return the position of the lowest value. O corresponds to north, 1 to east and so forth clockwise direction.
int findlowestDistance(int arr[])
{
    int size = 4;
    int lowestValue = arr[0]; // Initialize with the first element
    int lowestPosition = 0;   // Initialize with the position of the first element

    for (int i = 1; i < size; i++)
    {
        if (arr[i] < lowestValue)
        {
            lowestValue = arr[i]; // Update the lowest value
            lowestPosition = i;   // Update the position of the lowest value
        }
    }

    return lowestPosition;
}

// print the distance array to the console for verification
// void printDistance_array(int distance[MAZE_SIZE][MAZE_SIZE])
// {
//     for (int row = MAZE_SIZE - 1; row >= 0; row--)
//     {
//         for (int col = 0; col < MAZE_SIZE; col++)
//         {
//             fprintf(stderr, "%3d ", distance[col][row]);
//             fflush(stderr);
//         }
//         printtoconsole("\n");
//     }
//     printtoconsole("\n_______________________\n\r");
// }


// Turns the mouse towards the neighbour with the lowest distance. This should only be one lower than the current cell.
// lowest neighbour is either 0, 1, 2, 3, and marks the cell with the lowest distance. 0 corresponds to north, 1 to east and so forth
int turn_to_lowest_distance(int lowestNeighbour, int orientation)
{

    switch (lowestNeighbour - (orientation % 4))
    {
    case 0:
        // printtoconsole("mouse does not need to turn\n");
        break;
    case 2:
    case -2:
        // printtoconsole("Lowest neighbour behind. mouse needs to turn 180 degrees\n");
        // API_turnRight();
        // orientation = orientation + 1;
        // API_turnRight();
        // orientation = orientation + 1;
        turn180();
        // TODO: as this variable is not defined again in this function as a global variable it directly effects the orientation variable outside of this function
        // Is that a problem ? this lead to the orientation beeing updated before the actual goal position is reached.
        orientation = orientation + 2;
        break;
    case 1:
    case -3:
        // printtoconsole("Lowest neighbour to the right. One right turn\n");
        // API_turnRight();
        turnRight();
        orientation = orientation + 1;
        break;
    case -1:
    case 3:
        // printtoconsole("lowest neighbour is to the left. One left turn\n");
        // API_turnLeft();
        turnLeft();
        orientation = orientation - 1;
        break;
    default: {
        // UART
        char buffer[60];
        sprintf(buffer, "!!!!!!!!!!!!!ERROR!!!!!!!!!!!\n");
        putsUART1(buffer);
        sprintf(buffer, "Could not turn towards neighbour with lowest distance\n");
        putsUART1(buffer);
        break;
    }
    }
    return orientation;
}


// pushes all open neighbours to the distance_Open_Neighbours array
void openNeighbours(int x, int y, struct CellData walls[MAZE_SIZE][MAZE_SIZE], int distance_Open_Neighbours[4], int distance[MAZE_SIZE][MAZE_SIZE])
{
    distance_Open_Neighbours[0] = 255;
    distance_Open_Neighbours[1] = 255;
    distance_Open_Neighbours[2] = 255;
    distance_Open_Neighbours[3] = 255;

    // print initialized distanceOfOpenNeighbours
    // printin with a for loop somehow always lead to an error without a message to resolve it.

    // printtoconsoleEnter("distance_Open_Neighbours initialized with 255");
    // printIntToConsolewithSpace(distance_Open_Neighbours[0]);
    // printIntToConsolewithSpace(distance_Open_Neighbours[1]);
    // printIntToConsolewithSpace(distance_Open_Neighbours[2]);
    // printIntToConsolewithSpace(distance_Open_Neighbours[3]);
    // printtoconsole("\n");

    if (walls[x][y].north == WAY)
    {
        distance_Open_Neighbours[0] = distance[x][y + 1];
        // printtoconsoleEnter("North = way; Push distance!");
        // printtoconsoleEnter("the distance of the point north is:");
        // printIntToConsolewithSpace(distance_Open_Neighbours[0]);
        // printtoconsoleEnter("\n");
    }
    if (walls[x][y].east == WAY)
    {
        distance_Open_Neighbours[1] = distance[x + 1][y];
        // printtoconsoleEnter("East = way; Push distance!");
        // printtoconsoleEnter("the distance of the point east is:");
        // printIntToConsolewithSpace(distance_Open_Neighbours[1]);
        // printtoconsoleEnter("\n");
    }
    if (walls[x][y].south == WAY)
    {
        distance_Open_Neighbours[2] = distance[x][y - 1];
        // printtoconsoleEnter("South = way; Push distance!");
        // printtoconsoleEnter("the distance of the point south is:");
        // printIntToConsolewithSpace(distance_Open_Neighbours[2]);
        // printtoconsoleEnter("\n");
    }
    if (walls[x][y].west == WAY)
    {
        distance_Open_Neighbours[3] = distance[x - 1][y];
        // printtoconsoleEnter("West = way; Push distance!");
        // printtoconsoleEnter("the distance of the point west is:");
        // printIntToConsolewithSpace(distance_Open_Neighbours[3]);
        // printtoconsoleEnter("\n");
    }

    // printtoconsoleEnter("distance_Open_Neighbours after update");
    // printIntToConsolewithSpace(distance_Open_Neighbours[0]);
    // printIntToConsolewithSpace(distance_Open_Neighbours[1]);
    // printIntToConsolewithSpace(distance_Open_Neighbours[2]);
    // printIntToConsolewithSpace(distance_Open_Neighbours[3]);
    // printtoconsole("\n");
}



// From initMazeSolver()
// TODO: do they have to be declared as extern variables to be visible in the scope of the different states of the state machine ?
// Or in one global struct ?
struct CellData walls[MAZE_SIZE][MAZE_SIZE];
int orientation = 16;
int x = 0;
int y = 0;
int distance[MAZE_SIZE][MAZE_SIZE];
int distance_Open_Neighbours[4];
int lowestNeighbour = 6;
Stack *currentLevel = createStack(STACK_SIZE); // current level is needed for the floodfill function
Stack *nextLevel = createStack(STACK_SIZE);    // next level is needed for the floodfill function



/* 
*   Newly added Functions that implement things that before 
*   happened within the main function                           
*/

void initMazeSolver() {

    // initialize all walls to UNKNOWN
    for (int row = 0; row < MAZE_SIZE; row++)
    {
        for (int col = 0; col < MAZE_SIZE; col++)
        {
            walls[row][col].north = UNKNOWN;
            walls[row][col].east = UNKNOWN;
            walls[row][col].south = UNKNOWN;
            walls[row][col].west = UNKNOWN;
            walls[row][col].center = UNKNOWN;
        }
    }
    // initialize all outer cells with a wall on the side that faces outward
    // first indice refers to the row, second to the column. Out of comfort I used the first index as the x-coordinate and the second as the y-coordinate.
    for (int i = 0; i < MAZE_SIZE; i++)
    {
        walls[0][i].west = WALL;
        walls[i][0].south = WALL;
        walls[MAZE_SIZE - 1][i].east = WALL;
        walls[i][MAZE_SIZE - 1].north = WALL;
    }

    // initDistance(distance);
    // TODO: Implement print functions via UART
    // printMaze(walls, distance);
    floodFill(currentLevel, nextLevel, distance, walls, 0);
    // printDistance_array(distance);
}

static struct MazeSolverState maze_solver_state = {
    .curr_state = EXPLORATION_TO_CENTER,
    .prev_state = EXPLORATION_TO_CENTER,
    .curr_phase = EXPLORATION_TO_CENTER,
    .just_switched_state = true,
    .just_startet_execution = true
};

void switchMazeSolverStateTo(enum MazeSolverStates new_state) {
    maze_solver_state.prev_state = maze_solver_state.curr_state;
    maze_solver_state.curr_state = new_state;
    maze_solver_state.just_switched_state = true;
}


/*
* Replaces the main function of the Maze Solver algorithm from the simulation implementation
*/
void mazeSolver() {
    // Init only at the beginning of the execution of this algorithm
    if (maze_solver_state.just_startet_execution) {
        initMazeSolver();
        maze_solver_state.just_startet_execution = false;
        currMovementControlParameters.is_movement_goal_reached = true;
    }

    /*
    *   State Machine:
    *   What was before a while loop, is now seperated into different states
    *   Therefore every call to move the robot is now in it's own state
    *   This ensures that the robot waits until the movement is finished before executing the next movement or wall detection procedure
    */

    // TODO: Write down the states in the order of execution

    switch (maze_solver_state.curr_state)
    {
    case EXPLORATION_TO_CENTER:
        /* Enter the State */
        if (maze_solver_state.just_switched_state) {
            // Reset the just_switched_state flag
            maze_solver_state.just_switched_state = false;

            char buffer[50];
            sprintf(buffer, "now starting EXPLORATION_TO_CENTER\n");
            putsUART1(buffer);

            // Setting the current phase
            maze_solver_state.curr_phase = EXPLORATION_TO_CENTER;
        }

        // TODO: Old: these comments show the thought process that led to the current structure of the state machine
        // Only execute if the robot is standing still at the center of a cell
        // -> therefore only when he just the current movement goal has been reached
        // -> Alternative have a flag that is set when the robot is moving / standing still
        // if (currMovementControlParameters.is_movement_goal_reached) {
        //     // The is_movement_goal_reached flag is set to false whenever a new movement primitive is started
        //     // Therefore this function will run to the first movement command and then has to return. 
        //     // It has to be ensured that the on the next execution of this function the just executed movement is not executed again
        //     // --> Put the each movement command in it's own state and switch to the next state after the movement is finished
        //     // explorationToCenter();
        // }

        /* Execute */
        // From the explorationToCenter() function (which corresponds to the first while loop in the simulation implementation)
        // This section should only be executed once in every state loop
        // meaning: this state has to be left right after its content has been executed once or this section has to be put back into the Enter the State section
        updateWalls(x, y, orientation, walls);
        floodFill(currentLevel, nextLevel, distance, walls, 0);
        // printDistance_array(distance);
        // API_setColor(x, y, 'G'); // set the current cell to green in the simulator, to track which cells have been visited
        // printMaze(walls);

        openNeighbours(x, y, walls, distance_Open_Neighbours, distance);

        lowestNeighbour = findlowestDistance(distance_Open_Neighbours);
        // Now follows the first movement command: Turn to the lowest neighbour
        // Therefore we have to switch to the respective state: TURN_TO_LOWEST_DISTANCE 
        // after first checking the goal condition

        /* Exit the State */
        // If checkGoal returns 0, the goal has been reached
        // and the algorithm can move on to the next phase
        // TODO: should the goal be checked right at the beginning of the state ? 
        // Doing otherwise, as currently implemented, would lead to the execution of the functions in the Entered the State section
        if (checkGoal(x, y) == 0)
        {
            switchMazeSolverStateTo(EXPLORATION_TO_START);

            // UART
            char buffer[40];
            sprintf(buffer, "CENTER is reached\n\n");
            putsUART1(buffer);
            // printMaze(walls, distance);
            // printDistance_array(distance);
            break;

            // Setting the current phase
            maze_solver_state.curr_phase = EXPLORATION_TO_START;
        }

        // If the goal is not yet reached, the algorithm can move on to the next state as described above
        switchMazeSolverStateTo(TURN_TO_LOWEST_DISTANCE);

        break;
    
    case EXPLORATION_TO_START:
        /* Enter the State */
        if (maze_solver_state.just_switched_state) {
            // Reset the just_switched_state flag
            maze_solver_state.just_switched_state = false;

            char buffer[50];
            sprintf(buffer, "now starting EXPLORATION_TO_START\n");
            putsUART1(buffer);

            maze_solver_state.curr_phase = EXPLORATION_TO_START;
        }

        /* Execute */
        // From the explorationToCenter() function (which corresponds to the first while loop in the simulation implementation)
        // This section should only be executed once in every state loop
        // meaning: this state has to be left right after its content has been executed once or this section has to be put back into the Enter the State section
        updateWalls(x, y, orientation, walls);
        floodFill(currentLevel, nextLevel, distance, walls, 0);
        // printDistance_array(distance);
        // API_setColor(x, y, 'G'); // set the current cell to green in the simulator, to track which cells have been visited
        // printMaze(walls);

        openNeighbours(x, y, walls, distance_Open_Neighbours, distance);

        lowestNeighbour = findlowestDistance(distance_Open_Neighbours);
        // Now follows the first movement command: Turn to the lowest neighbour
        // Therefore we have to switch to the respective state: TURN_TO_LOWEST_DISTANCE 
        // after first checking the goal condition

        /* Exit the State */
        // If checkStart returns 0, the start has been reached
        // and the algorithm can move on to the next phase
        if (checkStart(x, y) == 0)
        {
            switchMazeSolverStateTo(FINAL_RUN);

            // UART
            char buffer[40];
            sprintf(buffer, "START is reached\n\n");
            putsUART1(buffer);
            break;
        }

        // If the goal is not yet reached, the algorithm can move on to the next state as described above
        switchMazeSolverStateTo(TURN_TO_LOWEST_DISTANCE);
        break;

    case FINAL_RUN:
        /* Enter the State */
        if (maze_solver_state.just_switched_state) {
            // Reset the just_switched_state flag
            maze_solver_state.just_switched_state = false;

            // only one call of the final floodfill to initialize the distance array with the optimal path
            final_floodFill(currentLevel, nextLevel, distance, walls);
            // printDistance_array(distance); // print for verification

            char buffer[50];
            sprintf(buffer, "now starting FINAL_RUN\n");
            putsUART1(buffer);

            maze_solver_state.curr_phase = FINAL_RUN;
        }


        /* Execute */
        openNeighbours(x, y, walls, distance_Open_Neighbours, distance);

        lowestNeighbour = findlowestDistance(distance_Open_Neighbours);

        /* Exit the State */
        // If checkGoal returns 0, the goal has been reached
        // and the algorithm can move on to the next phase
        if (checkGoal(x, y) == 0)
        {
            // The goal has been reached and the algorithm can stop.
            // Switch to the according robot state STOP in the robots main state machine
            switchRobotStateTo(STOP);
            break;
        }

        // If the goal is not yet reached, the algorithm can move on to the next state as described above
        switchMazeSolverStateTo(TURN_TO_LOWEST_DISTANCE);
        break;

    case TURN_TO_LOWEST_DISTANCE: {
        int intended_orientation;

        /* Enter the State */
        if (maze_solver_state.just_switched_state) {
            // Reset the just_switched_state flag
            maze_solver_state.just_switched_state = false;

            char buffer[50];
            sprintf(buffer, "now starting TURN_TO_LOWEST_DISTANCE\n");
            putsUART1(buffer);

            // Continuing from below the findlowestDistance() function call
            // Now comes the first movement command: Turn to the lowest neighbour
            // Since this line initiates a movement primitive, it should only be executed once and than no more 
            // until the movement is finished and the state is re-entered from a different state
            intended_orientation = turn_to_lowest_distance(lowestNeighbour, orientation); // orientation should be created as a pointer
            // Now follows the second movement command: Move forward to the next cell with the lowest distance
            // Therefore we have to switch to the respective state: MOVE_FORWARD as soon as the just started movement is finished
        }

        /* Exit the State */
        if (currMovementControlParameters.is_movement_goal_reached) {
            // Updated the Orientation now that the turn is actually completed
            orientation = intended_orientation;
            // Leave the state as soon as the movement is finished
            switchMazeSolverStateTo(MOVE_FORWARD);
        }
        break;
    }
    case MOVE_FORWARD:
        /* Enter the State */
        if (maze_solver_state.just_switched_state) {
            // Reset the just_switched_state flag
            maze_solver_state.just_switched_state = false;

            char buffer[50];
            sprintf(buffer, "now starting MOVE_FORWARD\n");
            putsUART1(buffer);

            // Continuing from below the turn_to_lowest_distance() function call
            // Now comes the second movement command: Move forward to the next cell with the lowest distance
            moveForward();
        }

        /* Exit the State */
        if (currMovementControlParameters.is_movement_goal_reached) {
            // TODO: Update the x and y coordinates ?
            // Leave the state as soon as the movement is finished
            // Switch back to the state based on the current phase (exp to center / exp to start / final run)
            switchMazeSolverStateTo(maze_solver_state.curr_phase);
        }
        break;

    default:
        break;
    }

}


// void explorationToCenter() {
//     updateWalls(x, y, orientation, walls);
//     floodFill(currentLevel, nextLevel, distance, walls, 0);
//     // printDistance_array(distance);
//     // API_setColor(x, y, 'G'); // set the current cell to green in the simulator, to track which cells have been visited
//     // printMaze(walls);

//     openNeighbours(x, y, walls, distance_Open_Neighbours, distance);

//     lowestNeighbour = findlowestDistance(distance_Open_Neighbours);

//     orientation = turn_to_lowest_distance(lowestNeighbour, orientation); // orientation should be created as a pointer

//     // move forward to the next cell with the lowest distance
//     API_moveForward(1);

//     switch (orientation % 4)
//     {
//     case 0:
//         y++;
//         // printtoconsole("Orientation is north\n");
//         break;
//     case 1:
//         x++;
//         // printtoconsole("Orientation is east\n");
//         break;
//     case 2:
//         y--;
//         // printtoconsole("Orientation is south\n");
//         break;
//     case 3:
//         x--;
//         // printtoconsole("Orientation is west\n");
//         break;
//     default:
//         printtoconsole("orientation is invalid\n");
//     }
// }

// void explorationToStart() {}

// void finalRun() {}







// Function to format and print a string
void printString2UART(const char *str) {
    // Calculate the required buffer size based on the length of the input string
    int bufferSize = snprintf(NULL, 0, "%s", str) + 1; // +1 for the null-terminator

    // Dynamically allocate memory for the buffer
    char *buffer = (char *)malloc(bufferSize);

    if (buffer == NULL) {
        // Handle memory allocation failure if needed
        perror("Memory allocation failed");
        return;
    }

    // Format the string into the dynamically allocated buffer
    snprintf(buffer, bufferSize, "%s", str);

    // Print the formatted string
    putsUART1(buffer);

    // Free the dynamically allocated memory
    free(buffer);
}