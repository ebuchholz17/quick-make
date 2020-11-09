#ifndef QUICKMAKE_UTIL_H
#define QUICKMAKE_UTIL_H

// TODO(ebuchholz): consider building in automatic alignment
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

void readLine (char *data, char **start, char **end) {
    *start = data;
    int index = 0;
    while (data[index] != 0 && data[index] != '\n' && data[index] != '\r') {
        ++index;
    }
    *end = data + index;
}

void readLine (char *data, char **start, char **end, char **nextLineStart) {
    *start = data;
    int index = 0;
    while (data[index] != 0 && data[index] != '\n' && data[index] != '\r') {
        ++index;
    }
    *end = data + index;

    if (**end == 0) {
        *nextLineStart = 0;
    }
    else {
        if (**end == '\r') {
            *nextLineStart = *end + 2;
        }
        else {
            *nextLineStart = *end + 1;
        }
    }
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

//copied from webassembly prototype
char *numToString (int num, memory_arena *memory) {
    const int maxStringLength = 20;
    char numberBuffer[maxStringLength] = {};

    char *currentDigit = numberBuffer + (maxStringLength - 1);
    char *stringEnd = currentDigit;

    bool isNegative = false;
    if (num < 0) {
        isNegative = true;
        num = -num;
    }

    do {
        --currentDigit;
        *currentDigit = '0' + num % 10;
        num /= 10;
    } while (num != 0);

    if (isNegative) {
        --currentDigit;
        *currentDigit = '-';
    }

    unsigned int stringLength = (unsigned int)(stringEnd - currentDigit);
    char *string = (char *)allocateMemorySize(memory, stringLength + 1);

    for (unsigned int i = 0; i < stringLength; ++i) {
        string[i] = *currentDigit;
        ++currentDigit;
    }
    string[stringLength] = 0;

    return string;
}

unsigned int bigEndianToLittleEndian (unsigned int num){
    unsigned int low = ((num >> 24) & 0xff);
    unsigned int mid0 = ((num >> 16) & 0xff);
    unsigned int mid1 = ((num >> 8) & 0xff);
    unsigned int high = (num & 0xff);

    return low |
           (mid0 << 8) | 
           (mid1 << 16) | 
           (high << 24);
}

unsigned short bigEndianToLittleEndian (unsigned short num){
    return ((num >> 8) & 0xff) | 
           ((num & 0xff) << 8);
}

char *writeNumToBuffer (int num, char *buffer) {
    const int maxStringLength = 20;
    char numberBuffer[maxStringLength] = {};

    char *currentDigit = numberBuffer + (maxStringLength - 1);
    char *stringEnd = currentDigit;

    bool isNegative = false;
    if (num < 0) {
        isNegative = true;
        num = -num;
    }

    do {
        --currentDigit;
        *currentDigit = '0' + num % 10;
        num /= 10;
    } while (num != 0);

    if (isNegative) {
        --currentDigit;
        *currentDigit = '-';
    }
    unsigned int stringLength = (unsigned int)(stringEnd - currentDigit);
    char *string = buffer;

    for (unsigned int i = 0; i < stringLength; ++i) {
        string[i] = *currentDigit;
        ++currentDigit;
    }
    //string[stringLength] = 0;

    return string + stringLength;
    //return string + stringLength + 1;
}

char *writeStringToBuffer (char *string, char *buffer) {
    char *letter = string;
    char *cursor = buffer;
    while (*letter != 0) {
        *cursor = *letter;
        ++cursor;
        ++letter;
    }

    return cursor;
}

char *allocateString (char *string, memory_arena *memory) {
    int length = 0;
    char *letter = string;
    while(*letter != 0) {
        ++letter;
        ++length;
    }
    char *newString = (char *)allocateMemorySize(memory, sizeof(char) * (length+1));

    for (int i = 0; i < length; ++i) {
        newString[i] = string[i];
    }
    newString[length] = 0;
    return newString;
}

char *appendString (char *first, char *second, memory_arena *memory) {
    char *a = first;
    char *b = second;

    unsigned int firstLength = 0;
    unsigned int secondLength = 0;

    while (*a != 0) {
        ++a;
        ++firstLength;
    }
    while (*b != 0) {
        ++b;
        ++secondLength;
    }

    char *result = (char *)allocateMemorySize(memory, firstLength + secondLength + 1);
    char *currentLetter = result;
    for (unsigned int i = 0; i < firstLength; ++i) {
        *currentLetter = first[i];
        ++currentLetter;
    }
    for (unsigned int i = 0; i < secondLength; ++i) {
        *currentLetter = second[i];
        ++currentLetter;
    }
    *currentLetter = 0;

    return result;
}

//void numToString (float num, char *buffer, int maxLength = -1) {
//
//}

//float stringToFloat (char *string) {
//    char *end = string;
//    while (*end != 0) {
//        ++end;
//    }
//    --end;
//    return stringToFloat(string, end);
//}

bool stringsAreEqual (int length, char *a, char *b) {
    char *bIndex = b;
    for (int i = 0; i < length; ++i, ++bIndex) {
        if (*bIndex == 0 || a[i] != *bIndex) {
            return false;
        }
    }
    return (*bIndex == 0);
}

bool stringsAreEqual (char *a, char *b) {
    int i, j;
    for (i = 0, j = 0; a[i] && b[j]; ++i, ++j) {
        if (a[i] != b[j]) { 
            return false;
        }
    }
    return a[i] == 0 && b[j] == 0;
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

static unsigned int rngSeed;

inline void setRNGSeed (unsigned int seed) {
    rngSeed = seed;
}

#define MAX_RAND_NUMBER (((1U << 31)-1) >> 16)

inline unsigned int randomUint () {
    rngSeed = (rngSeed * 1103515245U + 12345U) % (1U << 31);
    return (rngSeed & ((1U << 31)-1)) >> 16;
}

inline float randomFloat () {
    unsigned int randomNum = randomUint();
    return (float)randomNum / (float)(MAX_RAND_NUMBER);
}


#endif
