#include <stdio.h>
#include <math.h>
#include <stdlib.h>
// #include "API.h"

// log functions for simulator

// void printf(char *text)
// {
//     fprintf(stderr, "%s", text);

// }

// void printfEnter(char *text)
// {
//     fprintf(stderr, "%s\n", text);
//     fflush(stderr);
// }

// void printIntToConsole(int number)
// {
//     fprintf(stderr, "%d", number);
//     fflush(stderr);
// }
// void printIntToConsolewithSpace(int number)
// {
//     fprintf(stderr, "%2d   ", number);
//     fflush(stderr);
// }

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
        printf("direction conversion is not possible");
    }
    return direction;
}

// void setWallsforMMS(x, y, orientation)
// {

//     if (API_wallLeft())
//     {
//         // printf("Set left wall");
//         API_setWall(x, y, orientationToDirection((orientation - 1) % 4));
//     }

//     if (API_wallRight())
//     {
//         // printf("Set right wall");
//         API_setWall(x, y, orientationToDirection((orientation + 1) % 4));
//     }
//     if (API_wallFront())
//     {
//         // printf("Set front wall");
//         API_setWall(x, y, orientationToDirection(orientation));
//     }
//     return;
// }

int checkGoal(int x, int y)
{
    if ((x == 7 || x == 8) && (y == 7 || y == 8))
    {
        printf("The goal has been reached");
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
// void updateWalls(int x, int y, int orientation, struct CellData walls[16][16])
// {
//     walls[x][y].center = EXPLORED;

//     switch (orientation % 4)
//     {
//     case 0:
//         if (API_wallLeft())
//         {
//             walls[x][y].west = WALL;
//             if (x > 0)
//             {
//                 walls[x - 1][y].east = WALL;
//             }
//         }
//         if (!API_wallLeft())
//         {
//             walls[x][y].west = WAY;
//             if (x > 0)
//             {
//                 walls[x - 1][y].east = WAY;
//             }
//         }

//         if (API_wallFront())
//         {
//             walls[x][y].north = WALL;
//             if (y < 15)
//             {
//                 walls[x][y + 1].south = WALL;
//             }
//         }
//         if (!API_wallFront())
//         {
//             walls[x][y].north = WAY;
//             if (y < 15)
//             {
//                 walls[x][y + 1].south = WAY;
//             }
//         }

//         if (API_wallRight())
//         {
//             walls[x][y].east = WALL;
//             if (x < 15)
//             {
//                 walls[x + 1][y].west = WALL;
//             }
//         }
//         if (!API_wallRight())
//         {
//             walls[x][y].east = WAY;
//             if (x < 15)
//             {
//                 walls[x + 1][y].west = WAY;
//             }
//         }

//         // wall to the back of the mouse is always a way, as the mouse came from there
//         // if(walls[x][y].south == UNKNOWN){
//         //             walls[x][y].south = WAY;
//         // }

//         break;
//     case 1:
//         if (API_wallLeft())
//         {
//             walls[x][y].north = WALL;
//             if (y < 15)
//             {
//                 walls[x][y + 1].south = WALL;
//             }
//         }
//         if (!API_wallLeft())
//         {
//             walls[x][y].north = WAY;
//             if (y < 15)
//             {
//                 walls[x][y + 1].south = WAY;
//             }
//         }

//         if (API_wallFront())
//         {
//             walls[x][y].east = WALL;
//             if (x < 15)
//             {
//                 walls[x + 1][y].west = WALL;
//             }
//         }
//         if (!API_wallFront())
//         {
//             walls[x][y].east = WAY;
//             if (x < 15)
//             {
//                 walls[x + 1][y].west = WAY;
//             }
//         }

//         if (API_wallRight())
//         {
//             walls[x][y].south = WALL;
//             if (y > 0)
//             {
//                 walls[x][y - 1].north = WALL;
//             }
//         }
//         if (!API_wallRight())
//         {
//             walls[x][y].south = WAY;
//             if (y > 0)
//             {
//                 walls[x][y - 1].north = WAY;
//             }
//         }
//         // if(walls[x][y].west == UNKNOWN){
//         //             walls[x][y].west = WAY;
//         // }

//         break;
//     case 2:
//         if (API_wallLeft())
//         {
//             walls[x][y].east = WALL;
//             if (x < 15)
//             {
//                 walls[x + 1][y].west = WALL;
//             }
//         }
//         if (!API_wallLeft())
//         {
//             walls[x][y].east = WAY;
//             if (x < 15)
//             {
//                 walls[x + 1][y].west = WAY;
//             }
//         }

//         if (API_wallFront())
//         {
//             walls[x][y].south = WALL;
//             if (y > 0)
//             {
//                 walls[x][y - 1].north = WALL;
//             }
//         }
//         if (!API_wallFront())
//         {
//             walls[x][y].south = WAY;
//             if (y > 0)
//             {
//                 walls[x][y - 1].north = WAY;
//             }
//         }

//         if (API_wallRight())
//         {
//             walls[x][y].west = WALL;
//             if (x > 0)
//             {
//                 walls[x - 1][y].east = WALL;
//             }
//         }
//         if (!API_wallRight())
//         {
//             walls[x][y].west = WAY;
//             if (x > 0)
//             {
//                 walls[x - 1][y].east = WAY;
//             }
//         }
//         // if(walls[x][y].north == UNKNOWN){
//         //             walls[x][y].north = WAY;
//         // }

//         break;
//     case 3:
//         if (API_wallLeft())
//         {
//             walls[x][y].south = WALL;
//             if (y > 0)
//             {
//                 walls[x][y - 1].north = WALL;
//             }
//         }
//         if (!API_wallLeft())
//         {
//             walls[x][y].south = WAY;
//             if (y > 0)
//             {
//                 walls[x][y - 1].north = WAY;
//             }
//         }

//         if (API_wallFront())
//         {
//             walls[x][y].west = WALL;
//             if (x > 0)
//             {
//                 walls[x - 1][y].east = WALL;
//             }
//         }
//         if (!API_wallFront())
//         {
//             walls[x][y].west = WAY;
//             if (x > 0)
//             {
//                 walls[x - 1][y].east = WAY;
//             }
//         }
//         if (API_wallRight())
//         {
//             walls[x][y].north = WALL;
//             if (y < 15)
//             {
//                 walls[x][y + 1].south = WALL;
//             }
//         }
//         if (!API_wallRight())
//         {
//             walls[x][y].north = WAY;
//             if (y < 15)
//             {
//                 walls[x][y + 1].south = WAY;
//             }
//         }
//         // if(walls[x][y].east == UNKNOWN){
//         //             walls[x][y].east = WAY;
//         // }

//         break;
//     default:
//         printf("walls were not set correctly");
//         break;
//     }
//     return;
// }

int MAZE_SIZE = 16;

// prints the maze with each cell.
// | or - represents a wall
// x is set for an unknown wall
// * is set for a way
// Center is represented by Unexplored / Explored
void printMaze(struct CellData maze[MAZE_SIZE][MAZE_SIZE])
{
    //     printfEnter("______________________________________");
    //     printfEnter("______________ NEW MAZE ______________");
    //     printfEnter("______________________________________");

    // Print the top border
    printf(" ");
    printf("+");
    printf("\n");
    // printf("+" );
    // for (int col = 0; col < MAZE_SIZE; col++) {
    //     printf("---+");
    // }

    for (int row = MAZE_SIZE - 1; row >= 0; row--)
    {
        // Print the contents of the current row
        // print separation row
        printf(" ");
        printf("+");
        for (int col = 0; col < MAZE_SIZE; col++)
        {
            printf("---+");
        }
        printf("\n");

        // print north wall
        printf(" ");
        printf(" ");
        for (int col = 0; col < MAZE_SIZE; col++)
        {
            printf("o");
            switch (maze[col][row].north)
            {
            case UNKNOWN:
                printf("x");
                break;
            case WALL:
                printf("–");
                break;
            case WAY:
                printf("*");
                break;
            case EXPLORED:
                printf("#");
                break;
            }
            printf("o ");
        }
        printf("\n");

        // add the row number in the beginning; two digits number move the cells
        // printf("|");
        for (int col = 0; col < MAZE_SIZE; col++)
        {
            printf(" ");

            // print the west wall of the current cell
            switch (maze[col][row].west)
            {
            case UNKNOWN:
                printf("x");
                break;
            case WALL:
                printf("|");
                break;
            case WAY:
                printf("*");
                break;
            case EXPLORED:
                printf("#");
                break;
            }
            // Print the center cell value
            switch (maze[col][row].center)
            {
            case UNKNOWN:
                printf("U");
                break;
            case WALL:
                printf("F");
                break;
            case WAY:
                printf("F");
                break;
            case EXPLORED:
                printf("E");
                break;
            }

            // Print the east wall of the current cell
            switch (maze[col][row].east)
            {
            case UNKNOWN:
                printf("x");
                break;
            case WALL:
                printf("|");
                break;
            case WAY:
                printf("*");
                break;
            case EXPLORED:
                printf("#");
                break;
            }
        }
        printf("\n");

        // print the south wall
        printf(" ");
        printf(" ");
        for (int col = 0; col < MAZE_SIZE; col++)
        {
            printf("o");
            switch (maze[col][row].south)
            {
            case UNKNOWN:
                printf("x");
                break;
            case WALL:
                printf("–");
                break;
            case WAY:
                printf("*");
                break;
            case EXPLORED:
                printf("#");
                break;
            }
            printf("o ");
        }
        printf("\n");
    }

    // Print the row's bottom border
    printf("+");
    for (int col = 0; col < MAZE_SIZE; col++)
    {
        printf("-");
        // printf("%i", col);
        printf("-+");
    }
    printf("\n");
}

// function that prints relevant info for the current point
// void printUpdate(int x, int y, int distance[16][16], struct CellData walls[16][16], int distanceOfNeighbouringCells[4])
// {
//     printf("/////////////////UPDATE////////////////////");
//     printf("Current Position x and y: ");
//     printIntToConsolewithSpace(x);
//     printIntToConsolewithSpace(y);
//     printf("\n");
//     printf("Distance of current position:");
//     printIntToConsolewithSpace(distance[x][y]);
//     printf("\n");
//     printf("Walls of Current Position n, e, s, w (0 = unknown, 1 = wall, 2 = way):");
//     printIntToConsolewithSpace(walls[x][y].north);
//     printIntToConsolewithSpace(walls[x][y].east);
//     printIntToConsolewithSpace(walls[x][y].south);
//     printIntToConsolewithSpace(walls[x][y].west);
//     printf("\n");
//     printf("Distance of Neighbouring cells n, e, s, w:");
//     for (int i = 0; i < 4; i++)
//     {
//         printIntToConsolewithSpace(distanceOfNeighbouringCells[i]);
//     }
//     printf("\n");
//     printf("/////////////////END////////////////////");
//     return;
// }

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

void initDistance(int distance[MAZE_SIZE][MAZE_SIZE])
{
    int n = MAZE_SIZE / 2;
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            distance[i][j] = 14 - i - j;
        }
    }

    // mirror to the bottom left quadrant

    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            distance[i + n][j] = distance[n - 1 - i][j];
        }
    }

    // mirror to the top right quadrant
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < n; j++)
        {
            distance[i][j + n] = distance[i][n - 1 - j];
        }
    }

    // mirror to the bottom right quadrant

    for (int i = 0; i < MAZE_SIZE; i++)
    {
        for (int j = 0; j < n; j++)
        {
            distance[i + n][j + n] = distance[n - 1 - i][n - 1 - j];
        }
    }
}

// print the distance array to the console for verification
void printDistance_array(int distance[MAZE_SIZE][MAZE_SIZE])
{
    for (int i = 0; i < MAZE_SIZE; i++)
    {
        for (int j = 0; j < MAZE_SIZE; j++)
        {
            printf("%i  ", distance[i][j]);
        }
        printf("\n");
    }
}

// void relative_direction_next_cell(int lowestNeighbour, int direction_mouse){
//         int orientation = direction_mouse % 4;

//         if(lowestNeighbour == orientation){
//             printf("mouse does not need to turn");
//         }
//         else if(lowestNeighbour - orientation == abs(2)){
//             printf("Lowest neighbour behind. mouse needs to turn 180 degrees");
//             API_turnRight();
//             orientation = orientation + 1;
//             API_turnRight();
//             orientation = orientation + 1;
//         }
//         else if(lowestNeighbour - orientation == 1 || lowestNeighbour - orientation == -3){
//             printf("Lowest neighbour to the right. One right turn");
//             API_turnRight();
//             orientation = orientation + 1;
//         }

//         else if(lowestNeighbour - orientation == -1 || lowestNeighbour - orientation == 3){
//             printf("lowest neighbour is to the left. One left turn");
//             API_turnLeft();
//             orientation = orientation - 1;
//         }
//         else{
//             printf("!!!!!!!!!!!!!ERROR!!!!!!!!!!!");
//             printf("Could not turn towards neighbour with lowest distance");
//         }
// }

/*///////////////////////
*
* Starting with main
*
///////////////////////*/

int main(int argc, char *argv[])
{
    struct CellData walls[16][16];
    int orientation = 15;
    int x = 0;
    int y = 0;
    int distance[MAZE_SIZE][MAZE_SIZE];
    int distance_Open_Neighbours[4];
    int lowestNeighbour = 6;
    int temp = 0;
    int a = 0;
    // int temp_orientation = 0;

    initDistance(distance);
    printDistance_array(distance);

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

    /////////////////////////////////
    // left wall following algorithm
    //
    /////////////////////////////////
    while (checkGoal(x, y))
    {

        // setWallsforMMS(x, y, orientation);
        // updateWalls(x, y, orientation, walls);
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

        // printfEnter("distance_Open_Neighbours initialized with 255");
        // printIntToConsolewithSpace(distance_Open_Neighbours[0]);
        // printIntToConsolewithSpace(distance_Open_Neighbours[1]);
        // printIntToConsolewithSpace(distance_Open_Neighbours[2]);
        // printIntToConsolewithSpace(distance_Open_Neighbours[3]);
        // printf("\n");

        if (walls[x][y].north == WAY)
        {
            distance_Open_Neighbours[0] = distance[x][y + 1];
            // printfEnter("North = way; Push distance!");
            // printfEnter("the distance of the point north is:");
            // printIntToConsolewithSpace(distance_Open_Neighbours[0]);
            // printfEnter("\n");
        }
        if (walls[x][y].east == WAY)
        {
            distance_Open_Neighbours[1] = distance[x + 1][y];
            // printfEnter("East = way; Push distance!");
            // printfEnter("the distance of the point east is:");
            // printIntToConsolewithSpace(distance_Open_Neighbours[1]);
            // printfEnter("\n");
        }
        if (walls[x][y].south == WAY)
        {
            distance_Open_Neighbours[2] = distance[x][y - 1];
            // printfEnter("South = way; Push distance!");
            // printfEnter("the distance of the point south is:");
            // printIntToConsolewithSpace(distance_Open_Neighbours[2]);
            // printfEnter("\n");
        }
        if (walls[x][y].west == WAY)
        {
            distance_Open_Neighbours[3] = distance[x - 1][y];
            // printfEnter("West = way; Push distance!");
            // printfEnter("the distance of the point west is:");
            // printIntToConsolewithSpace(distance_Open_Neighbours[3]);
            // printfEnter("\n");
        }

        // printfEnter("distance_Open_Neighbours after update");
        // printIntToConsolewithSpace(distance_Open_Neighbours[0]);
        // printIntToConsolewithSpace(distance_Open_Neighbours[1]);
        // printIntToConsolewithSpace(distance_Open_Neighbours[2]);
        // printIntToConsolewithSpace(distance_Open_Neighbours[3]);
        // printf("\n");

        // lowestNeighbour = findlowestDistance(distance_Open_Neighbours);
        printf("lowestNeighbour is:\n");
        printf("%i", lowestNeighbour);
        printf("\n");
        printf("Current Orientation is:\n");
        printf("%d", orientation % 4);
        printf("\n");

        // determine turns to get to the lowest neighbour
        lowestNeighbour = 1;
        a = orientation % 4;
        temp = lowestNeighbour - (orientation % 4);
        

        // if((orientation%4) == lowestNeighbour){
        //     printf("mouse does not need to turn");
        // }
        // else if(temp == 2 || temp == -2){
        //     printf("Lowest neighbour behind. mouse needs to turn 180 degrees");
        //     // API_turnRight();
        //     // orientation = orientation + 1;
        //     // API_turnRight();
        //     // orientation = orientation + 1;
        // }
        // else if(lowestNeighbour - (orientation%4) == 1 || lowestNeighbour - (orientation%4) == -3){
        //     printf("Lowest neighbour to the right. One right turn");
        //     // API_turnRight();
        //     // orientation = orientation + 1;
        // }

        // else if(lowestNeighbour - (orientation%4) == -1 || lowestNeighbour - (orientation%4) == 3){
        //     printf("lowest neighbour is to the left. One left turn");
        //     // API_turnLeft();
        //     // orientation = orientation - 1;
        // }
        // else{
        //     printf("!!!!!!!!!!!!!ERROR!!!!!!!!!!!");
        //     printf("Could not turn towards neighbour with lowest distance");
        // }

        switch (lowestNeighbour - (orientation % 4))
        {
        case 0:
            printf("mouse does not need to turn");
            break;
        case 2:
        case -2:
            printf("Lowest neighbour behind. mouse needs to turn 180 degrees");
            break;
        case 1:
        case -3:
            printf("Lowest neighbour to the right. One right turn");
            break;
        case -1:
        case 3:
            printf("lowest neighbour is to the left. One left turn");
            break;
        default:
            printf("!!!!!!!!!!!!!ERROR!!!!!!!!!!!");
            printf("Could not turn towards neighbour with lowest distance");
        }

        /////////////////////////////////
        // find relative position the open neighbour with lowest distance
        /////////////////////////////////

        // printfEnter("lowestNeighbour is:");
        // printIntToConsole(lowestNeighbour);
        // printf("\n");
        // printfEnter("Current Orientation is:");
        // printIntToConsole(orientation % 4);
        // printf("\n");

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

        // orientation = temp_orientation;

        // switch (orientation % 4)
        // {
        // case 0:
        //     printfEnter("Orientation is north");
        //     break;
        // case 1:
        //     printfEnter("Orientation is east");
        //     break;
        // case 2:
        //     printfEnter("Orientation is south");
        //     break;
        // case 3:
        //     printfEnter("Orientation is west");
        //     break;
        // default:
        //     printfEnter("orientation is invalid");
        // }

        switch (orientation % 4)
        {
        case 0:
            y++;
            // printfEnter("Orientation is north");
            break;
        case 1:
            x++;
            // printfEnter("Orientation is east");
            break;
        case 2:
            y--;
            // printfEnter("Orientation is south");
            break;
        case 3:
            x--;
            // printfEnter("Orientation is west");
            break;
        default:
            printf("orientation is invalid");
        }
        orientation++;
    }
}