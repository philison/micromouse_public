#include <stdio.h>
#include <stdbool.h>
#include <math.h>


// function to initialize an array with the size of the maze. The center, i.e. goal is set to zero. 
// Each cell represents the distance to the center. 
// int *initDistance(maze_size){
//     int n = maze_size / 2; // Size of the 2D array
//     int array[n][n];
    
//     int n_double = maze_size;
//     int distance[n_double][n_double];
    

//     // Initialize the upper left quadrant of the array that we can later mirror
//     for (int i = 0; i < n; i++) {
//         for (int j = 0; j < n; j++) {
//             array[i][j] = 14 - i - j;
//         }
//     }  
//     //copy the initial array to the final array in the upper left quadrant
//     for (int i = 0; i < n; i++) {
//         for (int j = 0; j < n; j++) {
//             distance[i][j] = array[i][j];
//         }
//     }
    
//     //mirror to the bottom left quadrant
//     for (int i = 0; i < n; i++) {
//         for (int j = 0; j < n; j++) {
//             distance[i + n][j] = array[n - 1 - i][j];
//         }
//     }
//     // mirror to the top right quadrant
//     for (int i = 0; i < n ; i++) {
//         for (int j = 0; j < n; j++) {
//             distance[i][j + n] = array[i][n - 1 - j];
//         }
//     }
//     //mirror to the bottom right quadrant
//     for (int i = 0; i < n * 2; i++) {
//         for (int j = 0; j < n; j++) {
//             distance[i + n][j + n] = array[n - 1 - i][n - 1 - j];
//         }
//     }
    
//   /* //this was just for debugging
//     // Print the initialized array
//     for (int i = 0; i < n; i++) {
//         for (int j = 0; j < n; j++) {
//             printf("%2d ", array[i][j]);
//         }
//         printf("\n");
//     }
    
//     printf("--------------------------------------\n");
    
//     for (int i = 0; i < n_double; i++) {
//         for (int j = 0; j < n_double; j++) {
//             printf("%2d ", distance[i][j]);
//         }
//         printf("\n");
//     }
//     */

//     return distance;
// }


// enum Cell_Border {UNKNOWN, WALL, WAY};

// // create the 3D array maze 16 x 16 x 5. 
// // Each cell has the distance as well as four fields for the surrounding walls. Each can be Unknown, Wall, Way
// int *maze(maze_size){

//     for (int i = 0; i < maze_size; i++) {
//         for (int j = 0; j < maze_size; j++) {
//             maze[i][j][0] = initDistance[i][j];
//             for (int k = 1; k < 5; k++) {
//                 // Set the first element to a number (you can replace this with your desired numeric value)
//                 array[i][j][k] = UNKNOWN;
//             }
            
//         }
//     }

// }


