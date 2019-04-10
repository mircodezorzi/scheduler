#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "curse.h"
#include "process.h"
#include "utils.h"
#include "memory.h"

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
    printf("\033[38;5;255m");
}

void draw_dynamicmemory(struct DynamicMemory* m, int x, int y, int w, int h){
    float range = (((float)(w - 2)) / ((float)(m->size)));
    draw_border(x, y, w, h);
    cursorto(x + 1, y + 1);
    for(struct Block* i = m->block; i != NULL; i = i->next) {
        printf("\033[38;5;%dm", colorhash(i->name, strlen(i->name)));
        for(int j = 0; j < range * i->size - 1; j++) {
            printf(i->allocated ? "\u2588" : " ");
        }
    }
    printf("\033[38;5;255m");
}

void draw_process(Process p, int x, int y, int w, int h){
    draw_border(x, y, h, w);
    int block_size = 2;
    p.nstages = 3;
    int center = w + x / 2;
    for(int i = 0; i < p.nstages; i++) {
        draw_block(x + i * block_size + 1,
                   p.stages[i].type == Io ? center + block_size : center - block_size,
                   block_size,
                   block_size,
                   255, 0);
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
    for(int i = 0; i < nentries; i++) {
        m.entries[i].length = strlen(entries[i].label) + 1;
        if(m.entries[i].type == ProcessStage)
            m.entries[i].count = entries[i].count;
    }
    m.nentries = nentries;
    m.selected = 0;
    m.scrolled = 0;
    return m;
}

void menu_print(int w, char* str, int len){
    char format[w];
    if(len > w) {
        printf("...");
        for(int i = len - w + 2; i < len; i++)
            putchar(str[i]);
    } else {
        for(int i = -1; i < w - len; i++)
            putchar(' ');
        for(int i = 0; i < len; i++)
            putchar(str[i]);
    }
}

void menu_draw(Menu* m){
    char format[128];
    int pad = 10;
    int cl = 1;

    m->x = (term_w - m->w) / 2;

    draw_border(m->x, m->y, m->w, m->h);

    cursorto(m->x + m->w / 2 - strlen(m->title) / 2 - 2, m->y);
    printf("\u2524 %s \u251C", m->title);
    //draw_line(m->x + pad, m->y, m->h, 1, 1);

    m->nelements = m->nentries;
    for(int i = m->scrolled; i < m->scrolled + m->h - 2 && i < m->nelements; i++) {
        cursorto(m->x + 1, m->y + cl++);
        if(m->selected == i)
            printf("\033[0;30;41m");
        switch(m->entries[i].type) {
        case String:
            sprintf(format, "%%-%d.%ds\u2502", pad - 1, pad - 1);
            printf(format, m->entries[i].label);
            menu_print(m->w - pad - 3, ((char*)m->entries[i].value), m->entries[i].length);
            break;
        case Integer:
            sprintf(format, "%%-%d.%ds\u2502%%%d.1d", pad - 1, pad - 1, m->w - pad - 2);
            printf(format, m->entries[i].label, *((int*)m->entries[i].value));
            break;
        case Boolean:
            sprintf(format, "%%-%d.%ds\u2502%*s[%c]", pad - 1, pad - 1, m->w - pad - 4, "",\
            *((int*)m->entries[i].value) ? '*' : ' ');
            printf(format, m->entries[i].label);
            break;
        case Float:
            break;
        case ProcessStage:
            if(!(*m->entries[i].count)) break;
            printf("\033[0;30;0m");
            draw_line(m->x, m->y + cl - 1, m->w, 0, 1);
            cursorto(m->x + pad, m->y + cl - 1);
            printf("\u2534");
            m->nelements = m->nentries + *m->entries[i].count;
            // TODO: this reallocates memory for the stages every redraw, really slow!!
            struct Stage* new = realloc(((struct Stage*)m->entries[i].value), *m->entries[i].count);
            if(new == NULL)
                break;
            m->entries[i].value = new;
            for(int j = 0; j < *m->entries[i].count; j++) {
                if(m->selected == i + j)
                    printf("\033[0;30;41m");
                cursorto(m->x + 1, m->y + cl++);
                    printf("[%c] %d",
                        (*((struct Stage*)m->entries[i].value)).type == Io ? '*' : ' ',
                        (*((struct Stage*)m->entries[i].value)).t_length);
                if(m->selected == i + j)
                    printf("\033[0;30;0m");
            }
            cl++;
            printf("\033[0;30;0m");
            draw_line(m->x, m->y + cl - 1, m->w, 0, 1);
            //cursorto(m->x + pad, m->y + cl - 1);
            //printf("\u2534");
            i += *m->entries[i].count;
            break;
        }
        if(m->selected == i)
            printf("\033[0;30;0m");
    }

    close:
    clear(0, term_h - 2, 1, term_w);
    cursorto(1, term_h);
    CMDDEF("C-C", "exit");
    CMDDEF("Tab", "next");
}

void menu_input(Menu* m){
    char key;
    while(1) {
        menu_draw(m);
        switch(key = getchar()) {

        case CTRLMASK(KEY_UP):
            goto next;
        case CTRLMASK(KEY_DOWN):
            goto prev;
        case CTRLMASK('c'):
            clear(m->x, m->y, m->h, m->w);
            return;

        case '\033':
            getchar();
            switch(getchar()) {
            case 'A':
                prev:
                if(m->selected == 0) {
                    m->selected = m->nelements - 1;
                    m->scrolled = m->nelements + 2 - m->h;
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
            if(m->selected == m->nelements - 2) {
                m->selected = 0;
                m->scrolled = 0;
            } else {
                m->selected++;
                if(m->selected > m->h - 4 && m->scrolled < m->h) {
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
                switch(key) {
                case KEY_UP:
                    (*((int*)m->entries[m->selected].value))--;
                    break;
                case KEY_DOWN:
                    (*((int*)m->entries[m->selected].value))++;
                    break;
                default:
                    if(key < '0' || key > '9') break;
                    (*((int*)m->entries[m->selected].value)) =\
                    (*((int*)m->entries[m->selected].value)) * 10 + key - 0x30;
                    break;
                }
            case Boolean:
                if(key != ' ') break;
                (*((int*)m->entries[m->selected].value)) =\
                !(*((int*)m->entries[m->selected].value));
                break;
            case Float:
                break;
            case ProcessStage:
                //(*((struct Stage*)(m->entries + m->selected))).t_length = 10;
                endwin();
                exit(0);
                //(*(((*(struct Stage*)(m->entries + m->selected).value))).type);
                break;
            }
            break;
        }
    }
}
