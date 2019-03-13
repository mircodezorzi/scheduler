#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "curse.h"
#include "process.h"
#include "utils.h"

void draw_border(int x, int y, int w, int h){
    cursorto(x, y)
    printf("\u250C");
    for(int i = 0; i < w - 2; i++)
        printf("\u2500");
    printf("\u2510");

    for(int i = 1; i < h; i++) {
        cursorto(x, y + i)
        printf("\u2502%*s\u2502", w - 2, "");
    }

    cursorto(x, y + h);
    printf("\u2514");
    for(int i = 0; i < w - 2; i++)
        printf("\u2500");
    printf("\u2518");
}

/**
 * Draw a bar graph based on the PID of a process.
 */
void draw_process_graph(Process *p, int n, int x, int y, int w, int h){
    draw_border(x, y, w, h);
    for(int i = 0; i < n; i++) {
        printf("\033[38;5;%dm", colorhash(p[i].name, strlen(p[i].name)));
        for(int j = 0; j < p[i].name[1] - 0x30; j++) {
            cursorto(x + i + 1, y + h - 1 - j);
            printf("\u2588");
        }
    }
    printf("\033[38;5;255m");
}

Menu menu_new(Entry *entries, int nentries, int x, int y, int w, int h){
    Menu m;
    m.x = x;
    m.y = y;
    m.w = w;
    m.h = h;
    m.entries = entries;
    for(int i = 0; i < nentries; i++)
        m.entries[i].length = 0;
    m.nentries = nentries;
    m.selected = 0;
    m.scrolled = 0;
    return m;
}

void menu_print(int w, char* str, int len){
    char format[w];
    if(len > w) {
        putchar('.');
        putchar('.');
        putchar('.');
        for(int i = len - w + 2; i < len; i++)
            putchar(str[i]);
    } else {
        for(int i = -1; i < w - len; i++)
            putchar(' ');
        for(int i = 0; i < len; i++)
            putchar(str[i]);
    }
}

void menu_draw(Menu m){
    char format[128];
    int pad = 8;
    int cl = 1;

    draw_border(m.x, m.y, m.w, m.h);
    for(int i = m.scrolled; i < m.scrolled + m.h - 1; i++) {
        cursorto(m.x + 1, m.y + cl++);
        if(m.selected == i)
            printf("\033[0;30;41m");
        switch(m.entries[i].type) {
        case String:
            sprintf(format, "%%-%d.%ds", pad, pad);
            printf(format, m.entries[i].label);
            menu_print(m.w - pad - 3, ((char*)m.entries[i].value), m.entries[i].length);
            break;
        case Integer:
            sprintf(format, "%%-%d.%ds %%%d.d", pad, pad, m.w - pad - 3, m.w - pad - 3);
            printf(format, m.entries[i].label, *((int*)m.entries[i].value));
            break;
        }
        if(m.selected == i)
            printf("\033[0;30;0m");
    }
}

void menu_input(Menu *m){
    char key;
    while(1) {
        menu_draw(*m);
        switch(key = getchar()) {

        case CTRLMASK('c'):
            return;

        case '\033':
            getchar();
            switch(getchar()) {
            case 'A':

                if(m->selected == 0) {
                    m->selected = m->nentries - 1;
                    m->scrolled = m->nentries + 1 - m->h;
                } else {
                    m->selected--;
                    if(m->selected < m->scrolled + 1 && m->scrolled > 0)
                        m->scrolled--;
                }
                break;
            case 'B':
                goto next;
            }
            break;

        case '\t':
            next:
            if(m->selected == m->nentries - 1) {
                m->selected = 0;
                m->scrolled = 0;
            } else {
                m->selected++;
                if(m->selected > m->h - 3 && m->scrolled < m->h - 3) {
                    m->scrolled++;
                }
            }
            break;

        case 127:
            switch(m->entries[m->selected].type) {
            case String:
                if(!m->entries[m->selected].length) break;
                *(((char*)(m->entries[m->selected].value))\
                + --m->entries[m->selected].length) = '\0';
                break;
            case Integer:
                (*((int*)m->entries[m->selected].value)) /= 10;
                break;
            }
            break;

        default:
            switch(m->entries[m->selected].type) {
            case String:
                if(m->entries[m->selected].length == 127) break;
                *(((char*)(m->entries[m->selected].value))\
                + m->entries[m->selected].length++) = key;
                *(((char*)(m->entries[m->selected].value))\
                + m->entries[m->selected].length) = '\0';
            case Integer:
                if(key < '0' || key > '9') break;
                (*((int*)m->entries[m->selected].value)) =\
                (*((int*)m->entries[m->selected].value)) * 10 + key - 0x30;
                break;
            }
            break;
        }
    }
}
