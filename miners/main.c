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

pthread_mutex_t mutex;
int bank = 0;

void * miner(void * a_id){
  int64_t id = (int64_t)a_id;
  for(int i = 0; i < 20; i++){
    pthread_mutex_lock(&mutex);
    printf("Miner %ld gathered 10 gold\n", id);
    bank += 10;
    pthread_mutex_unlock(&mutex);
    sleep(2);
  }
  pthread_exit(0);
}

void * trader(void * a_id){
  int64_t id = (int64_t)a_id;
  for(int i = 0; i < 20; ++i){
    pthread_mutex_lock(&mutex);
    if(bank <= 0){
      printf("The warehouse is empty, cannot sell!\n");
    } else {
      printf("Trader %ld sold 10 gold\n", id);
      bank -= 10;
    }
    pthread_mutex_unlock(&mutex);
    sleep(2);
  }
  pthread_exit(0);
}

int main(int argc, char *argv[]){
  int miners = 1;
  int traders = 1;
  pthread_t threads[miners + traders];
  int counter = 0;

  if(argc == 3){
    miners = atoi(argv[1]);
    traders = atoi(argv[2]);
  }

  pthread_mutex_init(&mutex, NULL);

  // printf("Miners: %d\nTraders: %d\n", miners, traders);


  for(int i = 0; i < miners; i++){
    int rc = pthread_create(&threads[counter++], NULL, &miner, (void*)((int64_t)i+1));
    if(rc){
        printf("Err for pthread create\n");
    }
  }

  for(int i = 0; i < traders; i++){
    int rc = pthread_create(&threads[counter++], NULL, &trader, (void*)((int64_t)i+1));
    if(rc){
        printf("Err for pthread create\n");
    }
  }

  for(int i = 0; i < miners + traders; i++){
    pthread_join(threads[i], NULL);
  }

  printf("Gold: %d\n", bank);

  pthread_mutex_destroy(&mutex);

  return 0;
}
