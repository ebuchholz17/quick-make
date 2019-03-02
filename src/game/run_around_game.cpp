#include "run_around_game.h"

static bool sameSign (float a, float b) {
    bool aNegative = a < 0.0f;
    bool bNegative = b < 0.0f;
    return aNegative == bNegative;
}

// From RTCD
static bool lineSegmentTriangleIntersection (line *lineSegment, triangleWithNormals *tri, float *t, vector3 *normal) {
    vector3 pq = lineSegment->b - lineSegment->a;
    vector3 pa = tri->p0 - lineSegment->a;
    vector3 pb = tri->p1 - lineSegment->a;
    vector3 pc = tri->p2 - lineSegment->a;

    vector3 m = crossProduct(pq, pc);
    float u = dotProduct(pb, m);
    float v = -dotProduct(pa, m);
    if (!sameSign(u, v)) { return false; }
    float w = dotProduct(crossProduct(pq, pb), pa);
    if (!sameSign(u, w)) { return false; }

    float denom = 1.0f / (u + v + w);
    u *= denom;
    v *= denom;
    w *= denom;

    vector3 r = u*tri->p0 + v*tri->p1 + w*tri->p2;
    vector3 ab = lineSegment->b - lineSegment->a;
    *t = dotProduct(r - lineSegment->a, ab) / dotProduct(ab, ab);

    *normal = normalize(u * tri->n0 + v * tri->n1 + w * tri->n2);

    if (*t >= 0.0f && *t <= 1.0f) { return true; }
    return false;
}

static line *pushLineOntoRenderCommand (render_command_list *renderCommands, render_command_lines *lineCommand) {
    assert(renderCommands->memory.size + sizeof(line) < renderCommands->memory.capacity);

    line *result = (line *)((char *)renderCommands->memory.base + renderCommands->memory.size);
    renderCommands->memory.size += sizeof(line);

    // must finish with the line command before doing other ones
    assert(result == lineCommand->lines + lineCommand->numLines);
    lineCommand->numLines++;

    return result;
}
                                        
static void *pushRenderCommand (render_command_list *renderCommands, 
                         render_command_type type, 
                         unsigned int size) 
{
    void *renderCommand = 0;
    int requiredSize = sizeof(render_command_header) + size;
    if (renderCommands->memory.size + requiredSize < renderCommands->memory.capacity) {
        render_command_header *header = (render_command_header *)((char *)renderCommands->memory.base + 
                                                                  renderCommands->memory.size);
        renderCommands->memory.size += requiredSize;
        header->type = type;
        renderCommand = (char *)header + sizeof(render_command_header);
    }
    else {
        // Out of memory error?
    }
    return renderCommand;
}

static void *allocateMemorySize (memory_arena *memory, unsigned int size) {
    assert(memory->size + size < memory->capacity);
    void *result = (char *)memory->base + memory->size;
    memory->size += size;
    return result;
}

static void readLine (char *data, char **start, char **end) {
    *start = data;
    int index = 0;
    while (data[index] != 0 && data[index] != '\n') {
        ++index;
    }
    *end = data + index;
}

static int stringToInt (char *start, char *end) {
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
static float stringToFloat (char *start, char *end) {
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

//static float stringToFloat (char *string) {
//    char *end = string;
//    while (*end != 0) {
//        ++end;
//    }
//    --end;
//    return stringToFloat(string, end);
//}

// TODO(ebuchholz): move somewhere else
static bool stringsAreEqual (int length, char *a, char *b) {
    char *bIndex = b;
    for (int i = 0; i < length; ++i, ++bIndex) {
        if (*bIndex == 0 || a[i] != *bIndex) {
            return false;
        }
    }
    return (*bIndex == 0);
}

static void parseOBJ (void *objData, game_assets *assets, int key, memory_arena *workingMemory) {
    int numMeshes = assets->numMeshes;
    assert(numMeshes < MAX_NUM_MESHES);

    mesh_asset *meshAsset = (mesh_asset *)allocateMemorySize(&assets->assetMemory, sizeof(mesh_asset *));
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
        while (*wordEnd != ' ' && *wordEnd != '\n' && *wordEnd!= 0) { ++wordEnd; }
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
            start++;
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
        while (*wordEnd != ' ' && *wordEnd != '\n' && *wordEnd!= 0) { ++wordEnd; }
        int length = (int)(wordEnd - wordStart);

        if (stringsAreEqual(length, wordStart, "v")) {
            float (*tempPosition)[3] = &tempPositions[tempPositionIndex];
            wordEnd++;
            wordStart = wordEnd;
            while (*wordEnd != ' ' && *wordEnd != '\n' && *wordEnd!= 0) { ++wordEnd; }
            (*tempPosition)[0] = stringToFloat(wordStart, wordEnd - 1);

            wordEnd++;
            wordStart = wordEnd;
            while (*wordEnd != ' ' && *wordEnd != '\n' && *wordEnd!= 0) { ++wordEnd; }
            (*tempPosition)[1] = stringToFloat(wordStart, wordEnd - 1);

            wordEnd++;
            wordStart = wordEnd;
            while (*wordEnd != ' ' && *wordEnd != '\n' && *wordEnd!= 0) { ++wordEnd; }
            (*tempPosition)[2] = stringToFloat(wordStart, wordEnd - 1);

            tempPositionIndex++;
        }
        else if (stringsAreEqual(length, wordStart, "vt")) {
            float (*tempTexCoord)[2] = &tempTexCoords[tempTexCoordIndex];
            wordEnd++;
            wordStart = wordEnd;
            while (*wordEnd != ' ' && *wordEnd != '\n' && *wordEnd!= 0) { ++wordEnd; }
            (*tempTexCoord)[0] = stringToFloat(wordStart, wordEnd - 1);

            wordEnd++;
            wordStart = wordEnd;
            while (*wordEnd != ' ' && *wordEnd != '\n' && *wordEnd!= 0) { ++wordEnd; }
            (*tempTexCoord)[1] = stringToFloat(wordStart, wordEnd - 1);

            tempTexCoordIndex++;
        }
        else if (stringsAreEqual(length, wordStart, "vn")) {
            float (*tempNormal)[3] = &tempNormals[tempNormalIndex];
            wordEnd++;
            wordStart = wordEnd;
            while (*wordEnd != ' ' && *wordEnd != '\n' && *wordEnd!= 0) { ++wordEnd; }
            (*tempNormal)[0] = stringToFloat(wordStart, wordEnd - 1);

            wordEnd++;
            wordStart = wordEnd;
            while (*wordEnd != ' ' && *wordEnd != '\n' && *wordEnd!= 0) { ++wordEnd; }
            (*tempNormal)[1] = stringToFloat(wordStart, wordEnd - 1);

            wordEnd++;
            wordStart = wordEnd;
            while (*wordEnd != ' ' && *wordEnd != '\n' && *wordEnd!= 0) { ++wordEnd; }
            (*tempNormal)[2] = stringToFloat(wordStart, wordEnd - 1);

            tempNormalIndex++;
        }
        else if (stringsAreEqual(length, wordStart, "f")) {
            // TODO(ebuchholz): handle the case where text coords and/or normals are missing
            for (int i = 0; i < 3; ++i) {
                wordEnd++;
                wordStart = wordEnd;
                while (*wordEnd != ' ' && *wordEnd != '\n' && *wordEnd!= 0) { ++wordEnd; }

                char *indexStart = wordStart;
                char *indexEnd = indexStart;
                while (*indexEnd != ' ' && *indexEnd != '\n' && *indexEnd != '/' && *wordEnd!= 0) { ++indexEnd; }
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

                while (*indexEnd != ' ' && *indexEnd != '\n' && *indexEnd != '/' && *wordEnd!= 0) { ++indexEnd; }
                if (indexStart != indexEnd) {
                    int index = stringToInt(indexStart, indexEnd - 1) - 1;
                    float *texCoordsPointer = (float *)loadedMesh->texCoords.values + (uvIndex * 2);
                    texCoordsPointer[0] = tempTexCoords[index][0];
                    texCoordsPointer[1] = tempTexCoords[index][1];
                    uvIndex++;
                }
                indexEnd++;
                indexStart = indexEnd;

                while (*indexEnd != ' ' && *indexEnd != '\n' && *indexEnd != '/' && *wordEnd!= 0) { ++indexEnd; }
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
            start++;
        }
    }
    //for (int i = 0; i < (int)numTriangles; ++i) {
    //    int temp = ((int *)loadedMesh->indices.values)[i*3+1];
    //    ((int *)loadedMesh->indices.values)[i*3+1] = ((int *)loadedMesh->indices.values)[i*3+2];
    //    ((int *)loadedMesh->indices.values)[i*3+2] = temp;
    //}
}

// parses .obj but also makes a copy of some of the data to use for level data
static void parseLevelOBJ (void *objData, game_assets *assets, int meshKey, int levelMeshKey,
                           memory_arena *workingMemory) 
{
    parseOBJ(objData, assets, meshKey, workingMemory);

    int numLevelMeshes = assets->numLevelMeshes;
    assert(numLevelMeshes < MAX_NUM_LEVEL_MESHES);

    level_mesh *levelMesh = (level_mesh *)allocateMemorySize(&assets->assetMemory, sizeof(level_mesh));
    assets->levelMeshes[levelMeshKey] = levelMesh;
    assets->numLevelMeshes++;

    levelMesh->key = (level_mesh_key)levelMeshKey;

    loaded_mesh_asset *meshAssetData = (loaded_mesh_asset *)workingMemory->base;

    // NOTE(ebuchholz): assuming every index is unique and every 3 of them for a triangle, so
    // we can just loop through these positions and make a triangle for every 9 (3 vertices)
    float_mesh_attribute *positions = &meshAssetData->positions;
    int numPositions = positions->count;
    float_mesh_attribute *normals = &meshAssetData->normals;
    levelMesh->triangleCount = numPositions / 9;
    levelMesh->triangles = (triangleWithNormals *)allocateMemorySize(&assets->assetMemory, sizeof(triangleWithNormals) * levelMesh->triangleCount);
    levelMesh->triangleAABBs = (aabb *)allocateMemorySize(&assets->assetMemory, sizeof(aabb) * levelMesh->triangleCount);
    levelMesh->boundingBox = {}; 
    int numTriangles = 0;
    for (int i = 0; i < numPositions; i += 9) {
        triangleWithNormals *tri = levelMesh->triangles + numTriangles;
        tri->p0.x = ((float *)positions->values)[i];
        tri->p0.y = ((float *)positions->values)[i+1];
        tri->p0.z = ((float *)positions->values)[i+2];

        tri->p1.x = ((float *)positions->values)[i+3];
        tri->p1.y = ((float *)positions->values)[i+4];
        tri->p1.z = ((float *)positions->values)[i+5];

        tri->p2.x = ((float *)positions->values)[i+6];
        tri->p2.y = ((float *)positions->values)[i+7];
        tri->p2.z = ((float *)positions->values)[i+8];

        tri->n0.x = ((float *)normals->values)[i];
        tri->n0.y = ((float *)normals->values)[i+1];
        tri->n0.z = ((float *)normals->values)[i+2];

        tri->n1.x = ((float *)normals->values)[i+3];
        tri->n1.y = ((float *)normals->values)[i+4];
        tri->n1.z = ((float *)normals->values)[i+5];

        tri->n2.x = ((float *)normals->values)[i+6];
        tri->n2.y = ((float *)normals->values)[i+7];
        tri->n2.z = ((float *)normals->values)[i+8];

        // TODO(ebuchholz): probably better just to keep track of the min/max coordinates, but
        // I already have the functions for these so it's easier
        aabb triangleAABB = getTriangleBounds(tri->p0, tri->p1, tri->p2);
        *(levelMesh->triangleAABBs + numTriangles) = triangleAABB;
        levelMesh->boundingBox = unionAABB(levelMesh->boundingBox, triangleAABB);

        numTriangles++;
    }
}

static void parseBitmap (void *fileData, game_assets *assets, int key, memory_arena *workingMemory) {
    int numTextures = assets->numTextures;
    assert(numTextures < MAX_NUM_TEXTURES);

    texture_asset *textureAsset = (texture_asset *)allocateMemorySize(&assets->assetMemory, sizeof(texture_asset *));
    assets->textures[key] = textureAsset;
    assets->numTextures++;

    loaded_texture_asset *loadedBitmap = (loaded_texture_asset *)allocateMemorySize(workingMemory, sizeof(loaded_texture_asset));
    loadedBitmap->key = key;

    bitmap_header *header = (bitmap_header *)fileData;    
    unsigned char *pixels = (unsigned char *)((char *)fileData + header->bitmapOffset);

    int width = header->width;
    int height = header->height;
    loadedBitmap->width = width;
    loadedBitmap->height = height;
    unsigned char *bitmapPixels = (unsigned char *)allocateMemorySize(workingMemory, sizeof(unsigned char) * 4 * width * height);
    loadedBitmap->pixels = (unsigned int *)bitmapPixels;

    unsigned int numPixelValues = width * height * 3;
    unsigned int numBitmapValues = 0;
    for (unsigned int i = 0; i < numPixelValues; i += 3) {
        bitmapPixels[numBitmapValues] = pixels[i+2];
        bitmapPixels[numBitmapValues+1] = pixels[i+1];
        bitmapPixels[numBitmapValues+2] = pixels[i];
        bitmapPixels[numBitmapValues+3] = 0xff;
        numBitmapValues += 4;
    }
}

static void pushAsset (asset_list *assetList, char *path, asset_type type, int key1, int key2) {
    assert(assetList->numAssetsToLoad < assetList->maxAssetsToLoad);
    asset_to_load *assetToLoad = assetList->assetsToLoad + assetList->numAssetsToLoad;
    assetList->numAssetsToLoad++;
    assetToLoad->path = path;
    assetToLoad->type = type;
    assetToLoad->key1 = key1;
    assetToLoad->key2 = key2;
}

static void pushAsset (asset_list *assetList, char *path, asset_type type, int key) {
    pushAsset(assetList, path, type, key, -1);
}

// TODO(ebuchholz): Maybe pack everything into a single file and load that?
extern "C" void getGameAssetList (asset_list *assetList) {
    pushAsset(assetList, "assets/meshes/cube.obj", ASSET_TYPE_OBJ, MESH_KEY_CUBE);
    pushAsset(assetList, "assets/meshes/cylinder.obj", ASSET_TYPE_OBJ, MESH_KEY_CYLINDER);
    pushAsset(assetList, "assets/meshes/thumbstick.obj", ASSET_TYPE_OBJ, MESH_KEY_THUMBSTICK);
    pushAsset(assetList, "assets/meshes/button.obj", ASSET_TYPE_OBJ, MESH_KEY_BUTTON);
    pushAsset(assetList, "assets/meshes/flag.obj", ASSET_TYPE_OBJ, MESH_KEY_FLAG);
    pushAsset(assetList, "assets/meshes/spikes.obj", ASSET_TYPE_OBJ, MESH_KEY_SPIKES);
    pushAsset(assetList, "assets/meshes/youwin.obj", ASSET_TYPE_OBJ, MESH_KEY_YOUWIN);

    pushAsset(assetList, "assets/textures/green.bmp", ASSET_TYPE_BMP, TEXTURE_KEY_GREEN);
    pushAsset(assetList, "assets/textures/blue.bmp", ASSET_TYPE_BMP, TEXTURE_KEY_BLUE);
    pushAsset(assetList, "assets/textures/white.bmp", ASSET_TYPE_BMP, TEXTURE_KEY_WHITE);
    pushAsset(assetList, "assets/textures/flag.bmp", ASSET_TYPE_BMP, TEXTURE_KEY_FLAG);
    pushAsset(assetList, "assets/textures/black_texture.bmp", ASSET_TYPE_BMP, TEXTURE_KEY_BLACK_TEXTURE);

    pushAsset(assetList, "assets/meshes/ground.obj", ASSET_TYPE_LEVEL_OBJ, MESH_KEY_GROUND, LEVEL_MESH_KEY_GROUND);
    pushAsset(assetList, "assets/meshes/hill.obj", ASSET_TYPE_LEVEL_OBJ, MESH_KEY_HILL, LEVEL_MESH_KEY_HILL);
    pushAsset(assetList, "assets/meshes/loop.obj", ASSET_TYPE_LEVEL_OBJ, MESH_KEY_LOOP, LEVEL_MESH_KEY_LOOP);
    pushAsset(assetList, "assets/meshes/loop_ramp.obj", ASSET_TYPE_LEVEL_OBJ, MESH_KEY_LOOP_RAMP, LEVEL_MESH_KEY_LOOP_RAMP);
    pushAsset(assetList, "assets/meshes/loop_ramp2.obj", ASSET_TYPE_LEVEL_OBJ, MESH_KEY_LOOP_RAMP2, LEVEL_MESH_KEY_LOOP_RAMP2);
    pushAsset(assetList, "assets/meshes/tricky_area.obj", ASSET_TYPE_LEVEL_OBJ, MESH_KEY_TRICKY_AREA, LEVEL_MESH_KEY_TRICKY_AREA);
    pushAsset(assetList, "assets/meshes/cliff.obj", ASSET_TYPE_LEVEL_OBJ, MESH_KEY_CLIFF, LEVEL_MESH_KEY_CLIFF);
    pushAsset(assetList, "assets/meshes/towers.obj", ASSET_TYPE_LEVEL_OBJ, MESH_KEY_TOWERS, LEVEL_MESH_KEY_TOWERS);
    pushAsset(assetList, "assets/meshes/under_loop.obj", ASSET_TYPE_LEVEL_OBJ, MESH_KEY_UNDER_LOOP, LEVEL_MESH_KEY_UNDER_LOOP);
}

extern "C" void parseGameAsset (void *assetData, asset_type type, int key1, int key2,
                                game_memory *gameMemory, memory_arena *workingMemory) 
{
    game_state *gameState = (game_state *)gameMemory->memory;
    if (!gameState->assetsInitialized) {
        gameState->assetsInitialized = true;

        gameState->memory = {};
        gameState->memory.size = 0;
        gameState->memory.capacity = gameMemory->memoryCapacity - sizeof(game_state);
        gameState->memory.base = (char *)gameMemory->memory + sizeof(game_state);

        gameState->assets = {};
        game_assets *assets = &gameState->assets;
        assets->assetMemory = {};
        assets->assetMemory.size = 0;
        assets->assetMemory.capacity = 10 * 1024 * 1024; // 1MB of asset data???
        assets->assetMemory.base = allocateMemorySize(&gameState->memory, assets->assetMemory.capacity); 
        assets->numMeshes = 0;
    } 
    // parse data
    // create/copy stuff into game memory
    // place whatever theplatform needs at the beginningof working memory
    switch (type) {
    default:
        assert(false); // must provide a valid type
        break;
    case ASSET_TYPE_OBJ:
        parseOBJ(assetData, &gameState->assets, key1, workingMemory);
        break;
    case ASSET_TYPE_LEVEL_OBJ:
        parseLevelOBJ(assetData, &gameState->assets, key1, key2, workingMemory);
        break;
    case ASSET_TYPE_BMP:
        parseBitmap(assetData, &gameState->assets, key1, workingMemory);
        break;
    }
}

static void drawModel (mesh_key meshKey, texture_key textureKey, 
                       matrix4x4 modelMatrix, render_command_list *renderCommands) {
    render_command_model *modelCommand = 
        (render_command_model *)pushRenderCommand(renderCommands,
                                                 RENDER_COMMAND_MODEL,
                                                 sizeof(render_command_model));
    modelCommand->meshKey = meshKey;
    modelCommand->textureKey = textureKey;
    modelCommand->modelMatrix = modelMatrix;
}

static render_command_lines *startLines (render_command_list *renderCommands) {
    render_command_lines *lineCommand = 
        (render_command_lines *)pushRenderCommand(renderCommands, RENDER_COMMAND_LINES, sizeof(render_command_lines));
    lineCommand->lines = (line *)((char *)renderCommands->memory.base + renderCommands->memory.size);
    lineCommand->numLines = 0; // zero this out: was a bug where the value from the last frame persisted
    return lineCommand;
}

static void drawLine (float ax, float ay, float az, float bx, float by, float bz, render_command_list *renderCommands, render_command_lines *lineCommand) {
    line *line = pushLineOntoRenderCommand(renderCommands, lineCommand);
    line->a.x = ax;
    line->a.y = ay;
    line->a.z = az;
    line->b.x = bx;
    line->b.y = by;
    line->b.z = bz;
}

// same as drawTriangle but without making a new render command
//static void drawMeshTriangle (triangle *t, render_command_list *renderCommands, render_command_lines *lineCommand) {
//    drawLine(t->p0.x, t->p0.y, t->p0.z, t->p1.x, t->p1.y, t->p1.z, renderCommands, lineCommand);
//    drawLine(t->p1.x, t->p1.y, t->p1.z, t->p2.x, t->p2.y, t->p2.z, renderCommands, lineCommand);
//    drawLine(t->p2.x, t->p2.y, t->p2.z, t->p0.x, t->p0.y, t->p0.z, renderCommands, lineCommand);
//}

//static void drawTriangle (triangle *t, render_command_list *renderCommands) {
//    render_command_lines *lineCommand = startLines(renderCommands);
//    drawLine(t->p0.x, t->p0.y, t->p0.z, t->p1.x, t->p1.y, t->p1.z, renderCommands, lineCommand);
//    drawLine(t->p1.x, t->p1.y, t->p1.z, t->p2.x, t->p2.y, t->p2.z, renderCommands, lineCommand);
//    drawLine(t->p2.x, t->p2.y, t->p2.z, t->p0.x, t->p0.y, t->p0.z, renderCommands, lineCommand);
//}

//static void drawLevelMesh (level_mesh *levelMesh, render_command_list *renderCommands) {
//    render_command_lines *lineCommand = startLines(renderCommands);
//    triangle *triangles = levelMesh->triangles;
//    for (int i = 0; i < levelMesh->triangleCount; ++i) {
//        drawMeshTriangle(triangles + i, renderCommands, lineCommand);
//    }
//}

//static void drawCircle (float centerX, float centerY, float centerZ, float radius, render_command_list *renderCommands) {
//    render_command_lines *lineCommand = startLines(renderCommands);
//    const int numPoints = 16;
//    float deltaAngle = (2.0f * PI) / numPoints;
//    for (int i = 0; i < numPoints; ++i) {
//        drawLine(centerX + radius * cosf(deltaAngle * (float)i), centerY + radius * sinf(deltaAngle * (float)i), centerZ, 
//                 centerX + radius * cosf(deltaAngle * (float)((i+1)%numPoints)), centerY + radius * sinf(deltaAngle * (float)((i+1)%numPoints)), centerZ, 
//                 renderCommands, lineCommand);
//    }
//}

void drawAABB (aabb *aabb, render_command_list *renderCommands) {
    render_command_lines *lineCommand = startLines(renderCommands);
    vector3 min = aabb->min;
    vector3 max = aabb->max;
    drawLine(min.x, min.y, min.z, max.x, min.y, min.z, renderCommands, lineCommand);
    drawLine(max.x, min.y, min.z, max.x, min.y, max.z, renderCommands, lineCommand);
    drawLine(max.x, min.y, max.z, min.x, min.y, max.z, renderCommands, lineCommand);
    drawLine(min.x, min.y, max.z, min.x, min.y, min.z, renderCommands, lineCommand);
    drawLine(min.x, max.y, min.z, max.x, max.y, min.z, renderCommands, lineCommand);
    drawLine(max.x, max.y, min.z, max.x, max.y, max.z, renderCommands, lineCommand);
    drawLine(max.x, max.y, max.z, min.x, max.y, max.z, renderCommands, lineCommand);
    drawLine(min.x, max.y, max.z, min.x, max.y, min.z, renderCommands, lineCommand);
    drawLine(min.x, min.y, min.z, min.x, max.y, min.z, renderCommands, lineCommand);
    drawLine(max.x, min.y, min.z, max.x, max.y, min.z, renderCommands, lineCommand);
    drawLine(max.x, min.y, max.z, max.x, max.y, max.z, renderCommands, lineCommand);
    drawLine(min.x, min.y, max.z, min.x, max.y, max.z, renderCommands, lineCommand);
}

static void growAABBByLineSensor (line sensor, aabb *boundingBox) {
    aabb lineABox = {};
    lineABox.min = sensor.a;
    lineABox.max = sensor.a;
    aabb lineBBox = {};
    lineBBox.min = sensor.b;
    lineBBox.max = sensor.b;
    *boundingBox = unionAABB(*boundingBox, lineABox);
    *boundingBox = unionAABB(*boundingBox, lineBBox);
}

// TODO(ebuchholz): use up direction instead of orientation?
static void setupWallSensors (player_state *player, float sensorLength, aabb *linesAABB) {
    vector3 forward = player->orientation * Vector3(0.0f, 0.0f, -1.0f);
    vector3 side = player->orientation * Vector3(1.0f, 0.0f, 0.0f);
    player->wallSensors[0].a = player->pos;
    player->wallSensors[0].b = player->pos + forward * sensorLength;
    growAABBByLineSensor(player->wallSensors[0], linesAABB);

    player->wallSensors[1].a = player->pos;
    player->wallSensors[1].b = player->pos + (normalize(forward + side)) * sensorLength;
    growAABBByLineSensor(player->wallSensors[1], linesAABB);

    player->wallSensors[2].a = player->pos;
    player->wallSensors[2].b = player->pos + side * sensorLength;
    growAABBByLineSensor(player->wallSensors[2], linesAABB);

    player->wallSensors[3].a = player->pos;
    player->wallSensors[3].b = player->pos + normalize(side - forward) * sensorLength;
    growAABBByLineSensor(player->wallSensors[3], linesAABB);

    player->wallSensors[4].a = player->pos;
    player->wallSensors[4].b = player->pos - forward * sensorLength;
    growAABBByLineSensor(player->wallSensors[4], linesAABB);

    player->wallSensors[5].a = player->pos;
    player->wallSensors[5].b = player->pos + normalize(-forward - side) * sensorLength;
    growAABBByLineSensor(player->wallSensors[5], linesAABB);

    player->wallSensors[6].a = player->pos;
    player->wallSensors[6].b = player->pos - side * sensorLength;
    growAABBByLineSensor(player->wallSensors[6], linesAABB);

    player->wallSensors[7].a = player->pos;
    player->wallSensors[7].b = player->pos + normalize(forward - side) * sensorLength;
    growAABBByLineSensor(player->wallSensors[7], linesAABB);
}

static void setupCollisionSensors (player_state *player, float playerRadius, float sensorLength, 
                                   vector3 firstDirection, vector3 secondDirection,
                                   aabb *linesAABB) 
{
    player->collisionSensors[1].a = player->pos + firstDirection * playerRadius;
    player->collisionSensors[1].b = player->collisionSensors[1].a + (-player->upDirection * sensorLength);
    growAABBByLineSensor(player->collisionSensors[1], linesAABB);

    player->collisionSensors[2].a = player->pos - firstDirection * playerRadius;
    player->collisionSensors[2].b = player->collisionSensors[2].a + (-player->upDirection * sensorLength);
    growAABBByLineSensor(player->collisionSensors[2], linesAABB);

    vector3 zVector = Vector3(0.0f, 0.0f, playerRadius);
    player->collisionSensors[3].a = player->pos + secondDirection * playerRadius;
    player->collisionSensors[3].b = player->collisionSensors[3].a + (-player->upDirection * sensorLength);
    growAABBByLineSensor(player->collisionSensors[3], linesAABB);

    player->collisionSensors[4].a = player->pos - secondDirection * playerRadius;
    player->collisionSensors[4].b = player->collisionSensors[4].a + (-player->upDirection * sensorLength);
    growAABBByLineSensor(player->collisionSensors[4], linesAABB);

    vector3 angleVector = normalize(firstDirection + secondDirection);
    player->collisionSensors[5].a = player->pos + angleVector * playerRadius;
    player->collisionSensors[5].b = player->collisionSensors[5].a + (-player->upDirection * sensorLength);
    growAABBByLineSensor(player->collisionSensors[5], linesAABB);

    player->collisionSensors[6].a = player->pos - angleVector * playerRadius;
    player->collisionSensors[6].b = player->collisionSensors[6].a + (-player->upDirection * sensorLength);
    growAABBByLineSensor(player->collisionSensors[6], linesAABB);

    vector3 otherAngleVector = normalize(firstDirection - secondDirection);
    player->collisionSensors[7].a = player->pos + otherAngleVector * playerRadius;
    player->collisionSensors[7].b = player->collisionSensors[7].a + (-player->upDirection * sensorLength);
    growAABBByLineSensor(player->collisionSensors[7], linesAABB);

    player->collisionSensors[8].a = player->pos - otherAngleVector * playerRadius;
    player->collisionSensors[8].b = player->collisionSensors[8].a + (-player->upDirection * sensorLength);
    growAABBByLineSensor(player->collisionSensors[8], linesAABB);
}

static void startPlayerOnNoGround (player_state *player) {
    player->onGround = false;
    vector3 up = Vector3(0.0f, 1.0f, 0.0f);

    vector3 newBackward = -normalize(player->velocity - dotProduct(player->velocity, up) * up);
    vector3 newSide = normalize(crossProduct(up, newBackward));

    matrix4x4 newOrientationMatrix = {};
    newOrientationMatrix.m[0] = newSide.x;
    newOrientationMatrix.m[1] = newSide.y;
    newOrientationMatrix.m[2] = newSide.z;
    newOrientationMatrix.m[3] = 0.0f;
    newOrientationMatrix.m[4] = up.x;
    newOrientationMatrix.m[5] = up.y;
    newOrientationMatrix.m[6] = up.z;
    newOrientationMatrix.m[7] = 0.0f;
    newOrientationMatrix.m[8] = newBackward.x;
    newOrientationMatrix.m[9] = newBackward.y;
    newOrientationMatrix.m[10] = newBackward.z;
    newOrientationMatrix.m[11] = 0.0f;
    newOrientationMatrix.m[12] = 0.0f;
    newOrientationMatrix.m[13] = 0.0f;
    newOrientationMatrix.m[14] = 0.0f;
    newOrientationMatrix.m[15] = 1.0f;

    player->orientation = transpose(newOrientationMatrix);
}

void respawnPlayer (player_state *player) {
    player->mode = PLAYER_SURFACE_MODE_FLOOR;
    player->upDirection = Vector3(0.0f, 1.0f, 0.0f);
    player->slopeDirection = Vector3(0.0f, 1.0f, 0.0f);
    player->pos = Vector3(0.0f, 10.0f, 0.0f);
    player->groundSpeed = Vector3(0.0f, 0.0f, 0.0f);
    player->velocity = Vector3(0.0f, 0.0f, 0.0f);
    player->onGround = false;
    player->dead = false;
    player->deadTimer = 0.0f;
}

bool getJumpButtonPressed (game_input *input) {
    bool jumpPressed = false;
    if (input->upButton) {
        jumpPressed = true;
    }

    int virtualButtonBoundsStartX = 960 - 150;
    int virtualButtonBoundsEndX = 960;
    int virtualButtonBoundsStartY = 540 - 150;
    int virtualButtonBoundsEndY = 540;
    if (input->pointerDown) {
        if (input->pointerX > virtualButtonBoundsStartX && input->pointerX < virtualButtonBoundsEndX &&
            input->pointerY > virtualButtonBoundsStartY && input->pointerY < virtualButtonBoundsEndY) 
        {
            jumpPressed = true;
        }
    }
    if (input->pointer2Down) {
        if (input->pointer2X > virtualButtonBoundsStartX && input->pointer2X < virtualButtonBoundsEndX &&
            input->pointer2Y > virtualButtonBoundsStartY && input->pointer2Y < virtualButtonBoundsEndY) 
        {
            jumpPressed = true;
        }
    }
    return jumpPressed;
}

vector3 getMovementVector (player_state *player, game_input *input, matrix4x4 cameraOrientation, float *newStickX, float *newStickY) {
    vector3 direction = Vector3();

    vector3 forward = Vector3(0.0f, 0.0f, -1.0f);
    vector3 side = Vector3(1.0f, 0.0f, 0.0f);

    matrix4x4 cameraMatrix = cameraOrientation;
    cameraMatrix.m[3] = 0.0f;
    cameraMatrix.m[7] = 0.0f;
    cameraMatrix.m[11] = 0.0f;
    cameraMatrix = transpose(cameraMatrix);
    if (player->mode == PLAYER_SURFACE_MODE_WALL) {
        forward = Vector3(0.0f, 1.0f, 0.0f);
    }

    if (input->forwardButton) {
        direction += forward;
    }
    if (input->backButton) {
        direction -= forward;
    }
    if (input->leftButton) {
        direction -= side;
    }
    if (input->rightButton) {
        direction += side;
    }

    int virtualStickBoundsStartX = 0;
    int virtualStickBoundsEndX = 150;
    int virtualStickBoundsStartY = 540 - 150;
    int virtualStickBoundsEndY = 540;
    int virtualStickStartX = 21;
    int virtualStickEndX = 129;
    int virtualStickStartY = 540 - 129;
    int virtualStickEndY = 540 - 21;
    int virtualStickCenterX = virtualStickStartX + (virtualStickEndX - virtualStickStartX) / 2;
    int virtualStickCenterY = virtualStickStartY + (virtualStickEndY - virtualStickStartY) / 2;

    if (input->pointerDown) {
        if (input->pointerX > virtualStickBoundsStartX && input->pointerX < virtualStickBoundsEndX &&
            input->pointerY > virtualStickBoundsStartY && input->pointerY < virtualStickBoundsEndY) 
        {
            int stickX = input->pointerX;
            int stickY = input->pointerY;
            if (stickX < virtualStickStartX) { stickX = virtualStickStartX; }
            if (stickX > virtualStickEndX) { stickX = virtualStickEndX; }
            if (stickY < virtualStickStartY) { stickY = virtualStickStartY; }
            if (stickY > virtualStickEndY) { stickY = virtualStickEndY; }
            direction.x = ((float)stickX - (float)virtualStickCenterX) / 54.0f;
            if (player->mode == PLAYER_SURFACE_MODE_WALL) {
                direction.y = -((float)stickY - (float)virtualStickCenterY) / 54.0f;
                direction.z = 0.0f;
            }
            else {
                direction.z = ((float)stickY - (float)virtualStickCenterY) / 54.0f;
                direction.y = 0.0f;
            }
            direction = normalize(direction);

            *newStickX = (float)stickX - (float)virtualStickCenterX;
            *newStickY = (float)stickY - (float)virtualStickCenterY;
        }
    }

    // QQQ consolidate
    if (input->pointer2Down) {
        if (input->pointer2X > virtualStickBoundsStartX && input->pointer2X < virtualStickBoundsEndX &&
            input->pointer2Y > virtualStickBoundsStartY && input->pointer2Y < virtualStickBoundsEndY) 
        {
            int stickX = input->pointer2X;
            int stickY = input->pointer2Y;
            if (stickX < virtualStickStartX) { stickX = virtualStickStartX; }
            if (stickX > virtualStickEndX) { stickX = virtualStickEndX; }
            if (stickY < virtualStickStartY) { stickY = virtualStickStartY; }
            if (stickY > virtualStickEndY) { stickY = virtualStickEndY; }
            direction.x = ((float)stickX - (float)virtualStickCenterX) / 54.0f;
            if (player->mode == PLAYER_SURFACE_MODE_WALL) {
                direction.y = -((float)stickY - (float)virtualStickCenterY) / 54.0f;
                direction.z = 0.0f;
            }
            else {
                direction.z = ((float)stickY - (float)virtualStickCenterY) / 54.0f;
                direction.y = 0.0f;
            }
            direction = normalize(direction);

            *newStickX = (float)stickX - (float)virtualStickCenterX;
            *newStickY = (float)stickY - (float)virtualStickCenterY;
        }
    }

    return direction;
}
static void debugPlayerMovement (player_state *player, vector3 inputDirection, bool upPressed, matrix4x4 cameraOrientation) {
    const float PLAYER_ACCELERATION = 5.0f;
    const float PLAYER_DECELERATION = 20.0f;
    const float PLAYER_FRICTION = 5.0f;
    const float SLOPE_FACTOR = 7.0f;

    matrix4x4 cameraMatrix = cameraOrientation;
    cameraMatrix.m[3] = 0.0f;
    cameraMatrix.m[7] = 0.0f;
    cameraMatrix.m[11] = 0.0f;
    cameraMatrix = transpose(cameraMatrix);

    // Position
    vector3 groundSpeedDirection = normalize(player->groundSpeed);
    float initialGroundSpeed = length(player->groundSpeed);
    vector3 newGroundSpeedDirection = inputDirection;
    if (length(newGroundSpeedDirection) > 0.0f) {

        newGroundSpeedDirection = cameraMatrix * newGroundSpeedDirection;
        // project direction onto surface plane
        newGroundSpeedDirection = newGroundSpeedDirection - dotProduct(newGroundSpeedDirection, player->slopeDirection) * player->slopeDirection;

        newGroundSpeedDirection = transpose(player->orientation) * newGroundSpeedDirection;
        newGroundSpeedDirection.y = 0.0f;
        newGroundSpeedDirection = normalize(newGroundSpeedDirection);

        vector3 groundSpeedChange = (PLAYER_ACCELERATION * DELTA_TIME) * newGroundSpeedDirection;
        player->groundSpeed += groundSpeedChange;

        vector3 directionDiff = newGroundSpeedDirection - groundSpeedDirection;
        if (length(groundSpeedDirection) > 0.0f && length(directionDiff) > 0.0f) {
            if (length(inputDirection) > 0.0f && initialGroundSpeed > 0.0f) 
            {
                float amountInOppositeDirection = dotProduct(directionDiff, -groundSpeedDirection);
                amountInOppositeDirection *= amountInOppositeDirection;
                if (amountInOppositeDirection > 0.0f) {
                    vector3 decelerationVector = -groundSpeedDirection * amountInOppositeDirection * PLAYER_DECELERATION * DELTA_TIME;
                    decelerationVector *= (length(player->groundSpeed) / MAX_GROUND_SPEED);
                    player->groundSpeed += decelerationVector;
                }
            }
        }
    }

    if (length(inputDirection) == 0.0f && length(player->groundSpeed) > 0.0f) 
    {
        vector3 frictionVector = DELTA_TIME * PLAYER_FRICTION * normalize(player->groundSpeed);
        if (length(player->groundSpeed) < length(frictionVector)) {
            player->groundSpeed = Vector3();
        }
        else {
            player->groundSpeed -= frictionVector;
        }
    }

    if (length(player->groundSpeed) > MAX_GROUND_SPEED) {
        player->groundSpeed = MAX_GROUND_SPEED * normalize(player->groundSpeed);
    }

    // TODO(ebuchholz): fix
    vector3 slopeDiff = player->slopeDirection - Vector3(0.0f, 1.0f, 0.0f);
    if (length(slopeDiff) > EPSILON) {
        //slopeDiff = normalize(slopeDiff);
        float slopeModifier = dotProduct(slopeDiff, Vector3(0.0f, 0.0f, -1.0f));
        vector3 slopeVector= Vector3(0.0f, 0.0f, -1.0f) * slopeModifier * SLOPE_FACTOR * DELTA_TIME;
        slopeVector = transpose(player->orientation) * slopeVector;
        slopeVector.y = 0.0f;
        player->groundSpeed += slopeVector;
        slopeModifier = dotProduct(slopeDiff, Vector3(1.0f, 0.0f, 0.0f));
        slopeVector= Vector3(1.0f, 0.0f, 0.0f) * slopeModifier * SLOPE_FACTOR * DELTA_TIME;
        slopeVector = transpose(player->orientation) * slopeVector;
        slopeVector.y = 0.0f;
        player->groundSpeed += slopeVector;
    }
    //vector3 downhillDirection = crossProduct(player->slopeDirection, Vector3(0.0f, 1.0f, 0.0f));
    //if (length(downhillDirection) > EPSILON) {
    //    downhillDirection = crossProduct(player->slopeDirection, downhillDirection);
    //    player->groundSpeed += downhillDirection * SLOPE_FACTOR * DELTA_TIME;
    //}

    // TODO(ebuchholz): better integration?
    if (length(player->groundSpeed) > 0.0f) {
        vector3 normalizedGroundSpeed = normalize(player->groundSpeed);
        vector3 localSide = normalize(crossProduct(normalizedGroundSpeed, Vector3(0.0f, 1.0f, 0.0)));
        matrix4x4 localRotation = {};
        localRotation.m[0] = localSide.x;
        localRotation.m[1] = localSide.y;
        localRotation.m[2] = localSide.z;
        localRotation.m[3] = 0.0f;
        localRotation.m[4] = 0.0f; 
        localRotation.m[5] = 1.0f;
        localRotation.m[6] = 0.0f;
        localRotation.m[7] = 0.0f;
        localRotation.m[8] = -normalizedGroundSpeed.x;
        localRotation.m[9] = -normalizedGroundSpeed.y;
        localRotation.m[10] = -normalizedGroundSpeed.z;
        localRotation.m[11] = 0.0f;
        localRotation.m[12] = 0.0f;
        localRotation.m[13] = 0.0f;
        localRotation.m[14] = 0.0f;
        localRotation.m[15] = 1.0f;
        // TODO(ebuchholz): better turning

        vector3 localForward = normalize(player->orientation * Vector3(0.0f, 0.0f, -1.0f));
        vector3 newSide = normalize(crossProduct(localForward, player->slopeDirection));
        vector3 newBackward = normalize(crossProduct(newSide, player->slopeDirection));
        matrix4x4 newOrientationMatrix = {};
        newOrientationMatrix.m[0] = newSide.x;
        newOrientationMatrix.m[1] = newSide.y;
        newOrientationMatrix.m[2] = newSide.z;
        newOrientationMatrix.m[3] = 0.0f;
        newOrientationMatrix.m[4] = player->slopeDirection.x;
        newOrientationMatrix.m[5] = player->slopeDirection.y;
        newOrientationMatrix.m[6] = player->slopeDirection.z;
        newOrientationMatrix.m[7] = 0.0f;
        newOrientationMatrix.m[8] = newBackward.x;
        newOrientationMatrix.m[9] = newBackward.y;
        newOrientationMatrix.m[10] = newBackward.z;
        newOrientationMatrix.m[11] = 0.0f;
        newOrientationMatrix.m[12] = 0.0f;
        newOrientationMatrix.m[13] = 0.0f;
        newOrientationMatrix.m[14] = 0.0f;
        newOrientationMatrix.m[15] = 1.0f;

        player->orientation = transpose(localRotation * newOrientationMatrix);
        player->groundSpeed = localRotation * player->groundSpeed;
        
        vector3 rotatedGroundSpeed = player->orientation * player->groundSpeed;

        player->velocity = rotatedGroundSpeed;
        player->pos += player->velocity * DELTA_TIME;

    }

    if (upPressed) {
        player->velocity += player->slopeDirection * 7.0f;
        startPlayerOnNoGround(player);
    }
    //if (input->downButton) {
    //    player->pos.y -= PLAYER_ACCELERATION * DELTA_TIME;
    //}


    float playerRadius = 0.4f;
    float sensorLength = 1.25f;
    player->collisionSensors[0].a = player->pos;
    player->collisionSensors[0].b = player->pos + (-player->upDirection * sensorLength);

    aabb lineABox = {};
    lineABox.min = player->collisionSensors[0].a;
    lineABox.max = player->collisionSensors[0].a;

    aabb lineBBox = {};
    lineBBox.min = player->collisionSensors[0].b;
    lineBBox.max = player->collisionSensors[0].b;

    aabb linesAABB = unionAABB(lineABox, lineBBox);

    if (player->mode == PLAYER_SURFACE_MODE_FLOOR || player->mode == PLAYER_SURFACE_MODE_CEILING) {
        vector3 xVector = Vector3(1.0f, 0.0f, 0.0f);
        vector3 zVector = Vector3(0.0f, 0.0f, 1.0f);
        setupCollisionSensors(player, playerRadius, sensorLength, xVector, zVector, &linesAABB);
    }
    else if (player->mode == PLAYER_SURFACE_MODE_WALL) { 
        vector3 yVector = Vector3(0.0f, 1.0f, 0.0f);
        vector3 sideVector = crossProduct(Vector3(0.0f, 1.0f, 0.0f), player->upDirection);
        setupCollisionSensors(player, playerRadius, sensorLength, yVector, sideVector, &linesAABB);
    }
    setupWallSensors(player, 0.55f, &linesAABB);
    player->boundingBox = linesAABB;
}


static void playerAirMovement (player_state *player, 
                               game_input *input, matrix4x4 cameraOrientation) 
{
    const float PLAYER_AIR_ACCELERATION = 10.0f;
    const float GRAVITY = 10.0f;
    vector3 forward = Vector3(0.0f, 0.0f, -1.0f);
    vector3 up = Vector3(0.0f, 1.0f, 0.0f);
    vector3 side = Vector3(1.0f, 0.0f, 0.0f);

    matrix4x4 cameraMatrix = cameraOrientation;
    cameraMatrix.m[3] = 0.0f;
    cameraMatrix.m[7] = 0.0f;
    cameraMatrix.m[11] = 0.0f;
    cameraMatrix = transpose(cameraMatrix);

    // Position
    vector3 newInputDirection = Vector3();
    if (input->forwardButton) {
        newInputDirection += forward;
    }
    if (input->backButton) {
        newInputDirection -= forward;
    }
    if (input->leftButton) {
        newInputDirection -= side;
    }
    if (input->rightButton) {
        newInputDirection += side;
    }
    if (length(newInputDirection) > 0.0f) {
        newInputDirection = cameraMatrix * newInputDirection;
        // project direction onto xz plane
        newInputDirection = newInputDirection - dotProduct(newInputDirection, up) * up;

        //newInputDirection =player->orientation * newInputDirection;
        //newInputDirection.y = 0.0f;
        newInputDirection = normalize(newInputDirection);

        vector3 speedChange = (PLAYER_AIR_ACCELERATION * DELTA_TIME) * newInputDirection;
        player->velocity += speedChange;
    }
    player->velocity += Vector3(0.0f, -1.0f, 0.0f) * GRAVITY * DELTA_TIME; // gravity
    player->pos += player->velocity * DELTA_TIME;

    float playerRadius = 0.4f;
    float sensorLength = 0.5f; // shorter length when in the air
    player->collisionSensors[0].a = player->pos;
    player->collisionSensors[0].b = player->pos + (-player->upDirection * sensorLength);

    aabb lineABox = {};
    lineABox.min = player->collisionSensors[0].a;
    lineABox.max = player->collisionSensors[0].a;

    aabb lineBBox = {};
    lineBBox.min = player->collisionSensors[0].b;
    lineBBox.max = player->collisionSensors[0].b;

    aabb linesAABB = unionAABB(lineABox, lineBBox);

    vector3 xVector = Vector3(1.0f, 0.0f, 0.0f);
    vector3 zVector = Vector3(0.0f, 0.0f, 1.0f);
    setupCollisionSensors(player, playerRadius, sensorLength, xVector, zVector, &linesAABB);
    setupWallSensors(player, 0.55f, &linesAABB);
    player->boundingBox = linesAABB;
}

static void processPlayerLevelCollisions (player_state *player, level_chunks *levelChunks, level_mesh **levelMeshes, memory_arena *tempMemory) {
    // test mesh triangles for intersection
    aabb playerBB = player->boundingBox;

    // TODO(ebuchholz): better separate floor and wall collisions
    level_chunk_intersection_result *results = 
        (level_chunk_intersection_result *)((char *)tempMemory->base + tempMemory->size);
    int numResults = 0;
    for (int i = 0; i < levelChunks->numChunks; ++i) {
        level_chunk *levelChunk = &levelChunks->chunks[i];
        level_mesh *levelMesh = levelMeshes[levelChunk->levelMeshKey];

        aabb levelMeshBB = levelMesh->boundingBox;
        aabb localPlayerBB = playerBB;
        localPlayerBB.min -= levelChunk->position;
        localPlayerBB.max -= levelChunk->position;

        if (aabbIntersection(localPlayerBB, levelMeshBB)) {
            triangleWithNormals *triangles = levelMesh->triangles;
            aabb *triangleAABBs = levelMesh->triangleAABBs;
            for (int j = 0; j < levelMesh->triangleCount; ++j) {
                float t = -1.0f;
                triangleWithNormals *tri = triangles + j;
                aabb *triangleAABB = triangleAABBs + j;
                if (aabbIntersection(localPlayerBB, *triangleAABB)) {
                    // Wall collision: push player out 
                    for (int k = 0; k < NUM_WALL_SENSORS; ++k ) {
                        line localPlayerSensor = player->wallSensors[k];
                        localPlayerSensor.a -= levelChunk->position;
                        localPlayerSensor.b -= levelChunk->position;

                        vector3 intersectionNormal = Vector3();

                        if (lineSegmentTriangleIntersection(&localPlayerSensor, tri, &t, &intersectionNormal)) {
                            level_chunk_intersection_result *result = 
                                (level_chunk_intersection_result *)allocateMemorySize(tempMemory, sizeof(level_chunk_intersection_result));
                            numResults++;
                            result->intersectionPoint = localPlayerSensor.a + t * (localPlayerSensor.b - localPlayerSensor.a);
                            result->intersectionPoint += levelChunk->position;
                            result->triangleNormal = intersectionNormal;//normalize(crossProduct(tri->p1 - tri->p0, tri->p2 - tri->p0));
                            //result->triangleNormal = normalize(crossProduct(tri->p1 - tri->p0, tri->p2 - tri->p0));
                            result->sensorIndex = k;
                            result->wall = true;
                        }
                    }

                    // Floor collision: put player at the height + orientation of the triangle
                    for (int k = 0; k < NUM_COLLISION_SENSORS; ++k ) {
                        line localPlayerSensor = player->collisionSensors[k];
                        localPlayerSensor.a -= levelChunk->position;
                        localPlayerSensor.b -= levelChunk->position;

                        vector3 intersectionNormal = Vector3();

                        if (lineSegmentTriangleIntersection(&localPlayerSensor, tri, &t, &intersectionNormal)) {
                            if (!player->onGround && dotProduct(player->velocity, intersectionNormal) > 0.0f) { continue; }
                            level_chunk_intersection_result *result = 
                                (level_chunk_intersection_result *)allocateMemorySize(tempMemory, sizeof(level_chunk_intersection_result));
                            numResults++;
                            result->intersectionPoint = localPlayerSensor.a + t * (localPlayerSensor.b - localPlayerSensor.a);
                            result->intersectionPoint += levelChunk->position;
                            result->intersectionPoint += player->upDirection;
                            result->triangleNormal = intersectionNormal;//normalize(crossProduct(tri->p1 - tri->p0, tri->p2 - tri->p0));
                            //result->triangleNormal = normalize(crossProduct(tri->p1 - tri->p0, tri->p2 - tri->p0));
                            result->sensorIndex = k;
                            result->wall = false;
                        }
                    }
                }
            }
        }
    }
    if (numResults > 0) {
        vector3 triangleNormal = Vector3();
        vector3 bestIntersectionPoint = Vector3();
        float bestDotProduct = -1;
        bool floorIntersection = false;
        for (int i = 0; i < numResults; ++i) {
            level_chunk_intersection_result *result = results + i;
            if (result->wall) {
                line playerSensor = player->wallSensors[result->sensorIndex];
                float amountOut = dotProduct(result->intersectionPoint - playerSensor.b, result->triangleNormal);
                // TODO(ebuchholz): eliminate ground speed in the direction of the wall collision
                player->pos += amountOut * result->triangleNormal;

                if (player->onGround) {
                    vector3 localNormal = player->orientation * result->triangleNormal;
                    vector3 correctionVector = localNormal * dotProduct(player->groundSpeed, localNormal);
                    correctionVector.z = -correctionVector.z;
                    player->groundSpeed += correctionVector;
                }
                else {

                }
            }
            else {
                floorIntersection = true;
                line playerSensor = player->collisionSensors[result->sensorIndex];
                float amountUp = dotProduct(result->intersectionPoint - playerSensor.b, player->upDirection);
                if (amountUp > bestDotProduct) {
                    bestIntersectionPoint = result->intersectionPoint;
                    triangleNormal = result->triangleNormal;
                    bestDotProduct = amountUp;
                }
            }
        }
        if (floorIntersection) {
            player->onGround = true;
            player->slopeDirection = triangleNormal;

            vector3 displacementVector = bestIntersectionPoint - player->pos + (0.5f * -player->upDirection);
            vector3 projectedVector = dotProduct(player->upDirection, displacementVector) * player->upDirection;
            player->pos += projectedVector;

            float surfaceAngle = acosf(dotProduct(Vector3(0.0f, 1.0f, 0.0f), triangleNormal)); // should be normalized
            surfaceAngle *= 180.0f / PI;

            while (surfaceAngle > 180.0f) {
                surfaceAngle -= 180.0f;
            }

            if (surfaceAngle < 45.0f) {
                player->mode = PLAYER_SURFACE_MODE_FLOOR;
            }
            else if (surfaceAngle >= 45.0f && surfaceAngle < 135.0f) {
                player->mode = PLAYER_SURFACE_MODE_WALL;
            }
            else if (surfaceAngle >= 135.0f) {
                player->mode = PLAYER_SURFACE_MODE_CEILING;
            }

            switch (player->mode) {
                case PLAYER_SURFACE_MODE_FLOOR: 
                {
                    player->upDirection = Vector3(0.0f, 1.0f, 0.0f);
                } break;
                case PLAYER_SURFACE_MODE_WALL:
                {
                    vector3 wallNormal = triangleNormal;
                    wallNormal.y = 0.0f;
                    wallNormal = normalize(wallNormal);
                    player->upDirection = wallNormal;
                } break;
                case PLAYER_SURFACE_MODE_CEILING:
                {
                    player->upDirection = Vector3(0.0f, -1.0f, 0.0f);
                } break;
            }
        }
    }
    else {
        // falling state
        player->mode = PLAYER_SURFACE_MODE_FLOOR;
        player->upDirection = Vector3(0.0f, 1.0f, 0.0f);
        player->slopeDirection = Vector3(0.0f, 1.0f, 0.0f);
        if (player->onGround) {
            startPlayerOnNoGround(player);
        }
        //player->orientation = identityMatrix4x4();
    }
}

void debugCameraMovement (debug_camera *debugCamera, game_input *input) {
    const float CAMERA_SPEED = 3.0f;
    const float CAMERA_TURN_SPEED = 1.0f;

    // Position
    vector3 moveVector = {};
    if (input->turnUpButton) {
        moveVector.z -= CAMERA_SPEED * DELTA_TIME;
    }
    if (input->turnDownButton) {
        moveVector.z += CAMERA_SPEED * DELTA_TIME;
    }
    if (input->turnLeftButton) {
        moveVector.x -= CAMERA_SPEED * DELTA_TIME;
    }
    if (input->turnRightButton) {
        moveVector.x += CAMERA_SPEED * DELTA_TIME;
    }

    // Rotation
    if (input->pointerJustDown) {
        debugCamera->lastPointerX = input->pointerX;
        debugCamera->lastPointerY = input->pointerY;
    }
    if (input->pointerDown) {
        int pointerDX = input->pointerX - debugCamera->lastPointerX;
        int pointerDY = input->pointerY - debugCamera->lastPointerY;

        float yaw = (float)pointerDX * 0.25f;
        float pitch = (float)pointerDY * 0.25f;

        debugCamera->rotation = quaternionFromAxisAngle(Vector3(0, 1, 0), -yaw * DELTA_TIME) * (debugCamera->rotation);
        debugCamera->rotation = (debugCamera->rotation) * quaternionFromAxisAngle(Vector3(1, 0, 0), -pitch * DELTA_TIME);

        debugCamera->lastPointerX = input->pointerX;
        debugCamera->lastPointerY = input->pointerY;
    }
    // Move in the direction of the current rotation
    moveVector = rotateVectorByQuaternion(moveVector, debugCamera->rotation);
    debugCamera->pos += moveVector;
}

// TODO(ebuchholz: better virtual keyboard
//vector3 inputDirectionFromTouch (game_input *input) {
//    float stickCenterX = 54;
//    float stickCenterY = 540 - 54;
//
//}

static void addLevelChunk (level_chunks *levelChunks, mesh_key meshKey, level_mesh_key levelMeshKey, vector3 position) {
    assert(levelChunks->numChunks < MAX_NUM_LEVEL_CHUNKS);
    level_chunk *levelChunk = &levelChunks->chunks[levelChunks->numChunks];
    levelChunks->numChunks++;
    levelChunk->meshKey = meshKey;
    levelChunk->levelMeshKey = levelMeshKey;
    levelChunk->position = position;
}

void addFlag (game_state *gameState, float x, float y, float z) {
    assert(gameState->numFlags < 300);

    flag *flag = &gameState->flags[gameState->numFlags];
    gameState->numFlags++;

    flag->pos.x = x;
    flag->pos.y = y;
    flag->pos.z = z;

    float flagHalfWidth = 0.5f;
    float flagHeight = 2.0f;
    float flagHalfDepth = 0.15f;

    flag->hitbox.min = flag->pos + Vector3(0.4f + -flagHalfWidth, 0.0f, -flagHalfDepth);
    flag->hitbox.max = flag->pos + Vector3(0.4f + flagHalfWidth, flagHeight, flagHalfDepth);
}

void addSpike (game_state *gameState, float x, float y, float z) {
    assert(gameState->numSpikes < 300);

    spike *spike = &gameState->spikes[gameState->numSpikes];
    gameState->numSpikes++;

    spike->pos.x = x;
    spike->pos.y = y;
    spike->pos.z = z;

    float spikeHalfWidth = 0.4f;
    float spikeHeight = 0.8f;
    float spikeHalfDepth = 0.4f;

    spike->hitbox.min = spike->pos + Vector3(-spikeHalfWidth, 0.0f, -spikeHalfDepth);
    spike->hitbox.max = spike->pos + Vector3(spikeHalfWidth, spikeHeight, spikeHalfDepth);
}

extern "C" void updateGame (game_input *input, game_memory *gameMemory, render_command_list *renderCommands) { 
    game_state *gameState = (game_state *)gameMemory->memory;
    if (!gameState->gameInitialized) {
        gameState->gameInitialized = true;

        debug_camera *debugCamera = &gameState->debugCamera;
        debugCamera->pos = {};
        debugCamera->pos.x = 3.0f;
        debugCamera->pos.y = 3.0f;
        debugCamera->pos.z = 3.0f;
        debugCamera->rotation = 
            quaternionFromAxisAngle(Vector3(0.0f, 1.0f, 0.0f), 45.0f * (PI / 180.0f)) *
            quaternionFromAxisAngle(Vector3(1.0f, 0.0f, 0.0f), -33.0f * (PI / 180.0f));
        debugCamera->lastPointerX = 0;
        debugCamera->lastPointerY = 0;

        // probably unecessary
        player_state *player = &gameState->player;
        player->pos = {};
        player->boundingBox = {};
        player->collisionSensors[0] = {};
        player->collisionSensors[1] = {};
        player->collisionSensors[2] = {};
        player->collisionSensors[3] = {};
        player->collisionSensors[4] = {};
        player->mode = PLAYER_SURFACE_MODE_FLOOR;
        player->upDirection = Vector3(0.0f, 1.0f, 0.0f);
        player->slopeDirection = Vector3(0.0f, 1.0f, 0.0f);
        player->orientation = identityMatrix4x4();
        player->onGround = true;

        gameState->numSpikes = 0;
        gameState->numFlags = 0;
        gameState->numFlagsCollected = 0;

        debugCamera->lookAtTarget = gameState->player.pos;
        debugCamera->up = gameState->player.upDirection;

        // set up the level
        gameState->levelChunks.numChunks = 0;
        addLevelChunk(&gameState->levelChunks, MESH_KEY_GROUND, LEVEL_MESH_KEY_GROUND, Vector3());
        addLevelChunk(&gameState->levelChunks, MESH_KEY_LOOP, LEVEL_MESH_KEY_LOOP, Vector3());
        addLevelChunk(&gameState->levelChunks, MESH_KEY_LOOP_RAMP, LEVEL_MESH_KEY_LOOP_RAMP, Vector3());
        addLevelChunk(&gameState->levelChunks, MESH_KEY_LOOP_RAMP2, LEVEL_MESH_KEY_LOOP_RAMP2, Vector3());
        addLevelChunk(&gameState->levelChunks, MESH_KEY_HILL, LEVEL_MESH_KEY_HILL, Vector3());
        addLevelChunk(&gameState->levelChunks, MESH_KEY_TRICKY_AREA, LEVEL_MESH_KEY_TRICKY_AREA, Vector3());
        addLevelChunk(&gameState->levelChunks, MESH_KEY_CLIFF, LEVEL_MESH_KEY_CLIFF, Vector3());
        addLevelChunk(&gameState->levelChunks, MESH_KEY_TOWERS, LEVEL_MESH_KEY_TOWERS, Vector3());
        addLevelChunk(&gameState->levelChunks, MESH_KEY_UNDER_LOOP, LEVEL_MESH_KEY_UNDER_LOOP, Vector3());

        addSpike(gameState, -37.62f, 15.34f, 24.05f);
        addSpike(gameState, -37.75f, 15.34f, 34.21f);
        addSpike(gameState, -41.83f, 15.341f, 35.80f);
        addSpike(gameState, -39.041f, 15.34f, 38.17f);
        addSpike(gameState, -36.106f, 15.34f, 38.17f);
        addSpike(gameState, -37.75f, 15.34f, 45.33f);
        addSpike(gameState, -39.96f, 15.34f, 46.84f);
        addSpike(gameState, -33.37f, 15.289f, 48.84f);

        addFlag(gameState, -21.366f, -30.964f, 15.248f);
        addFlag(gameState, 8.8649f, 5.53f, -50.875f);
        addFlag(gameState, -1.07f, -2.059f, -58.21f);
        addFlag(gameState, -32.75f, 41.264f, -20.321f);
        addFlag(gameState, -38.836f, 15.263f, 48.421f);
        addFlag(gameState, -16.621f, 52.106f, 39.056f);

        respawnPlayer(player);
    }
    // general purpose temporary storage
    gameState->tempMemory = {};
    gameState->tempMemory.size = 0;
    gameState->tempMemory.capacity = gameMemory->tempMemoryCapacity;
    gameState->tempMemory.base = (char *)gameMemory->tempMemory;

    //debugCameraMovement(&gameState->debugCamera, input);

    vector3 cameraPosTarget = gameState->player.pos + gameState->player.orientation * Vector3(0.0f, 2.0f, 4.0f);
    gameState->debugCamera.pos = gameState->debugCamera.pos + 0.1f * (cameraPosTarget - gameState->debugCamera.pos);

    vector3 lookAtTarget = gameState->player.pos;
    gameState->debugCamera.lookAtTarget = gameState->debugCamera.lookAtTarget + 0.1f * (lookAtTarget - gameState->debugCamera.lookAtTarget);

    vector3 upTarget = gameState->player.upDirection;
    gameState->debugCamera.up = normalize(gameState->debugCamera.up + 0.1f * (upTarget - gameState->debugCamera.up));

    matrix4x4 viewMatrix = createLookAtMatrix(gameState->debugCamera.pos.x, gameState->debugCamera.pos.y,gameState->debugCamera.pos.z,
                                              gameState->debugCamera.lookAtTarget.x, gameState->debugCamera.lookAtTarget.y, gameState->debugCamera.lookAtTarget.z,
                                              gameState->debugCamera.up.x, gameState->debugCamera.up.y, gameState->debugCamera.up.z);

    player_state *player = &gameState->player;
    float stickX = 0.0f; 
    float stickY = 0.0f;
    vector3 inputDirection = getMovementVector(&gameState->player, input, viewMatrix, &stickX, &stickY);
    bool jumpPressed = getJumpButtonPressed(input);
    if (gameState->player.dead) {
        player->deadTimer += DELTA_TIME;
        if (player->deadTimer >= DEAD_LENGTH) {
            respawnPlayer(player);
        }
    }
    if (!gameState->player.dead) {
        if (gameState->player.onGround) {
            debugPlayerMovement(&gameState->player, inputDirection, jumpPressed, viewMatrix);
        }
        else {
            playerAirMovement(&gameState->player, input, viewMatrix);
        }
    }

    if (gameState->player.pos.y < -30.0f) {
        respawnPlayer(&gameState->player);
    }

    // TODO(ebuchholz): get screen dimensions from render commands? and use them
    matrix4x4 projMatrix = createPerspectiveMatrix(0.1f, 1000.0f, (16.0f / 9.0f), 80.0f);
    //matrix4x4 viewMatrix = createViewMatrix(gameState->debugCamera.rotation, 
    //                                        gameState->debugCamera.pos.x,
    //                                        gameState->debugCamera.pos.y,
    //                                        gameState->debugCamera.pos.z);

    render_command_set_camera *setCameraCommand = 
        (render_command_set_camera *)pushRenderCommand(renderCommands,
                                                       RENDER_COMMAND_SET_CAMERA,
                                                       sizeof(render_command_set_camera));
    setCameraCommand->viewMatrix = viewMatrix;
    setCameraCommand->projMatrix = projMatrix;

    //drawLevelMesh(gameState->assets.levelMeshes[LEVEL_MESH_KEY_TEST_GROUND], renderCommands);
    for (int i = 0; i < gameState->levelChunks.numChunks; ++i) {
        level_chunk *levelChunk = &gameState->levelChunks.chunks[i];
        aabb bb = gameState->assets.levelMeshes[levelChunk->levelMeshKey]->boundingBox;
        bb.min += levelChunk->position;
        bb.max += levelChunk->position;
        //drawAABB(&bb, renderCommands);
        matrix4x4 levelPosMatrix = translationMatrix(levelChunk->position.x, levelChunk->position.y, levelChunk->position.z);
        drawModel(levelChunk->meshKey, TEXTURE_KEY_GREEN, levelPosMatrix, renderCommands);
    }

    processPlayerLevelCollisions(&gameState->player, &gameState->levelChunks, gameState->assets.levelMeshes, &gameState->tempMemory);

    matrix4x4 modelMatrix = {};
    if (gameState->player.dead) {
        modelMatrix = translationMatrix(gameState->player.pos.x, gameState->player.pos.y, gameState->player.pos.z) * rotationMatrixFromAxisAngle(Vector3(1.0f, 0.0f, 0.0f), PI * 0.5f) * translationMatrix(0.0f, -0.5f, 0.0f) * scaleMatrix(1.0f);
    }
    else {
        modelMatrix = translationMatrix(gameState->player.pos.x, gameState->player.pos.y, gameState->player.pos.z) * gameState->player.orientation * translationMatrix(0.0f, -0.5f, 0.0f) * scaleMatrix(1.0f);
    }
    drawModel(MESH_KEY_CYLINDER, TEXTURE_KEY_BLUE, modelMatrix, renderCommands);


    for (int i = 0; i < gameState->numSpikes; ++i) {
        spike *spike = &gameState->spikes[i];
        modelMatrix = translationMatrix(spike->pos);
        // QQQ
        //drawAABB(&spike->hitbox, renderCommands);
        drawModel(MESH_KEY_SPIKES, TEXTURE_KEY_WHITE, modelMatrix, renderCommands);

        if (aabbIntersection(spike->hitbox, player->boundingBox)) {
            player->dead = true;
        }
    }

    for (int i = 0; i < gameState->numFlags; ++i) {
        flag *flag = &gameState->flags[i];
        if (flag->collected) { continue; }
        modelMatrix = translationMatrix(flag->pos);
        // QQQ
        //drawAABB(&flag->hitbox, renderCommands);
        drawModel(MESH_KEY_FLAG, TEXTURE_KEY_FLAG, modelMatrix, renderCommands);

        if (aabbIntersection(flag->hitbox, player->boundingBox)) {
            flag->collected = true;
            gameState->numFlagsCollected++;
        }
    }

    viewMatrix = createLookAtMatrix(0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f);

    projMatrix = {};
    projMatrix.m[0] = 2.0f / 960.0f;
    projMatrix.m[1] = 0.0f;
    projMatrix.m[2] = 0.0f;
    projMatrix.m[3] = -1.0f;
    projMatrix.m[4] = 0.0f; 
    projMatrix.m[5] = 2.0f / 540.0f;
    projMatrix.m[6] = 0.0f;
    projMatrix.m[7] = -1.0f;
    projMatrix.m[8] = 0.0f;
    projMatrix.m[9] = 0.0f;
    projMatrix.m[10] = -2.0f / (1000.0f - 0.1f);
    projMatrix.m[11] = -(0.1f + 1000.0f)/(1000.0f - 0.1f);
    projMatrix.m[12] = 0.0f;
    projMatrix.m[13] = 0.0f;
    projMatrix.m[14] = 0.0f;
    projMatrix.m[15] = 1.0f;

    setCameraCommand = (render_command_set_camera *)pushRenderCommand(renderCommands,
                                                                      RENDER_COMMAND_SET_CAMERA,
                                                                      sizeof(render_command_set_camera));
    setCameraCommand->viewMatrix = viewMatrix;
    setCameraCommand->projMatrix = projMatrix;

    modelMatrix = translationMatrix(21.0f + 54.0f + stickX, -25.0f, -(21.0f + 54.0f) + stickY) * scaleMatrix(50.0f);
    drawModel(MESH_KEY_THUMBSTICK, TEXTURE_KEY_BLACK_TEXTURE, modelMatrix, renderCommands);

    matrix4x4 buttonPressedRotation = {};
    if (jumpPressed) {
        buttonPressedRotation = rotationMatrixFromAxisAngle(Vector3(1.0, 0.0f, 0.0f), -0.5f);
    }
    else {
        buttonPressedRotation = identityMatrix4x4();
    }
    modelMatrix = translationMatrix(960.0f - 85.0f, -25.0f, -65.0f) * buttonPressedRotation * scaleMatrix(55.0f);
    drawModel(MESH_KEY_BUTTON, TEXTURE_KEY_BLACK_TEXTURE, modelMatrix, renderCommands);

    if (gameState->numFlagsCollected >= TOTAL_NUM_FLAGS) {
        modelMatrix = translationMatrix(300.0f, -25.0f, -300.0f) * scaleMatrix(55.0f);
        drawModel(MESH_KEY_YOUWIN, TEXTURE_KEY_WHITE, modelMatrix, renderCommands);
    }

    // collision sensors
    //drawAABB(&gameState->player.boundingBox, renderCommands);

    //render_command_lines *lineCommand = startLines(renderCommands);
    //for (int i = 0; i < NUM_COLLISION_SENSORS; ++i) {
    //    line *sensor = &gameState->player.collisionSensors[i];
    //    drawLine(sensor->a.x, sensor->a.y, sensor->a.z, sensor->b.x, sensor->b.y, sensor->b.z, renderCommands, lineCommand);
    //}
    //for (int i = 0; i < NUM_WALL_SENSORS; ++i) {
    //    line *sensor = &gameState->player.wallSensors[i];
    //    drawLine(sensor->a.x, sensor->a.y, sensor->a.z, sensor->b.x, sensor->b.y, sensor->b.z, renderCommands, lineCommand);
    //}

    // orientation
    //vector3 forward = Vector3(0.0f, 0.0f, -1.0f);
    //vector3 up = Vector3(0.0f, 1.0f, 0.0f);
    //vector3 side = Vector3(1.0f, 0.0f, 0.0f);

    //forward = gameState->player.orientation * forward;
    //up = gameState->player.orientation * up;
    //side = gameState->player.orientation * side;

    //lineCommand = startLines(renderCommands);
    //drawLine(gameState->player.pos.x, gameState->player.pos.y, gameState->player.pos.z,
    //         gameState->player.pos.x + side.x, gameState->player.pos.y + side.y, gameState->player.pos.z + side.z,
    //         renderCommands, lineCommand);
    //drawLine(gameState->player.pos.x, gameState->player.pos.y, gameState->player.pos.z,
    //         gameState->player.pos.x + up.x, gameState->player.pos.y + up.y, gameState->player.pos.z + up.z,
    //         renderCommands, lineCommand);
    //drawLine(gameState->player.pos.x, gameState->player.pos.y, gameState->player.pos.z,
    //         gameState->player.pos.x + forward.x, gameState->player.pos.y + forward.y, gameState->player.pos.z + forward.z,
    //         renderCommands, lineCommand);
}
