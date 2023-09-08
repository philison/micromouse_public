#include <stdio.h>
#include <math.h>
#include "API.h"
#include "init.h"

void printtoconsole(char* text) {
    fprintf(stderr, "%s\n", text);
    fflush(stderr);
}

void printIntToConsole(int number) {
    fprintf(stderr, "%d    ", number);
    fflush(stderr);
}

// bool reachedGoal(int x, int y){
//     if((x == 8 || x == 9 ) && (y == 8 || y == 9) ){
//         printtoconsole("The goal has been reached");
//         return TRUE;
//     }
//     return FALSE;
// }

// bool nextCellDiscovered(int x, int y, int direction){
//     // first check if cell outside of border
//     switch (direction){
//         case 0:
//             if(y_pos == )
//             if(maze[15 - y_pos + 1][x_pos] == 0){
//                 return FALSE;
//             }
//             return TRUE;
//             break;
//         case 1: 
//             if(maze[15 - y_pos][x_pos + 1] == 0){
//                 return FALSE;
//             }
//             return TRUE;
//             break;
//         case 2: 
//              if(maze[15 - y_pos -1][x_pos] == 0){
//                 return FALSE;
//             }
//             return TRUE;
//             break;
//         case 3:
//              if(maze[15 - y_pos][x_pos - 1] == 0){
//                 return FALSE;
//             }
//             return TRUE;
//             break;
//         default: 
//             printtoconsole("direction is invalid");

//     }
// }
 

int main(int argc, char* argv[]) {

    int size = 16;

    int maze[size][size];

////////////////////////////////////////////////////////////////////////////////
    // initialize array with distance to the center: 
    int n = size/2; // Size of the 2D array
    int array[n][n];
    
    int distance[size][size];

    // Initialize the array with the desired pattern
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            array[i][j] = 14 - i - j;
        }
    }
    
    //copy the initial array
    
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            distance[i][j] = array[i][j];
        }
    }
    
    //mirror to the bottom left quadrant
    
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            distance[i + n][j] = array[n - 1 - i][j];
        }
    }
    
    // mirror to the top right quadrant
    for (int i = 0; i < n ; i++) {
        for (int j = 0; j < n; j++) {
            distance[i][j + n] = array[i][n - 1 - j];
        }
    }
    
    //mirror to the bottom right quadrant

    
    for (int i = 0; i < n * 2; i++) {
        for (int j = 0; j < n; j++) {
            distance[i + n][j + n] = array[n - 1 - i][n - 1 - j];
        }
    }

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
    // for(int i = 0; i < size; i++){
    //     for(int j = 0; j < size; j++){
    //         printIntToConsole(maze[i][j]);
    //     }
    //     printtoconsole("\n");
    // }

    // starting with the walking


    printtoconsole("Running...");

    int direction = 0; // either 0, 1, 2, 3 clockwise+
    int x_pos = 0;
    int y_pos = 15;
    int walking = 1;

    API_setColor(0, 0, 'G');
    API_setText(0, 0, "start");


   while (walking){

    // if((maze[15-y_pos + 1][x_pos] == 0) && API_wallFront()){
    //     API_moveForward(1);
    // }

//normal
        // left wall following
    
    if (!API_wallLeft()) {
        API_turnLeft();
        direction =  direction - 1;
        if(direction < 0 ){
            direction = 3;
        }
    }
    while (API_wallFront()) {
        API_turnRight();
        direction = direction + 1;
        if(direction == 4){
        direction = 0;
        }
    }
    API_moveForward(1);

// //backtracking
//    if(maze[x_pos][y_pos] == 1){
//     API_turnLeft();
//       direction =  direction - 1;
//             if(direction < 0 ){
//                 direction = 3;
//             }
//     API_turnLeft();
//     direction =  direction - 1;
//             if(direction < 0 ){
//                 direction = 3;
//             }
//     if (!API_wallRight()) {
//            API_turnRight();
//             direction = direction + 1;
//             if(direction == 4){
//             direction = 0;
//             }
//         }
//         while (API_wallFront()) {
//            API_turnLeft();
//             direction =  direction - 1;
//             if(direction < 0 ){
//                 direction = 3;
//             }
//         }
//         API_moveForward(1);
//    }



// log the direction the mouse is going and updating the position.
    switch (direction){
        case 0:
            y_pos = y_pos + 1;
            //printtoconsole("up");
            //API_setColor(x_pos , y_pos, 'G');
            maze[y_pos][x_pos] = 1;
            break;
        case 1: 
            x_pos = x_pos + 1;
            //printtoconsole("right");
            //API_setColor(x_pos , y_pos, 'G');
            maze[y_pos][x_pos] = 1;
            break;
        case 2: 
            y_pos = y_pos - 1;
            //printtoconsole("down");
            //API_setColor(x_pos , y_pos, 'G');
            maze[y_pos][x_pos] = 1;
            break;
        case 3:
            x_pos = x_pos - 1;
            //printtoconsole("left");
            //API_setColor(x_pos , y_pos, 'G');
            maze[y_pos][x_pos] = 1;
            break;
        default: 
            printtoconsole("direction is invalid");

    }
//color the field the mouse has just visited.
        API_setColor(x_pos, y_pos, 'G');
        // maze[15 + x_pos][y_pos] = 1;  // somehow this leads to errors and wrong printing. 
        // should be done within the switch statements. 
        // print maze
        for(int i = 0; i < size; i++){
            for(int j = 0; j < size; j++){
                printIntToConsole(maze[i][j]);
            }
            printtoconsole("\n");
        }

    printtoconsole("-----------------------\n");


        // if(x_pos == 8 && y_pos == 8){
        //     walking = 0;
        //     printtoconsole("reached goal");
        // }



    
   }
    return 0;
    
}