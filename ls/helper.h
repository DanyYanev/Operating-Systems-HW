//--------------------------------------------
//NAME: Dany Yanev
//CLASS: XIA
//NUMBER: 08
//PROBLEM: ls
//FILE NAME: helper.h
//FILE PURPOSE: Small helper functions
//--------------------------------------------

#ifndef HELPER_H_
#define HELPER_H_

#define TAG_l 0
#define TAG_A 1
#define TAG_R 2
#define TAG_MULT 3

//--------------------------------------------
// FUNCTION: allowed_files
// returns whether or not file should be displayed according to flags
// PARAMETERS:
// filename - path to file
// flags - pointer to flags bitmap
//----------------------------------------------
int allowed_files(char * filename, int * flags){
    if(strcmp(filename, ".") == 0 || strcmp(filename, "..") == 0){ //If flag A is on dont skip hidden files
        return 0;
    } else if(filename[0] == '.' && flags[TAG_A] == 0){
        return 0;
    }

    return 1;
}

//--------------------------------------------
// FUNCTION: convert_date
// Converts time_t params into proper string for -l
// PARAMETERS:
// stime - time_t struct to get all needed information for building -l
//----------------------------------------------
char *convert_date(time_t stime) {
    // "Malloc never fails" - Aleksander Spasov
    char *date = malloc(sizeof(char) * 56);
    struct tm *tm = localtime(&stime);

    if (strftime(date, 56, "%b %d %H:%M", tm)) {
        return date;
    } else {
        perror("Strftime failed :)");
        free(date);
        return NULL;
    }
}

//--------------------------------------------
// FUNCTION: get_filename
// return pointer to just filename from whole path
// PARAMETERS:
// path - path :)
//----------------------------------------------
char * get_filename(char * path){
    for(int i = strlen(path); i >= 0; i--){
        if(path[i] == '/'){
            return path + i + 1;
        }
    }

    return path;
}


#endif
