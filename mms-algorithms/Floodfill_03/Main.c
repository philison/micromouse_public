#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include "API.h"

// global variables

void printtoconsole(char *text)
{
    fprintf(stderr, "%s\n", text);
    fflush(stderr);
}

void printIntToConsole(int number)
{
    fprintf(stderr, "%d    ", number);
    fflush(stderr);
}

// stack functionality

int stack[255];
int top = -1; // Initialize the top of the stack to -1

void push(int value)
{
    if (top < 255 - 1)
    {
        stack[++top] = value;
    }
    else
    {
    }
}

int pop()
{
    if (top >= 0)
    {
        return stack[top--];
    }
    else
    {
        return -1; // Return an error value
    }
}
void clearStack()
{
    top = -1; // Reset the top index to empty the stack
}

// other maze solving systems

int checkGoal(int x, int y)
{
    if ((x == 7 || x == 8) && (y == 7 || y == 8))
    {
        printtoconsole("The goal has been reached");
        return 0;
    }
    return 1;
}

char orientationToDirection(int orientation)
{
    char direction;
    switch (orientation)
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
        printtoconsole("direction is invalid");
    }
    return direction;
}

void setWallsforMMS(x_pos, y_pos, orientation)
{

    if (API_wallLeft())
    {
        int orientation_wall = orientation - 1;
        if (orientation_wall < 0)
        {
            orientation_wall = 3;
        }
        API_setWall(x_pos, y_pos, orientationToDirection(orientation_wall));
    }

    if (API_wallRight())
    {
        int orientation_wall = orientation + 1;
        if (orientation_wall == 4)
        {
            orientation_wall = 0;
        }
        API_setWall(x_pos, y_pos, orientationToDirection(orientation_wall));
    }
    if (API_wallFront())
    {
        API_setWall(x_pos, y_pos, orientationToDirection(orientation));
    }
    return;
}

// create enums and struct for an array containing all wall positions of the maze
// Define an enum for the possible values

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


// wall to the back of the mouse is always a way, as the mouse came from there
// be carefull if mous moved forward, turned and then the walls are set.

void setWallsforArray(int x_pos, int y_pos, int orientation, struct CellData walls[16][16])
{
    orientation = orientation%4;

    switch (orientation)
    {
    case 0:
        if (API_wallLeft())
        {
            walls[x_pos][y_pos].west = WALL;
            if(x_pos >0){
                walls[x_pos - 1][y_pos].west = WALL;
            }
        }
        else
        {
            walls[x_pos][y_pos].west = WAY;
            if(x_pos >0){
                walls[x_pos - 1][y_pos].west = WAY;
            }
        }

        if (API_wallFront())
        {
            walls[x_pos][y_pos].north = WALL;
            if(y_pos < 15){
                walls[x_pos][y_pos + 1].north = WALL;
            }
        }
        else
        {
            walls[x_pos][y_pos].north = WAY;
            if(y_pos < 15){
                walls[x_pos][y_pos + 1].north = WAY;
            }
        }

        if (API_wallRight())
        {
            walls[x_pos][y_pos].east = WALL;
            if(x_pos < 15){
                walls[x_pos + 1][y_pos].east = WALL;
            }
        }
        else
        {
            walls[x_pos][y_pos].east = WAY;
            if(x_pos < 15){
                walls[x_pos + 1][y_pos].east = WAY;
            }
        }

// wall to the back of the mouse is always a way, as the mouse came from there
        // if(walls[x_pos][y_pos].south == UNKNOWN){
        //             walls[x_pos][y_pos].south = WAY;
        // }

        break;
    case 1:
        if (API_wallLeft())
        {
            walls[x_pos][y_pos].north = WALL;
            if(y_pos < 15){
                walls[x_pos][y_pos + 1].north = WALL;
            }
        }
        else
        {
            walls[x_pos][y_pos].north = WAY;
            if(y_pos < 15){
                walls[x_pos][y_pos + 1].north = WAY;
            }
            
        }

        if (API_wallFront())
        {
            walls[x_pos][y_pos].east = WALL;
            if(x_pos < 15){
                walls[x_pos + 1][y_pos].east = WALL;
            }
        }
        else
        {
            walls[x_pos][y_pos].east = WAY;
            if(x_pos < 15){
                walls[x_pos + 1][y_pos].east = WAY;
            }
        }

        if (API_wallRight())
        {
            walls[x_pos][y_pos].south = WALL;
            if(y_pos > 0){
                walls[x_pos][y_pos - 1].south = WALL;
            }
        }
        else
        {
            walls[x_pos][y_pos].south = WAY;
            if(y_pos > 0){
                walls[x_pos][y_pos - 1].south = WAY;
            }
        }
        // if(walls[x_pos][y_pos].west == UNKNOWN){
        //             walls[x_pos][y_pos].west = WAY;
        // }

        break;
    case 2:
        if (API_wallLeft())
        {
            walls[x_pos][y_pos].east = WALL;
            if(x_pos < 15){
                walls[x_pos + 1][y_pos].east = WALL;
            }
        }
        else
        {
            walls[x_pos][y_pos].east = WAY;
            if(x_pos < 15){
                walls[x_pos + 1][y_pos].east = WAY;
            }
        }

        if (API_wallFront())
        {
            walls[x_pos][y_pos].south = WALL;
            if(y_pos > 0){
                walls[x_pos][y_pos - 1].south = WALL;
            }
        }
        else
        {
            walls[x_pos][y_pos].south = WAY;
            if(y_pos > 0){
                walls[x_pos][y_pos - 1].south = WAY;
            }
        }

        if (API_wallRight())
        {
            walls[x_pos][y_pos].west = WALL;
            if(x_pos >0){
                walls[x_pos - 1][y_pos].west = WALL;
            }
        }
        else
        {
            walls[x_pos][y_pos].west = WAY;
            if(x_pos >0){
                walls[x_pos - 1][y_pos].west = WAY;
            }
        }
        // if(walls[x_pos][y_pos].north == UNKNOWN){
        //             walls[x_pos][y_pos].north = WAY;
        // }

        break;
    case 3:
        if (API_wallLeft())
        {
            walls[x_pos][y_pos].south = WALL;
            if(y_pos > 0){
                walls[x_pos][y_pos - 1].south = WALL;
            }
        }
        else
        {
            walls[x_pos][y_pos].south = WAY;
            if(y_pos > 0){
                walls[x_pos][y_pos - 1].south = WAY;
            }
        }

        if (API_wallFront())
        {
            walls[x_pos][y_pos].west = WALL;
            if(x_pos >0){
                walls[x_pos - 1][y_pos].west = WALL;
            }
        }
        else
        {
            walls[x_pos][y_pos].west = WAY;
            if(x_pos >0){
                walls[x_pos - 1][y_pos].west = WAY;
            }
        }
        if (API_wallRight())
        {
            walls[x_pos][y_pos].north = WALL;
            if( y_pos < 15){
                walls[x_pos][y_pos + 1].north = WALL;
            }
        }
        else
        {
            walls[x_pos][y_pos].north = WAY;
            if(y_pos < 15){
                walls[x_pos][y_pos + 1].north = WAY;
            }
        }
        // if(walls[x_pos][y_pos].east == UNKNOWN){
        //             walls[x_pos][y_pos].east = WAY;
        // }

        break;
    default:
        printtoconsole("walls were not set correctly");
        break;
    }
    return;
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


// function to print several values as an update
void printUpdate(int x_pos, int y_pos, int distance[16][16], struct CellData walls[16][16], int distanceOfNeighbouringCells[4] ){
    printtoconsole("/////////////////UPDATE////////////////////");
    printtoconsole("Current Position x_pos and y_pos: ");
    printIntToConsole(x_pos);
    printIntToConsole(y_pos);
    printtoconsole("\n");
    printtoconsole("Distance of current position:");
    printIntToConsole(distance[x_pos][y_pos]);
    printtoconsole("\n");
    printtoconsole("Walls of Current Position n, e, s, w (0 = unknown, 1 = wall, 2 = way):");
    printIntToConsole(walls[x_pos][y_pos].north);
    printIntToConsole(walls[x_pos][y_pos].east);
    printIntToConsole(walls[x_pos][y_pos].south);
    printIntToConsole(walls[x_pos][y_pos].west);
    printtoconsole("\n");
    printtoconsole("Distance of Neighbouring cells n, e, s, w:");
          for (int i = 0; i < 4; i++)
        {
            printIntToConsole(distanceOfNeighbouringCells[i]);
        }
        printtoconsole("\n");
    printtoconsole("/////////////////END////////////////////");
    return;

}

int main(int argc, char *argv[])
{

    struct CellData walls[16][16];

    int size = 16;
    int visited[size][size];

    int orientation = 0; // either 0, 1, 2, 3 clockwise+
    int x_pos = 0;
    int y_pos = 0;
    int walking = 1;
    int goalPosition = 1;

    int lowestNeighbour = 6;
    int currentNodeDistance;
    int distanceOfNeighbouringCells[4];
        // 0 is north
        // 1 east
        // 2 south
        // 3 west

    // initialize all walls to UNKNOWN
    for (int i = 0; i < 16; i++)
    {
        for (int j = 0; j < 16; j++)
        {
            walls[i][j].north = UNKNOWN;
            walls[i][j].east = UNKNOWN;
            walls[i][j].south = UNKNOWN;
            walls[i][j].west = UNKNOWN;
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
    //////////////////////////////////////////////////////
    //////////////////////////////////////////////////////
    // printing for verification
    // printtoconsole("Testing the wall initialization");
    // printtoconsole("upper left corner");
    // printIntToConsole(walls[0][0].north);
    // printIntToConsole(walls[0][0].east);
    // printIntToConsole(walls[0][0].south);
    // printIntToConsole(walls[0][0].west);
    // printtoconsole("\n");
    // printtoconsole("lower right corner");
    // printIntToConsole(walls[15][15].north);
    // printIntToConsole(walls[15][15].east);
    // printIntToConsole(walls[15][15].south);
    // printIntToConsole(walls[15][15].west);
    // printtoconsole("\n");
    // printtoconsole("middle of upper side");
    // printIntToConsole(walls[7][0].north);
    // printIntToConsole(walls[7][0].east);
    // printIntToConsole(walls[7][0].south);
    // printIntToConsole(walls[7][0].west);
    // printtoconsole("\n");
    // printtoconsole("middle of south side");
    // printIntToConsole(walls[7][15].north);
    // printIntToConsole(walls[7][15].east);
    // printIntToConsole(walls[7][15].south);
    // printIntToConsole(walls[7][15].west);
    // printtoconsole("\n");
    // printtoconsole("middle of west side");
    // printIntToConsole(walls[0][7].north);
    // printIntToConsole(walls[0][1].east);
    // printIntToConsole(walls[0][1].south);
    // printIntToConsole(walls[0][1].west);
    // printtoconsole("\n");
    // printtoconsole("middle of east side");
    // printIntToConsole(walls[15][7].north);
    // printIntToConsole(walls[15][1].east);
    // printIntToConsole(walls[15][1].south);
    // printIntToConsole(walls[15][1].west);
    // printtoconsole("\n");

    ////////////////////////////////////////////////////////////////////////////////
    // initialize array with distance to the center:
    int n = size / 2; // Size of the 2D array
    int distance[size][size];

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

    for (int i = 0; i < n * 2; i++)
    {
        for (int j = 0; j < n; j++)
        {
            distance[i + n][j + n] = distance[n - 1 - i][n - 1 - j];
        }
    }

    // print the distance array to the console for verification
    for (int i = 0; i < size; i++)
    {
        for (int j = 0; j < size; j++)
        {
            printIntToConsole(distance[i][j]);
        }
        printtoconsole("\n");
    }

    ////////////////////////////////////////////////////////////////////////////////

    // initialize the visited array, to track if a cell has been visited.

    for (int i = 0; i < size; i++)
    {
        for (int j = 0; j < size; j++)
        {
            visited[i][j] = 0;
        }
    }
    // print the visited array
    //  for(int i = 0; i < size; i++){
    //      for(int j = 0; j < size; j++){
    //          printIntToConsole(visited[i][j]);
    //      }
    //      printtoconsole("\n");
    //  }

    // start with the walking


// //////////////////////   Init is finished



    visited[x_pos][y_pos] = 2;
    API_setColor(x_pos, y_pos, 'R');
    printtoconsole("one inital move forward");
    API_setText(0, 0, "Start");

    // one initial move to the front, to avoid a wall in the back o fthe mouse
    // necessary???????
    API_moveForward();
    y_pos = y_pos + 1;
    setWallsforMMS(x_pos, y_pos, orientation);
    setWallsforArray(x_pos, y_pos, orientation, walls);
    visited[x_pos][y_pos] = 1;


    printtoconsole("____----_____---______------_______-----______------______-------");
    printtoconsole("starting to explore...");
    printtoconsole("____----_____---______------_______-----______------______-------");

 

    while (goalPosition == 1)
    {

        API_setColor(x_pos, y_pos, 'G');
        visited[x_pos][y_pos] = 1; // somehow this leads to errors and wrong printing.
        // should be done within the switch statements.
        // check if the goal is reached.
        goalPosition = checkGoal(x_pos, y_pos);

        printUpdate(x_pos, y_pos, distance, walls, distanceOfNeighbouringCells);

        printtoconsole("_________________________");
        printtoconsole("Set Walls");

        // set walls for simulation and in array "walls"
        setWallsforMMS(x_pos, y_pos, orientation);

        setWallsforArray(x_pos, y_pos, orientation, walls);

        printUpdate(x_pos, y_pos, distance, walls, distanceOfNeighbouringCells);

        ////////////////////////////
        /////modified Flood fill algorithm

        
        for (int i = 0; i < 4; i++)
        {
            distanceOfNeighbouringCells[i] = 255;
        }

        // push current node for comparison
        push(distance[x_pos][y_pos]);

        // updating distances of all necessary surrounding cells
        int current_x = x_pos;
        int current_y = y_pos;

        printtoconsole("_________________________");
        printtoconsole("Starting Distance Update");
        printtoconsole("_________________________");

        while (top != -1)
        { // keep going until stack is empty

            for (int i = 0; i < 4; i++)
            {
                distanceOfNeighbouringCells[i] = 255;
            }

            currentNodeDistance = pop();
    // print statement to give necessary info for debugging
            printUpdate(x_pos, y_pos, distance, walls, distanceOfNeighbouringCells);

            // add all open neighbouring cells to the array distanceOfNeighbouringCells
            if (walls[current_x][current_y].north == (WAY || UNKNOWN) )
            {
                distanceOfNeighbouringCells[0] = distance[current_x][current_y + 1];
                printtoconsole("The node north of the current position is beeing pushed for comparison");
            }
            if (walls[current_x][current_y].east == (WAY || UNKNOWN ) )
            {
                distanceOfNeighbouringCells[1] = distance[current_x + 1][current_y];
                printtoconsole("The node east of the current position is beeing pushed for comparison");
            }
            if (walls[current_x][current_y].south == (WAY || UNKNOWN) )
            {
                distanceOfNeighbouringCells[2] = distance[current_x][current_y - 1];
                printtoconsole("The node south of the current position is beeing pushed for comparison");
            }
            if (walls[current_x][current_y].west == (WAY || UNKNOWN) )
            {
                distanceOfNeighbouringCells[3] = distance[current_x - 1][current_y];
                printtoconsole("The node west of the current position is beeing pushed for comparison");
            }

            //    if the current is one higher than the lowest, all good and we can repeat
            printtoconsole("Current Node Distance for comparison:");
            printIntToConsole(currentNodeDistance);
            printtoconsole("\n");
            printtoconsole("lowest distance +1:");
            int lowDistance = distanceOfNeighbouringCells[findlowestDistance(distanceOfNeighbouringCells)] + 1;
            printIntToConsole(lowDistance);
            printtoconsole("\n");

            if (currentNodeDistance == distanceOfNeighbouringCells[findlowestDistance(distanceOfNeighbouringCells)] + 1)
            {
                printtoconsole("Current node is lowest neighbour + 1");
            }
            // if the current is the lowest, than we have to increase the current by 1 + min neighbour and push the min neighbour in the stack
            else
            {
                printtoconsole("////////////////////////");
                printtoconsole("  //  UPDATE NODES  //  ");
                printtoconsole("////////////////////////");
                printtoconsole("distance of neighbouring cells:");
                for (int i = 0; i < 3; i++)
                {
                    printIntToConsole(distanceOfNeighbouringCells[i]);
                }
                printtoconsole("\n");
                printtoconsole("Before Update");
                printIntToConsole(distance[current_x][current_y]);
                printtoconsole("\n");

                distance[current_x][current_y] = 1 + distanceOfNeighbouringCells[findlowestDistance(distanceOfNeighbouringCells)];

                printtoconsole("After Update");
                printIntToConsole(distance[current_x][current_y]);
                printtoconsole("\n");

                switch (findlowestDistance(distanceOfNeighbouringCells))
                {
                case 0:
                    current_y = current_y + 1;
                    break;
                case 1:
                    current_x = current_x + 1;
                    break;
                case 2:
                    current_y = current_y - 1;
                    break;
                case 3:
                    current_x = current_x - 1;
                    break;

                default:
                    break;
                }
                // push open neighbours from this cell

                push(distance[current_x][current_y]);
            }
        }

        for (int i = 0; i < 4; i++)
        {
            distanceOfNeighbouringCells[i] = 255;
        }

        printtoconsole("_________________________");
        printtoconsole("Finished Distance Update");
        printtoconsole("_________________________");

        printtoconsole("------------------------");
        printtoconsole("Start Moving to the next cell");
        printtoconsole("------------------------");

        // moving to the neighbouring cell with the lowest distance

        // find lowest neighbour
        if (walls[x_pos][y_pos].north == WAY)
            {
                distanceOfNeighbouringCells[0] = distance[x_pos][y_pos + 1];
                printtoconsole("North = way; Push distance!");
                printtoconsole("the distance of the point north is:");
                printIntToConsole(distanceOfNeighbouringCells[0]);
                printtoconsole("\n");
            }

        if (walls[x_pos][y_pos].east == WAY)
            {
                distanceOfNeighbouringCells[1] = distance[x_pos + 1][y_pos];
                printtoconsole("East = way; Push distance!");
                printtoconsole("the distance of the point East is:");
                printIntToConsole(distanceOfNeighbouringCells[1]);
                printtoconsole("\n");
            }
            
        if (walls[x_pos][y_pos].south == WAY)
        {
                distanceOfNeighbouringCells[2] = distance[x_pos][y_pos - 1];
                printtoconsole("South = way; Push distance!");
                printtoconsole("the distance of the point South is:");
                printIntToConsole(distanceOfNeighbouringCells[2]);
                printtoconsole("\n");
        }
        if (walls[x_pos][y_pos].west == WAY)
        {
                distanceOfNeighbouringCells[3] = distance[x_pos - 1][y_pos];
                printtoconsole("West = way; Push distance!");
                printtoconsole("the distance of the point West is:");
                printIntToConsole(distanceOfNeighbouringCells[3]);
                printtoconsole("\n");
        }


        // find the lowest neighbour direction
        lowestNeighbour = findlowestDistance(distanceOfNeighbouringCells);
        printtoconsole("Lowest Neighbour is at position:");
        printIntToConsole(lowestNeighbour);
        printtoconsole("\n");
        printtoconsole("distances of neighbouring cells: ");
        for (int i = 0; i < 4; i++)
        {
            printIntToConsole(distanceOfNeighbouringCells[i]);
        }
        printtoconsole("\n");

        // turning mouse towards lowest neighbour

        if(lowestNeighbour == orientation){
            printtoconsole("mouse does not need to turn");
        }
        else if(lowestNeighbour - orientation == abs(2)){
            printtoconsole("Lowest neighbour behind. mouse needs to turn 180 degrees");
            API_turnRight();
            orientation = orientation + 1;
            API_turnRight();
            orientation = orientation + 1;
        }
        else if(lowestNeighbour - orientation == 1 || lowestNeighbour - orientation == -3){
            printtoconsole("Lowest neighbour to the right. One right turn");
            API_turnRight();
            orientation = orientation + 1;
        }

        else if(lowestNeighbour - orientation == -1 || lowestNeighbour - orientation == 3){
            printtoconsole("lowest neighbour is to the left. One left turn");
            API_turnLeft();
            orientation = orientation - 1;
        }
        else{
            printtoconsole("!!!!!!!!!!!!!ERROR!!!!!!!!!!!");
            printtoconsole("Could not turn towards neighbour with lowest distance");
        }
        API_moveForward();
    
        // print the distance array to the console for verification
        for (int i = 0; i < size; i++)
        {
            for (int j = 0; j < size; j++)
            {
                printIntToConsole(distance[j][i]);
            }
            printtoconsole("\n");
        }

        printtoconsole("------------------------");
        printtoconsole("updating position x_y after movement");
        printtoconsole("------------------------");
        printtoconsole("Position before Update:");
        printUpdate(x_pos, y_pos, distance, walls, distanceOfNeighbouringCells);

        // log the orientation the mouse is going and updating the position.
        switch (orientation)
        {
        case 0:
            y_pos = y_pos + 1;
            break;
        case 1:
            x_pos = x_pos + 1;
            break;
        case 2:
            y_pos = y_pos - 1;
            break;
        case 3:
            x_pos = x_pos - 1;
            break;
        default:
            printtoconsole("orientation is invalid");
        }

        printtoconsole("Position after Update:");
        printUpdate(x_pos, y_pos, distance, walls, distanceOfNeighbouringCells);

        printtoconsole("------------------------");
        printtoconsole("Finished Moving to the next cell");
        printtoconsole("------------------------");

        
        /////////////////////////////////
        // left wall following algorithm

        // if (!API_wallLeft()) {
        //     API_turnLeft();
        //     orientation =  orientation - 1;
        //     if(orientation < 0 ){
        //         orientation = 3;
        //     }
        // }
        // while (API_wallFront()) {
        //     API_turnRight();
        //     orientation = orientation + 1;
        //     if(orientation == 4){
        //     orientation = 0;
        //     }
        // }
        // API_moveForward(1);

        

        // print visited
        for (int i = 0; i < size; i++)
        {
            for (int j = 0; j < size; j++)
            {
                printIntToConsole(visited[j][i]);
            }
            printtoconsole("\n");
        }
        printtoconsole("-----------------------\n");
    }

    printtoconsole("the goal has been reached");
    return 0;
}