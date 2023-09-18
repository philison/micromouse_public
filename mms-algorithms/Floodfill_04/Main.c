#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include "API.h"

#define MAZE_SIZE 16
#define STACK_SIZE 256

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

int checkGoal(int x, int y)
{
    if ((x == 7 || x == 8) && (y == 7 || y == 8))
    {
        printtoconsole("The goal has been reached");
        return 0;
    }
    return 1;
}

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
// Function that sets walls for the x / y position and the connecting walls for the neighbouring cells
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

        // wall to the back of the mouse is always a way, as the mouse came from there
        // if(walls[x][y].south == UNKNOWN){
        //             walls[x][y].south = WAY;
        // }

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
        // if(walls[x][y].west == UNKNOWN){
        //             walls[x][y].west = WAY;
        // }

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
        // if(walls[x][y].north == UNKNOWN){
        //             walls[x][y].north = WAY;
        // }

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
        // if(walls[x][y].east == UNKNOWN){
        //             walls[x][y].east = WAY;
        // }

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
// // Center is represented by Unexplored / Explored
void printMaze(struct CellData maze[MAZE_SIZE][MAZE_SIZE])
{
    //     printtoconsoleEnter("______________________________________");
    //     printtoconsoleEnter("______________ NEW MAZE ______________");
    //     printtoconsoleEnter("______________________________________");

    // Print the top border
    printtoconsole(" ");
    printtoconsole("+");
    for (int col = 0; col < MAZE_SIZE; col++)
    {
        printtoconsole("-");
        printIntToConsole(col);
        printtoconsole("-+");
    }
    printtoconsole("\n");
    // printtoconsole("+" );
    // for (int col = 0; col < MAZE_SIZE; col++) {
    //     printtoconsole("---+");
    // }

    for (int row = MAZE_SIZE - 1; row >= 0; row--)
    {
        // Print the contents of the current row
        // print separation row
        printtoconsole(" ");
        printtoconsole("+");
        for (int col = 0; col < MAZE_SIZE; col++)
        {
            printtoconsole("---+");
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
                printtoconsole("x");
                break;
            case WALL:
                printtoconsole("–");
                break;
            case WAY:
                printtoconsole("*");
                break;
            case EXPLORED:
                printtoconsole("#");
                break;
            }
            printtoconsole("o ");
        }
        printtoconsole("\n");

        // add the row number in the beginning; two digits number move the cells
        printIntToConsole(1);
        // printtoconsole("|");
        for (int col = 0; col < MAZE_SIZE; col++)
        {
            printtoconsole(" ");

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
                printtoconsole("U");
                break;
            case WALL:
                printtoconsole("F");
                break;
            case WAY:
                printtoconsole("F");
                break;
            case EXPLORED:
                printtoconsole("E");
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
                printtoconsole("x");
                break;
            case WALL:
                printtoconsole("–");
                break;
            case WAY:
                printtoconsole("*");
                break;
            case EXPLORED:
                printtoconsole("#");
                break;
            }
            printtoconsole("o ");
        }
        printtoconsole("\n");
    }

    // Print the row's bottom border
    printtoconsole("+");
    for (int col = 0; col < MAZE_SIZE; col++)
    {
        printtoconsole("-");
        printIntToConsole(col);
        printtoconsole("-+");
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

// Function to perform flood-fill without recursion
void floodFill(Stack *currentLevel, Stack *nextLevel, int distance[MAZE_SIZE][MAZE_SIZE], struct CellData walls[MAZE_SIZE][MAZE_SIZE])
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
    // set the center cells always in first
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
    }
}

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

// old distance init, but can also be done with one initial distance setup and the center point in the middle
// void initDistance(int distance[MAZE_SIZE][MAZE_SIZE])
// {
//     int n = MAZE_SIZE / 2;
//     for (int i = 0; i < n; i++)
//     {
//         for (int j = 0; j < n; j++)
//         {
//             distance[i][j] = 14 - i - j;
//         }
//     }

//     // mirror to the bottom left quadrant

//     for (int i = 0; i < n; i++)
//     {
//         for (int j = 0; j < n; j++)
//         {
//             distance[i + n][j] = distance[n - 1 - i][j];
//         }
//     }

//     // mirror to the top right quadrant
//     for (int i = 0; i < n; i++)
//     {
//         for (int j = 0; j < n; j++)
//         {
//             distance[i][j + n] = distance[i][n - 1 - j];
//         }
//     }

//     // mirror to the bottom right quadrant

//     for (int i = 0; i < MAZE_SIZE; i++)
//     {
//         for (int j = 0; j < n; j++)
//         {
//             distance[i + n][j + n] = distance[n - 1 - i][n - 1 - j];
//         }
//     }
// }

// print the distance array to the console for verification
void printDistance_array(int distance[MAZE_SIZE][MAZE_SIZE])
{
    for (int row = MAZE_SIZE - 1; row >= 0; row--)
    {
        for (int col = 0; col < MAZE_SIZE; col++)
        {
            fprintf(stderr, "%2d ", distance[col][row]);
            fflush(stderr);
        }
        printtoconsole("\n");
    }
    printtoconsole("\n_______________________\n\r");
}

void relative_direction_next_cell(int lowestNeighbour, int direction_mouse)
{
    int orientation = direction_mouse % 4;

    switch (lowestNeighbour - (orientation % 4))
    {
    case 0:
        printtoconsole("mouse does not need to turn");
        break;
    case 2:
    case -2:
        printtoconsole("Lowest neighbour behind. mouse needs to turn 180 degrees");
        // API_turnRight();
        // orientation = orientation + 1;
        // API_turnRight();
        // orientation = orientation + 1;
        break;
    case 1:
    case -3:
        printtoconsole("Lowest neighbour to the right. One right turn");
        // API_turnRight();
        // orientation = orientation + 1;
        break;
    case -1:
    case 3:
        printtoconsole("lowest neighbour is to the left. One left turn");
        //  API_turnLeft();
        //  orientation = orientation - 1;
        break;
    default:
        printtoconsole("!!!!!!!!!!!!!ERROR!!!!!!!!!!!");
        printtoconsole("Could not turn towards neighbour with lowest distance");
    }
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
    Stack *currentLevel = createStack(STACK_SIZE);
    Stack *nextLevel = createStack(STACK_SIZE);
    // int temp_orientation = 0;

    // initialize all walls to UNKNOWN
    for (int row = 0; row < 16; row++)
    {
        for (int col = 0; col < 16; col++)
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
    for (int i = 0; i < 16; i++)
    {
        walls[0][i].west = WALL;
        walls[i][0].north = WALL;
        walls[15][i].east = WALL;
        walls[i][15].south = WALL;
    }

    // initDistance(distance);
    floodFill(currentLevel, nextLevel, distance, walls);
    printDistance_array(distance);

    /////////////////////////////////
    // left wall following algorithm
    //
    /////////////////////////////////
    while (checkGoal(x, y))
    {

        setWallsforMMS(x, y, orientation);
        updateWalls(x, y, orientation, walls);
        printDistance_array(distance);
        printMaze(walls);

        /////////////////////////////////
        // find open neighbour with lowest distance
        /////////////////////////////////

        // init distanceOfOpenNeighbours with highest possible distance
        //  somehow initialization with a for loop was not possible

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

        lowestNeighbour = findlowestDistance(distance_Open_Neighbours);
        // printtoconsole("lowestNeighbour is: ");
        // printIntToConsole(lowestNeighbour);
        // printtoconsole("\n");
        // printtoconsole("Current Orientation is: ");
        // printIntToConsole(orientation % 4);
        // printtoconsole("\n");

        /////////////////////////////////
        // find relative position the open neighbour with lowest distance
        /////////////////////////////////

        // printtoconsoleEnter("lowestNeighbour is:");
        // printIntToConsole(lowestNeighbour);
        // printtoconsole("\n");
        // printtoconsoleEnter("Current Orientation is:");
        // printIntToConsole(orientation % 4);
        // printtoconsole("\n");

        // if (!API_wallLeft())
        // {
        //     API_turnLeft();
        //     orientation--;
        // }
        // while (API_wallFront())
        // {
        //     API_turnRight();
        //     orientation++;
        // }
        // API_moveForward(1);

// turning the mouse to the direction of the lowest neighbour
        switch (lowestNeighbour - (orientation % 4))
        {
        case 0:
            printtoconsole("mouse does not need to turn\n");
            break;
        case 2:
        case -2:
            printtoconsole("Lowest neighbour behind. mouse needs to turn 180 degrees\n");
            API_turnRight();
            orientation = orientation + 1;
            API_turnRight();
            orientation = orientation + 1;
            break;
        case 1:
        case -3:
            printtoconsole("Lowest neighbour to the right. One right turn\n");
            API_turnRight();
            orientation = orientation + 1;
            break;
        case -1:
        case 3:
            printtoconsole("lowest neighbour is to the left. One left turn\n");
             API_turnLeft();
             orientation = orientation - 1;
            break;
        default:
            printtoconsole("!!!!!!!!!!!!!ERROR!!!!!!!!!!!\n");
            printtoconsole("Could not turn towards neighbour with lowest distance\n");
        }
        // move forward to the next cell with the lowest distance
        API_moveForward(1);

    

        switch (orientation % 4)
        {
        case 0:
            y++;
            // printtoconsoleEnter("Orientation is north");
            break;
        case 1:
            x++;
            // printtoconsoleEnter("Orientation is east");
            break;
        case 2:
            y--;
            // printtoconsoleEnter("Orientation is south");
            break;
        case 3:
            x--;
            // printtoconsoleEnter("Orientation is west");
            break;
        default:
            printtoconsole("orientation is invalid\n");
        }
        floodFill(currentLevel, nextLevel, distance, walls);
    }
}