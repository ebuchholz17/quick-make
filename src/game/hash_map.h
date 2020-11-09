#ifndef HASH_MAP_H 
#define HASH_MAP_H

#define HASH_MAP_TYPE_STRUCT(x) HASH_MAP_TYPE_STRUCT_(x)
#define HASH_MAP_TYPE_STRUCT_(x) x##_hash_map
#define HASH_MAP_ENTRY_STRUCT(x) HASH_MAP_ENTRY_STRUCT_(x)
#define HASH_MAP_ENTRY_STRUCT_(x) x##_hash_map_value
#define HASH_MAP_INIT(x) HASH_MAP_INIT_(x)
#define HASH_MAP_INIT_(x) x##HashMapInit

unsigned int hashMapHashString (char *string) {
    unsigned int hash = 5381;
    int c;
    assert(string);
    do {
        c = *string++;
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
    } while (c);

    return hash;
}

#endif

struct HASH_MAP_ENTRY_STRUCT(HASH_MAP_TYPE) {
    // NOTE(ebuchholz): hash map not responsible for managing string
    char *key;
    HASH_MAP_TYPE value;
};

struct HASH_MAP_TYPE_STRUCT(HASH_MAP_TYPE) {
    HASH_MAP_ENTRY_STRUCT(HASH_MAP_TYPE) *entries;
    unsigned int capacity;
};

void storeHashMapValue (HASH_MAP_TYPE_STRUCT(HASH_MAP_TYPE) *hashMap, HASH_MAP_TYPE value, char *key) {
    unsigned int hash = hashMapHashString(key);
    unsigned int mapIndex = hash % hashMap->capacity;
    unsigned int originalMapIndex = mapIndex; // check that we actually find it after looping all the way around
    HASH_MAP_ENTRY_STRUCT(HASH_MAP_TYPE) *existingEntry = &hashMap->entries[mapIndex];
    while (existingEntry->key != 0) {
        mapIndex = (mapIndex + 1) % hashMap->capacity;
        existingEntry = &hashMap->entries[mapIndex];
        assert(mapIndex != originalMapIndex);
    }
    HASH_MAP_ENTRY_STRUCT(HASH_MAP_TYPE) entry = {};
    entry.key = key;
    entry.value = value;
    hashMap->entries[mapIndex] = entry;
}

HASH_MAP_TYPE *getHashMapValuePointer (HASH_MAP_TYPE_STRUCT(HASH_MAP_TYPE) *hashMap, char *key) {
    HASH_MAP_TYPE *result;

    unsigned int hash = hashMapHashString(key);
    unsigned int mapIndex = hash % hashMap->capacity;
    unsigned int originalMapIndex = mapIndex; // check that we actually find it after looping all the way around

    while (true) {
        HASH_MAP_ENTRY_STRUCT(HASH_MAP_TYPE) *entry = &hashMap->entries[mapIndex];
        if (stringsAreEqual(key, entry->key)) {
            result = &entry->value;
            break;
        }
        else {
            mapIndex = (mapIndex + 1) % hashMap->capacity;
            assert(mapIndex != originalMapIndex);
        }
    }

    return result;
}

HASH_MAP_TYPE getHashMapValue (HASH_MAP_TYPE_STRUCT(HASH_MAP_TYPE) *hashMap, char *key) {
    HASH_MAP_TYPE *result = getHashMapValuePointer(hashMap, key);
    return *result;
}

HASH_MAP_TYPE_STRUCT(HASH_MAP_TYPE) HASH_MAP_INIT(HASH_MAP_TYPE) (memory_arena *memory, int capacity) {
    HASH_MAP_TYPE_STRUCT(HASH_MAP_TYPE) result = {};
    result.capacity = capacity;
    result.entries = (HASH_MAP_ENTRY_STRUCT(HASH_MAP_TYPE) *)allocateMemorySize(memory, capacity * sizeof(HASH_MAP_ENTRY_STRUCT(HASH_MAP_TYPE)));
    for (int i = 0; i < capacity; ++i) {
        result.entries[i] = {};
    }
    return result;
}

#undef HASH_MAP_TYPE
