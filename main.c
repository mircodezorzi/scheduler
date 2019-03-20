#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>

#include "curse.h"
#include "process.h"
#include "utils.h"

Process menu_process_new(){
    Process p;
    p.priority = 0;
    p.t_arrival = 0;
    p.t_duration = 0;
    Entry entries[] = {
        ENTRY("Name",     String,   p.name),
        ENTRY("Priority", Integer, &p.priority),
        ENTRY("Arrival",  Integer, &p.t_arrival),
        ENTRY("Duration", Integer, &p.t_duration),
    };
    Menu menu = menu_new(entries, SIZE(entries), "tmp", 5, 15, 30, 6);
    menu_input(&menu);
    return p;
}

int main(){

    initwin();

    Process schedule[] = {
        { .name = "p1", .t_arrival = 0,   .t_duration = 45, .pid = 1 },
        { .name = "p2", .t_arrival = 10,  .t_duration = 30, .pid = 2 },
        { .name = "p3", .t_arrival = 25,  .t_duration = 40, .pid = 3 },
        { .name = "p4", .t_arrival = 30,  .t_duration = 30, .pid = 4 },
        { .name = "p5", .t_arrival = 60,  .t_duration =  5, .pid = 5 },
    };

    draw_process_graph(schedule, 5, 0, 0, 50, 12);
    draw_line(30, 30, 10, 1, 1);
    draw_line(31, 30, 10, 1, 0);
    draw_line(30, 29, 10, 0, 1);
    draw_line(30, 28, 10, 0, 0);

    while(1) {
        switch(getchar()) {
            case 'a':
                menu_process_new();
                break;
            default:
                endwin();
                return 0;
        }
    }

}
