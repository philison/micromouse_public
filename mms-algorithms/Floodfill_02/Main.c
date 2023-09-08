#include <stdio.h>
#include <math.h>
#include "API.h"

// global variables

void printtoconsole(char* text) {
    fprintf(stderr, "%s\n", text);
    fflush(stderr);
}

void printIntToConsole(int number) {
    fprintf(stderr, "%d    ", number);
    fflush(stderr);
}

// some stack functionality


int stack[255];
int top = -1; // Initialize the top of the stack to -1

void push(int value) {
    if (top < 255 - 1) {
        stack[++top] = value;
    } else {
    }
}

int pop() {
    if (top >= 0) {
        return stack[top--];
    } else {
        return -1; // Return an error value
    }
}
void clearStack() {
    top = -1; // Reset the top index to empty the stack
}

// other maze solving systems

int checkGoal(int x, int y){
    if((x == 7 || x == 8 ) && (y == 7 || y == 8) ){
        printtoconsole("The goal has been reached");
        return 0;
    }
    return 1;
}

char orientationToDirection(int orientation){
    char direction;
    switch (orientation){
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


void setWallsforMMS(x_pos, y_pos, direction){

    if(API_wallLeft()){ 
        int orientation =  direction - 1;
        if(orientation < 0 ){
            orientation = 3;
        }
        API_setWall(x_pos, y_pos, orientationToDirection(orientation));
    }

    if(API_wallRight()){ 
        int orientation =  direction + 1;
        if(orientation == 4 ){
            orientation = 0;
        }
        API_setWall(x_pos, y_pos, orientationToDirection(orientation));
    }

    if(API_wallFront()){ 
        API_setWall(x_pos, y_pos, orientationToDirection(direction));
    }
    return;

}

// create enums and struct for an array containing all wall positions of the maze
    // Define an enum for the possible values

enum CellValue {
    UNKNOWN,
    WALL,
    WAY
};

// Define a structure to hold the four values
struct CellData {
    enum CellValue north;
    enum CellValue east;
    enum CellValue south;
    enum CellValue west;
};


void setWallsforArray(int x_pos, int y_pos, int direction, struct CellData walls[16][16]){
    switch (direction)
    {
    case 0:
        if(API_wallLeft()){
            walls[x_pos][y_pos].west = WALL;
        }
        else{walls[x_pos][y_pos].west = WAY;}

        if(API_wallFront()){
            walls[x_pos][y_pos].north = WALL;
        }
        else{walls[x_pos][y_pos].north = WAY;}

        if(API_wallRight()){
            walls[x_pos][y_pos].east = WALL;
        }
        else{walls[x_pos][y_pos].east = WAY;}

        break;
    case 1:
        if(API_wallLeft()){
            walls[x_pos][y_pos].north = WALL;
        }
        else{walls[x_pos][y_pos].north = WAY;}

        if(API_wallFront()){
            walls[x_pos][y_pos].east = WALL;
        }
        else{walls[x_pos][y_pos].east = WAY;}

        if(API_wallRight()){
            walls[x_pos][y_pos].south = WALL;
        }
        else{walls[x_pos][y_pos].south = WAY;}
        
        break;
    case 2:
        if(API_wallLeft()){
            walls[x_pos][y_pos].east = WALL;
        }
        else{walls[x_pos][y_pos].east = WAY;}

        if(API_wallFront()){
            walls[x_pos][y_pos].south = WALL;
        }
        else{walls[x_pos][y_pos].south = WAY;}
        
        if(API_wallRight()){
            walls[x_pos][y_pos].west = WALL;
        }
        else{walls[x_pos][y_pos].west = WAY;}

        break;
    case 3:
        if(API_wallLeft()){
            walls[x_pos][y_pos].south = WALL;
        }
        else{walls[x_pos][y_pos].south = WAY;}

        if(API_wallFront()){
            walls[x_pos][y_pos].west = WALL;
        }
        else{walls[x_pos][y_pos].west = WAY;}
        if(API_wallRight()){
            walls[x_pos][y_pos].north = WALL;
        }
        else{walls[x_pos][y_pos].north = WAY;}

        break;
    default:
        break;
    }
    return;

}


int findlowestDistance(int arr[]) {
    int size = 4;
    int lowestValue = arr[0]; // Initialize with the first element
    int lowestPosition = 0;   // Initialize with the position of the first element

    for (int i = 1; i < size; i++) {
        if (arr[i] < lowestValue) {
            lowestValue = arr[i];  // Update the lowest value
            lowestPosition = i;    // Update the position of the lowest value
        }
    }

    return lowestPosition;
}




int main(int argc, char* argv[]) {

    struct CellData walls[16][16];

    int size = 16;
    int maze[size][size];

// initialize all walls to UNKNOWN
    for (int i = 0; i < 16; i++) {
        for (int j = 0; j < 16; j++) {
            walls[i][j].north = UNKNOWN;
            walls[i][j].east = UNKNOWN;
            walls[i][j].south = UNKNOWN;
            walls[i][j].west = UNKNOWN;
        }
    }
    // initialize all upper cells with north walls
    for (int i = 0; i < 16; i++)
    {
        walls[0][i].north = WALL;
        walls[i][0].west = WALL;
        walls[15][i].east = WALL;
        walls[i][15].south = WALL;
    }
    // just printing for verification

    // printIntToConsole(walls[15][15].north);
    // printIntToConsole(walls[15][15].east);
    // printIntToConsole(walls[15][15].south);
    // printIntToConsole(walls[15][15].west);
    // printtoconsole("\n");
    

////////////////////////////////////////////////////////////////////////////////
    // initialize array with distance to the center: 
    int n = size/2; // Size of the 2D array 
    int distance[size][size];
    
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            distance[i][j] = 14 - i - j;
        }
    }
    
    //mirror to the bottom left quadrant
    
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            distance[i + n][j] = distance[n - 1 - i][j];
        }
    }
    
    // mirror to the top right quadrant
    for (int i = 0; i < n ; i++) {
        for (int j = 0; j < n; j++) {
            distance[i][j + n] = distance[i][n - 1 - j];
        }
    }
    
    //mirror to the bottom right quadrant
    
    for (int i = 0; i < n * 2; i++) {
        for (int j = 0; j < n; j++) {
            distance[i + n][j + n] = distance[n - 1 - i][n - 1 - j];
        }
    }

    //print the distance array to the console for verification
    for(int i = 0; i < size; i++){
        for(int j = 0; j < size; j++){
            printIntToConsole(distance[i][j]);
        }
        printtoconsole("\n");
    }

////////////////////////////////////////////////////////////////////////////////

// initialize the maze array, to track if a cell has been visited. 

    for(int i = 0; i < size; i++){
        for(int j = 0; j < size; j++){
            maze[i][j] = 0;
        }
    }
    //print the maze array
    // for(int i = 0; i < size; i++){
    //     for(int j = 0; j < size; j++){
    //         printIntToConsole(maze[i][j]);
    //     }
    //     printtoconsole("\n");
    // }

    // start with the walking

    printtoconsole("starting to explore...");

    int direction = 0; // either 0, 1, 2, 3 clockwise+
    int x_pos = 0;
    int y_pos = 0;
    int walking = 1;
    int goalPosition = 1;


    API_setColor(0, 0, 'G');
    API_setText(0, 0, "start");


   while (goalPosition == 1){

    setWallsforMMS(x_pos, y_pos, direction);

    

    ////////////////////////////
    /////modified Flood fill algorithm

    
    int lowestNeighbour = 6;
    int secondlowestNeighbour = 6;
    int distanceOfNeighbouringCells[4]; // position 0 is current cell, 
    int currentNodeDistance;
    // 1 is north
    // 2 east
    // 3 south
    // 4 west
    for(int i = 0; i < 4; i++){
        distanceOfNeighbouringCells[i]= 255;
    }
    
    for(int i = 0; i < 5; i++){
        printIntToConsole(distanceOfNeighbouringCells[i]);
    }
    printtoconsole("\n");

    printtoconsole("current distance:");
    printIntToConsole(distance[x_pos][y_pos]);
    printtoconsole("\n");

// set walls in array
    setWallsforArray(x_pos, y_pos, direction, walls);

// push current node for comparison
    push(distance[x_pos][y_pos]);

// updating distances of all necessary surrounding cells
    int current_x = x_pos;
    int current_y = y_pos;

    while(top != -1){ //keep going until stack is empty

        currentNodeDistance = pop();
        
// add all open neighbouring cells to the array distanceOfNeighbouringCells
        if(walls[current_x][current_y].north == WAY){
                    distanceOfNeighbouringCells[0] = distance[current_x][current_y + 1];
        }
        if(walls[current_x][current_y].east == WAY){
                    distanceOfNeighbouringCells[1] = distance[current_x + 1][current_y];
        }
        if(walls[current_x][current_y].south == WAY){
                    distanceOfNeighbouringCells[2] = distance[current_x][current_y - 1];
        }
        if(walls[current_x][current_y].west == WAY){
                    distanceOfNeighbouringCells[3] = distance[current_x - 1][current_y];
        }
    //    if the current is ony higher than the lowest, all good and we can repeat
        if(currentNodeDistance == findlowestDistance(distanceOfNeighbouringCells) + 1){
        }
        // if the current is the lowest, than we have to increase the current by 1 + min neighbour and push the min neighbour in the stack
        else{
            distance[current_x][current_y] = 1 + distanceOfNeighbouringCells[findlowestDistance(distanceOfNeighbouringCells)];
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



// moving to the neighbouring cell with the lowest distance
    switch (direction)
    {
    case 0:
        if(!API_wallLeft()){
            distanceOfNeighbouringCells[3] = distance[x_pos - 1][y_pos];
            printtoconsole("no wall left, push left distance");
        }
        if(!API_wallFront()){
            distanceOfNeighbouringCells[1] = distance[x_pos][y_pos + 1];
            printtoconsole("no wall infront, push front distance");
            printtoconsole("the distance of the point ahead is:");
            printIntToConsole(distanceOfNeighbouringCells[1]);
            printtoconsole("\n");
        }
        if(!API_wallRight()){
            distanceOfNeighbouringCells [2] = distance[x_pos + 1][y_pos];
            printtoconsole("no wall right, push right distance");

        }
        // find the lowest neighbour direction
        lowestNeighbour = findlowestDistance(distanceOfNeighbouringCells);
        printtoconsole("distances of neighbouring cells: ");
        for(int i = 0; i < 5; i++){
        printIntToConsole(distanceOfNeighbouringCells[i]);
        }   
        printtoconsole("\n");

        printtoconsole("lowest neighbour straight ahead");
        printIntToConsole(lowestNeighbour);
        printtoconsole("\n");

        // update current position, if all neighbours are higher
        if(lowestNeighbour == 0){
            distanceOfNeighbouringCells[0] = 255;
            secondlowestNeighbour = findlowestDistance(distanceOfNeighbouringCells);
            distance[x_pos][y_pos] = distanceOfNeighbouringCells[secondlowestNeighbour] + 1;
        }
        // turn towards lowest neighbour
        if (lowestNeighbour == 2){
            API_turnRight();
            direction = direction + 1;
            if(direction == 4){
            direction = 0;
            }
        }
        else if(lowestNeighbour == 4){
            API_turnLeft();
            direction =  direction - 1;
            if(direction < 0 ){
            direction = 3;
            }
        }
        API_moveForward();
        break;

    case 1:
        if(!API_wallLeft()){
            distanceOfNeighbouringCells [1] = distance[x_pos][y_pos + 1];
        }
        if(!API_wallFront()){
            distanceOfNeighbouringCells [2] = distance[x_pos + 1][y_pos];
        }
        if(!API_wallRight()){
            distanceOfNeighbouringCells [3] = distance[x_pos][y_pos - 1];
        }
        // find the lowest neighbour direction
        lowestNeighbour = findlowestDistance(distanceOfNeighbouringCells);
        // update current position, if neighbours are all higher
        if(lowestNeighbour == 0){
            distanceOfNeighbouringCells[0] = 255;
            secondlowestNeighbour = findlowestDistance(distanceOfNeighbouringCells);
            distance[x_pos][y_pos] = distanceOfNeighbouringCells[secondlowestNeighbour] + 1;
        }
        // turn towards lowest neighbour
        if (lowestNeighbour == 3){
            API_turnRight();
            direction = direction + 1;
            if(direction == 4){
            direction = 0;
            }
        }
        else if(lowestNeighbour == 1){
            API_turnLeft();
            direction =  direction - 1;
            if(direction < 0 ){
            direction = 3;
            }
        }
        API_moveForward();
        break;

    case 2:
        if(!API_wallLeft()){
            distanceOfNeighbouringCells[2] = distance[x_pos + 1][y_pos];
        }
        if(!API_wallFront()){
            distanceOfNeighbouringCells[3] = distance[x_pos][y_pos - 1];
        }
        if(!API_wallRight()){
            distanceOfNeighbouringCells[3] = distance[x_pos - 1][y_pos];
        }
        // find the lowest neighbour direction
        lowestNeighbour = findlowestDistance(distanceOfNeighbouringCells);
        // update current position, if neighbours are all higher
        if(lowestNeighbour == 0){
            distanceOfNeighbouringCells[0] = 255;
            secondlowestNeighbour = findlowestDistance(distanceOfNeighbouringCells);
            distance[x_pos][y_pos] = distanceOfNeighbouringCells[secondlowestNeighbour] + 1;
        }
        // turn towards lowest neighbour
        if (lowestNeighbour == 4){
            API_turnRight();
            direction = direction + 1;
            if(direction == 4){
            direction = 0;
            }
        }
        else if(lowestNeighbour == 2){
            API_turnLeft();
            direction =  direction - 1;
            if(direction < 0 ){
            direction = 3;
            }
        }
        API_moveForward();

        break;

    case 3:
        if(!API_wallLeft()){
            distanceOfNeighbouringCells[3] = distance[x_pos][y_pos - 1];
        }
        if(!API_wallFront()){
            distanceOfNeighbouringCells[3] = distance[x_pos - 1][y_pos];
        }
        if(!API_wallRight()){
            distanceOfNeighbouringCells[3] = distance[x_pos][y_pos - 1];
        }
        // find the lowest neighbour direction
        lowestNeighbour = findlowestDistance(distanceOfNeighbouringCells);
        // update current position, if neighbours are all higher
        if(lowestNeighbour == 0){
            distanceOfNeighbouringCells[0] = 255;
            secondlowestNeighbour = findlowestDistance(distanceOfNeighbouringCells);
            distance[x_pos][y_pos] = distanceOfNeighbouringCells[secondlowestNeighbour] + 1;
        }
        // turn towards lowest neighbour
        if (lowestNeighbour == 1){
            API_turnRight();
            direction = direction + 1;
            if(direction == 4){
            direction = 0;
            }
        }
        else if(lowestNeighbour == 3){
            API_turnLeft();
            direction =  direction - 1;
            if(direction < 0 ){
            direction = 3;
            }
        }
        API_moveForward();

        break;
    default:
        break;
    }

 //print the distance array to the console for verification
    for(int i = 0; i < size; i++){
        for(int j = 0; j < size; j++){
            printIntToConsole(distance[j][i]);
        }
        printtoconsole("\n");
    }


//////////////////
/////// DFS


//setting walls at explored cells

   
//    following the left wall until dead end is reached

//check if unexplored neighbouring cells are there
// check that cell is within the maze and not outside

// switch (direction)
// {
// case 0:
//     if((maze[x_pos - 1][y_pos] == 0) && !API_wallLeft() ){
//         API_turnLeft();
//         direction =  direction - 1;
//         if(direction < 0 ){
//         direction = 3;
//         }
//     }
//     while(API_wallFront() /*|| maze[x_pos][y_pos + 1] == 1*/ ){
//         API_turnRight();
//         direction = direction + 1;
//         if(direction == 4){
//         direction = 0;
//         }
//     }
//     break;

// case 1:
//     if((maze[x_pos][y_pos + 1] == 0) && !API_wallLeft() ){
//         API_turnLeft();
//         direction =  direction - 1;
//         if(direction < 0 ){
//         direction = 3;
//         }
//     }
//     while(API_wallFront() /*|| maze[x_pos + 1][y_pos] == 1*/ ){
//         API_turnRight();
//         direction = direction + 1;
//         if(direction == 4){
//         direction = 0;
//         }
//     }
//     break;

// case 2:

//     if((maze[x_pos +1 ][y_pos] == 0) && !API_wallLeft() ){
//         API_turnLeft();
//         direction =  direction - 1;
//         if(direction < 0 ){
//         direction = 3;
//         }
//     }
//     while(API_wallFront()/* || maze[x_pos][y_pos - 1] == 1 */){
//         API_turnRight();
//         direction = direction + 1;
//         if(direction == 4){
//         direction = 0;
//         }
//     }
//     break;

// case 3:

//     if((maze[x_pos][y_pos - 1] == 0) && !API_wallLeft() ){
//         API_turnLeft();
//         direction =  direction - 1;
//         if(direction < 0 ){
//         direction = 3;
//         }
//     }
//     while(API_wallFront()/* || maze[x_pos - 1][y_pos] == 1*/ ){
//         API_turnRight();
//         direction = direction + 1;
//         if(direction == 4){
//         direction = 0;
//         }
//     }
//     break;

// default:
//     break;
// }

// API_moveForward();



/////////////////////////////////
//left wall following algorithm
    
    // if (!API_wallLeft()) {
    //     API_turnLeft();
    //     direction =  direction - 1;
    //     if(direction < 0 ){
    //         direction = 3;
    //     }
    // }
    // while (API_wallFront()) {
    //     API_turnRight();
    //     direction = direction + 1;
    //     if(direction == 4){
    //     direction = 0;
    //     }
    // }
    // API_moveForward(1);

// log the direction the mouse is going and updating the position.
    switch (direction){
        case 0:
            y_pos = y_pos + 1;
            //printtoconsole("up");
            //API_setColor(x_pos , y_pos, 'G');
            // maze[x_pos][y_pos] = 1;
            break;
        case 1: 
            x_pos = x_pos + 1;
            //printtoconsole("right");
            //API_setColor(x_pos , y_pos, 'G');
            // maze[x_pos][y_pos] = 1;
            break;
        case 2: 
            y_pos = y_pos - 1;
            //printtoconsole("down");
            //API_setColor(x_pos , y_pos, 'G');
            // maze[x_pos][y_pos] = 1;
            break;
        case 3:
            x_pos = x_pos - 1;
            //printtoconsole("left");
            //API_setColor(x_pos , y_pos, 'G');
            // maze[x_pos][y_pos] = 1;
            break;
        default: 
            printtoconsole("direction is invalid");

    }
//color the field the mouse has just visited.
        API_setColor(x_pos, y_pos, 'G');
        maze[x_pos][y_pos] = 1;  // somehow this leads to errors and wrong printing. 
        // should be done within the switch statements. 
// check if the goal is reached. 
        goalPosition = checkGoal(x_pos, y_pos);
        // print maze
        for(int i = 0; i < size; i++){
            for(int j = 0; j < size; j++){
                printIntToConsole(maze[j][i]);
            }
            printtoconsole("\n");
        }
    printtoconsole("-----------------------\n");

    
   }

   printtoconsole("the goal has been reached");
    return 0;
    
}