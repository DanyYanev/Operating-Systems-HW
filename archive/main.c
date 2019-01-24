#define _GNU_SOURCE

#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdint.h>

void *get_primes(void *arg) {
    int64_t max = (int64_t) arg;
    int primes = 0;

    printf("Prime calculation started for N=%ld\n", max);

    for (int i = 2; i <= max; i++) {
        int divided = 0;
        for (int j = i - 1; j != 1; j--) {
            if (i % j == 0) {
                divided = 1;
                break;
            }
        }

        if (!divided)
            primes++;
    }

    printf("Number of primes for N=%ld is %d\n", max, primes);

    return NULL;
}


int main() {
    while (1) {
        char *command = NULL;
        size_t n = 0;
        getline(&command, &n, stdin);

        if (command[0] == 'e') {
            free(command);
            pthread_exit(0);
        } else if (command[0] == 'p') {
            pthread_t id;
            int primes = atoi(command + 2);

            pthread_create(&id, NULL, &get_primes, (void*)((int64_t)primes));
        } else {
            printf(
                "Supported commands:\n\
                p N - Starts a new calculation for the number of primes from 1 to N\n\
                e - Waits for all calculations to finish and exits\n"
            );
        }

        free(command);
    }

    return 0;
}
