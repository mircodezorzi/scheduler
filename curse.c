#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "curse.h"
#include "process.h"
#include "utils.h"

void clear(int x, int y, int h, int w){
    for(int i = 0; i < h; i++) {
        cursorto(x, y + i);
        printf("%*s", w, "");
        for(int j = 0; j < w; j++) putchar(' ');
    }
}

void draw_border(int x, int y, int w, int h){
    cursorto(x, y);
    printf("\u250C");
    for(int i = 0; i < w - 2; i++)
        printf("\u2500");
    printf("\u2510");

    for(int i = 1; i < h - 1; i++) {
        cursorto(x, y + i);
        printf("\u2502%*s\u2502", w - 2, "");
    }

    cursorto(x, y + h - 1);
    printf("\u2514");
    for(int i = 0; i < w - 2; i++)
        printf("\u2500");
    printf("\u2518");
}

/**
 * Draw line of x, y position and n length
 * @param int d if 0 draw an horizzontal line, else a vertical one
 * @param int e if 1 draw special character at the ends of the line
 */
void draw_line(int x, int y, int n, int d, int e){
    cursorto(x, y);
    for(int i = 0; i < n; i++) {
        if(!i && e) {
            printf("%s", d ? "\u252C" : "\u251C");
        } else if(!(n - i - 1) && e) {
            printf("%s", d ? "\u2534" : "\u2524");
        } else {
            printf("%s", d ? "\u2502" : "\u2500");
        }
        if(d) cursorto(x, y + i + 1);
    }
}

/**
 * @param int c block color
 * @param int s if 1 draw a shadow arround the block
 */
void draw_block(int x, int y, int w, int h, int c, int s){
    printf("\033[38;5;%dm", c);
    for(int i = 0; i < h; i++) {
        cursorto(x, y + i);
        for(int j = 0; j < w; j++) {
            printf("\u2588");
        }
    }
    printf("\033[38;5;255m");
    if(s) {
        cursorto(x + 1, y + h);
        for(int i = 0; i < w; i++)
            printf("\u2591");
        for(int i = 0; i < h; i++) {
            cursorto(x + w, y + i + 1);
            printf("\u2591");
        }
    }
}

/**
 * Draw a bar graph based on the PID of a process.
 * Bar color is based on the name of the process.
 */
void draw_process_graph(Process *p, int n, int x, int y, int w, int h, int bw){

    char block[8][4] = {
        "\u2581", "\u2582", "\u2583", "\u2584",
        "\u2585", "\u2586", "\u2587", "\u2588"
    };

    int lower = p[0].pid;
    int upper = p[0].pid;
    for(int i = 0; i < n; i++) {
        if(p[i].pid > lower) lower = p[i].pid;
        if(p[i].pid < upper) upper = p[i].pid;
    }

    float range = (((float)(upper) / (float)(lower)) * (h - 2));

    draw_border(x, y, w, h);
    for(int i = 0; i < n; i++) {
        printf("\033[38;5;%dm", colorhash(p[i].name, strlen(p[i].name)));
        for(int k = 0; k < bw; k++) {
            for(int j = 0; j < p[i].pid * range; j++) {
                if(k + i * bw + 1 > w - 2) goto close;
                cursorto(x + k + i * bw + 1, y + h - 2 - j);
                printf("%s", block[7]);
            }
        }
    }

    close:
    printf("\033[38;5;255m");
    draw_process_graph_info(p, n, x, y + h + 1, w, h);
}

void draw_process_graph_info(Process *p, int n, int x, int y, int w, int h){
    cursorto(x, y);
    for(int i = 0; i < n; i++) {
        printf("\033[38;5;%dm", colorhash(p[i].name, strlen(p[i].name)));
        printf(" \u2588 \033[38;5;255m %s", p[i].name);
    }
}

Menu menu_new(Entry* entries, int nentries, char* title, int x, int y, int w, int h){
    Menu m;
    m.x = x;
    m.y = y;
    m.w = w;
    m.h = h;
    m.title = title;
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
    int pad = 10;
    int cl = 1;

    m.x = (term_w - m.w) / 2;

    draw_border(m.x, m.y, m.w, m.h);

    //cursorto(m.x + pad / 2 - strlen(m.title) / 2 - 2, m.y);
    cursorto(m.x + m.w / 2 - strlen(m.title) / 2 - 2, m.y);
    printf("\u2524 %s \u251C", m.title);

    for(int i = m.scrolled; i < m.scrolled + m.h - 2; i++) {
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
            sprintf(format, "%%-%d.%ds %%%d.d", pad, pad, m.w - pad - 3);
            printf(format, m.entries[i].label, *((int*)m.entries[i].value));
            break;
        }
        if(m.selected == i)
            printf("\033[0;30;0m");
    }

    draw_line(m.x + pad, m.y, m.h, 1, 1);

    clear(0, term_h - 2, 1, term_w);
    cursorto(1, term_h);
    CMDDEF("C-C", "exit");
    CMDDEF("Tab", "next");
}

void menu_input(Menu* m){
    char key;
    while(1) {
        menu_draw(*m);
        switch(key = getchar()) {

        case CTRLMASK('c'):
            clear(m->x, m->y, m->h, m->w);
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
                if(m->selected > m->h - 2 && m->scrolled < m->h - 2) {
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
            if(key < ' ' || key > '~') break;
            switch(m->entries[m->selected].type) {
            case String:
                if(m->entries[m->selected].length > 127) break;
                *(((char*)(m->entries[m->selected].value))\
                + m->entries[m->selected].length++) = key;
                *(((char*)(m->entries[m->selected].value))\
                + m->entries[m->selected].length) = '\0';
                break;
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
