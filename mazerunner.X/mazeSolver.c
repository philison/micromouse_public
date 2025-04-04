#include "mazeSolver.h"

// log functions for simulator
void printtoconsole(char *text)
{
    // fprintf(stderr, "%s", text);
    // fflush(stderr);
    // printf("%s", text); // TESTING
}

void printIntToConsole(int number)
{
    // fprintf(stderr, "%d", number);
    // fflush(stderr);
    // printf("%d", number); // TESTING
}

// only needed for the simulator, as it accepts char as the orientation
char orientationToDirection(int orientation)
{
    char direction;

    switch (orientation % 4)
    {
    case 0:
        direction = 'n';
        break;
    case 1:
        direction = 'e';
        break;
    case 2:
        direction = 's';
        break;
    case 3:
        direction = 'w';
        break;
    default:
        printtoconsole("direction conversion is not possible\n"); // TESTING
    }
    return direction;
}

void setWallsforMMS(int x, int y, int orientation)
{

    if (isWallLeft())
    {
        // printtoconsole("Set left wall");
        // API_setWall(x, y, orientationToDirection((orientation - 1) % 4));
        char buffer[40];
        sprintf(buffer, "sW-%i-%i-%c\n", x, y, orientationToDirection((orientation - 1) % 4));
        putsUART1(buffer);
    }

    if (isWallRight())
    {
        // printtoconsole("Set right wall");
        // API_setWall(x, y, orientationToDirection((orientation + 1) % 4));
        char buffer[40];
        sprintf(buffer, "sW-%i-%i-%c\n", x, y, orientationToDirection((orientation + 1) % 4));
        putsUART1(buffer);
    }
    if (isWallFront())
    {
        // printtoconsole("Set front wall");
        // API_setWall(x, y, orientationToDirection(orientation));
        char buffer[40];
        sprintf(buffer, "sW-%i-%i-%c\n", x, y, orientationToDirection(orientation));
        putsUART1(buffer);
    }
    return;
}

// turns false if the goal is reached
int checkGoal(int x, int y)
{
    if ((x == 2 || x == 3) && (y == 2 || y == 3))
    {
        printString2UARTmax60("The goal has been reached\n");
        return 0;
    }
    return 1;
}

// turns false if the start is reached
int checkStart(int x, int y)
{
    if (x == 0 && y == 0)
    {
        printString2UARTmax60("The start has been reached\n");
        return 0;
    }
    return 1;
}

// // define values for each cell
// enum CellValue
// {
//     UNKNOWN,
//     WALL,
//     WAY,
//     EXPLORED
// };

// // Define a structure to hold the four values
// struct CellData
// {
//     enum CellValue north;
//     enum CellValue east;
//     enum CellValue south;
//     enum CellValue west;
//     enum CellValue center;
// };

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
            if (y < MAZE_SIZE-1)
            {
                walls[x][y + 1].south = WALL;
            }
        }
        if (!isWallFront())
        {
            walls[x][y].north = WAY;
            if (y < MAZE_SIZE-1)
            {
                walls[x][y + 1].south = WAY;
            }
        }

        if (isWallRight())
        {
            walls[x][y].east = WALL;
            if (x < MAZE_SIZE-1)
            {
                walls[x + 1][y].west = WALL;
            }
        }
        if (!isWallRight())
        {
            walls[x][y].east = WAY;
            if (x < MAZE_SIZE-1)
            {
                walls[x + 1][y].west = WAY;
            }
        }

        break;
    case 1:
        if (isWallLeft())
        {
            walls[x][y].north = WALL;
            if (y < MAZE_SIZE-1)
            {
                walls[x][y + 1].south = WALL;
            }
        }
        if (!isWallLeft())
        {
            walls[x][y].north = WAY;
            if (y < MAZE_SIZE-1)
            {
                walls[x][y + 1].south = WAY;
            }
        }

        if (isWallFront())
        {
            walls[x][y].east = WALL;
            if (x < MAZE_SIZE-1)
            {
                walls[x + 1][y].west = WALL;
            }
        }
        if (!isWallFront())
        {
            walls[x][y].east = WAY;
            if (x < MAZE_SIZE-1)
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
            if (x < MAZE_SIZE-1)
            {
                walls[x + 1][y].west = WALL;
            }
        }
        if (!isWallLeft())
        {
            walls[x][y].east = WAY;
            if (x < MAZE_SIZE-1)
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
            if (y < MAZE_SIZE-1)
            {
                walls[x][y + 1].south = WALL;
            }
        }
        if (!isWallRight())
        {
            walls[x][y].north = WAY;
            if (y < MAZE_SIZE-1)
            {
                walls[x][y + 1].south = WAY;
            }
        }

        break;
    default: {
        // UART
        char buffer[40];
        sprintf(buffer, "walls were not set correctly\n");
        putsUART1(buffer);
        break;
    }
    }
    return;
}


// // prints the maze with each cell.
// // | or - represents a wall
// // x is set for an unknown side /(wall)
// // * is set for a way
// // Center is represented by Unexplored with "UU or if explored the distance is shown. "
void printMaze(struct CellData maze[MAZE_SIZE][MAZE_SIZE], int distance[MAZE_SIZE][MAZE_SIZE])
{
    //     printtoconsoleEnter("______________________________________");
    //     printtoconsoleEnter("______________ NEW MAZE ______________");
    //     printtoconsoleEnter("______________________________________");

    // Print the top border
    printtoconsole(" ");
    printtoconsole("+");
    for (int col = 0; col < MAZE_SIZE; col++)
    {
        printtoconsole("");
        fprintf(stderr, "%2d", col);
        fflush(stderr);
        printtoconsole("--+");
    }
    printtoconsole("\n");

    for (int row = MAZE_SIZE - 1; row >= 0; row--)
    {
        // Print the contents of the current row
        // print separation row
        printtoconsole(" ");
        printtoconsole("+");
        for (int col = 0; col < MAZE_SIZE; col++)
        {
            printtoconsole("----+");
        }
        printtoconsole("\n");

        // print north wall
        printtoconsole(" ");
        printtoconsole(" ");
        for (int col = 0; col < MAZE_SIZE; col++)
        {
            printtoconsole("o");
            switch (maze[col][row].north)
            {
            case UNKNOWN:
                printtoconsole("xx");
                break;
            case WALL:
                printtoconsole("–-");
                break;
            case WAY:
                printtoconsole("**");
                break;
            case EXPLORED:
                printtoconsole("##");
                break;
            }
            printtoconsole("o ");
        }
        printtoconsole("\n");

        // add the row number in the beginning; two digits number move the cells
        fprintf(stderr, "%2d", MAZE_SIZE - 1 - row);
        fflush(stderr);
        for (int col = 0; col < MAZE_SIZE; col++)
        {

            // print the west wall of the current cell
            switch (maze[col][row].west)
            {
            case UNKNOWN:
                printtoconsole("x");
                break;
            case WALL:
                printtoconsole("|");
                break;
            case WAY:
                printtoconsole("*");
                break;
            case EXPLORED:
                printtoconsole("#");
                break;
            }
            // Print the center cell value
            switch (maze[col][row].center)
            {
            case UNKNOWN:
                printtoconsole("UU");
                break;
            case WALL:
                printtoconsole("FF");
                break;
            case WAY:
                printtoconsole("FF");
                break;
            case EXPLORED:
                // printtoconsole("E");
                fprintf(stderr, "%2d", distance[col][row]);
                fflush(stderr);
                break;
            }

            // Print the east wall of the current cell
            switch (maze[col][row].east)
            {
            case UNKNOWN:
                printtoconsole("x");
                break;
            case WALL:
                printtoconsole("|");
                break;
            case WAY:
                printtoconsole("*");
                break;
            case EXPLORED:
                printtoconsole("#");
                break;
            }
            printtoconsole(" ");
        }
        printtoconsole("\n");

        // print the south wall
        printtoconsole(" ");
        printtoconsole(" ");
        for (int col = 0; col < MAZE_SIZE; col++)
        {
            printtoconsole("o");
            switch (maze[col][row].south)
            {
            case UNKNOWN:
                printtoconsole("xx");
                break;
            case WALL:
                printtoconsole("–-");
                break;
            case WAY:
                printtoconsole("**");
                break;
            case EXPLORED:
                printtoconsole("##");
                break;
            }
            printtoconsole("o ");
        }
        printtoconsole("\n");
    }

    // Print the row's bottom border
    // printtoconsole("+");
    // for (int col = 0; col < MAZE_SIZE; col++)
    // {
    //     printtoconsole("-");
    //     printIntToConsole(col);
    //     printtoconsole("-+");
    // }
    // printtoconsole("\n");
    printtoconsole(" ");
    printtoconsole("+");
    for (int col = 0; col < MAZE_SIZE; col++)
    {
        printtoconsole("");
        fprintf(stderr, "%2d", col);
        fflush(stderr);
        printtoconsole("--+");
    }
    printtoconsole("\n");
}


/************************************
 *
 *   Non_recursive Implementation
 *      Floodfill distance update
 *
 *************************************/
// typedef struct
// {
//     int x, y;
// } Point;

// // Stack data structure
// typedef struct
// {
//     Point *items;
//     int top;
//     int capacity;
// } Stack;

// Initialize a stack with a given capacity
Stack *createStack(int capacity)
{
    // Disable interrupts
    __builtin_disable_interrupts();

    Stack *stack = (Stack *)malloc(sizeof(Stack));
    stack->capacity = capacity;
    stack->top = -1;
    stack->items = (Point *)malloc(sizeof(Point) * capacity);

    // Check if stack was created successfully
    // By checking if the created pointer is NULL
    if (stack->items == NULL) {
        printString2UARTmax60("Cannot allocate memory. Check HeapSize in IDE\n");
    }
    // else 
    // { 
    //     // printString2UARTmax60("Memory Allocated successfully\n");
    //     // printf ("Memory Allocated successfully \n");
    // }

    // Re-enable interrupts
    __builtin_enable_interrupts();

    return stack;
}

// Push an element onto the stack
void push(Stack *stack, Point item)
{
    // Disable interrupts
    __builtin_disable_interrupts();
    // __builtin_disi(0x3FFF); /* disable interrupts */

    if (stack->top == stack->capacity - 1)
    {
        // printString2UARTmax60("stack full\n");
        // Stack is full, handle error or resize
        return;
    }
    // printString2UARTmax60("before stack\n");
    stack->items[++stack->top] = item;
    // printString2UARTmax60("after stack\n");

    // Re-enable interrupts
    // __builtin_disi(0x0000); /* enable interrupts */
    __builtin_enable_interrupts();
}

// Pop an element from the stack
Point pop(Stack *stack)
{
    // Disable interrupts
    __builtin_disable_interrupts();
    // __builtin_disi(0x3FFF); /* disable interrupts */

    if (stack->top == -1)
    {
        // Stack is empty, handle error
        Point p = {-1, -1}; // Return an invalid point to indicate an error
        return p;
    }
    return stack->items[stack->top--];

    // Re-enable interrupts
    // __builtin_disi(0x0000); /* enable interrupts */
    __builtin_enable_interrupts();
}

// Swap the contents of two stacks
void swapStacks(Stack **stack1, Stack **stack2)
{
     // Disable interrupts
    __builtin_disable_interrupts();
    // __builtin_disi(0x3FFF); /* disable interrupts */


    Stack *temp = *stack1;
    *stack1 = *stack2;
    *stack2 = temp;

    // Re-enable interrupts
    // __builtin_disi(0x0000); /* enable interrupts */
    __builtin_enable_interrupts();
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
    // printString2UARTmax60("flood\n"); // TESTING
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
    // printString2UARTmax60("floodfill done\n"); // TESTING
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
void printDistance_array(int distance[MAZE_SIZE][MAZE_SIZE])
{
    for (int row = MAZE_SIZE - 1; row >= 0; row--)
    {
        for (int col = 0; col < MAZE_SIZE; col++)
        {
            // fprintf(stderr, "%3d ", distance[col][row]);
            // fflush(stderr);
            printf("%3d ", distance[col][row]);
        }
        printtoconsole("\n");
    }
    printtoconsole("\n_______________________\n\r");
}


// Turns the mouse towards the neighbour with the lowest distance. This should only be one lower than the current cell.
// lowest neighbour is either 0, 1, 2, 3, and marks the cell with the lowest distance. 0 corresponds to north, 1 to east and so forth
int turn_to_lowest_distance(int lowestNeighbour, int orientation)
{

    switch (lowestNeighbour - (orientation % 4))
    {
    case 0:
        // printtoconsole("mouse does not need to turn\n");
        maze_solver_state.already_aligned_with_lowest_distance = true;
        break;
    case 2:
    case -2: {
        // printtoconsole("Lowest neighbour behind. mouse needs to turn 180 degrees\n");
        // API_turnRight();
        // orientation = orientation + 1;
        // API_turnRight();
        // orientation = orientation + 1;
        turn180();
        // TODO: as this variable is not defined again in this function as a global variable it directly effects the orientation variable outside of this function
        // Is that a problem ? this lead to the orientation beeing updated before the actual goal position is reached.
        orientation = orientation + 2;

        // Update Sim State on Pc
        char buffer[5];
        sprintf(buffer, "tR\n");
        putsUART1(buffer);
        sprintf(buffer, "tR\n");
        putsUART1(buffer);
        break;
    }
    case 1:
    case -3: {
        // printtoconsole("Lowest neighbour to the right. One right turn\n");
        // API_turnRight();
        turnRight();
        orientation = orientation + 1;

        // Update Sim State on Pc
        char buffer[5];
        sprintf(buffer, "tR\n");
        putsUART1(buffer);
        break;
    }
    case -1:
    case 3: {
        // printtoconsole("lowest neighbour is to the left. One left turn\n");
        // API_turnLeft();
        turnLeft();
        orientation = orientation - 1;

        // Update Sim State on Pc
        char buffer[5];
        sprintf(buffer, "tL\n");
        putsUART1(buffer);
        break;
    }
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
    char buffer[60];
    sprintf(buffer, "distance_Open_Neighbours after update\n");
    putsUART1(buffer);
    sprintf(buffer, "%i, %i, %i, %i\n", distance_Open_Neighbours[0], distance_Open_Neighbours[1], distance_Open_Neighbours[2], distance_Open_Neighbours[3]);
    putsUART1(buffer);
}



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

// Define global variables here
struct CellData walls[MAZE_SIZE][MAZE_SIZE];
int orientation = 16;
int x = 0;
int y = 0;
int distance[MAZE_SIZE][MAZE_SIZE];
int distance_Open_Neighbours[4];
int lowestNeighbour = 6;

// DEBUGGING
// Stack *currentLevel; // Define currentLevel globally
// Stack *nextLevel;    // Define nextLevel globally
// DEBUGGING


/* 
*   Newly added Functions that implement things that before 
*   happened within the main function                           
*/

void initMazeSolver(Stack *currentLevel, Stack *nextLevel) {
    // printString2UARTmax60("initMS\n");
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
    // printMaze(walls, distance); // TESTING
    floodFill(currentLevel, nextLevel, distance, walls, 0);
    // printDistance_array(distance); // TESTING
}

extern struct MazeSolverState maze_solver_state = {
    .curr_state = EXPLORATION_TO_CENTER,
    .prev_state = EXPLORATION_TO_CENTER,
    .curr_phase = EXPLORATION_TO_CENTER,
    .just_switched_state = true,
    .just_startet_execution = true,
    .already_aligned_with_lowest_distance = false
};

void switchMazeSolverStateTo(enum MazeSolverStates new_state) {
    maze_solver_state.prev_state = maze_solver_state.curr_state;
    maze_solver_state.curr_state = new_state;
    maze_solver_state.just_switched_state = true;
}


/************************************
 *
 *   Maze Solver Algorithm
 *
 *************************************/

/**
 * Replaces the main function of the Maze Solver algorithm from the simulation implementation
 * Contains the Maze Solver State Machine that gets called in the EXECUTE state of the Robot State Machine
*/
void mazeSolver(Stack *currentLevel, Stack *nextLevel) {

    // printString2UARTmax60("mazeSolver() called\n"); // TESTING
    // Init only at the beginning of the execution of this algorithm
    if (maze_solver_state.just_startet_execution) {
        // printString2UARTmax60("MS just_startet_execution\n"); // TESTING
        initMazeSolver(nextLevel, currentLevel);
        maze_solver_state.just_startet_execution = false;
        currMovementControlParameters.is_movement_goal_reached = false; // was true before
    }

    /*
    *   State Machine:
    *   What was before a while loop, is now seperated into different states
    *   Therefore every call to move the robot is now in it's own state
    *   This ensures that the robot waits until the movement is finished before executing the next movement or wall detection procedure
    */

    switch (maze_solver_state.curr_state)
    {
    case EXPLORATION_TO_CENTER: {
        // printString2UARTmax60("MS: EXPLORATION_TO_CENTER\n"); // TESTING
        /* Enter the State */
        if (maze_solver_state.just_switched_state) {
            // Reset the just_switched_state flag
            maze_solver_state.just_switched_state = false;

            // printString2UARTmax60("now starting EXPLORATION_TO_CENTER\n"); // TESTING
            // char buffer[40];
            // sprintf(buffer, "orientation: %i = %s\n", orientation, getOrientationString(orientation)); // TESTING
            // putsUART1(buffer);

            // sprintf(buffer, "position: %i, %i\n", x, y); // TESTING
            // putsUART1(buffer);

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
        setWallsforMMS(x, y, orientation); // Just for testing to update the SIM
        updateWalls(x, y, orientation, walls);
        floodFill(currentLevel, nextLevel, distance, walls, 0);
        // printDistance_array(distance);
        // API_setColor(x, y, 'G'); // set the current cell to green in the simulator, to track which cells have been visited
        char buffer[20];
        sprintf(buffer, "sC-%i-%i-%c\n", x, y, 'G');
        putsUART1(buffer);
        // printMaze(walls);

        openNeighbours(x, y, walls, distance_Open_Neighbours, distance);

        lowestNeighbour = findlowestDistance(distance_Open_Neighbours);
        // Print the lowest neighbour for debugging as cardinal direction using getOrientationString
        // char buffer[40];
        // sprintf(buffer, "lowestNeighbour: %i = %s\n", lowestNeighbour, getOrientationString(lowestNeighbour)); // TESTING
        // putsUART1(buffer);
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

            printString2UARTmax60("CENTER is reached\n\n");
            // printMaze(walls, distance); // TESTING
            // printDistance_array(distance); // TESTING

            // Setting the current phase
            // maze_solver_state.curr_phase = EXPLORATION_TO_START;

            break;
        }

        // If the goal is not yet reached, the algorithm can move on to the next state as described above
        switchMazeSolverStateTo(TURN_TO_LOWEST_DISTANCE);

        break;
    }
    case EXPLORATION_TO_START: {
        // printString2UARTmax60("MS: EXPLORATION_TO_START\n"); // TESTING
        /* Enter the State */
        if (maze_solver_state.just_switched_state) {
            // Reset the just_switched_state flag
            maze_solver_state.just_switched_state = false;

            char buffer[50];
            sprintf(buffer, "now starting EXPLORATION_TO_START\n"); // TESTING
            putsUART1(buffer);

            // Setting the current phase
            maze_solver_state.curr_phase = EXPLORATION_TO_START;
        }

        /* Execute */
        // From the explorationToCenter() function (which corresponds to the first while loop in the simulation implementation)
        // This section should only be executed once in every state loop
        // meaning: this state has to be left right after its content has been executed once or this section has to be put back into the Enter the State section
        updateWalls(x, y, orientation, walls);
        floodFill(currentLevel, nextLevel, distance, walls, 1);
        // printDistance_array(distance);
        // API_setColor(x, y, 'G'); // set the current cell to green in the simulator, to track which cells have been visited
        char buffer[20];
        sprintf(buffer, "sC-%i-%i-%c\n", x, y, 'G');
        putsUART1(buffer);
        // printMaze(walls);

        openNeighbours(x, y, walls, distance_Open_Neighbours, distance);

        lowestNeighbour = findlowestDistance(distance_Open_Neighbours);
        // sprintf(buffer, "lowestNeighbour: %d\n", lowestNeighbour); // TESTING
        // putsUART1(buffer);
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
    }
    case FINAL_RUN:
        // printString2UARTmax60("MS: FINAL_RUN\n"); // TESTING
        /* Enter the State */
        if (maze_solver_state.just_switched_state) {
            // Reset the just_switched_state flag
            maze_solver_state.just_switched_state = false;
            // Setting the current phase
            maze_solver_state.curr_phase = FINAL_RUN;

            // only one call of the final floodfill to initialize the distance array with the optimal path
            final_floodFill(currentLevel, nextLevel, distance, walls);
            // printDistance_array(distance); // print for verification // TESTING

            char buffer[50];
            sprintf(buffer, "now starting FINAL_RUN\n");
            putsUART1(buffer);
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
            // switchRobotStateTo(STOP);
            robot_state.execution_finished = true;
            break;
        }

        // If the goal is not yet reached, the algorithm can move on to the next state as described above
        switchMazeSolverStateTo(TURN_TO_LOWEST_DISTANCE);
        break;

    case TURN_TO_LOWEST_DISTANCE: {
        // printString2UARTmax60("MS: TURN_TO_LOWEST_DISTANCE\n"); // TESTING
        // int intended_orientation;

        /* Enter the State */
        if (maze_solver_state.just_switched_state) {
            // Reset the just_switched_state flag
            maze_solver_state.just_switched_state = false;

            // printString2UARTmax60("now starting TURN_TO_LOWEST_DISTANCE\n"); // TESTING

            // Continuing from below the findlowestDistance() function call
            // Now comes the first movement command: Turn to the lowest neighbour
            // Since this line initiates a movement primitive, it should only be executed once and than no more 
            // until the movement is finished and the state is re-entered from a different state
            // intended_orientation = turn_to_lowest_distance(lowestNeighbour, orientation); // orientation should be created as a pointer
            orientation = turn_to_lowest_distance(lowestNeighbour, orientation); // orientation should be created as a pointer
            // buffer[40];
            // sprintf(buffer, "orientation: %i = %s\n", orientation, getOrientationString(orientation)); // TESTING
            // putsUART1(buffer);
            // Now follows the second movement command: Move forward to the next cell with the lowest distance
            // Therefore we have to switch to the respective state: MOVE_FORWARD as soon as the just started movement is finished
        }

        /* Exit the State */
        // Either if the turn movement is finished or if the mouse is already aligned with the lowest distance
        if (currMovementControlParameters.is_movement_goal_reached || maze_solver_state.already_aligned_with_lowest_distance) {
            if (maze_solver_state.already_aligned_with_lowest_distance) {
                // If the mouse is already aligned with the lowest distance, there is no need to turn again
                // printString2UARTmax60("already aligned with lowest distance\n"); // TESTING
                maze_solver_state.already_aligned_with_lowest_distance = false;
            }
            // Updated the Orientation now that the turn is actually completed
            // orientation = intended_orientation;
            // Leave the state as soon as the movement is finished
            switchMazeSolverStateTo(MOVE_FORWARD);
        }

        break;
    }
    case MOVE_FORWARD:
        // printString2UARTmax60("MS: MOVE_FORWARD\n"); // TESTING
        /* Enter the State */
        if (maze_solver_state.just_switched_state) {
            // Reset the just_switched_state flag
            maze_solver_state.just_switched_state = false;

            // printString2UARTmax60("now starting MOVE_FORWARD\n"); // TESTING

            // Continuing from below the turn_to_lowest_distance() function call
            // Now comes the second movement command: Move forward to the next cell with the lowest distance
            moveForward();
            // Update Sim State on Pc
            char buffer[5];
            sprintf(buffer, "mF\n");
            putsUART1(buffer);
        }

        /* Exit the State */
        if (currMovementControlParameters.is_movement_goal_reached) {
            // TODO: Update the x and y coordinates ?
            switch (orientation % 4)
                {
                case 0:
                    y++;
                    // printtoconsole("Orientation is north\n");
                    break;
                case 1:
                    x++;
                    // printtoconsole("Orientation is east\n");
                    break;
                case 2:
                    y--;
                    // printtoconsole("Orientation is south\n");
                    break;
                case 3:
                    x--;
                    // printtoconsole("Orientation is west\n");
                    break;
                default:
                    // printtoconsole("orientation is invalid\n");
                    printString2UARTmax60("orientation is invalid\n");
                }
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
// CAUTION: this requires the user to set a heap size in the MPLAB X IDE in order to use malloc
void printString2UARTmalloc(const char *str) {
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

// Function to format and print a string of max length 60
void printString2UARTmax60(const char *str) {
    char buffer[60];
    sprintf(buffer, "%s", str);
    putsUART1(buffer);
}

// Function to format and print a string of max length 10
void printString2UARTmax10(const char *str) {
    char buffer[10];
    sprintf(buffer, "%s", str);
    putsUART1(buffer);
}

// Function that returns a string with the integer orientation
//  turned into the corresponding cardinal direction
const char *getOrientationString(int orientation) {
    switch (orientation % 4)
    {
    case 0:
        return "NORTH";
    case 1:
        return "EAST";
    case 2:
        return "SOUTH";
    case 3:
        return "WEST";
    default:
        return "INVALID";
    }
}