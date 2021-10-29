#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define SIZE 9

// Sudoku board. Modify this to test your checker!
// 0 == empty cell; 1-9 is the filled in digit.
int board[SIZE][SIZE] = {
    {1,0,0,0,0,0,0,0,0},
    {0,2,0,0,0,0,0,0,0},
    {6,0,3,0,0,0,0,0,0},
    {0,0,0,4,0,0,0,0,0},
    {0,0,0,0,5,0,0,0,0},
    {0,0,0,0,0,6,0,0,0},
    {0,0,0,0,0,0,7,0,0},
    {0,0,0,0,0,0,0,8,0},
    {0,0,0,0,0,0,0,0,9},
};

bool row_check[SIZE] = {false};
bool col_check[SIZE] = {false};
bool box_check[SIZE] = {false};

void printBoard(int board[SIZE][SIZE]) {
    for(int i = 0; i<SIZE; i++) {
        for(int j = 0; j<SIZE; j++) {
            printf("%5d", board[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}

// Checks the given row for duplicate numbers, and updates the row_check
// value for that row appropriately. If no number is repeated in that row,
// row_check[row] will be set to true; otherwise, it will be false.
void* checkRow(void* args) {
		int col;
		int row = *((int*) args);
		bool seen[SIZE] = { false };
		
		for (col = 0; col < SIZE; ++col) {
			if (board[row][col] > 0) {
				if (seen[board[row][col] - 1]) {
					row_check[row] = false;
					return NULL;
				}
				seen[board[row][col] - 1] = true;
			}
		}
		row_check[row] = true;
		free(args);
    return NULL;
}

// Checks the given col for duplicate numbers, and updates the col_check
// value for that col appropriately. If no number is repeated in that col,
// col_check[col] will be set to true; otherwise, it will be false.
void* checkCol(void* args) {
		int row;
		int col = *((int*) args);
		bool seen[SIZE] = { false };
		
		for (row = 0; row < SIZE; ++row) {
			if (board[row][col] > 0) {
				if (seen[board[row][col] - 1]) {
					col_check[col] = false;
					return NULL;
				}
				seen[board[row][col] - 1] = true;
			}
		}
		col_check[col] = true;
		free(args);
    return NULL;
}

// Checks the given 3x3 box for duplicate numbers, and updates the box_check
// value for that box appropriately. If no number is repeated in that box,
// box_check[box] will be set to true; otherwise, it will be false.
void* checkBox(void* args) {
		int row;
		int col;
		int box = *((int*) args);
		int startRow = (box/3) * 3;
		int startCol = (box % 3) * 3;
		bool seen[SIZE] = { false };
	
		for (row = startRow; row < (startRow + 3); ++row) {
			for (col = startCol; col < (startCol + 3); ++col) {
				if (board[row][col] > 0) {
					if (seen[board[row][col] - 1]) {
						box_check[box] = false;
						return NULL;
					}
					seen[board[row][col] - 1] = true;
				}
			}
		}
		box_check[box] = true;
		free(args);
    return NULL;
}

// Spawn a thread to fill each cell in each result matrix.
// How many threads will you spawn?
int main() {
		int i;
    // 1. Print the board.
    printf("Board:\n");
    printBoard(board);
    
    // 2. Create pthread_t objects for our threads.
		pthread_t checkRowThreads[SIZE];
		pthread_t checkColThreads[SIZE];
		pthread_t checkBoxThreads[SIZE];
		
    
    // 3. Create a thread for each cell of each matrix operation.
		for(i = 0; i < SIZE; ++i) {
			int* j = malloc(sizeof(int));
			int* k = malloc(sizeof(int));
			int* l = malloc(sizeof(int));
			*j = i;
			*k = i;
			*l = i;
			pthread_create(&checkRowThreads[i], NULL, checkRow, j);
			pthread_create(&checkColThreads[i], NULL, checkCol, k);
			pthread_create(&checkBoxThreads[i], NULL, checkBox, l);
		}
    
    // 4. Wait for all threads to finish.
    for (i = 0; i < SIZE; ++i) {
			pthread_join(checkRowThreads[i], NULL);
		}
	
		for (i = 0; i < SIZE; ++i) {
			pthread_join(checkColThreads[i], NULL);
		}

		for (i = 0; i < SIZE; ++i) {
			pthread_join(checkBoxThreads[i], NULL);
		}
	
    // 5. Print the results.
    printf("Results:\n");
    bool all_rows_passed = true;
    printf("Rows:\n");
    for (int i = 0; i < SIZE; i++) {
        if (!row_check[i]) {
            printf("Row %i did not pass\n", i);
            all_rows_passed = false;
        }
    }
    if (all_rows_passed) {
        printf("All rows passed!\n");
    }
    
    bool all_cols_passed = true;
    printf("Cols:\n");
    for (int i = 0; i < SIZE; i++) {
        if (!col_check[i]) {
            printf("Col %i did not pass\n", i);
            all_cols_passed = false;
        }
    }
    if (all_cols_passed) {
        printf("All cols passed!\n");
    }
    
    bool all_boxes_passed = true;
    printf("Boxes:\n");
    for (int i = 0; i < SIZE; i++) {
        if (!box_check[i]) {
            printf("Box %i did not pass\n", i);
            all_boxes_passed = false;
        }
    }
    if (all_boxes_passed) {
        printf("All boxes passed!\n");
    }
    return 0;
}

