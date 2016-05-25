/**
 * fifteen.c
 *
 * Computer Science 50
 * Problem Set 3
 *
 * Implements Game of Fifteen (generalized to d x d).
 *
 * Usage: fifteen d
 *
 * whereby the board's dimensions are to be d x d,
 * where d must be in [DIM_MIN,DIM_MAX]
 *
 * Note that usleep is obsolete, but it offers more granularity than
 * sleep and is simpler to use than nanosleep; `man usleep` for more.
 */
 
#define _XOPEN_SOURCE 500

#include "cs50.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

// constants
#define DIM_MIN 3
#define DIM_MAX 9
#define ERROR -1

// board
int board[DIM_MAX][DIM_MAX];

// dimensions
int d;

// to avoid calculating square each time
int total;
// prototypes given
void clear(void);
void greet(void);
void init(void);
void draw(void);
bool move(int tile);
bool won(void);

//prototypes from own functions
void randomize(void);
void swap(int start_index, int goal_index);
int search(int tile);

void god(int numSteps , int answer[]);

int main(int argc, string argv[])
{
    // ensure proper usage
    if (argc != 2)
    {
        printf("Usage: fifteen d\n");
        return 1;
    }

    // ensure valid dimensions
    d = atoi(argv[1]);
    if (d < DIM_MIN || d > DIM_MAX)
    {
        printf("Board must be between %i x %i and %i x %i, inclusive.\n",
            DIM_MIN, DIM_MIN, DIM_MAX, DIM_MAX);
        return 2;
    }
    
    // calculate max number on board
    // NOTE: does this make sense to store? should d - 1 be stored too?
    total = d * d - 1;

    // greet user with instructions
    greet();

    // initialize the board
    init();
    // accept moves until game is won
    while (true)
    {
        // clear the screen
        clear();

        // draw the current state of the board
        draw();

        // check for win
        if (won())
        {
            printf("Complete!\n");
            break;
        }

        // prompt for move
        printf("Tile to move: ");
        int tile = GetInt();

        // move if possible, else report illegality
        if (!move(tile))
        {
            printf("\nIllegal move.\n");
            usleep(500000);
        }

        // sleep thread for animation's sake
        usleep(500000);
    }

    // success
    return 0;
}

/**
 * Clears screen using ANSI escape sequences.
 */
void clear(void)
{
    printf("\033[2J");
    printf("\033[%d;%dH", 0, 0);
}

/**
 * Greets player.
 */
void greet(void)
{
    clear();
    printf("WELCOME TO GAME OF FIFTEEN\n");
    usleep(2000000);
}

/**
 * Initializes the game's board with tiles numbered 1 through d*d - 1
 * (i.e., fills 2D array with values but does not actually print them).  
 */
void init(void)
{
    // int counter = 1;
    // for (int i = 0; i < d; i++)
    // {
    //     for (int j = 0; j < d; j++)
    //     {
    //         board[i][j] = counter;
    //         counter++; 
    //     }
    // }
    // // turn bottom right corner to blank
    board[d-1][d-1] = 0;
    // then pass to randomize
    randomize();
}

/**
 * swaps random tiles and then checks for solvability
 */
void randomize(void)
{
    // seed drand
    srand48(time(NULL));

    // first let's make a 1-dimensional array of length d^2 - 1 and initialize it to be
    // equivalent to the "solved" configuration
    int flat_board[total];
    for (int i = 0; i < total; i++){
        flat_board[i] = i+1;
    }

    // based on wikipedia example of Fisher-Yates Shuffle
    // https://en.wikipedia.org/wiki/Fisher–Yates_shuffle
    int foo;
    for (int counter = total-1; counter > 0; counter--){
        int randt = (drand48() * (counter + 1));
        foo = flat_board[counter];
        flat_board[counter] = flat_board[randt];
        flat_board[randt] = foo;
    }

    // check for solvability
    // uses information from 
    // https://www.cs.bham.ac.uk/~mdr/teaching/modules04/java2/TilesSolvability.html
    int inversions = 0;
    int checking;
    for (int i = 0; i < total - 1; i++){
        checking = flat_board[i];
        // if (checking == 0) {
        //     printf("%d %d\n", checking, i);
        // }
        for (int j = i + 1; j < total; j++){
            if (flat_board[j] < checking){
                inversions++;
            }
        }
    }

    // we're always placing the blank tile on the bottom right, so for this to be solvable,
    // if the board width is odd, inversions must be even
    // if the board width is even, inversions must be even
    // hey this makes it easy, we just need to adjust if inversions is odd

    // so switching the first two elements switches the parity of inversions,
    // and I believe it makes a 1-1 mapping of solvable and non-solvable configurations,
    // so this shouldn't interfere with randomness of the shuffle
    if (inversions % 2 != 0){
        foo = flat_board[0];
        flat_board[0] = flat_board[1];
        flat_board[1] = foo;
    }

    // now we just need to wrap the flat board onto the global board
    for (int i = 0; i < d; i++){
        for (int j = 0; j < d; j++){
            if(i < d-1 || j < d-1){
                board[i][j] = flat_board[i*d + j];
            }
        }
    }

    // int row_start, col_start, row_goal, col_goal;
    // for (int counter = total; counter > 0; counter--)
    // {
    //     int randt = (drand48() * total);
    //     row_start = counter / d;
    //     col_start = counter % d;
    //     row_goal = randt / d;
    //     col_goal = randt % d;
    //     swap(row_start, col_start, row_goal, col_goal);
    // }
    // // turn bottom right corner to blank
    // board[d-1][d-1] = 0;

    // // turn 2 by 2 array into 1 dimensional array
    // int flat_board[d * d];
    // for (int i = 0; i < d; i++)
    // {
    //     for (int j = 0; j < d; j++)
    //     {
    //         flat_board [(i * d) + j]= board[i][j]; 
    //     }
    // }
    // // count number of inversions
    // // ignores last element in array (guaranteed to be 0)
    // int inversions = 0;
    // for (int i = 0; i < d * d; i++)
    // {
    //     int temp = flat_board[i];
    //     for (int j = i + 1; j < d * d; j++)
    //     {
    //         if(flat_board[j] != 0)
    //         {
    //             if(temp > flat_board[j])
    //             {
    //                 inversions++;
    //             }
    //         }
    //     }
    // }
}    
/**
 * Prints the board in its current state.
 */
void draw(void)
{
    // loops through array in rows, printing value at each spot
    for (int i = 0; i < d; i++) 
    {
        for (int j = 0; j < d; j++)
        {
            // prints blank as an underscore
            if (board[i][j] == 0)
            {
                printf(" _ ");
            }
            // prints single digit numbers with a space in front for spacing
            else if (board[i][j] < 10)
            {
                printf(" %i ", board[i][j]);
            }
            else
            {
                printf("%i ", board[i][j]);
            }    
        }
        printf("\n");
    }
}

/**
 * If tile borders empty space, moves tile and returns true, else
 * returns false. 
 */
bool move(int tile)
{
    // make sure number of tile is within grid
    if (tile < 1 || tile > (total)) 
    {
        return false;
    }
    else 
    {
        int start_index, goal_index;
        // search for coordinates of tile to be moved;
        start_index = search(tile);

        // search for coordinates of blank tile
        // NOTE: Does it make more sense to store global variables of the blank tile and update them each move? 
        // Not sure about this ^ but updated this to an improved search
        goal_index = search(0);

        if (abs(2 * abs(start_index - goal_index) - (d+1)) == (d-1)){
            swap(start_index, goal_index);
            return true;
        }
        else{
            return false;
        }
    }    
}

/**
 * Outdated move function (uses 2 d locations)
 * 
    //makes sure either row or column of tiles to be switched are the same
     if (row_start == row_goal )
    {
    //      checks that tiles to be swapped are adjacent
          if so, swaps their values
         if ((col_start == (col_goal + 1)) || ((col_start == (col_goal - 1))))
         {
             swap(row_start, col_start, row_goal, col_goal);
             return true;
         }
          if not adjacent, move is illegal
         else 
         {
             return false;
         }
     }
     else if (col_start == col_goal)
     {
          checks that tiles to be swapped are adjacent
          if so, swaps their values
         if ((row_start == (row_goal + 1)) || ((row_start == (row_goal - 1))))
         {
             swap(row_start, col_start, row_goal, col_goal);
             return true;
         }
          if not adjacent, move is illegal
         else 
         {
             return false;
         }
     }  
      if neither row nor column are the same, move cannot be completed
     else 
     {
         return false;
     }
     
*/
/**
 * Returns true if game is won (i.e., board is in winning configuration), 
 * else false.
 */
bool won(void)
{
    //check to make sure bottom right corner is blank
    if (board[d-1][d-1] != 0)
    {
        return false;
    }
    int compare = 1;
    for (int i = 0; i < d; i++)
    {
        for (int j = 0; j < d; j++) 
        {
            if (compare == d * d)
            {
                return true;
            }
            if (board[i][j] != compare)
            {
                return false;
            }
                compare++;
        }
    }
    return true;
}

/**
 * given the locations of two tiles, swaps the tiles
 * modifies global variable board
 * old swap was with 2d locations
 *
 void swap(int row_start, int col_start, int row_goal, int col_goal)
{
 int temp = board[row_start][col_start];
 board[row_start][col_start] = board[row_goal][col_goal];
 board[row_goal][col_goal] = temp;
}
*/

void swap(int start_index, int goal_index){
    int col_start = start_index % d;
    int col_goal = goal_index % d;
    int row_start = (start_index - col_start)/d;
    int row_goal = (goal_index - col_goal)/d;
    int temp = board[row_start][col_start];
    board[row_start][col_start] = board[row_goal][col_goal];
    board[row_goal][col_goal] = temp;
}

// NOTE: condensed search_row and search_col into one function that returns one int equal to (row * d) + column
// This works because row, column < d for all valid positions
// @Dennis: I actually think it might have made more sense to change variables on the heap, rather than always converting
/**
 * Given an int value, searches array for that value
 * assumes that value is in the array (checked in move function)
 * returns the row coordinate
 */
int search(int tile)
{
    for (int i = 0; i < d; i++)
    {
        for (int j = 0; j < d; j++)            
        {
            if (board[i][j] == tile) 
            {
                return d * i + j;
            }
        }
    }
    return ERROR;
}

/** solves the puzzle
 * uses information from 
 * http://larc.unt.edu/ian/pubs/saml.pdf
 * 
 * actually I think this ppt gives more detail
 * http://msemac.redwoods.edu/~darnold/math45/laproj/f09/bishopmadsen/SamLoydSlides.pdf
 * but says on pg 47 there is a configuration that cannot be solved
 */

void god(int numSteps , int answer[])
{
    // finds path that would solve puzzle through brute force trying all the solutions
    // shouldn't need more than 31 steps to get to solution
    // so stop branch after level 32
    if(numSteps  == 32)
    {
        return 0;
    }
    int check;
    int nextCheck;
    // see if can move up
    if((check = god_up())!= ERROR)
    {
        // if this path doesn't move the tile that was just moved
        // check if steps have been made to avoid negative indexing array
        // always enter this on first step
        if((numSteps == 0) || (check != answer[numSteps-1]))
        {
            // store the tile that was moved
            answer[numSteps] = check;
            move(check);
            // if solved, return
            if(won())
            {
                return numSteps + 1;
            }
            // call recursively down to 31 steps
            if((nextCheck = solve(numSteps + 1, answer)))
            {
                return nextCheck;
            }
            
            // if this path didn't leave to being solved, move back down
            // (up a step in the tree)
            move(check);
        }
    }    
    
    // see if can move down
    if((check = god_down())!= ERROR)
    {
        // if this path doesn't move the tile that was just moved
        // check if steps have been made to avoid negative indexing array
        if((numSteps == 0) || (check != answer[numSteps-1]))
        {
            // store the tile that was moved
            answer[numSteps] =  check;
            move(check);
            // if solved, return
            if(won())
            {
                return numSteps + 1;
            }
            // call recursively to check if won
            if((nextCheck = solve(numSteps + 1, answer)))
            {
                return nextCheck;
            }
        }
        // if this path didn't leave to being solved, move back up
        move(check);
    }
    // see if can move right
    if((check = god_right())!= ERROR)
    {
        if((numSteps == 0) || (check != answer[numSteps-1]))
        {
            // store the tile that was moved
            answer[numSteps] = check;
            move(check);
            
            // if solved, return
            if(won())
            {
                return numSteps + 1;
            }
            // call recursively to check if won
            if((nextCheck = solve(numSteps + 1, answer)))
            {
                return nextCheck;
            }
            
            // if this path didn't leave to being solved, move back left
            move(check);
        }    
    }
    // see if can move left
    if((check = god_left())!= ERROR)
    {
        if((numSteps == 0) || (check != answer[numSteps-1]))
        {
        // store the tile that was moved
        answer[numSteps] = check;
        move(check);
        // if solved, return
        if(won())
        {
            return numSteps + 1;
        }
        // call recursively to check if won
        if((nextCheck = solve(numSteps + 1, answer)))
        {
            return nextCheck;
        }
        
        // if this path didn't leave to being solved, move back right
        move(check);
        }
    }
    return 0;
}
}
// all god moves check if possible
// moves blank to the right
bool god_right(void)
{
    int blank_index= search(0);
    if ((blank_index % d) != (d - 1))
    {
        swap(blank_index, (blank_index + 1));
        return true;
    }
    else
    {
        return false;
    }
}
// moves blank to the left
bool god_left(void)
{
    int blank_index= search(0);
    if (blank_index % d != 0)
    {
        swap(blank_index, (blank_index - 1));
        return true;
    }
    else
    {
        return false;
    }
}
// moves blank up
bool god_up(void)
{
    int blank_index= search(0);
    if (blank_index > d)
    {
        swap(blank_index, (blank_index - d));
        return true;
    }
    else
    {
        return false;
    }
}
// moves blank down
bool god_down(void)
{
    int blank_index= search(0);
    if ((blank_index / d) != (d - 1))
    {
        swap(blank_index, (blank_index + d));
        return true;
    }
    else
    {
        return false;
    }
}
