#include "quickmake_game.h"

#if 0
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
#endif
                                        
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



#if 0

static line *pushLineOntoRenderCommand (render_command_list *renderCommands, render_command_lines *lineCommand) {
    assert(renderCommands->memory.size + sizeof(line) < renderCommands->memory.capacity);

    line *result = (line *)((char *)renderCommands->memory.base + renderCommands->memory.size);
    renderCommands->memory.size += sizeof(line);

    // must finish with the line command before doing other ones
    assert(result == lineCommand->lines + lineCommand->numLines);
    lineCommand->numLines++;

    return result;
}
static void addSprite (sprite newSprite, sprite_list *spriteList) {
    assert(spriteList->numSprites < MAX_SPRITES_PER_FRAME);

    sprite *nextSprite = &spriteList->sprites[spriteList->numSprites];
    ++spriteList->numSprites;
    *nextSprite = newSprite;
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

static void drawAABB (aabb *aabb, render_command_list *renderCommands) {
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

static void debugCameraMovement (debug_camera *debugCamera, game_input *input) {
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
#endif

static void initInstruments (game_sounds *gameSounds) {
    instrument_type instrumentType;
    sound_instrument *instrument;
    sound_envelope *envelope;
    sound_waveform *waveform;

    instrumentType = INSTRUMENT_TYPE_PIANO;
    instrument = &gameSounds->instruments[instrumentType];
    instrument->type = instrumentType;
    envelope = &instrument->envelope;
    envelope->attackTime = 0.01f;
    envelope->attackVolume = 0.5f;
    envelope->decayTime = 0.07f;
    envelope->sustain = false;
    envelope->sustainVolume = 0.35f;
    envelope->releaseTime = 0.6f;
    waveform = &instrument->waveforms[0];
    waveform->waveType = OSCILLATOR_TYPE_SINE;
    waveform->volume = 0.6f;
    waveform->muliplier = 1.0f;
    waveform = &instrument->waveforms[1];
    waveform->waveType = OSCILLATOR_TYPE_SINE;
    waveform->volume = 0.3f;
    waveform->muliplier = 2.0f;
    waveform = &instrument->waveforms[2];
    waveform->waveType = OSCILLATOR_TYPE_SINE;
    waveform->volume = 0.1f;
    waveform->muliplier = 3.0f;
    instrument->numWaveForms = 3;
}

//static void growTransformGroupEntryList () {
//
//}

//static transform_group *addTransformGroup (transform_group_list *groupList, memory_arena *tempMemory) {
//    assert(groupList->numGroups < MAX_TRANSFORM_GROUPS);
//
//    transform_group *group = &groupList->groups[groupList->numGroups];
//    *group = {};
//    ++groupList->numGroups;
//
//    group->children.length = 0;
//    group->children.capacity = 16;
//    group->children.entries = (transform_group_entry *)allocateMemorySize(tempMemory, group->children.capacity * sizeof(transform_group_entry)); 
//
//    return group;
//}

//static void addChild () {
//
//}

// TODO(ebuchholz): Maybe pack everything into a single file and load that?
extern "C" void getGameAssetList (asset_list *assetList) {
    pushAsset(assetList, "assets/meshes/cube.obj", ASSET_TYPE_OBJ, MESH_KEY_CUBE);

    pushAsset(assetList, "assets/textures/atlas.txt", ASSET_TYPE_ATLAS, ATLAS_KEY_GAME, TEXTURE_KEY_GAME_ATLAS);
}

extern "C" void parseGameAsset (void *assetData, void *secondAssetData, asset_type type, int key, int secondKey,
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

        gameState->sineT = 0.0f;
    } 
    // parse data
    // create/copy stuff into game memory
    // place whatever theplatform needs at the beginningof working memory
    switch (type) {
    default:
        assert(false); // must provide a valid type
        break;
    case ASSET_TYPE_OBJ:
        parseOBJ(assetData, &gameState->assets, key, workingMemory);
        break;
    case ASSET_TYPE_BMP:
        parseBitmap(assetData, &gameState->assets, key, workingMemory);
        break;
    case ASSET_TYPE_ATLAS:
        parseBitmap(secondAssetData, &gameState->assets, secondKey, workingMemory);
        parseAtlas(assetData, &gameState->assets, key, secondKey, workingMemory);
        break;
    }
}

extern "C" void updateGame (game_input *input, game_memory *gameMemory, render_command_list *renderCommands) { 
    game_state *gameState = (game_state *)gameMemory->memory;
    if (!gameState->gameInitialized) {
        gameState->gameInitialized = true;
        gameState->sounds = {};
        initInstruments(&gameState->sounds);

        //initBlockGame(&gameState->memory, &gameState->blockGame);
        initPianoGame(&gameState->pianoGame);
    }
    // general purpose temporary storage
    gameState->tempMemory = {};
    gameState->tempMemory.size = 0;
    gameState->tempMemory.capacity = gameMemory->tempMemoryCapacity;
    gameState->tempMemory.base = (char *)gameMemory->tempMemory;

    // Zero memory here? since uint8array.fill is slow in firefix

    sprite_list spriteList = {};
    spriteList.sprites = (sprite *)allocateMemorySize(&gameState->tempMemory, sizeof(sprite) * MAX_SPRITES_PER_FRAME);
    spriteList.numSprites = 0;
    spriteList.matrixStack[0] = identityMatrix3x3();

    //transform_group_list groupList;
    //groupList.groups = (transform_group *)allocateMemorySize(&gameState->tempMemory, sizeof(transform_group) * MAX_TRANSFORM_GROUPS);
    //groupList.numGroups = 0;

    //transform_group *rootGroup = addTransformGroup(&groupList, &gameState->tempMemory);

    // TODO(ebuchholz): get screen dimensions from render commands? and use them
    float screenWidth = (float)renderCommands->windowWidth;
    float screenHeight = (float)renderCommands->windowHeight;

    float normalAspectRatio = GAME_WIDTH / GAME_HEIGHT;
    float actualAspectRatio = screenWidth / screenHeight;
    float gameScale;
    vector2 gameOrigin;
    if (actualAspectRatio < normalAspectRatio) {
        float widthRatio = screenWidth / GAME_WIDTH;
        gameScale = widthRatio;
        gameOrigin.x = 0.0f;
        gameOrigin.y = (screenHeight - (GAME_HEIGHT * gameScale)) / 2.0f;
    }
    else {
		float heightRatio = screenHeight / GAME_HEIGHT;
        gameScale = heightRatio;
        gameOrigin.x = (screenWidth - (GAME_WIDTH * gameScale)) / 2.0f;
        gameOrigin.y = 0.0f;
    }

    //gameState->visualizationT += DELTA_TIME;
    //render_command_background_visualization *visualizationCommand = 
    //    (render_command_background_visualization *)pushRenderCommand(renderCommands, 
    //                                                                 RENDER_COMMAND_BACKGROUND_VISUALIZATION,
    //                                                                 sizeof(render_command_background_visualization));
    //visualizationCommand->t = gameState->visualizationT;

    pushSpriteTransform(&spriteList, gameOrigin, gameScale);
    //updateBlockGame(&gameState->memory, &gameState->tempMemory, &gameState->assets, input, &gameState->blockGame, &spriteList);
    updatePianoGame(&gameState->sounds, &gameState->assets, input, &gameState->pianoGame, &spriteList);
    popSpriteMatrix(&spriteList);

    render_command_sprite_list *spriteListCommand = 
        (render_command_sprite_list *)pushRenderCommand(renderCommands,
                                                 RENDER_COMMAND_SPRITE_LIST,
                                                 sizeof(render_command_sprite_list));
    spriteListCommand->numSprites = spriteList.numSprites;
    spriteListCommand->sprites = (render_sprite *)allocateMemorySize(&renderCommands->memory, 
                                                                     sizeof(render_sprite) * spriteListCommand->numSprites);

    matrix3x3 spriteTransform;
    for (int i = 0; i < spriteList.numSprites; ++i) {
        sprite *sprite = &spriteList.sprites[i];

        // TODO(ebuchholz): base case for untransformed sprites (common case?)
        render_sprite *renderSprite = &spriteListCommand->sprites[i];
        spriteTransform = identityMatrix3x3();
        spriteTransform.m[2] = -sprite->anchor.x;
        spriteTransform.m[5] = -sprite->anchor.y;

        float scaledWidth = sprite->scale * sprite->width;
        float scaledHeight = sprite->scale * sprite->height;
        matrix3x3 scaleMatrix = scaleMatrix3x3(scaledWidth, scaledHeight);
        spriteTransform = scaleMatrix * spriteTransform;
        spriteTransform = rotationMatrix3x3(sprite->rotation) * spriteTransform;
        spriteTransform = translationMatrix(sprite->pos.x, sprite->pos.y) * spriteTransform;

        spriteTransform = sprite->parentTransform * spriteTransform;

        renderSprite->pos[0] = spriteTransform * Vector2(0.0f, 0.0f);
        renderSprite->pos[1] = spriteTransform * Vector2(1.0f, 0.0f);
        renderSprite->pos[2] = spriteTransform * Vector2(0.0f, 1.0f);
        renderSprite->pos[3] = spriteTransform * Vector2(1.0f, 1.0f);

        // determine uvs for texture atlases
        renderSprite->texCoord[0] = sprite->frameCorners[0];
        renderSprite->texCoord[1] = sprite->frameCorners[1];
        renderSprite->texCoord[2] = sprite->frameCorners[2];
        renderSprite->texCoord[3] = sprite->frameCorners[3];

        float red = (float)((sprite->tint >> 16) & (0xff)) / 255.0f;
        float green = (float)((sprite->tint >> 8) & (0xff)) / 255.0f;
        float blue = (float)((sprite->tint) & (0xff)) / 255.0f;

        vector4 color;
        color.r = red;
        color.g = green;
        color.b = blue;
        color.a = sprite->alpha;
        renderSprite->color[0] = color;
        renderSprite->color[1] = color;
        renderSprite->color[2] = color;
        renderSprite->color[3] = color;

        renderSprite->textureKey = sprite->textureKey;
    }
}

extern "C" void getGameSoundSamples (game_memory *gameMemory, game_sound_output *soundOutput) { 
    game_state *gameState = (game_state *)gameMemory->memory;
    game_sounds *gameSounds = &gameState->sounds;

    float volume = 1.0f;
    float dt = 1.0f / (float)soundOutput->samplesPerSecond;
    
    sound_sample *sampleOut = soundOutput->samples;
    for (int i = 0; i < soundOutput->sampleCount; ++i) 
    {
        float sampleValue = 0.0f;
        for (int soundIndex = 0; soundIndex < MAX_SOUNDS; ++soundIndex) {
            synth_sound *sound = gameSounds->sounds + soundIndex;
            sound_instrument *instrument = gameSounds->instruments + sound->instrumentType;
            if (sound->active) {
                sampleValue += updateSound(sound, instrument, dt);
            }
        }
        // clamp between -1 and 1
        sampleValue = sampleValue > 1.0f ? 1.0f : sampleValue;
        sampleValue = sampleValue < -1.0f ? -1.0f : sampleValue;
        sampleOut->value = sampleValue * volume;
        assert(sampleOut->value >= -1.0f && sampleOut->value <= 1.0f);
        ++sampleOut;
    }
}
