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

#include <cs50.h>
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
void swap(int row_start, int col_start, int row_goal, int col_goal);
int search_row(int tile);
int search_column(int tile);

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
    int counter = 1;
    for (int i = 0; i < d; i++)
    {
        for (int j = 0; j < d; j++)
        {
            board[i][j] = counter;
            counter++; 
        }
    }
    // turn bottom right corner to blank
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
    // based on wikipedia example of Fisher-Yates Shuffle
    // https://en.wikipedia.org/wiki/Fisher–Yates_shuffle
    // 
    int row_start, col_start, row_goal, col_goal;
    for (int counter = total; counter > 0; counter--)
    {
        int randt = (drand48() * total);
        row_start = counter / d;
        col_start = counter % d;
        row_goal = randt / d;
        col_goal = randt % d;
        swap(row_start, col_start, row_goal, col_goal);
    }
    // turn bottom right corner to blank
    board[d-1][d-1] = 0;
    // check for solvability
    // uses information from 
    // https://www.cs.bham.ac.uk/~mdr/teaching/modules04/java2/TilesSolvability.html
    // turn 2 by 2 array into 1 dimensional array
    int flat_board[d * d];
    for (int i = 0; i < d; i++)
    {
        for (int j = 0; j < d; j++)
        {
            flat_board [(i * d) + j]= board[i][j]; 
        }
    }
    // count number of inversions
    // ignores last element in array (guaranteed to be 0)
    int inversions = 0;
    for (int i = 0; i < d * d; i++)
    {
        int temp = flat_board[i];
        for (int j = i + 1; j < d * d; j++)
        {
            if(flat_board[j] != 0)
            {
                if(temp > flat_board[j])
                {
                    inversions++;
                }
            }
        }
    }
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
        int row_start, col_start, row_goal, col_goal;
        // search for coordinates of tile to be moved;
        row_start = search_row(tile);
        col_start = search_column(tile);
        
        // search for coordinates of blank tile
        // NOTE: Does it make more sense to store global variables of the blank tile and update them each move? 
        row_goal = search_row(0);
        col_goal = search_column(0);
        
        // makes sure either row or column of tiles to be switched are the same
        if (row_start == row_goal )
        {
            // checks that tiles to be swapped are adjacent
            // if so, swaps their values
            if ((col_start == (col_goal + 1)) || ((col_start == (col_goal - 1))))
            {
                swap(row_start, col_start, row_goal, col_goal);
                return true;
            }
            // if not adjacent, move is illegal
            else 
            {
                return false;
            }
        }
        else if (col_start == col_goal)
        {
            // checks that tiles to be swapped are adjacent
            // if so, swaps their values
            if ((row_start == (row_goal + 1)) || ((row_start == (row_goal - 1))))
            {
                swap(row_start, col_start, row_goal, col_goal);
                return true;
            }
            // if not adjacent, move is illegal
            else 
            {
                return false;
            }
        }  
        // if neither row nor column are the same, move cannot be completed
        else 
        {
            return false;
        }
    }    
}

/**
 * Returns true if game is won (i.e., board is in winning configuration), 
 * else false.
 */
bool won(void)
{
    // check to make sure bottom right corner is blank
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
 */
void swap(int row_start, int col_start, int row_goal, int col_goal)
{
    int temp = board[row_start][col_start];
    board[row_start][col_start] = board[row_goal][col_goal];
    board[row_goal][col_goal] = temp;
}

// NOTE: perhaps an array with array[0] = row and array[1] = column makes more logical sense than searching through the array twice
// however, would require malloc and global variable as pointer
/**
 * Given an int value, searches array for that value
 * assumes that value is in the array (checked in move function)
 * returns the row coordinate
 */
int search_row(int tile)
{
    for (int i = 0; i < d; i++)
    {
        for (int j = 0; j < d; j++)            
        {
            if (board[i][j] == tile) 
            {
                return i;
            }
        }
    }
    return ERROR;
}
/**
 * Given an int value, searches array for that value
 * assumes that value is in the array (checked in move function)
 * returns the column coordinate
 */
int search_column(int tile)
{
    for (int i = 0; i < d; i++)
    {
        for (int j = 0; j < d; j++)            
        {
            if (board[i][j] == tile) 
            {
                return j;
            }
        }
    }
    return ERROR;
}
