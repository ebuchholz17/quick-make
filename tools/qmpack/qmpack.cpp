#include <stdio.h>
#include <stdlib.h>

#define assert(exp) if (!(exp)) { *(int *)0 = 0; }
#define MAX_NUM_ASSETS 200

#include "memory.cpp"
#include "string.cpp"

typedef char *char_ptr;
#define LIST_TYPE char_ptr
#include "list.h"

#include "windows_platform.cpp"
#include "qmpack.h"

#define LIST_TYPE qmpack_staging_data
#include "list.h"


memory_arena initMemoryArena (unsigned int capacity) {
    memory_arena memoryArena = {};
    memoryArena.capacity = capacity;
    memoryArena.base = malloc(capacity);
    return memoryArena;
}

void freeMemoryArena (memory_arena *memory) {
    free(memory->base);
}

void assignFileNameFromFullPath (char *path, qmpack_file_header *fileHeader) {
    // assume the end of the file has an extension that starts with a "."
    char *fileStart, *fileEnd;
    char *cursor = path;
    while (*cursor != 0) {
        ++cursor;
    }
    --cursor;
    fileEnd = cursor;
    while (cursor != path && *cursor != '/' && *cursor != '\\') {
        if (*cursor == '.') {
            fileEnd = cursor - 1;
        }
        --cursor;
        fileStart = cursor;
    }
    // skip the '/' at the beginning of the file path
    if (cursor != path) {
        ++cursor;
    }

    int fileLength = 0;
    while (cursor != fileEnd) {
        fileHeader->name[fileLength++] = *cursor;
        ++cursor;
    }
    fileHeader->name[fileLength++] = *cursor;
    fileHeader->name[fileLength] = 0;
}

void addFilesToStagingDataList (qmpack_staging_data_list *stagingDataList, char_ptr_list *files, 
                                asset_type assetType, data_type dataType) {
    for (int i = 0; i < files->numValues; ++i) {
        qmpack_staging_data data = {};
        data.filePath = files->values[i];
        assignFileNameFromFullPath(data.filePath, &data.fileHeader);
        data.fileHeader.type = assetType;
        data.dataType = dataType;
        listPush(stagingDataList, data);
    }
}

int main (int argc, char **argv) {
    if (argc != 3) {
        printf("wrong arguments");
        return 1;
    }

    char *assetsDirPath = argv[1];
    char *outputPath = argv[2];

    memory_arena memory = initMemoryArena(1024 * 1024);
    memory_arena tempMemory = initMemoryArena(1024 * 1024);

    qmpack_staging_data_list files = qmpack_staging_dataListInit(&memory, MAX_NUM_ASSETS);

    // data
    char_ptr_list dataFilePaths = getFilePathsInDirOfType(appendString(assetsDirPath, "/data/", &tempMemory), "*.txt", &memory, &tempMemory);
    addFilesToStagingDataList(&files, &dataFilePaths, ASSET_TYPE_DATA, DATA_TYPE_TEXT);

    // meshes
    char_ptr_list objFilePaths = getFilePathsInDirOfType(appendString(assetsDirPath, "/meshes/", &tempMemory), "*.obj", &memory, &tempMemory);
    addFilesToStagingDataList(&files, &objFilePaths, ASSET_TYPE_OBJ, DATA_TYPE_TEXT);

    // midi
    char_ptr_list midiFilePaths = getFilePathsInDirOfType(appendString(assetsDirPath, "/midi/", &tempMemory), "*.midi", &memory, &tempMemory);
    addFilesToStagingDataList(&files, &midiFilePaths, ASSET_TYPE_MIDI, DATA_TYPE_BINARY);
    char_ptr_list midFilePaths = getFilePathsInDirOfType(appendString(assetsDirPath, "/midi/", &tempMemory), "*.mid", &memory, &tempMemory);
    addFilesToStagingDataList(&files, &midFilePaths, ASSET_TYPE_MIDI, DATA_TYPE_BINARY);

    // sounds
    char_ptr_list wavFilePaths = getFilePathsInDirOfType(appendString(assetsDirPath, "/sounds/", &tempMemory), "*.wav", &memory, &tempMemory);
    addFilesToStagingDataList(&files, &wavFilePaths, ASSET_TYPE_WAV, DATA_TYPE_BINARY);

    // textures
    char_ptr_list bmpFilePaths = getFilePathsInDirOfType(appendString(assetsDirPath, "/textures/", &tempMemory), "*.bmp", &memory, &tempMemory);
    addFilesToStagingDataList(&files, &bmpFilePaths, ASSET_TYPE_BMP, DATA_TYPE_BINARY);

    // atlas
    char_ptr_list atlasTextureFilePaths = getFilePathsInDirOfType(appendString(assetsDirPath, "/atlas/", &tempMemory), "*.bmp", &memory, &tempMemory);
    addFilesToStagingDataList(&files, &atlasTextureFilePaths, ASSET_TYPE_ATLAS_TEXTURE, DATA_TYPE_BINARY);
    char_ptr_list atlasDataFilePaths = getFilePathsInDirOfType(appendString(assetsDirPath, "/atlas/", &tempMemory), "*.txt", &memory, &tempMemory);
    addFilesToStagingDataList(&files, &atlasDataFilePaths, ASSET_TYPE_ATLAS_DATA, DATA_TYPE_TEXT);

    tempMemory.size = 0;

    FILE *output;
    fopen_s(&output, outputPath, "wb");
    assert(output);

    fwrite(&files.numValues, 1, sizeof(int), output);

    for (int i = 0; i < files.numValues; ++i) {
        qmpack_staging_data *stagingData = &files.values[i];

        FILE *assetFile;
        fopen_s(&assetFile, stagingData->filePath, "rb");
        assert(assetFile);

        fseek(assetFile, 0, SEEK_END);
        stagingData->fileHeader.size = ftell(assetFile);
        fseek(assetFile, 0, SEEK_SET);

        if (stagingData->dataType == DATA_TYPE_TEXT) {
            stagingData->fileHeader.size++;
        }

        fwrite(&stagingData->fileHeader, 1, sizeof(qmpack_file_header), output);

        char buffer[4096];
        size_t count;
        while ((count = fread(buffer, 1, sizeof(buffer), assetFile)) > 0) {
            fwrite(buffer, 1, count, output);
        }
        if (stagingData->dataType == DATA_TYPE_TEXT) {
            fwrite("\0", 1, 1, output);
        }

        fclose(assetFile);
    }
    fclose(output);

    return 0;
}
