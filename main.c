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

    int w = term_w;
    int h = term_h;

    Process p;
    p.priority = 0;
    p.memory = 0;
    p.t_arrival = 0;
    p.t_duration = 0;
    p.pid = pid++;
    p.nstages = 5;
    strcpy(p.name, spid);
    Entry entries[] = {
        NEWENTRY("Name",     String,   p.name),
        NEWENTRY("Memory",   Integer, &p.memory),
        NEWENTRY("Priority", Integer, &p.priority),
        NEWENTRY("Arrival",  Integer, &p.t_arrival),
        //NEWENTRY("Duration",  Integer, &p.t_duration),
        NEWENTRY("Stages", Integer, &p.nstages),
        { .label = "", .type = ProcessStage, .value = p.stages, .count = &p.nstages },
    };

    Menu menu = menu_new(entries, SIZE(entries), spid, 2, 2, w - 4, h - 6);
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

int menu_process_remove(){
    int pid = 0;
    int w = 40;
    Entry entries[] = {
        NEWENTRY("PID", Integer, &pid),
    };
    Menu menu = menu_new(entries, 1, "PID of process to remove", 1, 15, w - 2, 3);
    menu_input(&menu);
    return pid;
}

void schedule_pushback(Process* s, int n, Process p){
    s[n - 1] = p;
}

int main(){

    initwin();
    Process p = menu_process_new();
    endwin();
    return 0;

    initwin();

    struct DynamicMemory* memory = dynamicmemory_new(100);
    /* TODO: implement schedule and other process lists
             as linked lists or dynamic arrays
    */
    Process schedule[MAXSCHEDULE];
    int processes = 0;

    while(1) {

        draw_process_graph(schedule, processes, 0, 3, term_w, 12, 4);
        draw_dynamicmemory(memory, 0, 0, term_w, 3);
        if(processes > 0)
            draw_process(schedule[0], 10, 10, 10, 10);

        clear(0, term_h - 2, 1, term_w);
        cursorto(1, term_h);
        if(processes < MAXSCHEDULE)
            CMDDEF("a", "add process");
        CMDDEF("r", "remove process");

        Process tmp;
        int pid;

        switch(getchar()) {
            case 'a':
                if(processes >= MAXSCHEDULE) break;
                tmp = menu_process_new();
                if(!tmp.pid) break;
                schedule_pushback(schedule, ++processes, tmp);
                dynamicmemory_malloc(memory, tmp.memory, tmp.name);
                break;
            case 'r':
                pid = menu_process_remove();
                for(int i = 0; i < processes; i++) {
                    if(schedule[i].pid == pid) {
                        // TODO: remove process from schedule
                        strcpy(schedule[i].name, "removed");
                    }
                }
                break;
            case CTRLMASK('C'):
                endwin();
                return 0;
        }
    }

}
