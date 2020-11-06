#ifndef MEMORY_H
#define MEMORY_H

struct memory_arena {
    void *base;
    unsigned int size;
    unsigned int capacity;
};

//struct temp_memory {
//    memory_arena *memory;
//    unsigned int size;
//    unsigned int startingSize;
//};

#endif
