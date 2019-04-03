#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>

#include "curse.h"
#include "process.h"
#include "utils.h"
#include "memory.h"

#define MAXSCHEDULE 128

void alert(char* msg){
    int padding = (term_w - strlen(msg)) / 2;
    cursorto(padding, 2);
    HIGHLIGHT(msg);
}

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
    Menu menu = menu_new(entries, SIZE(entries), spid, 1, 15, w - 2, 7);
    menu_input(&menu);
    if(!(menu.entries[0].length)
      || p.t_arrival < 0
      || p.t_duration <= 0
      && p.priority >= 0

    ) {
        alert("Process invalid, please retry...");
        getchar();
        p.pid = 0;
        pid--;
    }
    return p;
}

void schedule_pushback(Process* s, int n, Process p){
    s[n - 1] = p;
}

void draw_dynamic_memory(struct DynamicMemory* m, int x, int y, int w, int h){
    float range = (((float)(m->size)) / ((float)(w)));
    for(struct Block* i = m->block; i->next != NULL; i = i->next) {
        for(int j = 0; j < range * i->size; j++) {
            printf("x");
        }
    }
}

int main(){

    struct DynamicMemory* memory = dynamicmemory_new(100);
    //dynamicmemory_malloc(memory, 10, 1);
    //draw_dynamic_memory(memory, 0, 0, 10, 10);


    initwin();

    Process schedule[MAXSCHEDULE];
    int processes = 0;

    while(1) {

        draw_process_graph(schedule, processes, 0, 0, term_w, 12, 4);

        clear(0, term_h - 2, 1, term_w);
        cursorto(1, term_h);
        if(processes < MAXSCHEDULE)
            CMDDEF("a", "add process");

        Process tmp;

        switch(getchar()) {
            case 'a':
                if(processes >= MAXSCHEDULE) break;
                tmp = menu_process_new();
                if(!tmp.pid) break;
                schedule_pushback(schedule, ++processes, tmp);
                break;
            case CTRLMASK('C'):
                endwin();
                return 0;
        }
    }

}
