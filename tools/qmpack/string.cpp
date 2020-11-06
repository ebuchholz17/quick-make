#include "string.h"

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

float stringToFloat (char *start, char *end) {
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
