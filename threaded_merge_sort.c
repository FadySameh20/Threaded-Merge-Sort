#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>


typedef struct Bounds {  //low and high bounds for the array
  int* arr;
  int low;
  int high;
} Bounds;


void* mergeSort(void*);
void merge(int*, int, int, int);
void checkThreadError(int); 
void checkMemoryFailure(int*);
void checkFileError(FILE*);


int main(int argc, char** argv) {
  int n;  //number of elements in the array
  char buff[255];
  
  FILE* fp;
  fp = fopen(argv[1], "r");
  checkFileError(fp);
  
  fscanf(fp, "%s", buff);  //scan number of elements in the array
  n = atoi(buff);  //convert string to integer
  
  Bounds bounds;  //Initial bounds of the array are: (0) and (n-1)
  bounds.low = 0;
  bounds.high = n - 1;
  bounds.arr = (int*) malloc(n * sizeof(int));  //allocate size for array
  checkMemoryFailure(bounds.arr);
  
  for(int i = 0; i < n; i++) {
    fscanf(fp, "%s", buff);  //scan elements of the array
    bounds.arr[i] = atoi(buff);
  }
  fclose(fp); 
  
  int check;
  pthread_t thread;  //create thread to call mergeSort() function
  check = pthread_create(&thread, NULL, mergeSort, &bounds);
  checkThreadError(check);  //check if there is error in thread creation
  check = pthread_join(thread, NULL);  //wait for the thread to finish
  checkThreadError(check);
  
  fp = fopen("output.txt", "w");
  checkFileError(fp);
  printf("Sorted array:\n");
  fprintf(fp, "Sorted array:\n");
  for(int i = 0; i < n; i++) {  //printing the sorted array
    printf("%d ", bounds.arr[i]);
    fprintf(fp, "%d ", bounds.arr[i]);
  }
  printf("\n");
  fclose(fp);
  
  free(bounds.arr);  //free the allocated memory for the array
  return 0;
}


void* mergeSort(void* arg) {
  Bounds* temp = arg;
  if(temp->low < temp->high)  //If the array contains more than one element
  {
    Bounds bounds[2];  //Bounds for each subarray
    pthread_t threads[2];  //Number of threads = Number of subproblems = 2
    int check;
    
    //Compute mid (this way is used to avoid overflow)
    int mid = temp->low + (temp->high - temp->low) / 2;
    
    //Seting bounds of the left subarray
    bounds[0].low = temp->low;
    bounds[0].high = mid; 
    bounds[0].arr = temp->arr;
    //Creating thread that sorts the left subarray
    check = pthread_create(&threads[0], NULL, mergeSort, &bounds[0]);
    checkThreadError(check);
    
    //Seting bounds of the right subarray
    bounds[1].low = mid + 1;
    bounds[1].high = temp->high;
    bounds[1].arr = temp->arr;
    //Creating thread that sorts the right subarray
    check = pthread_create(&threads[1], NULL, mergeSort, &bounds[1]);
    checkThreadError(check);
    
    for(int i = 0; i < 2; i++) {
      check = pthread_join(threads[i], NULL);  //wait for all threads to 							   finish
      checkThreadError(check);
    }
    
    merge(temp->arr, temp->low, mid, temp->high);  //combine subproblems
  }
}


void merge(int* arr, int low, int mid, int high) {
  int nL = mid - low + 1;  //number of elements in the left subarray
  int nR = high - mid;  //number of elements in the right subarray

  int* Left = (int*) malloc(nL * sizeof(int)); //allocate memory for arrays
  checkMemoryFailure(Left);
  int* Right = (int*) malloc(nR * sizeof(int));
  checkMemoryFailure(Right);

  for (int i = 0; i < nL; i++) {  //Filling left subarray
    Left[i] = arr[low + i];
  }
  
  for (int j = 0; j < nR; j++) {  //Filling right subarray
    Right[j] = arr[mid + 1 + j];
  }  

  int i = 0, j = 0, k = low;  //indices for left, right and merged arrays
  
  //Compare element in the left subarray with another in the right subarray and take the smaller to be put in the merged array
  while (i < nL && j < nR) {
      if (Left[i] <= Right[j]) {
          arr[k++] = Left[i++];
      }
      else {
          arr[k++] = Right[j++];
      }
  }

  //If left subarray still has elements that are not in the merged array
  while (i < nL) {
      arr[k++] = Left[i++];
  }
  
  //If right subarray still has elements that are not in the merged array
  while (j < nR) {
      arr[k++] = Right[j++];
  }
  
  free(Left);  //free the allicated memory for left and right subarrays
  free(Right);
}


void checkThreadError(int check) {
  if(check != 0)
  {
    printf("Error in creation of threads !");
    exit(-1);
  }
}


void checkMemoryFailure(int* ptr) {  //Check if malloc() fails
  if(ptr == NULL) {
    printf("Failure in memory allocation\n");
    exit(-1);
  }
}


void checkFileError(FILE* fp) {
  if(!fp)  //Check if there is an error in opening file
  {
    printf("Error can't open file !\n");
    exit(-1);
  } 
}
