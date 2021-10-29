#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

typedef struct ThreadArgs {
	int i;
	int MAX;
	int** matA;
	int** matB;
	int** matXResult;
} ThreadArgs;

int getRowIdx(int i, int MAX) {
	return i / MAX;
}

int getColIdx (int i, int MAX) {
	return i % MAX;
}

void freeMatrix(int** mat, int MAX) {
	for(int i = 0; i < MAX; i++){
    free(mat[i]);
	}
	free(mat);
}

int** makeMatrix(int n) {
	int **mat = (int **)malloc(n * sizeof(int*));
	for(int i = 0; i < n; i++) mat[i] = (int *)malloc(n * sizeof(int));
	
	return mat;
}

void fillMatrix(int** matrix, int MAX) {
    for(int i = 0; i<MAX; i++) {
        for(int j = 0; j<MAX; j++) {
            matrix[i][j] = rand()%10+1;
        }
    }
}

void printMatrix(int** matrix, int MAX) {
    for(int i = 0; i<MAX; i++) {
        for(int j = 0; j<MAX; j++) {
            printf("%5d", matrix[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}

// Fetches the appropriate coordinates from the argument, and sets
// the cell of matSumResult at the coordinates to the sum of the
// values at the coordinates of matA and matB.
void* computeSum(void* args) { // pass in the number of the ith thread
		ThreadArgs* threadArgs = (ThreadArgs*) args;
		
		int row = getRowIdx(threadArgs->i, threadArgs->MAX);
		int col = getColIdx(threadArgs->i, threadArgs->MAX);
		threadArgs->matXResult[row][col] = threadArgs->matA[row][col] + threadArgs->matB[row][col];
    free(threadArgs);
		return NULL;
}

// Fetches the appropriate coordinates from the argument, and sets
// the cell of matSumResult at the coordinates to the difference of the
// values at the coordinates of matA and matB.
void* computeDiff(void* args) { // pass in the number of the ith thread
		ThreadArgs* threadArgs = (ThreadArgs*) args;
		
		int row = getRowIdx(threadArgs->i, threadArgs->MAX);
		int col = getColIdx(threadArgs->i, threadArgs->MAX);
		threadArgs->matXResult[row][col] = threadArgs->matA[row][col] - threadArgs->matB[row][col];
    free(threadArgs);
		return NULL;
}

// Fetches the appropriate coordinates from the argument, and sets
// the cell of matSumResult at the coordinates to the inner product
// of matA and matB.
void* computeProduct(void* args) { // pass in the number of the ith thread
		ThreadArgs* threadArgs = (ThreadArgs*) args;
		
		int row = getRowIdx(threadArgs->i, threadArgs->MAX);
		int col = getColIdx(threadArgs->i, threadArgs->MAX);
		threadArgs->matXResult[row][col] = threadArgs->matA[row][col] * threadArgs->matB[row][col];
    free(threadArgs);
		return NULL;
}

// Spawn a thread to fill each cell in each result matrix.
// How many threads will you spawn?
int main() {
    srand(time(0));  // Do Not Remove. Just ignore and continue below.
    
		int MAX;
		int i;
		int** matA; 
		int** matB;

		int** matSumResult;
		int** matDiffResult; 
		int** matProductResult;

    // 0. Get the matrix size from the command line and assign it to MAX
    printf("Enter matrix size: ");
		scanf("%d", &MAX);
		matA = makeMatrix(MAX);
		matB = makeMatrix(MAX);
		
		matSumResult = makeMatrix(MAX);
		matDiffResult = makeMatrix(MAX); 
		matProductResult = makeMatrix(MAX);
    
    // 1. Fill the matrices (matA and matB) with random values.
    fillMatrix(matA, MAX);
    fillMatrix(matB, MAX);
    
    // 2. Print the initial matrices.
    printf("Matrix A:\n");
    printMatrix(matA, MAX);
    printf("Matrix B:\n");
    printMatrix(matB, MAX);
    
    // 3. Create pthread_t objects for our threads.
    pthread_t* sumThreads = malloc(MAX*MAX*sizeof(pthread_t));
		pthread_t* diffThreads = malloc(MAX*MAX*sizeof(pthread_t));
		pthread_t* productThreads = malloc(MAX*MAX*sizeof(pthread_t));
    // 4. Create a thread for each cell of each matrix operation.
    // 
    // You'll need to pass in the coordinates of the cell you want the thread
    // to compute.
    // 
    // One way to do this is to malloc memory for the thread number i, populate the coordinates
    // into that space, and pass that address to the thread. The thread will use that number to calcuate 
    // its portion of the matrix. The thread will then have to free that space when it's done with what's in that memory.
    
		for (i = 0; i < MAX*MAX; ++i) {
			ThreadArgs* threadArg = malloc(sizeof(ThreadArgs));
			threadArg->i = i;
			threadArg->MAX = MAX;
			threadArg->matA = matA;
			threadArg->matB = matB;
			threadArg->matXResult = matSumResult;
			pthread_create(&sumThreads[i], NULL, computeSum, threadArg);
		}
	
		for (i = 0; i < MAX*MAX; ++i) {
			ThreadArgs* threadArg = malloc(sizeof(ThreadArgs));
			threadArg->i = i;
			threadArg->MAX = MAX;
			threadArg->matA = matA;
			threadArg->matB = matB;
			threadArg->matXResult = matDiffResult;
			pthread_create(&sumThreads[i], NULL, computeDiff, threadArg);
		}

		for (i = 0; i < MAX*MAX; ++i) {
			ThreadArgs* threadArg = malloc(sizeof(ThreadArgs));
			threadArg->i = i;
			threadArg->MAX = MAX;
			threadArg->matA = matA;
			threadArg->matB = matB;
			threadArg->matXResult = matProductResult;
			pthread_create(&sumThreads[i], NULL, computeProduct, threadArg);
		}
	
    // 5. Wait for all threads to finish.
		
		for (i = 0; i < MAX*MAX; ++i) {
					pthread_join(sumThreads[i], NULL);
		}

		for (i = 0; i < MAX*MAX; ++i) {
			pthread_join(diffThreads[i], NULL);
		}

		for (i = 0; i < MAX*MAX; ++i) {
			pthread_join(productThreads[i], NULL);
		}
    
    // 6. Print the results.
    printf("Results:\n");
    printf("Sum:\n");
    printMatrix(matSumResult, MAX);
    printf("Difference:\n");
    printMatrix(matDiffResult, MAX);
    printf("Product:\n");
    printMatrix(matProductResult, MAX);
	
		freeMatrix(matA, MAX);
		freeMatrix(matB, MAX);
		freeMatrix(matSumResult, MAX);
		freeMatrix(matDiffResult, MAX);
		freeMatrix(matProductResult, MAX);
		free(sumThreads);
		free(diffThreads);
		free(productThreads);
    return 0;
  
}