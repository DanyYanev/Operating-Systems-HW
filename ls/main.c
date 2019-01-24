//--------------------------------------------
//NAME: Dany Yanev
//CLASS: XIA
//NUMBER: 08
//PROBLEM: ls
//FILE NAME: main.c
//FILE PURPOSE: main
//--------------------------------------------

#include "ls.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char *argv[]){
    int flags[4] = {0}; //[0] - l, [1] - A, [2] - R, [4] - multiple arguments
    int opt;
    char err[strlen("ls: invalid option -- ''") + 3];

    while ((opt = getopt(argc, argv, "lAR")) != -1) {
       switch (opt) {
       case 'l':
           flags[TAG_l] = 1;
           break;
       case 'A':
           flags[TAG_A] = 1;
           break;
       case 'R':
           flags[TAG_R] = 1;
           break;
       default: /* '?' */
           sprintf(err, "ls: invalid option -- '%c'", opt);
           perror(err);
           exit(EXIT_FAILURE);
       }
    }

    if(argc == optind){
        ls(".", flags, 1);
    } else {
        if(argc - optind > 1){
            flags[TAG_MULT] = 1;
        }

        for(int i = optind; argv[i] != NULL; i++){
            char * path = strdup(argv[i]);

            ls(path, flags, 1);


            if(argv[i + 1] != NULL){
                printf("\n");
            }

            free(path);
        }

    }

    exit(EXIT_SUCCESS);
}
