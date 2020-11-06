#ifndef LIST_H
#define LIST_H

#define LIST_TYPE_STRUCT(x) LIST_TYPE_STRUCT_(x)
#define LIST_TYPE_STRUCT_(x) x##_list
#define LIST_INIT(x) LIST_INIT_(x)
#define LIST_INIT_(x) x##ListInit

#endif

struct LIST_TYPE_STRUCT(LIST_TYPE) {
    LIST_TYPE *values;
    int numValues;
    int capacity;
};

void listPush (LIST_TYPE_STRUCT(LIST_TYPE) *list, LIST_TYPE value) {
    assert(list->numValues < list->capacity);

    list->values[list->numValues] = value;
    list->numValues++;
}

LIST_TYPE listSplice(LIST_TYPE_STRUCT(LIST_TYPE) *list, int index) {
    assert(index < list->numValues);

    LIST_TYPE result = list->values[index];
    list->numValues--;
    for (int i = index; i < list->numValues; ++i) {
        list->values[i] = list->values[i+1];
    }

    return result;
}

LIST_TYPE_STRUCT(LIST_TYPE) *listCopy (LIST_TYPE_STRUCT(LIST_TYPE) *source, memory_arena *memory) {
    LIST_TYPE_STRUCT(LIST_TYPE) *result = (LIST_TYPE_STRUCT(LIST_TYPE) *)allocateMemorySize(memory, sizeof(LIST_TYPE_STRUCT(LIST_TYPE)) + source->capacity * sizeof(LIST_TYPE));
    result->values = (LIST_TYPE *)((char *)result + sizeof(LIST_TYPE_STRUCT(LIST_TYPE))); 
    result->numValues = source->numValues;
    result->capacity = source->capacity;
    for (int i = 0; i < source->numValues; ++i) {
        result->values[i] = source->values[i];
    }
    return result;
}

LIST_TYPE_STRUCT(LIST_TYPE) LIST_INIT(LIST_TYPE) (memory_arena *memory, int capacity) {
    LIST_TYPE_STRUCT(LIST_TYPE) result = {};
    result.capacity = capacity;
    result.values = (LIST_TYPE *)allocateMemorySize(memory, capacity * sizeof(LIST_TYPE));
    return result;
}

#undef LIST_TYPE
