//--------------------------------------------
//NAME: Dany Yanev
//CLASS: XIA
//NUMBER: 08
//PROBLEM: ls
//FILE NAME: ls.h
//FILE PURPOSE: main ls function and printing
//--------------------------------------------
#ifndef LS_H_
#define LS_H_

#include <errno.h>
#include <time.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <pwd.h>
#include <grp.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "stack.h"
#include "helper.h"

int ls(char *, int *, int);

//--------------------------------------------
// FUNCTION: print_file
// Prints properly stats of files according to flags
// PARAMETERS:
// sb - stat of file
// filename - path to file
// flags - pointer to flags bitmap
//----------------------------------------------
int print_file(struct stat sb, char * filename, int * flags){

    switch (sb.st_mode & S_IFMT) {
           case S_IFBLK:  printf("b");           break;
           case S_IFCHR:  printf("c");           break;
           case S_IFDIR:  printf("d");           break;
           case S_IFIFO:  printf("p");           break;
           case S_IFLNK:  printf("l");           break;
           case S_IFREG:  printf("-");           break;
           case S_IFSOCK: printf("s");           break;
           default:       printf("Now what?");   break;
   }

   if(flags[TAG_l]){
        struct passwd * pw = getpwuid(sb.st_uid);
        struct group  *gr = getgrgid(sb.st_gid);
        char *date = convert_date(sb.st_mtim.tv_sec);

        if(date == NULL){
            return -1;
        }

        printf( (sb.st_mode & S_IRUSR) ? "r" : "-");
        printf( (sb.st_mode & S_IWUSR) ? "w" : "-");
        printf( (sb.st_mode & S_IXUSR) ? "x" : "-");
        printf( (sb.st_mode & S_IRGRP) ? "r" : "-");
        printf( (sb.st_mode & S_IWGRP) ? "w" : "-");
        printf( (sb.st_mode & S_IXGRP) ? "x" : "-");
        printf( (sb.st_mode & S_IROTH) ? "r" : "-");
        printf( (sb.st_mode & S_IWOTH) ? "w" : "-");
        printf( (sb.st_mode & S_IXOTH) ? "x" : "-");

        printf(" %ld %s %s %ld %s",
           sb.st_nlink,
           pw->pw_name,
           gr->gr_name,
           sb.st_size,
           date);

        free(date);
   }

   printf(" %s\n", get_filename(filename));

   return 1;
}

//--------------------------------------------
// FUNCTION: manage_dir
// Manages directory properly according to flags
// PARAMETERS:
// path - path to file
// flags - pointer to flags bitmap
// master - Deepness in recurtion (0 = first call)
// sumrun - Whether or not the function is called only to calculate the sum of blocks
// sumrun - in case of -1 just prints the file ignoring all flags (to stat the dir before itterating it later) -R
//----------------------------------------------
int manage_dir(char * path, int * flags, int master, int sumrun){
    DIR * dfd;
    struct dirent * dp;
    struct stack stack;
    char * pathname;
    int total = 0;

    stack_init(&stack);

    if ((dfd = opendir(path)) == NULL){
        char err[strlen("ls: cannot access ''") + strlen(path) + 3];
        sprintf(err, "ls: cannot access '%s'", path);
        perror(err);
        exit(EXIT_FAILURE);
    }

    //Print dir header if needed
    if((flags[TAG_MULT] || flags[TAG_R]) && (sumrun != 2)) {
        printf("%s:\n", path);
    }

    errno = 0;

    while ((dp = readdir(dfd)) != NULL){
        struct stat stbuf;;

        //Filter allowed files
        if(!allowed_files(dp->d_name, flags)){
            continue;
        }

        //Build path
        pathname = malloc(sizeof(char) * strlen(path) + strlen(dp->d_name));
        sprintf(pathname, "%s/%s",path,dp->d_name) ;

        if(lstat(pathname, &stbuf) == -1 ){
            char err[strlen("ls: cannot access ''") + strlen(path) + 3];
            sprintf(err, "ls: cannot access '%s'", path);
            perror(err);
            free(pathname);
            continue;
        }

        if(sumrun == 1)
            total += stbuf.st_blocks;
        else
            if(flags[TAG_R] && ((stbuf.st_mode & S_IFMT) == S_IFDIR)){
                stack_push(&stack, strdup(pathname));
                ls(pathname, flags, -1); //Strictly print file
            } else {
                ls(pathname, flags, master+1);
            }

        free(pathname);
    }

    if(errno != 0){
        perror("ls");
        exit(EXIT_FAILURE);
    }

    if( (closedir(dfd)) != 0){
        char err[strlen("ls: cannot access ''") + strlen(path) + 3];
        sprintf(err, "ls: cannot access '%s'", path);
        perror(err);
    }

    for(int i = 0; i < stack.size; i++){
        if(i < stack.size){
            printf("\n");
        }

        ls(stack.paths[i], flags, master+1);
    }

    stack_destroy(&stack);

    if(sumrun == 1){
        printf("total %d\n", total/2);
        manage_dir(path, flags, master, 2);
    }
}

//--------------------------------------------
// FUNCTION: ls
// Main ls function recursively called by all daughter
// PARAMETERS:
// path - path to file
// flags - pointer to flags bitmap
// master - Deepness in recurtion (0 = first call)
//----------------------------------------------
int ls(char * path, int * flags, int master){
    struct stat sb;
    int printflag = 1;

    if (lstat(path, &sb) == -1) {
        char err[strlen("ls: cannot access ''") + strlen(path) + 1];
        sprintf(err, "ls: cannot access '%s'", path);
        perror(err);
        exit(EXIT_FAILURE);
    }

   switch (sb.st_mode & S_IFMT) {
        case S_IFDIR:
            if((master > 1 && flags[TAG_R] == 0) || master == -1){ //Print regular
                print_file(sb, get_filename(path), flags);
            } else {
                if(flags[TAG_l])
                    manage_dir(path, flags, master + 1, 1);
                else
                    manage_dir(path, flags, master + 1, 0);
            }
            break;

        default:
            print_file(sb, get_filename(path), flags);
            break;
    }
}

#endif
