#include "memory.h"

void *allocateMemorySize (memory_arena *memory, unsigned int size) {
    assert(memory->size + size < memory->capacity);
    void *result = (char *)memory->base + memory->size;
    memory->size += size;
    return result;
}

void freeMemorySize (memory_arena *memory, unsigned int size) {
    assert(memory->size - size >= 0);
    memory->size -= size;
}

//temp_memory startTempMemory (memory_arena *memory) {
//    temp_memory result = {};
//    result.memory = memory;
//    result.size = 0;
//    result.startingSize = memory->size;
//    return result;
//}
//
//void *allocTempMemory (temp_memory *tempMemory, unsigned int size) {
//
//}
//
//void endTempMemory (temp_memory *tempMemory) {
//
//
//}

