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
void cheat(int **board, int **board_state, int row, int col);
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

// Initialize the boards
void init_boards(int ***board, int ***board_state, int rows, int cols)
{
  *board = (int **)malloc(rows * sizeof(int *));
  *board_state = (int **)malloc(rows * sizeof(int *));
  if (!(*board) || !(board_state))
  {
    printf("Error: Memory allocation failed.\n");
    exit(EXIT_FAILURE);
  }

  for (int i = 0; i < rows; i++)
  {
    (*board)[i] = (int *)calloc(cols, sizeof(int));
    (*board_state)[i] = (int *)calloc(cols, sizeof(int));
    if (!(*board)[i] || !(board_state)[i])
    {
      printf("Error: Memory allocation failed.\n");
      exit(EXIT_FAILURE);
    }
  }
}

// free the allocated memory from the boards
void free_boards(int **board, int **board_state, int rows)
{
  for (int i = 0; i < rows; i++)
  {
    free(board[i]);
    free(board_state[i]);
  }
  free(board);
  free(board_state);
}

// Prints the board headers
void print_board_headers(int cols)
{
  printf("   ");
  for (int i = 1; i <= cols; i++)
  {
    printf("%2d", i);
  }
  printf("\n   ");
  for (int i = 0; i < cols; i++)
  {
    printf("---");
  }
  printf("\n");
}

void print_board(int **board, int **board_state, int rows, int cols)
{
  for (int i = 0; i < rows; i++)
  {
    printf("%2d|", i + 1);
    for (int j = 0; j < cols; j++)
    {
      if (board_state[i][j] == CLOSED)
      {
        printf(" # ");
      }
      else if (board[i][j] == MINE)
      {
        printf(" * ");
      }
      else if (board[i][j] == FLAG)
      {
        printf(" @ ");
      }
      else
      {
        printf(" %d ", board[i][j]);
      }
    }
    printf("\n");
  }
}

// function that reads text (credits to my university professor)
int read_text(char str[], int size, int flag)
{
  int len;

  if (fgets(str, size, stdin) == NULL)
  {
    printf("Error: fgets() failed\n");
    exit(EXIT_FAILURE);
  }
  len = strlen(str);
  if (len > 0)
  {
    if (flag && (str[len - 1] == '\n'))
    {
      str[len - 1] = '\0';
      len--;
    }
  }
  else
  {
    printf("Error: No input\n");
    exit(EXIT_FAILURE);
  }
  return len;
}

// Parse coordinates (x, y) of a command
int parse_cords(char str[], int offset, int *x_out)
{
  int x, y;
  if (sscanf(str + offset, "(%d,%d)", &x, &y) == 2)
  {
    *x_out = x;
    return y;
  }
  return 0;
}

// Exit the game and free board occupied memory
void exit_game(int **board, int **board_state, int rows)
{
  printf("Thank you for playing!\n");
  free_boards(board, board_state, rows);
  exit(EXIT_SUCCESS);
}

// recursively opens a cell and reveal its content
int open_cell(int row, int col, int **board, int **board_state, int rows, int cols)
{
  if (row < 0 || row >= rows || col < 0 || col >= cols || board_state[row][col] == OPEN)
  {
    return 0;
  }

  if (board[row][col] == MINE)
  {
    printf("Game over! You hit a mine.\n");
    exit_game(board, board_state, rows);
  }

  board_state[row][col] = OPEN;
  int opened = 1;

  if (board[row][col] == 0)
  {
    for (int i = row - 1; i <= row + 1; i++)
    {
      for (int j = col - 1; j <= col + 1; j++)
      {
        if (i != row || j != col)
        {
          opened += open_cell(i, j, board, board_state, rows, cols);
        }
      }
    }
  }
  return opened;
}

// Mark a cell
void mark_cell(int row, int col, int **board, int **board_state)
{
  if (board_state[row][col] == OPEN)
  {
    printf("This cell is already open. You cannot mark it.\n");
  }
  else
  {
    board[row][col] = FLAG;
    board_state[row][col] = OPEN;
  }
}

// cheat and reveal the content of a cell
void cheat(int **board, int **board_state, int row, int col)
{
  if (row < 0 || col < 0 || board[row][col] == MINE)
  {
    printf("This cell contains a mine!\n");
  }
  else
  {
    printf("This cell contains: %d\n", board[row][col]);
  }
}

void place_mines(int **board, int rows, int cols, int total_mines, int safe_row, int safe_col)
{
  int placed = 0;
  while (placed < total_mines)
  {
    int rand_row = rand() % rows;
    int rand_col = rand() % cols;
    if (board[rand_row][rand_col] == MINE || (abs(rand_row - safe_row) <= 1 && abs(rand_col - safe_col) <= 1))
    {
      continue;
    }
    board[rand_row][rand_col] = MINE;
    placed++;
  }
}

// calculates the number of adjacent mines of each cell in the board
void calc_adj_mines(int **board, int rows, int cols)
{
  for (int i = 0; i < rows; i++)
  {
    for (int j = 0; j < cols; j++)
    {
      if (board[i][j] == MINE)
      {
        continue;
      }
      int count = 0;
      for (int x = i - 1; x <= i + 1; x++)
      {
        for (int y = j - 1; y <= j + 1; y++)
        {
          if (x >= 0 && x < rows && y >= 0 && y < cols && board[x][y] == MINE)
          {
            count++;
          }
        }
      }
      board[i][j] = count;
    }
  }
}