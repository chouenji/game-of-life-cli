#include <cmath>
#include <csignal>
#include <cstddef>
#include <cstdlib>
#include <iostream>
#include <unistd.h>

#define ROWS 20
#define COLS 20
#define CELLS 12

enum status { alive = 'A', dead = '*' };

struct Node {
  int row, col;
  unsigned char curr_stat, next_stat;
};

struct Cell {
  Node *node;
};

Node *board[ROWS][COLS];
Cell *cells[CELLS];

size_t generation = 0;
int population = std::size(cells);

// Main functions
void init_board();
void print_board();
void add_cell();
void check_neighbors();
void update_status();

// Helper functions
bool is_alive(int row, int col, int row_offset, int col_offset);

// Handler functions
void free_board();

void signal_handler(int s) {
  std::cout << "CTRL + C pressed SIGINT: " << s << std::endl;
  exit(EXIT_SUCCESS);
}

int main() {
  struct sigaction sigIntHandler;
  sigIntHandler.sa_handler = signal_handler;
  sigemptyset(&sigIntHandler.sa_mask);
  sigIntHandler.sa_flags = 0;

  sigaction(SIGINT, &sigIntHandler, NULL);

  init_board();
  add_cell();

  while (population > 0) {
    print_board();
    usleep(500000);
    check_neighbors();
    update_status();
    system("clear");
  }

  print_board();

  free_board();
  return 0;
}

void init_board() {
  for (int i = 0; i < ROWS; i++) {
    for (int j = 0; j < COLS; j++) {
      board[i][j] = (Node *)malloc(sizeof(Node));
      board[i][j]->row = i;
      board[i][j]->col = j;
      board[i][j]->curr_stat = dead;
    }
  }
}

void print_board() {
  std::cout << "Generation: " << generation << std::endl;
  std::cout << "Population: " << population << std::endl << std::endl;

  for (size_t i = 0; i < ROWS; i++) {
    for (size_t j = 0; j < COLS; j++) {
      std::cout << " " << board[i][j]->curr_stat;
    }

    std::cout << std::endl;
  }
}

void free_board() {
  for (size_t i = 0; i < ROWS; i++) {
    for (size_t j = 0; j < COLS; j++) {
      free(board[i][j]);
    }
  }
}

void add_cell() {
  size_t cols = std::ceil(std::sqrt(CELLS));
  size_t rows = int(CELLS / cols);
  size_t k = 0;

  for (size_t i = 0; i < rows; i++) {
    for (size_t j = 0; j < cols; j++) {
      cells[k] = (Cell *)malloc(sizeof(Cell));

      if (cells[k] == NULL) {
        std::cerr << "cell " << j << " is null." << std::endl;
        exit(EXIT_FAILURE);
      }

      cells[k]->node = board[ROWS / 2 + i][COLS / 2 + j];
      cells[k]->node->curr_stat = alive;

      k++;
    }
  }
}

void check_neighbors() {
  for (int i = 0; i < ROWS; i++) {
    for (int j = 0; j < COLS; j++) {
      if (population <= 0) {
        break;
      }

      bool top = is_alive(i, j, -1, 0);
      bool bottom = is_alive(i, j, 1, 0);
      bool right = is_alive(i, j, 0, 1);
      bool left = is_alive(i, j, 0, -1);

      bool diag_top_right = is_alive(i, j, -1, 1);
      bool diag_top_left = is_alive(i, j, -1, -1);
      bool diag_bottom_right = is_alive(i, j, 1, 1);
      bool diag_bottom_left = is_alive(i, j, 1, -1);

      size_t count_neighbors = top + bottom + right + left + diag_top_right +
                               diag_top_left + diag_bottom_right +
                               diag_bottom_left;

      if (board[i][j]->curr_stat == alive && count_neighbors != 2 &&
          count_neighbors != 3) {
        board[i][j]->next_stat = dead;
        population--;
      }

      else if (board[i][j]->curr_stat != alive && count_neighbors == 3) {
        board[i][j]->next_stat = alive;
        population++;
      }
    }
  }

  generation++;
}

bool is_alive(int row, int col, int row_offset, int col_offset) {
  if (row + row_offset < 0 || row + row_offset >= ROWS)
    return false;
  else if (col + col_offset < 0 || col + col_offset >= COLS)
    return false;

  return board[row + row_offset][col + col_offset]->curr_stat == alive;
}

void update_status() {
  for (size_t i = 0; i < ROWS; i++) {
    for (size_t j = 0; j < COLS; j++)
      if (board[i][j]->next_stat != '\0') {
        board[i][j]->curr_stat = board[i][j]->next_stat;
      }
  }
}
