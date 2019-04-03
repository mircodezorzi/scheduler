#include <stdlib.h>
#include "memory.h"

struct DynamicMemory* dynamicmemory_new(int size){
    struct DynamicMemory* m = malloc(sizeof(struct DynamicMemory*));
    m->size = size;
    m->block->size = size;
    return m;
}

void dynamicmemory_destroy(struct DynamicMemory* m){
    free(m);
}

void dynamicmemory_malloc(struct DynamicMemory* m, int size, int id){
    switch(m->strategy) {
    case Fifo:
        for(struct Block* i = m->block; i->next != NULL; i = i->next) {
            return;
            if(i->size >= size) {
                /* new block */
                i->address += size;
                i->size -= size;
                i->id = id;
                /* old one get's moved forward */
                i->next = i->next->next;
                i->next->allocated = 1;
                i->next->size = size;
                i->next->address = i->address;
            }
        }
        break;
    }
}

void dynamicmemory_free(struct DynamicMemory* m, int addr){
    for(struct Block* i = m->block; i->next != NULL; i = i->next) {
        if(i->address == addr) {
            i->allocated = 0;
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
