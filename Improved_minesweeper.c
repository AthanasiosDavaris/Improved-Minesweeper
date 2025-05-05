#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

// Constants of cell states
#define MINE 69
#define FLAG 70
#define CLOSED 0
#define OPEN 1

// function declerations
void init_boards(int ***board, int ***board_state, int rows, int cols);
void free_boards(int **board, int **board_state, int rows);
void print_board_headers(int cols);
void print_board(int **board, int **board_state, int rows, int cols);
int read_text(char str[], int size, int flag);
int parse_cords(char str[], int offset, int *x_out);
void exit_game(int **board, int **board_state, int rows);
int open_cell(int row, int col, int **board, int **board_state, int rows, int cols);
void mark_cell(int row, int col, int **board, int **board_state);
void cheat(int **board, int **board_state, int rows, int cols);
void place_mines(int **board, int rows, int cols, int total_mines, int safe_row, int safe_col);
void calc_adj_mines(int **board, int rows, int cols);

// Main Game
int main(void)
{
  srand(time(NULL)); // Initialized the random number generator

  int **board = NULL, **board_state = NULL;
  int rows, cols, difficulty, total_mines, opened_cells = 0;
  char input[20] = {0};

  // gets the board dimensions from the player
  do
  {
    printf("Enter the number of columns (min 8, max 16): ");
    scanf("%d", &cols);
    if (cols < 8 || cols > 16)
    {
      printf("Invalid input. Please try again. \n");
    }
  } while (cols < 8 || cols > 16);

  do
  {
    printf("Enter the number of rows (min 8, max 16): ");
    scanf("%d", &rows);
    if (rows < 8 || rows > 16)
    {
      printf("Invalid input. Please try again. \n");
    }
  } while (rows < 8 || rows > 16);

  // Intitalizing the boards
  init_boards(&board, &board_state, rows, cols);

  // Difficulty level
  do
  {
    printf("Select difficulty (1 = Easy, 2 = Medium, 3 = Hard, 4 = Impossible): ");
    scanf("%d", &difficulty);
    if (difficulty < 1 || difficulty > 4)
    {
      printf("Invalid input. Please try again.\n");
    }
  } while (difficulty < 1 || difficulty > 4);

  // Calculates the amount of mines on the board
  total_mines = (rows * cols * (5 + 5 * difficulty)) / 100;

  while (opened_cells < (rows * cols - total_mines))
  {
    // Print the board
    print_board_headers(cols);
    print_board(board, board_state, rows, cols);

    // player's move
    printf("Enter your move (e.g., open(x,y), mark(x,y), cheat(x,y), exit): ");
    read_text(input, sizeof(input), 1);

    // exit command
    if (strncmp(input, "exit", 4) == 0)
    {
      exit_game(board, board_state, rows);
    }

    int x, y;
    // open command
    if (strncmp(input, "open", 4) == 0 && parse_cords(input, 4, &x))
    {
      // Initialize mines at the start
      if (opened_cells == 0)
      {
        place_mines(board, rows, cols, total_mines, x - 1, y - 1);
        calc_adj_mines(board, rows, cols);
      }
      opened_cells = open_cell(x - 1, y - 1, board, board_state, rows, cols);
    }
    // mark command
    else if (strncmp(input, "mark", 4) == 0 && parse_cords(input, 4, &x))
    {
      mark_cell(x - 1, y - 1, board, board_state);
    }
    // cheat command
    else if (strncmp(input, "cheat", 5) == 0 && parse_cords(input, 5, &x))
    {
      cheat(board, board_state, x - 1, y - 1);
    }
    // error in command
    else
    {
      printf("Invalid input. Please use one of the following formats: open(x,y), mark(x,y), cheat(x,y), exit.\n");
    }
  }

  // Win message
  printf("Congratulations, you won the game!\n");
  exit_game(board, board_state, rows);
}