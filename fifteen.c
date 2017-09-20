/**
 * fifteen.c
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

// constants
#define DIM_MIN 3
#define DIM_MAX 9

// board
int board[DIM_MAX][DIM_MAX];
int boardwin[DIM_MAX][DIM_MAX];

// dimensions
int d;

// tiles count
int tile_total, tile_no;

// counters
int i, j, k;

// empty square
int e_row;
int e_col;





// prototypes
void clear(void);
void greet(void);
void init(void);
void win(void);
void draw(void);
bool move(int tile);
bool won(void);

int main(int argc, string argv[])
{
    // ensure proper usage
    if (argc != 2) {
        printf("Usage: fifteen d\n");
        return 1;
    }

    // ensure valid dimensions
    d = atoi(argv[1]);
    if (d < DIM_MIN || d > DIM_MAX) {
        printf("Board must be between %i x %i and %i x %i, inclusive.\n",
            DIM_MIN, DIM_MIN, DIM_MAX, DIM_MAX);
        return 2;
    }

    // open log
    FILE *file = fopen("log.txt", "w");
    if (file == NULL) {
        return 3;
    }

    tile_total = (d * d) - 1;

    // greet user with instructions
    greet();

    // initialize the board
    init();
    win();

    // accept moves until game is won
    while (true) {
        // clear the screen
        clear();

        // draw the current state of the board
        draw();

        // log the current state of the board (for testing)
        for (int i = 0; i < d; i++) {
            for (int j = 0; j < d; j++) {
                //if (board[i][j] == 0) {
                    //fprintf(file, "__");
                    //continue;
                //}
                fprintf(file, "%i", board[i][j]);

                if (j < d - 1) {
                    fprintf(file, "|");
                }

            }
            fprintf(file, "\n");
        }
        fflush(file);

        // check for win
        if (won()) {
            printf("ftw!\n");
            break;
        }

        // prompt for move
        printf("Tile to move: ");
        int tile = get_int();

        // quit if user inputs 0 (for testing)
        if (tile == 0) {
            break;
        }

        // log move (for testing)
        fprintf(file, "%i\n", tile);
        fflush(file);

        // move if possible, else report illegality
        if (!move(tile)) {
            printf("\nIllegal move.\n");
            usleep(500000);
        }

        // sleep thread for animation's sake
        //usleep(500000);
    }

    // close log
    fclose(file);

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
    //usleep(2000000);
}

/**
 * Initializes the game's board with tiles numbered 1 through d*d - 1
 * (i.e., fills 2D array with values but does not actually print them).
 */
void init(void)
{
    //init, start from top val, then go down (left to right)
    tile_no = tile_total;
    for (int i = 0; i < d; i++) {
        for (int j = 0; j < d; j++) {
            if (tile_no == 0) {
                e_row = i;
                e_col = j;
                break;
            }
            board[i][j] = tile_no;
            tile_no--;
        }
    }

    //if odd no of tiles, swap
    if (tile_total % 2 != 0) {
        int t1 = board[d - 1][d - 2];
        int t2 = board[d - 1][d - 3];
        board[d - 1][d - 2] = t2;
        board[d - 1][d - 3] = t1;
        return;
    }
}

/**
 * Prints the board in its current state.
 */
void draw(void)
{
    for (i = 0; i < d; i++) {
        for (j = 0; j < d; j++) {
            if (board[i][j] == 0) {
                printf("__");
                if (j > 0 && j < d - 1) {
                    printf("|");
                }
                continue;
            }
            printf("%02i", board[i][j]);
            if (j < d - 1) {
                printf("|");
            }
        }
        printf("\n");
    }
    printf("cursor: [%i][%i]\n", i, j);
}

/**
 * If tile borders empty space, moves tile and returns true, else
 * returns false.
 */
bool move(int tile)
{
    //
//testing square
int up = board[e_row + 1][e_col];
int down = board[e_row - 1][e_col];
int left = board[e_row][e_col - 1];
int right = board[e_row][e_col + 1];

//tile
int t_row;
int t_col;


    if (tile != up && tile != down && tile != left && tile != right) {
        printf("tile:%i, up: %i, down:%i, left:%i, right:%i\n",
        tile, up, down, left, right);
        return false;
    }

    for (i = 0; i < d; i++){
        for (j = 0; j < d; j++) {
            if (board[i][j] == tile) {
                t_row = i;
                t_col = j;
                printf("tile: [%i][%i]\n", i, j);
                break;
            }
        }
    }
    //reusing t1,t2 from init
    int t1 = 0;
    t1 = board[t_row][t_col];
    board[e_row][e_col] = t1;
    board[t_row][t_col] = 0;
    e_row = t_row;
    e_col = t_col;
    return true;

}

/**
 * Returns true if game is won (i.e., board is in winning configuration),
 * else false.
 */
void win(void)
{
    tile_no = 1;
    for (i = 0; i < d; i++) {
        for (j = 0; j < d; j++) {
        boardwin[i][j] = tile_no;
        tile_no++;
        if (i == d - 1 && j == d - 1) {
                boardwin[i][j] = 0;
                break;
            }
        }
    }
}

bool won(void)
{
    for (i = 0; i < d; i++) {
        for (j = 0; j < d; j++) {
            if (board[i][j] != boardwin[i][j]) {
                return false;
            }
        }
    }
return true;
}
