#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define SIZE 100

int array[SIZE];

void fillArrayWithRandomNumbers(int arr[SIZE]) {
    for(int i = 0; i<SIZE; i++) array[i] = rand()%100;
}

void printArray(int arr[SIZE]) {
    for(int i = 0; i<SIZE; i++) printf("%5d", arr[i]);
    printf("\n");
}

typedef struct StartEndIndexes {
    int start;
    int end;
} StartEndIndexes;

void merge(int start, int mid, int end) {
   // merge arrays back into main array
    int i, j, k;
    // copy left and right temporarily
    int leftCount = mid - start + 1;
    int rightCount = end - mid;
    int L[leftCount], R[rightCount];
    for(i = 0; i < leftCount; ++i) {
      L[i] = array[i + start];
    }
    for(i = 0; i < rightCount; ++i) {
      R[i] = array[i + mid + 1];
    }
  
    // merge them back into original array
    i = 0;
    j = 0;
    k = start;
    
    while (i < leftCount && j < rightCount) {
      if (L[i] < R[j]) {
        array[k] = L[i];
        ++i;
      } else {
        array[k] = R[j];
        ++j;
      }
      ++k;
    }
    
    while (i < leftCount) {
      array[k] = L[i];
      ++i;
      ++k;
    }
    while (j < rightCount) {
      array[k] = R[j];
      ++j;
      ++k;
    }
}

// Runs mergesort on the array segment described in the
// argument. Spawns two threads to mergesort each half
// of the array segment, and then merges the results.
void* mergeSort(void* args) {
		StartEndIndexes sei = *((StartEndIndexes*) args);
    if (sei.start >= sei.end) {
      return NULL;
    }
  
    pthread_t t1, t2;
		int mid = sei.start + ((sei.end - sei.start) / 2);
  
		StartEndIndexes bounds1 = {sei.start, mid};
		StartEndIndexes bounds2 = {mid + 1, sei.end};
		pthread_create(&t1, NULL, mergeSort, &bounds1);
		pthread_create(&t2, NULL, mergeSort, &bounds2);
    
    pthread_join(t1, NULL);
		pthread_join(t2, NULL);
    
    merge(sei.start, mid, sei.end);

		return NULL;
}

int main() {
    srand(time(0));
    StartEndIndexes sei;
    sei.start = 0;
    sei.end = SIZE - 1;
    
    // 1. Fill array with random numbers.
    fillArrayWithRandomNumbers(array);
    
    // 2. Print the array.
    printf("Unsorted array: ");
    printArray(array);
    
    // 3. Create a 2 threads for merge sort.
    pthread_t t1, t2;
		int mid = sei.start + ((sei.end - sei.start) / 2);
		StartEndIndexes bounds1 = {sei.start, mid};
		StartEndIndexes bounds2 = {mid + 1, sei.end};
 
		pthread_create(&t1, NULL, mergeSort, &bounds1);
		pthread_create(&t2, NULL, mergeSort, &bounds2);
    
		// 4. Wait for mergesort to finish.
    pthread_join(t1, NULL);
		pthread_join(t2, NULL);
    
    merge(sei.start, mid, sei.end);
  
		// 5. Print the sorted array.
    printf("Sorted array:   ");
    printArray(array);
}
