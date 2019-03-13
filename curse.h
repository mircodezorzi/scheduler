#ifndef CURSE_H
#define CURSE_H

#include "process.h"

#define CTRLMASK(k) ((k) & 0x1f)
#define ENTRY(l, t, v)\
    { .label = l, .type = t, .value = ((void*)(v))}

typedef struct {
    char *label;
    enum { String, Integer, Float, Boolean } type;
    int length;
    void *value;
} Entry;

typedef struct {
    Entry *entries;
    int nentries;
    int selected;
    int scrolled;
    int x, y;
    int w, h;
} Menu;

void draw_border(int x, int y, int w, int h);
void draw_process_graph(Process *p, int n, int x, int y, int w, int h);

Menu menu_new(Entry *entries, int nentries, int x, int y, int w, int h);
void menu_print(int w, char* str, int len);
void menu_draw(Menu m);
void menu_input(Menu *m);

#endif
