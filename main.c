#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>

#include "curse.h"
#include "process.h"
#include "utils.h"

#define MAXSCHEDULE 128

Process menu_process_new(){

    static int pid = 1;
    char spid[16];
    sprintf(spid, "PID %d", pid);

    int w = 40;

    Process p;
    p.priority = 0;
    p.t_arrival = 0;
    p.t_duration = 0;
    p.pid = pid++;
    Entry entries[] = {
        NEWENTRY("Name",     String,   p.name),
        NEWENTRY("Priority", Integer, &p.priority),
        NEWENTRY("Arrival",  Integer, &p.t_arrival),
        NEWENTRY("Duration", Integer, &p.t_duration),
    };
    Menu menu = menu_new(entries, SIZE(entries), spid, 1, 15, w - 2, 6);
    menu_input(&menu);
    return p;
}

void schedule_pushback(Process* s, int n, Process p){
    s[n - 1] = p;
}

int main(){

    initwin();

    Process schedule[MAXSCHEDULE];
    int processes = 0;

    while(1) {

        draw_process_graph(schedule, processes, 0, 0, term_w, 12, 2);
        draw_block(10, 10, 5, 5, 2, 1);

        clear(0, term_h - 2, 1, term_w);
        cursorto(1, term_h);
        if(processes < MAXSCHEDULE)
            CMDDEF("a", "add process");


        switch(getchar()) {
            case 'a':
                if(processes >= MAXSCHEDULE) break;
                schedule_pushback(schedule, ++processes, menu_process_new());
                break;
            case CTRLMASK('C'):
                endwin();
                return 0;
        }
    }

}
