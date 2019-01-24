//--------------------------------------------
//NAME: Dany Yanev
//CLASS: XIA
//NUMBER: 08
//PROBLEM: starcraft3
//FILE NAME: main.c
//FILE PURPOSE: main
//--------------------------------------------
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

#define M_CAPACITY 500
#define MAX_THREADS 200

typedef struct{
  int id;
  short * mines;
  int * bank;
} arg_struct;

pthread_mutex_t * mine_mutex;
pthread_mutex_t bank_mutex;
arg_struct * arg_arr;

int mines_count = 2;

void clear_resources(int code){
  free(mine_mutex);
  free(arg_arr);
  exit(code);
}

void cleanup(int code){ //Destroy mutexes and clear memory
  int rc = pthread_mutex_destroy(&bank_mutex);
  if(rc != 0){
    perror("Mutex failed.\n");
    clear_resources(EXIT_FAILURE);
  }

  for(int i = 0; i < mines_count; ++i){
    rc = pthread_mutex_destroy(&mine_mutex[i]);
    if(rc != 0){
      perror("Mutex failed.\n");
      clear_resources(EXIT_FAILURE);
    }
  }
  clear_resources(code);
}

void *scv(void * args){
  arg_struct * manual = (arg_struct *)args;
  int stash;
  int mines_state[mines_count];

  for(int i = 0; i < mines_count; ++i){
    mines_state[i] = 1; //1 - has minerals, 0 - empty
  }

  for(unsigned int i = 0; 1; i++){ //Overflowing unsigned integer is fine. -Wikipedia

    if(mines_state[i % mines_count] == 1){

      sleep(3);

      int rc = pthread_mutex_trylock(&mine_mutex[i % mines_count]);

      if(rc == 0){
        if(manual->mines[i % mines_count] <= 0){
          //Mark empty and check for exit condition

          mines_state[i % mines_count] = 0;
          rc = pthread_mutex_unlock(&mine_mutex[i % mines_count]);

          if(rc != 0){
            perror("Mutex func failed.\n");
            cleanup(EXIT_FAILURE);
          }

          int exit_c = 1;
          for(int i = 0; i < mines_count; i++){
            if(mines_state[i % mines_count] == 1)
              exit_c = 0;
          }
          if(exit_c){
            return NULL;
          }
          continue;
        }


        printf("SCV %d is mining from mineral block %d\n", manual->id, (i % mines_count) + 1); //Mines start from 1 :/

        if(manual->mines[i % mines_count] < 8){
          stash = manual->mines[i % mines_count];
          manual->mines[i % mines_count] = 0;
        } else {
          manual->mines[i % mines_count] -= 8;
          stash = 8;
        }
        rc = pthread_mutex_unlock(&mine_mutex[i % mines_count]);
        if(rc != 0){
          perror("Mutex func failed.\n");
          cleanup(EXIT_FAILURE);
        }

        printf("SCV %d is transporting minerals\n", manual->id);
        sleep(2);

        rc = pthread_mutex_lock(&bank_mutex);
        if(rc != 0){
          perror("Mutex func failed.\n");
          cleanup(EXIT_FAILURE);
        }

        *manual->bank += stash;
        printf("SCV %d delivered minerals to the Command center\n", manual->id);

        rc = pthread_mutex_unlock(&bank_mutex);
        if(rc != 0){
          perror("Mutex func failed.\n");
          cleanup(EXIT_FAILURE);
        }
      } else if(rc != EBUSY){
        perror("Mutex failed.\n");
        cleanup(EXIT_FAILURE);
      }
    }
  }
}

int main(int argc, char *argv[]) {

  if(argc == 2)
    mines_count = atoi(argv[1]);

  short mines[mines_count];
  int bank = 0;
  int scv_count = 0;
  int soldier_count = 0;
  int create_soldier;
  int create_scv;

  mine_mutex = malloc(mines_count * sizeof(pthread_mutex_t)); //Malloc nikoga ne fail-va -Aleksandur Spasov
  arg_arr = malloc(MAX_THREADS * sizeof(arg_struct));
  pthread_t threads[MAX_THREADS];

  //Filling mines and initializing mutexes
  for(int i = 0; i < mines_count; ++i){
    mines[i] = M_CAPACITY;
    int rc = pthread_mutex_init(&mine_mutex[i], NULL);
    if(rc != 0){
      perror("Mutex func failed.\n");
      cleanup(EXIT_FAILURE);
    }
  }

  int rc = pthread_mutex_init(&bank_mutex, NULL);
  if(rc != 0){
    perror("Mutex func failed.\n");
    cleanup(EXIT_FAILURE);
  }


  //Starting workers
  for(int i = 0; i < 5; ++i){
    arg_arr[scv_count].id = scv_count;
    arg_arr[scv_count].mines = mines;
    arg_arr[scv_count].bank = &bank;

    int rc = pthread_create(&threads[scv_count], NULL, scv, (void *)&arg_arr[scv_count]);

    if(rc != 0){
      perror("Err for pthread create\n");
      cleanup(EXIT_FAILURE);
    }else
      scv_count++;
  }

	while(1) {
		int ch = getchar();
		switch(ch) {
			case 'm':

        create_soldier = 0;

				int rc = pthread_mutex_lock(&bank_mutex);
        if(rc != 0){
          perror("Mutex failed.\n");
          cleanup(EXIT_FAILURE);
        }

				if(bank >= 50){
					bank -= 50;
          soldier_count++;
          create_soldier = 1;
        }
        rc = pthread_mutex_unlock(&bank_mutex);
        if(rc != 0){
          perror("Mutex failed.\n");
          cleanup(EXIT_FAILURE);
        }

        if(create_soldier){
          sleep(1);
          printf("You wanna peace of me, boy?\n");
          if(soldier_count == 20){
            //Endgame condition
            for(int i = 0; i < scv_count; i++){
              int rc = pthread_join(threads[i], NULL);

              if(rc != 0){
                perror("Join error\n");
                cleanup(EXIT_FAILURE);
              }
            }

            int rc = pthread_mutex_lock(&bank_mutex);
            if(rc != 0){
              perror("Mutex failed.\n");
              cleanup(EXIT_FAILURE);
            }

            printf("Map minerals %d, player minerals %d, SCVs %d, Marines %d\n", mines_count * M_CAPACITY, bank, scv_count, soldier_count);

            rc = pthread_mutex_unlock(&bank_mutex);
            if(rc != 0){
              perror("Mutex failed.\n");
              cleanup(EXIT_FAILURE);
            }
            cleanup(EXIT_SUCCESS);
          }
				} else {
					printf("Not enough minerals!\n");
				}

				break;
			case 's':

        create_scv = 0;

				rc = pthread_mutex_lock(&bank_mutex);
        if(rc != 0){
          perror("Mutex failed.\n");
          cleanup(EXIT_FAILURE);
        }
				if(bank >= 50){
          create_scv = 1;
        }
        rc = pthread_mutex_unlock(&bank_mutex);
        if(rc != 0){
          perror("Mutex failed.\n");
          cleanup(EXIT_FAILURE);
        }

        if(create_scv){
          sleep(4);

          arg_arr[scv_count].id = scv_count;
          arg_arr[scv_count].mines = mines;
          arg_arr[scv_count].bank = &bank;

          int rc = pthread_create(&threads[scv_count], NULL, scv, (void *)&arg_arr[scv_count]);

          if(rc != 0){
            perror("Err for pthread create\n");
            cleanup(EXIT_FAILURE);
          } else {
            //We shall not scam you for your minerals.
            int rc = pthread_mutex_lock(&bank_mutex);
            if(rc != 0){
              perror("Mutex failed.\n");
              cleanup(EXIT_FAILURE);
            }

            bank -= 50;

            rc = pthread_mutex_unlock(&bank_mutex);
            if(rc != 0){
              perror("Mutex failed.\n");
              cleanup(EXIT_FAILURE);
            }
            scv_count++;
            printf("SCV good to go, sir.\n");
          }
				} else {
					printf("Not enough minerals.\n");
				}
        break;
		}
	}
  // exit(EXIT_FAILURE);
}
