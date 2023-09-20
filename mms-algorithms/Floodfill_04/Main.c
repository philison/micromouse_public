#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include "API.h"

#define MAZE_SIZE 16
#define STACK_SIZE 256

/************************************
 *
 *   Functions needed for the simulator for logging and setting goals
 *
 *************************************/

// log functions for simulator
void printtoconsole(char *text)
{
    fprintf(stderr, "%s", text);
    fflush(stderr);
}

void printIntToConsole(int number)
{
    fprintf(stderr, "%d", number);
    fflush(stderr);
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
        printtoconsole("direction conversion is not possible");
    }
    return direction;
}

void setWallsforMMS(x, y, orientation)
{

    if (API_wallLeft())
    {
        // printtoconsole("Set left wall");
        API_setWall(x, y, orientationToDirection((orientation - 1) % 4));
    }

    if (API_wallRight())
    {
        // printtoconsole("Set right wall");
        API_setWall(x, y, orientationToDirection((orientation + 1) % 4));
    }
    if (API_wallFront())
    {
        // printtoconsole("Set front wall");
        API_setWall(x, y, orientationToDirection(orientation));
    }
    return;
}

// turns false if the goal is reached
int checkGoal(int x, int y)
{
    if ((x == 7 || x == 8) && (y == 7 || y == 8))
    {
        printtoconsole("The goal has been reached");
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
        if (API_wallLeft())
        {
            walls[x][y].west = WALL;
            if (x > 0)
            {
                walls[x - 1][y].east = WALL;
            }
        }
        if (!API_wallLeft())
        {
            walls[x][y].west = WAY;
            if (x > 0)
            {
                walls[x - 1][y].east = WAY;
            }
        }

        if (API_wallFront())
        {
            walls[x][y].north = WALL;
            if (y < 15)
            {
                walls[x][y + 1].south = WALL;
            }
        }
        if (!API_wallFront())
        {
            walls[x][y].north = WAY;
            if (y < 15)
            {
                walls[x][y + 1].south = WAY;
            }
        }

        if (API_wallRight())
        {
            walls[x][y].east = WALL;
            if (x < 15)
            {
                walls[x + 1][y].west = WALL;
            }
        }
        if (!API_wallRight())
        {
            walls[x][y].east = WAY;
            if (x < 15)
            {
                walls[x + 1][y].west = WAY;
            }
        }

        break;
    case 1:
        if (API_wallLeft())
        {
            walls[x][y].north = WALL;
            if (y < 15)
            {
                walls[x][y + 1].south = WALL;
            }
        }
        if (!API_wallLeft())
        {
            walls[x][y].north = WAY;
            if (y < 15)
            {
                walls[x][y + 1].south = WAY;
            }
        }

        if (API_wallFront())
        {
            walls[x][y].east = WALL;
            if (x < 15)
            {
                walls[x + 1][y].west = WALL;
            }
        }
        if (!API_wallFront())
        {
            walls[x][y].east = WAY;
            if (x < 15)
            {
                walls[x + 1][y].west = WAY;
            }
        }

        if (API_wallRight())
        {
            walls[x][y].south = WALL;
            if (y > 0)
            {
                walls[x][y - 1].north = WALL;
            }
        }
        if (!API_wallRight())
        {
            walls[x][y].south = WAY;
            if (y > 0)
            {
                walls[x][y - 1].north = WAY;
            }
        }

        break;
    case 2:
        if (API_wallLeft())
        {
            walls[x][y].east = WALL;
            if (x < 15)
            {
                walls[x + 1][y].west = WALL;
            }
        }
        if (!API_wallLeft())
        {
            walls[x][y].east = WAY;
            if (x < 15)
            {
                walls[x + 1][y].west = WAY;
            }
        }

        if (API_wallFront())
        {
            walls[x][y].south = WALL;
            if (y > 0)
            {
                walls[x][y - 1].north = WALL;
            }
        }
        if (!API_wallFront())
        {
            walls[x][y].south = WAY;
            if (y > 0)
            {
                walls[x][y - 1].north = WAY;
            }
        }

        if (API_wallRight())
        {
            walls[x][y].west = WALL;
            if (x > 0)
            {
                walls[x - 1][y].east = WALL;
            }
        }
        if (!API_wallRight())
        {
            walls[x][y].west = WAY;
            if (x > 0)
            {
                walls[x - 1][y].east = WAY;
            }
        }

        break;
    case 3:
        if (API_wallLeft())
        {
            walls[x][y].south = WALL;
            if (y > 0)
            {
                walls[x][y - 1].north = WALL;
            }
        }
        if (!API_wallLeft())
        {
            walls[x][y].south = WAY;
            if (y > 0)
            {
                walls[x][y - 1].north = WAY;
            }
        }

        if (API_wallFront())
        {
            walls[x][y].west = WALL;
            if (x > 0)
            {
                walls[x - 1][y].east = WALL;
            }
        }
        if (!API_wallFront())
        {
            walls[x][y].west = WAY;
            if (x > 0)
            {
                walls[x - 1][y].east = WAY;
            }
        }
        if (API_wallRight())
        {
            walls[x][y].north = WALL;
            if (y < 15)
            {
                walls[x][y + 1].south = WALL;
            }
        }
        if (!API_wallRight())
        {
            walls[x][y].north = WAY;
            if (y < 15)
            {
                walls[x][y + 1].south = WAY;
            }
        }

        break;
    default:
        printtoconsole("walls were not set correctly");
        break;
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
        fprintf(stderr, "%2d", 15 - row);
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
        push(currentLevel, (Point){7, 7});
        push(currentLevel, (Point){7, 8});
        push(currentLevel, (Point){8, 8});
        push(currentLevel, (Point){8, 7});
    }

    // set the start cell as goal to walk back out again
    if (goal == 1)
    {
        push(currentLevel, (Point){0, 0});
    }
    if (goal == 2)
    {
        push(currentLevel, (Point){1, 14});
    }

    if (goal == 3)
    {
        push(currentLevel, (Point){14, 14});
    }
    if (goal == 4)
    {
        push(currentLevel, (Point){14, 1});
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
    push(currentLevel, (Point){7, 7});
    push(currentLevel, (Point){7, 8});
    push(currentLevel, (Point){8, 8});
    push(currentLevel, (Point){8, 7});

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
            fprintf(stderr, "%3d ", distance[col][row]);
            fflush(stderr);
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
        break;
    case 2:
    case -2:
        // printtoconsole("Lowest neighbour behind. mouse needs to turn 180 degrees\n");
        API_turnRight();
        orientation = orientation + 1;
        API_turnRight();
        orientation = orientation + 1;
        break;
    case 1:
    case -3:
        // printtoconsole("Lowest neighbour to the right. One right turn\n");
        API_turnRight();
        orientation = orientation + 1;
        break;
    case -1:
    case 3:
        // printtoconsole("lowest neighbour is to the left. One left turn\n");
        API_turnLeft();
        orientation = orientation - 1;
        break;
    default:
        printtoconsole("!!!!!!!!!!!!!ERROR!!!!!!!!!!!\n");
        printtoconsole("Could not turn towards neighbour with lowest distance\n");
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

/*///////////////////////
*
* Starting with main
*
///////////////////////*/

int main(int argc, char *argv[])
{
    struct CellData walls[MAZE_SIZE][MAZE_SIZE];
    int orientation = 16;
    int x = 0;
    int y = 0;
    int distance[MAZE_SIZE][MAZE_SIZE];
    int distance_Open_Neighbours[4];
    int lowestNeighbour = 6;
    Stack *currentLevel = createStack(STACK_SIZE); // current level is needed for the floodfill function
    Stack *nextLevel = createStack(STACK_SIZE);    // next level is needed for the floodfill function

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
    printMaze(walls, distance);
    floodFill(currentLevel, nextLevel, distance, walls, 0);
    printDistance_array(distance);

    /////////////////////////////////
    // Floodfill exploration Phase
    //
    /////////////////////////////////
    while (checkGoal(x, y))
    {

        setWallsforMMS(x, y, orientation);
        updateWalls(x, y, orientation, walls);
        floodFill(currentLevel, nextLevel, distance, walls, 0);
        // printDistance_array(distance);
        API_setColor(x, y, 'G'); // set the current cell to green in the simulator, to track which cells have been visited
        // printMaze(walls);

        openNeighbours(x, y, walls, distance_Open_Neighbours, distance);

        lowestNeighbour = findlowestDistance(distance_Open_Neighbours);

        orientation = turn_to_lowest_distance(lowestNeighbour, orientation); // orientation should be created as a pointer

        // move forward to the next cell with the lowest distance
        API_moveForward(1);

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
            printtoconsole("orientation is invalid\n");
        }
    }

    printtoconsole("START is reached\n");
    printtoconsole("-----------------------\n");
    printtoconsole("Now Walking on the fastes path to the goal\n");
    printtoconsole("\n\r\n");
    printMaze(walls, distance);
    printDistance_array(distance);

    while (checkStart(x, y))
    {
        setWallsforMMS(x, y, orientation);
        updateWalls(x, y, orientation, walls);
        floodFill(currentLevel, nextLevel, distance, walls, 1); // the last parameter can be set to either 0 or 1. This sets the position of the sink for the floodfill algorithm.
        // 0 is the center, 1 is the start cell.
        // printDistance_array(distance);
        API_setColor(x, y, 'G');
        // printMaze(walls);

        openNeighbours(x, y, walls, distance_Open_Neighbours, distance);

        lowestNeighbour = findlowestDistance(distance_Open_Neighbours);

        orientation = turn_to_lowest_distance(lowestNeighbour, orientation); // orientation should be created as a pointer

        // move forward to the next cell with the lowest distance
        API_moveForward(1);

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
            printtoconsole("orientation is invalid\n");
        }
    }

    printtoconsole("START is reached\n");
    printtoconsole("-----------------------\n");
    printtoconsole("Now Walking on the fastes path to the goal\n");
    printtoconsole("\n\r\n");

    // only one call of the final floodfill to initialize the distance array with the optimal path
    final_floodFill(currentLevel, nextLevel, distance, walls);
    printDistance_array(distance); // print for verification

    while (checkGoal(x, y))
    {
        // printDistance_array(distance); // print for verification
        API_setColor(x, y, 'R');
        // printMaze(walls);

        openNeighbours(x, y, walls, distance_Open_Neighbours, distance);

        lowestNeighbour = findlowestDistance(distance_Open_Neighbours);

        orientation = turn_to_lowest_distance(lowestNeighbour, orientation); // orientation should be created as a pointer

        // move forward to the next cell with the lowest distance
        API_moveForward(1);

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
            printtoconsole("orientation is invalid\n");
        }
    }
}