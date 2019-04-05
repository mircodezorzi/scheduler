#include <stdlib.h>
#include "memory.h"

struct DynamicMemory* dynamicmemory_new(int size){
    struct DynamicMemory* m = malloc(sizeof(struct DynamicMemory*));
    m->strategy = Fifo;
    m->size = size;

    m->block = malloc(sizeof(struct Block*));
    m->block->size = size;
    m->block->next = NULL;
    m->block->allocated = 0;
    m->block->address = 0;

    return m;
}

void dynamicmemory_destroy(struct DynamicMemory* m){
    free(m);
}

void dynamicmemory_malloc(struct DynamicMemory* m, int size, int id){
    switch(m->strategy) {
    case Fifo:
        for(struct Block* i = m->block; i != NULL; i = i->next) {
            if(i->size >= size && !i->allocated) {
                struct Block *new = malloc(sizeof(struct Block));
                new->next = i->next;
                new->size = i->size - size;
                new->address += size;
                new->allocated = 0;
                new->id = i->id;
                i->next = new;
                i->size = size;
                i->id = id;
                i->allocated = 1;
                return;
            }
        }
        break;
    }
}

void dynamicmemory_free(struct DynamicMemory* m, int addr){
    for(struct Block* i = m->block; i != NULL; i = i->next) {
        if(i->address == addr) {
            i->allocated = 0;
            return;
        }
    }
}

void dynamicmemory_compact(struct DynamicMemory* m){
    for(struct Block* i = m->block; i->next != NULL; i = i->next) {
        if(!i->allocated && !i->next->allocated) {
            i->size += i->next->size;
            i->next->next = i->next;
        }
    }
}
