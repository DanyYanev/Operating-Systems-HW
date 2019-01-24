//--------------------------------------------
// NAME: Daniel Yanev
// CLASS: XI A
// NUMBER: 08
// PROBLEM: miners n trader
//---------------------------------------------

#include<stdio.h>
#include<stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <errno.h>

#define BUYERS_COUNT 20
#define CARS_COUNT 5

pthread_mutex_t cars[CARS_COUNT];
pthread_t drivers[BUYERS_COUNT];
int counter = 0;

void * driver(void * a_id){
  int drove[CARS_COUNT];
  int keep_going;
  for(int i = 0; i < CARS_COUNT; i++){
    drove[i] = 0;
  }
  while(1){
    keep_going = 0;
    for(int i = 0; i < CARS_COUNT; i++){
      if(drove[i] == 0){
        if(pthread_mutex_trylock(&cars[i]) != EBUSY){
          printf("Buyer %ld takes car %d.\n", (int64_t)a_id, i+1);
          drove[i] = 1;
          sleep(1);
          printf("Buyer %ld returns car %d.\n", (int64_t)a_id, i+1);
          pthread_mutex_unlock(&cars[i]);
          // drove[i] = 1;
        }
        keep_going = 1;
      }
    }

    if(!keep_going){
      break;
    }
  }
  pthread_exit(0);
}

int main(){

  for(int i = 0; i < CARS_COUNT; i++){
    pthread_mutex_init(&cars[i], NULL);
  }

  for(int i = 0; i < BUYERS_COUNT; i++){
    int rc = pthread_create(&drivers[i], NULL, &driver, (void*)((int64_t)i+1));
    if(rc){
        printf("Err for pthread create\n");
    }
  }

  for(int i = 0; i < BUYERS_COUNT; i++){
    pthread_join(drivers[i], NULL);
  }

  for(int i = 0; i < CARS_COUNT; i++){
    pthread_mutex_destroy(&cars[i]);
  }

  return 0;
}
