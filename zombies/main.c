#include "ui.h"
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

int soldiers = 0;
int gold = 100;
int zombies = 2;
int health = 100;

pthread_t _;

void *miner(){
	while(1){
		pthread_mutex_lock(&mutex);
		gold+=10;
		pthread_cond_broadcast(&cond);
		print_gold(gold);
		pthread_mutex_unlock(&mutex);
		sleep(1);
	}
}

void *zombies_f(){
	while(1){
		for(int i = 0; i < 5; i++){
			if(i == 0){
				pthread_mutex_lock(&mutex);
				while(gold < 100){
					pthread_cond_wait(&cond, &mutex);
				}
				pthread_mutex_unlock(&mutex);
			}
			print_zombies(5 - i, zombies);
			sleep(1);
		}

		pthread_mutex_lock(&mutex);
		if(zombies > soldiers){
			health -= zombies - soldiers;
			print_fail("Zombie attack succeded :(!");
			if(health < 0){
				game_end(zombies);
			} else {
				print_health(health);
			}
		} else {
			print_succ("Zombie attack deflected! :)");
		}
		zombies *= 2;
		// print_zombies(zombies);
		pthread_mutex_unlock(&mutex);
	}
}

int main() {
	pthread_mutex_init(&mutex, NULL);

	init();
	print_gold(gold);
	print_soldiers(soldiers);
	// print_zombies(5, zombies);
	print_health(health);

	pthread_create(&_, NULL, zombies_f, NULL);


	while(1) {
		int ch = get_input();
		switch(ch) {
			case 'q':
				game_end(1);
				break;
			case 's':
				pthread_mutex_lock(&mutex);
				if(gold >= 10){
					print_soldiers(++soldiers);
					gold -= 10;
					print_gold(gold);
					print_msg("Soldier created!");
				} else {
					print_fail("Not enough gold!");
				}
				pthread_mutex_unlock(&mutex);
				break;
			case 'x':
				pthread_mutex_lock(&mutex);
				if(gold >= 100){
					soldiers += 10;
					print_soldiers(soldiers);
					gold -= 100;
					print_gold(gold);
					print_msg("10 x soldier created!");
				} else {
					print_fail("Not enough gold!");
				}
				pthread_mutex_unlock(&mutex);
				break;
			case 'm':

				pthread_mutex_lock(&mutex);
				if(gold >= 100){
					pthread_create(&_, NULL, miner, NULL);
					gold -= 100;
					print_gold(gold);
					print_msg("Miner created!");
				} else {
					print_fail("Not enough gold!");
				}
				pthread_mutex_unlock(&mutex);
				break;
		}
	}

	exit(0);
}
