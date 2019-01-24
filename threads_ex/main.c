//------------------------------------------------------------------------
// NAME: Daniel Yanev
// CLASS: XIa
// NUMBER: 8
// FILE NAME: main.c (unix file name)
//------------------------------------------------------------------------

#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

int calc_prime(int a){
  for(int i = a - 1; i >= 2; i--){
    if(a % i == 0) return 0;
  }
  return 1;
}

void * calc(void * a){
  int64_t n = (int64_t)a;
  printf("Prime calculation started for N=%ld\n", n);
  ssize_t count = 0;
  for(int i = 2; i < n; i++){
    if(calc_prime(i)){
      // printf("%d is prime, count %ldcalc((void*)&number)\n", i, count);
      count++;
    }
  }
  printf("Number of primes for N=%ld is %ld\n", n, count);
  pthread_exit(0);
}

int main(){

  char a;
  char *line = NULL;
  size_t len = 0;
  ssize_t nread;
  int counter = 0;
  pthread_t threads[100];
  // void * (*calc_ptr)(void*) = &calc;



  while((nread = getline(&line, &len, stdin)) != -1){
    line[strlen(line) - 1] = '\0';
    if(strcmp(line, "e") == 0){
      break;
    } else if(line[0] == 'p'){
      int number;
      sscanf(line + 2, "%d", &number);
      //printf("Number %d\n", number);
      int rc = pthread_create(&threads[counter++], NULL, &calc, (void*)((int64_t)number));
      if(rc){
          printf("Err for pthread create\n");
      }
    } else {
      printf("Supported commands:\np N - Starts a new calculation for the number of primes from 1 to N\ne - Waits for all calculations to finish and exits\n");
    }

    // printf("Before\n");
  } // END OF WHILE LOOP

  free(line);
  pthread_exit(0);
}
