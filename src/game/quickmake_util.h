#ifndef QUICKMAKE_UTIL_H
#define QUICKMAKE_UTIL_H

void *allocateMemorySize (memory_arena *memory, unsigned int size) {
    assert(memory->size + size < memory->capacity);
    void *result = (char *)memory->base + memory->size;
    memory->size += size;
    return result;
}

void readLine (char *data, char **start, char **end) {
    *start = data;
    int index = 0;
    while (data[index] != 0 && data[index] != '\n' && data[index] != '\r') {
        ++index;
    }
    *end = data + index;
}

int stringToInt (char *start, char *end) {
    int result = 0;
    bool negative = false;
    if (*start == '-') {
        negative = true;
        ++start;
    }
    while (start <= end) {
        char digit = *start;
        result *= 10;
        result += (int)(digit - '0');
        ++start;
    }
    if (negative) {
        result = -result;
    }
    return result;
}

// TODO(ebuchholz): just use atof or something
// TODO(ebuchholz): make sure the string is a valid float
float stringToFloat (char *start, char *end) {
    float result = 0.0;
    float integerPart = 0.0;
    float fractionalPart = 0.0;
    bool negative = false;
    if (*start == '-') {
        negative = true;
        ++start;
    }
    while (start <= end) {
        char digit = *start;
        if (*start == '.') {
            break;
        }
        integerPart *= 10.0f;
        float number = (float)((int)(digit - '0'));
        integerPart += number;
        ++start;
    }
    if (negative) {
        integerPart = -integerPart;
    }
    while (end > start) {
        char digit = *end;
        fractionalPart /= 10.0f;
        float number = (float)((int)(digit - '0'));
        fractionalPart += number;
        --end;
    }
    fractionalPart /= 10.0f;
    if (negative) {
        fractionalPart = -fractionalPart;
    }
    return integerPart + fractionalPart;
}

//float stringToFloat (char *string) {
//    char *end = string;
//    while (*end != 0) {
//        ++end;
//    }
//    --end;
//    return stringToFloat(string, end);
//}

// TODO(ebuchholz): move somewhere else
bool stringsAreEqual (int length, char *a, char *b) {
    char *bIndex = b;
    for (int i = 0; i < length; ++i, ++bIndex) {
        if (*bIndex == 0 || a[i] != *bIndex) {
            return false;
        }
    }
    return (*bIndex == 0);
}

unsigned int findLeastSignificantBit (unsigned int value, bool *found) {
    *found = false;
    for (unsigned int digit = 0; digit < 32; ++digit) {
        if (value & (1 << digit)) {
            *found = true;
            return digit;
        }
    }
    return 0;
}


#endif
