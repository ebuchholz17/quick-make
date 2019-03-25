#include "quickmake_assets.h"

void parseOBJ (void *objData, game_assets *assets, int key, memory_arena *workingMemory) {
    int numMeshes = assets->numMeshes;
    assert(numMeshes < MAX_NUM_MESHES);

    mesh_asset *meshAsset = (mesh_asset *)allocateMemorySize(&assets->assetMemory, sizeof(mesh_asset));
    assets->meshes[key] = meshAsset;
    assets->numMeshes++;
    meshAsset->key = (mesh_key)key;

    loaded_mesh_asset *loadedMesh = (loaded_mesh_asset *)allocateMemorySize(workingMemory, sizeof(loaded_mesh_asset));
    loadedMesh->key = key;

    char *start, *end;
    start = (char *)objData;

    unsigned int numPositions = 0;
    unsigned int numNormals = 0;
    unsigned int numTexCoords = 0;
    unsigned int numTriangles = 0;

    while (true) {
        readLine(start, &start, &end);

        char *wordStart = start;
        char *wordEnd = wordStart;
        while (*wordEnd != ' ' && wordEnd != end) { ++wordEnd; }
        int length = (int)(wordEnd - wordStart);

        if (stringsAreEqual(length, wordStart, "v")) {
            ++numPositions;
        }
        else if (stringsAreEqual(length, wordStart, "vt")) {
            ++numTexCoords;
        }
        else if (stringsAreEqual(length, wordStart, "vn")) {
            ++numNormals;
        }
        else if (stringsAreEqual(length, wordStart, "f")) {
            ++numTriangles;
        }

        start = end;
        if (*start == 0) {
            break;
        }
        else {
            if (*start == '\r') {
                start += 2;
            }
            else {
                start++;
            }
        }
    }

    float (*tempPositions)[3] = (float (*)[3])allocateMemorySize(workingMemory, sizeof(float) * 3 * numPositions);
    float (*tempTexCoords)[2] = (float (*)[2])allocateMemorySize(workingMemory, sizeof(float) * 2 * numTexCoords);
    float (*tempNormals)[3] = (float (*)[3])allocateMemorySize(workingMemory, sizeof(float) * 3 * numNormals);

    loadedMesh->positions.count = 3 * 3 * numTriangles;
    loadedMesh->texCoords.count = 2 * 3 * numTriangles;
    loadedMesh->normals.count = 3 * 3 * numTriangles;
    loadedMesh->indices.count =  3 * numTriangles;

    loadedMesh->positions.values = allocateMemorySize(workingMemory, sizeof(float) * loadedMesh->positions.count);
    loadedMesh->texCoords.values = allocateMemorySize(workingMemory, sizeof(float) * loadedMesh->texCoords.count);
    loadedMesh->normals.values = allocateMemorySize(workingMemory, sizeof(float) * loadedMesh->normals.count);
    loadedMesh->indices.values = allocateMemorySize(workingMemory, sizeof(int) * loadedMesh->indices.count);

    start = (char *)objData;
    int tempPositionIndex = 0;
    int tempTexCoordIndex = 0;
    int tempNormalIndex = 0;

    int positionIndex = 0;
    int uvIndex = 0;
    int normalIndex = 0;
    int indexIndex = 0;
    while (true) {
        readLine(start, &start, &end);

        char *wordStart = start;
        char *wordEnd = wordStart;
        while (*wordEnd != ' ' && wordEnd != end) { ++wordEnd; }
        int length = (int)(wordEnd - wordStart);

        if (stringsAreEqual(length, wordStart, "v")) {
            float (*tempPosition)[3] = &tempPositions[tempPositionIndex];
            wordEnd++;
            wordStart = wordEnd;
            while (*wordEnd != ' ' && wordEnd != end) { ++wordEnd; }
            (*tempPosition)[0] = stringToFloat(wordStart, wordEnd - 1);

            wordEnd++;
            wordStart = wordEnd;
            while (*wordEnd != ' ' && wordEnd != end) { ++wordEnd; }
            (*tempPosition)[1] = stringToFloat(wordStart, wordEnd - 1);

            wordEnd++;
            wordStart = wordEnd;
            while (*wordEnd != ' ' && wordEnd != end) { ++wordEnd; }
            (*tempPosition)[2] = stringToFloat(wordStart, wordEnd - 1);

            tempPositionIndex++;
        }
        else if (stringsAreEqual(length, wordStart, "vt")) {
            float (*tempTexCoord)[2] = &tempTexCoords[tempTexCoordIndex];
            wordEnd++;
            wordStart = wordEnd;
            while (*wordEnd != ' ' && wordEnd != end) { ++wordEnd; }
            (*tempTexCoord)[0] = stringToFloat(wordStart, wordEnd - 1);

            wordEnd++;
            wordStart = wordEnd;
            while (*wordEnd != ' ' && wordEnd != end) { ++wordEnd; }
            (*tempTexCoord)[1] = stringToFloat(wordStart, wordEnd - 1);

            tempTexCoordIndex++;
        }
        else if (stringsAreEqual(length, wordStart, "vn")) {
            float (*tempNormal)[3] = &tempNormals[tempNormalIndex];
            wordEnd++;
            wordStart = wordEnd;
            while (*wordEnd != ' ' && wordEnd != end) { ++wordEnd; }
            (*tempNormal)[0] = stringToFloat(wordStart, wordEnd - 1);

            wordEnd++;
            wordStart = wordEnd;
            while (*wordEnd != ' ' && wordEnd != end) { ++wordEnd; }
            (*tempNormal)[1] = stringToFloat(wordStart, wordEnd - 1);

            wordEnd++;
            wordStart = wordEnd;
            while (*wordEnd != ' ' && wordEnd != end) { ++wordEnd; }
            (*tempNormal)[2] = stringToFloat(wordStart, wordEnd - 1);

            tempNormalIndex++;
        }
        else if (stringsAreEqual(length, wordStart, "f")) {
            // TODO(ebuchholz): handle the case where text coords and/or normals are missing
            for (int i = 0; i < 3; ++i) {
                wordEnd++;
                wordStart = wordEnd;
                while (*wordEnd != ' ' && wordEnd != end) { ++wordEnd; }

                char *indexStart = wordStart;
                char *indexEnd = indexStart;
                while (*indexEnd != ' ' && indexEnd != end && *indexEnd != '/') { ++indexEnd; }
                if (indexStart != indexEnd) {
                    int index = stringToInt(indexStart, indexEnd - 1) - 1;
                    float *positionsPointer = (float *)loadedMesh->positions.values + (positionIndex * 3);
                    positionsPointer[0] = tempPositions[index][0];
                    positionsPointer[1] = tempPositions[index][1];
                    positionsPointer[2] = tempPositions[index][2];
                    ++positionIndex;
                }
                indexEnd++;
                indexStart = indexEnd;

                while (*indexEnd != ' ' && indexEnd != end && *indexEnd != '/') { ++indexEnd; }
                if (indexStart != indexEnd) {
                    int index = stringToInt(indexStart, indexEnd - 1) - 1;
                    float *texCoordsPointer = (float *)loadedMesh->texCoords.values + (uvIndex * 2);
                    texCoordsPointer[0] = tempTexCoords[index][0];
                    texCoordsPointer[1] = tempTexCoords[index][1];
                    uvIndex++;
                }
                indexEnd++;
                indexStart = indexEnd;

                while (*indexEnd != ' ' && indexEnd != end && *indexEnd != '/') { ++indexEnd; }
                if (indexStart != indexEnd) {
                    int index = stringToInt(indexStart, indexEnd - 1) - 1;
                    float *normalsPointer = (float *)loadedMesh->normals.values + (normalIndex * 3);
                    normalsPointer[0] = tempNormals[index][0];
                    normalsPointer[1] = tempNormals[index][1];
                    normalsPointer[2] = tempNormals[index][2];
                    ++normalIndex;
                }

                ((int *)(loadedMesh->indices.values))[indexIndex] = indexIndex;
                ++indexIndex;
            }
        }

        start = end;
        if (*start == 0) {
            break;
        }
        else {
            if (*start == '\r') {
                start += 2;
            }
            else {
                start++;
            }
        }
    }
    //for (int i = 0; i < (int)numTriangles; ++i) {
    //    int temp = ((int *)loadedMesh->indices.values)[i*3+1];
    //    ((int *)loadedMesh->indices.values)[i*3+1] = ((int *)loadedMesh->indices.values)[i*3+2];
    //    ((int *)loadedMesh->indices.values)[i*3+2] = temp;
    //}
}

// parses .obj but also makes a copy of some of the data to use for level data
void parseBitmap (void *fileData, game_assets *assets, int key, memory_arena *workingMemory) {
    int numTextures = assets->numTextures;
    assert(numTextures < MAX_NUM_TEXTURES);

    texture_asset *textureAsset = (texture_asset *)allocateMemorySize(&assets->assetMemory, sizeof(texture_asset)); 
    assets->textures[key] = textureAsset;
    assets->numTextures++;

    loaded_texture_asset *loadedBitmap = (loaded_texture_asset *)allocateMemorySize(workingMemory, sizeof(loaded_texture_asset));
    loadedBitmap->key = key;

    bitmap_header *header = (bitmap_header *)fileData;    
    unsigned int *pixels = (unsigned int *)((char *)fileData + header->bitmapOffset);

    int width = header->width;
    int height = header->height;
    textureAsset->width = width;
    textureAsset->height = height;
    loadedBitmap->width = width;
    loadedBitmap->height = height;
    unsigned char *bitmapPixels = (unsigned char *)allocateMemorySize(workingMemory, sizeof(unsigned char) * 4 * width * height);
    loadedBitmap->pixels = (unsigned int *)bitmapPixels;

    assert(header->bitsPerPixel == 32 && header->compression == 3);

    unsigned int alphaMask = ~(header->redMask | header->greenMask | header->blueMask);

    bool foundLeastSignificantDigit = false;
    unsigned int redMaskDigit = findLeastSignificantBit(header->redMask, &foundLeastSignificantDigit);
    assert(foundLeastSignificantDigit);
    unsigned int greenMaskDigit = findLeastSignificantBit(header->greenMask, &foundLeastSignificantDigit);
    assert(foundLeastSignificantDigit);
    unsigned int blueMaskDigit = findLeastSignificantBit(header->blueMask, &foundLeastSignificantDigit);
    assert(foundLeastSignificantDigit);
    unsigned int alphaMaskDigit = findLeastSignificantBit(alphaMask, &foundLeastSignificantDigit);
    assert(foundLeastSignificantDigit);

    unsigned int numPixelValues = width * height;
    unsigned int numBitmapValues = 0;
    for (unsigned int i = 0; i < numPixelValues; i++) {
        unsigned int color = pixels[i];

        unsigned char red = (unsigned char)((color & header->redMask) >> redMaskDigit);
        unsigned char green = (unsigned char)((color & header->greenMask) >> greenMaskDigit);
        unsigned char blue = (unsigned char)((color & header->blueMask) >> blueMaskDigit);
        unsigned char alpha = (unsigned char)((color & alphaMask) >> alphaMaskDigit);

        bitmapPixels[numBitmapValues] = red;
        bitmapPixels[numBitmapValues+1] = green;
        bitmapPixels[numBitmapValues+2] = blue;
        bitmapPixels[numBitmapValues+3] = alpha;
        numBitmapValues += 4;
    }
}

// djb2
// http://www.cse.yorku.ca/~oz/hash.html
unsigned int hashString (char *string) {
    unsigned int hash = 5381;
    int c;
    assert(string);
    do {
        c = *string++;
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
    } while (c);

    return hash;
}

void parseAtlas (void *atlasData, game_assets *assets, int atlasKey, int textureKey, memory_arena *workingMemory) {
    int numAtlases = assets->numAtlases;
    assert(numAtlases < MAX_NUM_ATLASES);

    atlas_asset *atlasAsset = (atlas_asset *)allocateMemorySize(&assets->assetMemory, sizeof(atlas_asset)); 
    assets->atlases[atlasKey] = atlasAsset;
    assets->numAtlases++;
    atlasAsset->atlasKey = (atlas_key)atlasKey;
    atlasAsset->textureKey = (texture_key)textureKey;

    // texture packer libgdx format
    char *start, *end, *nextLineStart;
    start = (char *)atlasData;
    readLine(start, &start, &end, &nextLineStart); // skip file name
    start = nextLineStart;
    readLine(start, &start, &end, &nextLineStart);

    char *wordStart = start;
    char *wordEnd = wordStart;
    while (*wordEnd != ' ' && wordEnd != end) { ++wordEnd; }
    ++wordEnd;
    wordStart = wordEnd;
    while (*wordEnd != ',' && wordEnd != end) { ++wordEnd; }
    atlasAsset->width = stringToInt(wordStart, wordEnd-1);

    wordEnd+=2;
    wordStart = wordEnd;
    while (*wordEnd != '\n' && *wordEnd != '\r' && wordEnd != end) { ++wordEnd; }
    atlasAsset->height = stringToInt(wordStart, wordEnd-1);

    start = nextLineStart;
    // skip format, filter, repeat
    readLine(start, &start, &end, &nextLineStart);
    start = nextLineStart;
    readLine(start, &start, &end, &nextLineStart);
    start = nextLineStart;
    readLine(start, &start, &end, &nextLineStart);
    start = nextLineStart;

    char *frameInfoStart = start;
    int numFrames = 0;
    while (true) {
        for (int i = 0; i < 7; ++i) {
            readLine(start, &start, &end, &nextLineStart);
            start = nextLineStart;
        }
        numFrames++;
        if (*(nextLineStart) == 0) { break; }
    }

    // allocate enough atlas_frames
    assert(numFrames < 500);
    for (int i = 0; i < 500; ++i) {
        atlasAsset->map.entries[i] = {};
    }
    // TODO(ebuchholz): get rid of this first scan or change structure of map

    // TODO(ebuchholz): more convenient file reading/string copying functions
    start = frameInfoStart;
    for (int i = 0; i < numFrames; ++i) {
        atlas_frame frame;
        readLine(start, &start, &end, &nextLineStart);
        int length = (int)(end - start);
        char *frameName = (char *)allocateMemorySize(&assets->assetMemory, 
                                                     sizeof(char) * length+1);
        for (int j = 0; j < length; ++j) {
            frameName[j] = start[j];
        }
        frameName[length] = 0;
        frame.key = frameName;

        start = nextLineStart;
        readLine(start, &start, &end, &nextLineStart);
        start = nextLineStart;
        readLine(start, &start, &end, &nextLineStart);

        start += 6;
        wordStart = start;
        wordEnd = wordStart;
        while (*wordEnd != ',' && wordEnd != end) { ++wordEnd; }
        int frameX = stringToInt(wordStart, wordEnd-1);

        wordEnd += 2;
        wordStart = wordEnd;
        while (*wordEnd != '\n' && *wordEnd != '\r' && wordEnd != end) { ++wordEnd; }
        int frameY = stringToInt(wordStart, wordEnd-1);

        start = nextLineStart;
        readLine(start, &start, &end, &nextLineStart);

        start += 8;
        wordStart = start;
        wordEnd = wordStart;
        while (*wordEnd != ',' && wordEnd != end) { ++wordEnd; }
        int frameWidth = stringToInt(wordStart, wordEnd-1);

        wordEnd += 2;
        wordStart = wordEnd;
        while (*wordEnd != '\n' && *wordEnd != '\r' && wordEnd != end) { ++wordEnd; }
        int frameHeight = stringToInt(wordStart, wordEnd-1);

        // skip the rest of the frame info
        start = nextLineStart;
        readLine(start, &start, &end, &nextLineStart);
        start = nextLineStart;
        readLine(start, &start, &end, &nextLineStart);
        start = nextLineStart;
        readLine(start, &start, &end, &nextLineStart);
        start = nextLineStart;

        frame.frameWidth = frameWidth;
        frame.frameHeight = frameHeight;

        float uvWidth = (float)frameWidth / (float)atlasAsset->width;
        float uvHeight = (float)frameHeight / (float)atlasAsset->height;
        frame.frameCorners[0] = Vector2((float)frameX / (float)atlasAsset->width,
                                        1.0f - (float)frameY / (float)atlasAsset->height);
        frame.frameCorners[1] = Vector2(frame.frameCorners[0].x + uvWidth, frame.frameCorners[0].y);
        frame.frameCorners[2] = Vector2(frame.frameCorners[0].x, frame.frameCorners[0].y - uvHeight);
        frame.frameCorners[3] = Vector2(frame.frameCorners[0].x + uvWidth, frame.frameCorners[0].y - uvHeight);

        unsigned int hash = hashString(frameName);
        unsigned int mapIndex = hash % 500;
        atlas_frame *existingFrame = &atlasAsset->map.entries[mapIndex];
        while (existingFrame->key != 0) {
            mapIndex = (mapIndex + 1) % 500;
            existingFrame = &atlasAsset->map.entries[mapIndex];
        }
        atlasAsset->map.entries[mapIndex] = frame;
    }
}

atlas_frame *getAtlasFrame (game_assets *assets, int atlasKey, char *frameName) {
    atlas_frame *result;

    atlas_asset *textureAtlas = assets->atlases[atlasKey];
    assert(textureAtlas);

    unsigned int hash = hashString(frameName);
    unsigned int mapIndex = hash % 500;
    unsigned int originalMapIndex = mapIndex; // check that we actually find it after looping all the way around
    while (true) {
        result = &textureAtlas->map.entries[mapIndex];
        if (stringsAreEqual(frameName, result->key)) {
            break;
        }
        else {
            mapIndex = (mapIndex + 1) % 500;
            assert(mapIndex != originalMapIndex);
        }
    }

    return result;
}

// get from atlas map function: calc hash, get item from array, check key, if conflict, get next and check key again

void pushAsset (asset_list *assetList, char *path, asset_type type, int key) {
    assert(assetList->numAssetsToLoad < assetList->maxAssetsToLoad);
    asset_to_load *assetToLoad = assetList->assetsToLoad + assetList->numAssetsToLoad;
    assetList->numAssetsToLoad++;
    assetToLoad->path = path;
    assetToLoad->type = type;
    assetToLoad->key = key;
    assetToLoad->secondKey = -1;
}

void pushAsset (asset_list *assetList, char *path, asset_type type, int key, int secondKey) {
    assert(assetList->numAssetsToLoad < assetList->maxAssetsToLoad);
    asset_to_load *assetToLoad = assetList->assetsToLoad + assetList->numAssetsToLoad;
    assetList->numAssetsToLoad++;
    assetToLoad->path = path;
    assetToLoad->type = type;
    assetToLoad->key = key;
    assetToLoad->secondKey = secondKey;
}
