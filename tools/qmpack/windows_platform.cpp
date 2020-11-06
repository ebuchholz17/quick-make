#include "windows_platform.h"

char_ptr_list getFilePathsInDirOfType (char *dirPath, char *type, memory_arena *memory, memory_arena *tempMemory) {
    char_ptr_list filePaths = char_ptrListInit(memory, MAX_NUM_ASSETS * sizeof(char *));

    char *searchPattern = appendString(dirPath, type, tempMemory);
    WIN32_FIND_DATAA fileFindData = {};
    HANDLE searchHandle = FindFirstFileA(searchPattern, &fileFindData);

    if (searchHandle != INVALID_HANDLE_VALUE) {
        listPush(&filePaths, appendString(dirPath, fileFindData.cFileName, memory));

        while (FindNextFileA(searchHandle, &fileFindData)) {
            listPush(&filePaths, appendString(dirPath, fileFindData.cFileName, memory));
        }
    }
    
    return filePaths;
}
