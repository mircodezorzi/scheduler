#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "utils.h"

void initwin(){
    srand(time(NULL));
    system("clear");
    system("stty raw");
    system("stty -echo");
    printf("\033[?25l");
}

void endwin(){
    system("clear");
    system("stty cooked");
    system("stty echo");
    printf("\033[?25h");
}

int colorhash(char *str, int n){
    int hash = 0;
    for(int i = 0; i < n; i++) {
        hash += str[i];
    }
    return hash % 232;
}
