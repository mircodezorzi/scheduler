#ifndef MEMORY_H
#define MEMORY_H

struct DynamicMemory {
    struct Block {
        int allocated;
        int size;
        int address;
        char name[128];
        struct Block *next;
    } *block;
    int size;
    enum { Fifo } strategy;
};

struct DynamicMemory *dynamicmemory_new();
void dynamicmemory_destroy(struct DynamicMemory *m);
void dynamicmemory_malloc(struct DynamicMemory *m, int size, char* name);
void dynamicmemory_free(struct DynamicMemory *m, int addr);
void dynamicmemory_compact(struct DynamicMemory *m);
#endif
