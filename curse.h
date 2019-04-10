#ifndef CURSE_H
#define CURSE_H

#include "process.h"
#include "memory.h"

#define CTRLMASK(k) ((k) & 0x1f)
#define NEWENTRY(l, t, v)\
    { .label = l, .type = t, .value = ((void*)(v))}

#define __DVORAK__

#if defined(__DVORAK__)
    #define KEY_UP 'h'
    #define KEY_DOWN 't'
#else
    #define KEY_UP 'k'
    #define KEY_DOWN 'j'
#endif

typedef struct {
    char* label;
    enum { String, Integer, Float, Boolean, ProcessStage } type;
    int length;
    void* value;
    int* count;
} Entry;

typedef struct {
    Entry* entries;
    int nentries;
    int nelements;
    int selected;
    int scrolled;
    char* title;
    int x, y;
    int w, h;
} Menu;

void clear(int x, int y, int h, int w);
void draw_border(int x, int y, int w, int h);
void draw_line(int x, int y, int n, int d, int e);
void draw_block(int x, int y, int w, int h, int c, int s);
void draw_process_graph(Process *p, int n, int x, int y, int w, int h, int bw);
void draw_process_graph_info(Process *p, int n, int x, int y, int w, int h);
void draw_dynamicmemory(struct DynamicMemory* m, int x, int y, int w, int h);
void draw_process(Process p, int x, int y, int w, int h);
Menu menu_new(Entry* entries, int nentries, char* title, int x, int y, int w, int h);
void menu_print(int w, char* str, int len);
void menu_draw(Menu* m);
void menu_input(Menu* m);

#endif
