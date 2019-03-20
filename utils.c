#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <time.h>
#include <unistd.h>

#include "utils.h"

void resize_handler(int sig) {
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    term_h = w.ws_row;
    term_w = w.ws_col;
    signal(SIGWINCH, resize_handler);
}

void initwin(){
    srand(time(NULL));
    system("clear");
    system("stty raw");
    system("stty -echo");
    printf("\033[?25l");
    resize_handler(0);
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
