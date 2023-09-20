#include <stdio.h>
#include <stdlib.h>

#define MAZE_SIZE 16
#define STACK_SIZE 256

/************************************
 *
 *   Maze Implementation and functions
 *
 *************************************/

enum CellValue
{
    UNKNOWN,
    WALL,
    WAY,
    EXPLORED
};

struct CellData
{
    enum CellValue north;
    enum CellValue east;
    enum CellValue south;
    enum CellValue west;
    enum CellValue center;
};

/************************************
 *
 *   Non_recursive Implementation
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
    int newValue = 0;
    for (int row = 0; row < MAZE_SIZE; row++)
    {
        for (int col = 0; col < MAZE_SIZE; col++)
        {
            distance[row][col] = 0;
            visited[row][col] = 0;
        }
    }

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
            // if (x + 1 < MAZE_SIZE)
            // {
            //     push(nextLevel, (Point){x + 1, y});
            // }
            // if (x - 1 >= 0)
            // {
            //     push(nextLevel, (Point){x - 1, y});
            // }
            // if (y + 1 < MAZE_SIZE)
            // {
            //     push(nextLevel, (Point){x, y + 1});
            // }
            // if (y - 1 >= 0)
            // {
            //     push(nextLevel, (Point){x, y - 1});
            // }

            // push open neighbors onto the stack
            if (((walls[x][y].east == UNKNOWN) || (walls[x][y].east == WAY)) && (x + 1 < MAZE_SIZE))
            {
                push(nextLevel, (Point){x + 1, y});
            }
            if (((walls[x][y].west == UNKNOWN) || (walls[x][y].west == WAY)) && (x - 1 >= 0))
            {
                push(nextLevel, (Point){x - 1, y});
            }
            if (((walls[x][y].south == UNKNOWN) || (walls[x][y].south == WAY)) && (y + 1 < MAZE_SIZE))
            {
                push(nextLevel, (Point){x, y + 1});
            }
            if (((walls[x][y].north == UNKNOWN) || (walls[x][y].north == WAY)) && (y - 1 >= 0))
            {
                push(nextLevel, (Point){x, y - 1});
            }
        }
        swapStacks(&nextLevel, &currentLevel);
        newValue++;
    }
}

int main()
{
    // Initialize your array here

    // Call floodFill to start the flood-fill algorithm
    int startX = 7;   // Replace with your desired starting point
    int startY = 7;   // Replace with your desired starting point
    int newValue = 0; // Replace with the new value you want to fill
    Stack *currentLevel = createStack(STACK_SIZE);
    Stack *nextLevel = createStack(STACK_SIZE);
    int distance[MAZE_SIZE][MAZE_SIZE];
    struct CellData walls[MAZE_SIZE][MAZE_SIZE];

    for (int row = 0; row < MAZE_SIZE; row++)
    {
        for (int col = 0; col < MAZE_SIZE; col++)
        {
            distance[row][col] = 0;
        }
    }

    // floodFill(startX, startY, newValue);

    //  if (startX < 0 || startX >= MAZE_SIZE || startY < 0 || startY >= MAZE_SIZE)
    //     return;

    // Initialize the stack and push the starting point
    // currentLevel = -1;
    // push(currentLevel, (Point){7, 7});
    // push(currentLevel, (Point){7, 8});
    // push(currentLevel, (Point){8, 8});
    // push(currentLevel, (Point){8, 7});

    push(currentLevel, (Point){15, 0});

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

    // Call floodFill to start the flood-fill algorithm
    floodFill(currentLevel, nextLevel, distance, walls);

    // Print the updated array
    for (int i = 0; i < MAZE_SIZE; i++)
    {
        for (int j = 0; j < MAZE_SIZE; j++)
        {
            printf("%3d ", distance[i][j]);
        }
        printf("\n");
    }

    return 0;
}

/************************************
 *
 *   Recursive Implementation Floodfill
 *
 *************************************/

// int distance[MAZE_SIZE][MAZE_SIZE];
// int visited[MAZE_SIZE][MAZE_SIZE];

// void FloodFill(int x, int y, int update_value){

//     if (x < 0 || x >= MAZE_SIZE || y < 0 || y >= MAZE_SIZE) return;
//     if (visited[x][y] == 1) return;

//     distance[x][y] = update_value;
//     visited[x][y] = 1;
//     update_value++;

//     FloodFill(x + 1, y, update_value);
//     FloodFill(x - 1, y, update_value);
//     FloodFill(x, y + 1, update_value);
//     FloodFill(x, y - 1, update_value);
// }

// int main()
// {

//     int x = 0;
//     int y = 0;
//     int update_value = 0;

//     //   zero initialization

//     for (int row = 0; row < MAZE_SIZE; row++)
//     {
//         for (int col = 0; col < MAZE_SIZE; col++)
//         {
//             distance[row][col] = 0;
//             visited[row][col] = 0;
//         }
//     }

//     //   print maze

//     for (int row = 0; row < MAZE_SIZE; row++)
//     {
//         for (int col = 0; col < MAZE_SIZE; col++)
//         {
//             printf("%3d ", distance[row][col]);
//         }
//         printf("\n");
//     }

//     FloodFill(x, y, update_value);

//     printf("\n------------------------------------------\n");

//     for (int row = 0; row < MAZE_SIZE; row++)
//     {
//         for (int col = 0; col < MAZE_SIZE; col++)
//         {
//             printf("%3d ", distance[row][col]);
//         }
//         printf("\n");
//     }

//     //   update distance

// }