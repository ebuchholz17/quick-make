#include "hitbox_editor.h"

void initHitboxEditor (memory_arena *memory, game_assets *assets, hitbox_editor* editor) {
    *editor = {};
    editor->currentAtlasIndex = -1;

    for (int i = 0; i < 50; ++i) {
        editor->frameAtlasIndices[i] = -1;
    }

    // convert game atlas to list of frames
    atlas_asset *gameAtlas = assets->atlases[ATLAS_KEY_GAME];
    for (int i = 0; i < 500; ++i) {
        atlas_frame *frame = &gameAtlas->map.entries[i];
        if (frame->key != 0) {
            editor->atlasIndices[editor->numAtlasIndices] = i;
            editor->numAtlasIndices++;
        }
    }
}

bool doButton (float x, float y, float width, float height, bool pointerJustDown, float pointerX, float pointerY, 
               game_assets *assets, atlas_key atlasKey, char *frame, sprite_list *spriteList) 
{
    bool inBox = false;
    if (pointerX >= x && pointerX < x + width && pointerY >= y && pointerY < y + height) {
        inBox = true;
    }

    unsigned int tint = 0xffffff;
    bool clicked = false;
    if (inBox) {
        if (pointerJustDown) {
            clicked = true;
            tint = 0xbbbbbb;
        }
        else {
            tint = 0xdddddd;
        }
    }
    addSprite(x, y, assets, atlasKey, frame, spriteList, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, tint);

    return clicked;
}

bool doLabelButton (float x, float y, float width, float height, char *label, bool pointerJustDown, float pointerX, float pointerY, 
                    game_assets *assets, atlas_key atlasKey, char *frame, sprite_list *spriteList) 
{
    bool result = doButton(x, y, width, height, pointerJustDown, pointerX, pointerY, 
                           assets, atlasKey,frame, spriteList);

    addText(x, y + 4, label, assets, TEXTURE_KEY_FONT, spriteList);
    return result;
}

void drawBox (sprite_list *spriteList, float originX, float originY, rectangle box, game_assets *assets, char *boxKey, int additionalOffsetX, int additionalOffsetY) {
        pushSpriteTransform(spriteList, Vector2((float)additionalOffsetX, (float)additionalOffsetY));
        pushSpriteTransform(spriteList, Vector2(originX, originY));
        pushSpriteTransform(spriteList, box.min);
        vector2 rectDims = box.max - box.min;
        vector2 rectScale = rectDims * (1.0f / 10.0f);
        matrix3x3 boxTransform = scaleMatrix3x3(rectScale.x, rectScale.y);

        pushSpriteMatrix(boxTransform, spriteList);

        addSprite(0.0f, 0.0f, assets, ATLAS_KEY_HITBOX_EDITOR, boxKey, spriteList, 0.0f, 0.0f, 1.0f, 0.0f, 0.3f);

        popSpriteMatrix(spriteList);
        popSpriteMatrix(spriteList);
        popSpriteMatrix(spriteList);
        popSpriteMatrix(spriteList);
}

void loadFileIntoHitboxEditor (hitbox_editor *editor, game_assets *assets, void *fileData, unsigned int fileSize) {
    char *start;
    char *end;
    char *nextLineStart;
    char *file = (char *)fileData;

    readLine(file, &start, &end, &nextLineStart);

    int numFrames = stringToInt(start, end-1);
    start = nextLineStart;
    //editor->currentDuration = numFrames;

    for (int i = 0; i < numFrames; ++i) {
        character_frame_data *frameData = &editor->animationData.frames[i];

        char frameName[255] = {};
        readLine(start, &start, &end, &nextLineStart);
        char *frameNameStart = start;
        int numLettersInFrameName = 0;
        while (frameNameStart != end) {
            frameName[numLettersInFrameName] = *frameNameStart;
            ++numLettersInFrameName;
            ++frameNameStart;
        }
        frameName[numLettersInFrameName] = 0;

        atlas_frame *frame = getAtlasFrame(assets, ATLAS_KEY_GAME, frameName);
        frameData->frameKey = frame->key;
        int mapIndex = (int)getAtlasFrameIndex(assets, ATLAS_KEY_GAME, frameName);
        for (int j = 0; j < 500; ++j) {
            if (editor->atlasIndices[j] == mapIndex) {
                editor->frameAtlasIndices[i] = j;
                break;
            }
        }
        start = nextLineStart;

        readLine(start, &start, &end, &nextLineStart);
        int offsetX = stringToInt(start, end-1);
        frameData->xOffset = offsetX;
        start = nextLineStart;

        readLine(start, &start, &end, &nextLineStart);
        int offsetY = stringToInt(start, end-1);
        frameData->yOffset = offsetY;
        start = nextLineStart;

        readLine(start, &start, &end, &nextLineStart);
        int duration = stringToInt(start, end-1);
        frameData->duration = duration;
        start = nextLineStart;

        readLine(start, &start, &end, &nextLineStart);
        int numHitboxes = stringToInt(start, end-1);
        frameData->numHitboxes = numHitboxes;
        start = nextLineStart;

        for (int hitboxIndex = 0; hitboxIndex < numHitboxes; ++hitboxIndex) {
            rectangle hitbox = {};

            readLine(start, &start, &end, &nextLineStart);
            int minX = stringToInt(start, end-1);
            hitbox.min.x = (float)minX;
            start = nextLineStart;

            readLine(start, &start, &end, &nextLineStart);
            int minY = stringToInt(start, end-1);
            hitbox.min.y = (float)minY;
            start = nextLineStart;
            
            readLine(start, &start, &end, &nextLineStart);
            int maxX = stringToInt(start, end-1);
            hitbox.max.x = (float)maxX;
            start = nextLineStart;

            readLine(start, &start, &end, &nextLineStart);
            int maxY = stringToInt(start, end-1);
            hitbox.max.y = (float)maxY;
            start = nextLineStart;

            frameData->hitboxes[hitboxIndex] = hitbox;
        }

        readLine(start, &start, &end, &nextLineStart);
        int numHurtboxes = stringToInt(start, end-1);
        frameData->numHurtboxes = numHurtboxes;
        start = nextLineStart;

        for (int hurtboxIndex = 0; hurtboxIndex < numHurtboxes; ++hurtboxIndex) {
            rectangle hurtbox = {};

            readLine(start, &start, &end, &nextLineStart);
            int minX = stringToInt(start, end-1);
            hurtbox.min.x = (float)minX;
            start = nextLineStart;

            readLine(start, &start, &end, &nextLineStart);
            int minY = stringToInt(start, end-1);
            hurtbox.min.y = (float)minY;
            start = nextLineStart;
            
            readLine(start, &start, &end, &nextLineStart);
            int maxX = stringToInt(start, end-1);
            hurtbox.max.x = (float)maxX;
            start = nextLineStart;

            readLine(start, &start, &end, &nextLineStart);
            int maxY = stringToInt(start, end-1);
            hurtbox.max.y = (float)maxY;
            start = nextLineStart;

            frameData->hurtboxes[hurtboxIndex] = hurtbox;
        }
    }

    if (numFrames > 0) {
        editor->currentFrame = 0;
        character_frame_data *newFrameData = &editor->animationData.frames[0];
        editor->offsetX = newFrameData->xOffset;
        editor->offsetY = newFrameData->yOffset;
        editor->numCurrentHitboxes = newFrameData->numHitboxes;
        for (int i = 0; i < editor->numCurrentHitboxes; ++i) {
            editor->hitboxes[i] = newFrameData->hitboxes[i];
        }
        editor->numCurrentHurtboxes = newFrameData->numHurtboxes;
        for (int i = 0; i < editor->numCurrentHurtboxes; ++i) {
            editor->hurtboxes[i] = newFrameData->hurtboxes[i];
        }
        editor->currentAtlasIndex = editor->frameAtlasIndices[editor->currentFrame];
        editor->currentDuration = newFrameData->duration;

    }
}

void prepareHitboxDataToSave (hitbox_editor *editor, memory_arena *memory, 
                              void **fileDataPointer, unsigned int *fileSizePointer) 
{
    // assume 20KB max size for simplicity
    void *file = allocateMemorySize(memory, 20 * 1024);
    char *fileCursor = (char *)file;

    int numFrames = 0;
    for (int i = 0; i < 50; ++i) {
        character_frame_data *frameData = &editor->animationData.frames[i];
        if (frameData->frameKey == 0 || stringsAreEqual("none", frameData->frameKey)) {
            break;
        } 
        else {
            ++numFrames;
        }
    }

    fileCursor = writeNumToBuffer(numFrames, fileCursor);
    fileCursor = writeStringToBuffer("\n", fileCursor);

    for (int i = 0; i < numFrames; ++i) {
        character_frame_data *frameData = &editor->animationData.frames[i];

        fileCursor = writeStringToBuffer(frameData->frameKey, fileCursor);
        fileCursor = writeStringToBuffer("\n", fileCursor);

        fileCursor = writeNumToBuffer(frameData->xOffset, fileCursor);
        fileCursor = writeStringToBuffer("\n", fileCursor);

        fileCursor = writeNumToBuffer(frameData->yOffset, fileCursor);
        fileCursor = writeStringToBuffer("\n", fileCursor);

        fileCursor = writeNumToBuffer(frameData->duration, fileCursor);
        fileCursor = writeStringToBuffer("\n", fileCursor);

        fileCursor = writeNumToBuffer(frameData->numHitboxes, fileCursor);
        fileCursor = writeStringToBuffer("\n", fileCursor);

        for (int hitboxIndex = 0; hitboxIndex < frameData->numHitboxes; ++hitboxIndex) {
            rectangle hitbox = frameData->hitboxes[hitboxIndex];

            fileCursor = writeNumToBuffer((int)hitbox.min.x, fileCursor);
            fileCursor = writeStringToBuffer("\n", fileCursor);

            fileCursor = writeNumToBuffer((int)hitbox.min.y, fileCursor);
            fileCursor = writeStringToBuffer("\n", fileCursor);

            fileCursor = writeNumToBuffer((int)hitbox.max.x, fileCursor);
            fileCursor = writeStringToBuffer("\n", fileCursor);

            fileCursor = writeNumToBuffer((int)hitbox.max.y, fileCursor);
            fileCursor = writeStringToBuffer("\n", fileCursor);
        }

        fileCursor = writeNumToBuffer(frameData->numHurtboxes, fileCursor);
        fileCursor = writeStringToBuffer("\n", fileCursor);

        for (int hurtboxIndex = 0; hurtboxIndex < frameData->numHurtboxes; ++hurtboxIndex) {
            rectangle hurtbox = frameData->hurtboxes[hurtboxIndex];

            fileCursor = writeNumToBuffer((int)hurtbox.min.x, fileCursor);
            fileCursor = writeStringToBuffer("\n", fileCursor);

            fileCursor = writeNumToBuffer((int)hurtbox.min.y, fileCursor);
            fileCursor = writeStringToBuffer("\n", fileCursor);

            fileCursor = writeNumToBuffer((int)hurtbox.max.x, fileCursor);
            fileCursor = writeStringToBuffer("\n", fileCursor);

            fileCursor = writeNumToBuffer((int)hurtbox.max.y, fileCursor);
            fileCursor = writeStringToBuffer("\n", fileCursor);
        }
    }

    *fileDataPointer = (void *)file;
    *fileSizePointer = (unsigned int)(fileCursor - (char *)file);
}

void updateHitboxEditor (memory_arena *memory, memory_arena *tempMemory, game_assets *assets, game_input *input, 
                         sprite_list *spriteList, hitbox_editor *editor)
{
    // memory for dynamically created strings
    memory_arena stringMemory = {};
    stringMemory.capacity = 512 * 1024;
    stringMemory.base = allocateMemorySize(tempMemory, stringMemory.capacity);

    matrix3x3 gameTransform = peekSpriteMatrix(spriteList);
    vector3 localPointerPos = Vector3((float)input->pointerX, (float)input->pointerY, 1.0f);
    localPointerPos = inverse(gameTransform) * localPointerPos;

    // zoom out a bit
    //pushSpriteTransform(spriteList, Vector2(GAME_WIDTH/2.0f, GAME_HEIGHT/2.0f), 0.5f);

    bool buttonPressed = false;
    editor->requestFileLoad = false;
    if (doLabelButton(6, 3, 32, 16, "load", input->pointerJustDown, localPointerPos.x, localPointerPos.y,
                 assets, ATLAS_KEY_HITBOX_EDITOR, "editor_button_backing", spriteList)) 
    {
        buttonPressed = true;
        // load handling
        editor->requestFileLoad = true;
    }
    editor->requestFileSave = false;
    if (doLabelButton(6, 22, 32, 16, "save", input->pointerJustDown, localPointerPos.x, localPointerPos.y,
                 assets, ATLAS_KEY_HITBOX_EDITOR, "editor_button_backing", spriteList)) 
    {
        buttonPressed = true;
        // save handling
        editor->requestFileSave = true;
        void *fileData = 0;
        unsigned int fileSize = 0;
        prepareHitboxDataToSave(editor, tempMemory, &fileData, &fileSize);

        editor->fileToSaveData = fileData;
        editor->fileToSaveSize = fileSize;
    }
    if (doButton(64, 3, 16, 16, input->pointerJustDown, localPointerPos.x, localPointerPos.y,
                 assets, ATLAS_KEY_HITBOX_EDITOR, "arrow_up_button", spriteList)) 
    {
        buttonPressed = true;
        if (editor->currentAtlasIndex >= 0) {
            editor->currentAtlasIndex--;
        }
    }
    if (doButton(64, 22, 16, 16, input->pointerJustDown, localPointerPos.x, localPointerPos.y,
                 assets, ATLAS_KEY_HITBOX_EDITOR, "arrow_down_button", spriteList)) 
    {
        buttonPressed = true;
        if (editor->currentAtlasIndex < editor->numAtlasIndices - 1) {
            editor->currentAtlasIndex++;
        }
    }

    if (doButton(264, 3, 16, 16, input->pointerJustDown, localPointerPos.x, localPointerPos.y,
                 assets, ATLAS_KEY_HITBOX_EDITOR, "arrow_up_button", spriteList)) 
    {
        buttonPressed = true;
        editor->currentDuration++;
    }
    if (doButton(264, 22, 16, 16, input->pointerJustDown, localPointerPos.x, localPointerPos.y,
                 assets, ATLAS_KEY_HITBOX_EDITOR, "arrow_down_button", spriteList)) 
    {
        buttonPressed = true;
        if (editor->currentDuration > 0) {
            editor->currentDuration--;
        }
    }
    addText(285, 16, appendString(numToString(editor->currentDuration, &stringMemory), " frames", &stringMemory), assets, TEXTURE_KEY_FONT, spriteList);

    bool loadFrame = false;
    int nextFrame = 0;
    if (doButton(160, 187, 16, 16, input->pointerJustDown, localPointerPos.x, localPointerPos.y,
                 assets, ATLAS_KEY_HITBOX_EDITOR, "arrow_left_button", spriteList)) 
    {
        buttonPressed = true;
        if (editor->currentFrame > 0) {
            nextFrame = editor->currentFrame - 1;
            loadFrame = true;
        }
        // prev frame handler
    }

    if (doButton(205, 187, 16, 16, input->pointerJustDown, localPointerPos.x, localPointerPos.y,
                 assets, ATLAS_KEY_HITBOX_EDITOR, "arrow_right_button", spriteList)) 
    {
        buttonPressed = true;
        // next frame handler
        if (editor->currentFrame < 50 - 1) {
            nextFrame = editor->currentFrame + 1;
            loadFrame = true;
        }
    }

    bool startPlayback = false;
    if (doButton(237, 187, 32, 16, input->pointerJustDown, localPointerPos.x, localPointerPos.y,
                 assets, ATLAS_KEY_HITBOX_EDITOR, "play_button", spriteList)) 
    {
        buttonPressed = true;
        startPlayback = true;
    }
    if (input->yKey.justPressed) {
        editor->loop = !editor->loop;
    }
    if (editor->loop) {
        addText(239, 205, "loop", assets, TEXTURE_KEY_FONT, spriteList);
    }
    bool duplicate = false;
    if (doLabelButton(285, 187, 32, 16, "dup.", input->pointerJustDown, localPointerPos.x, localPointerPos.y,
                 assets, ATLAS_KEY_HITBOX_EDITOR, "editor_button_backing", spriteList)) 
    {
        buttonPressed = true;
        duplicate = true;
    }
    bool remove = false;
    if (doLabelButton(333, 187, 32, 16, "rem.", input->pointerJustDown, localPointerPos.x, localPointerPos.y,
                 assets, ATLAS_KEY_HITBOX_EDITOR, "editor_button_backing", spriteList)) 
    {
        buttonPressed = true;
        remove = true;
    }

    int offsetX = editor->offsetX;
    int offsetY = editor->offsetY;
 
    float originX = 190;
    float originY = 170;

    int additionalOffsetX = 0;
    int additionalOffsetY = 0;

    rectangle currentEditingBox = {};
    switch (editor->state) {
        case HITBOX_EDITOR_STATE_NORMAL: {
            if (!buttonPressed && input->pointerJustDown) {
                editor->dragStartX = (int)localPointerPos.x;
                editor->dragStartY = (int)localPointerPos.y;
                editor->state = HITBOX_EDITOR_STATE_DRAGGING_SPRITE;
            }
            else if (input->aKey.justPressed && editor->numCurrentHurtboxes < MAX_NUM_HURTBOXES) {
                editor->dragStartX =  (int)localPointerPos.x - (int)originX;
                editor->dragStartY =  (int)localPointerPos.y - (int)originY;
                editor->boxTypeIsHitbox = false;
                editor->state = HITBOX_EDITOR_STATE_DRAGGING_BOX;
            }
            else if (input->sKey.justPressed && editor->numCurrentHitboxes < MAX_NUM_HITBOXES) {
                editor->dragStartX = (int)localPointerPos.x - (int)originX; 
                editor->dragStartY = (int)localPointerPos.y - (int)originY; 
                editor->boxTypeIsHitbox = true;
                editor->state = HITBOX_EDITOR_STATE_DRAGGING_BOX;
            }
            else if (input->dKey.justPressed) {
                bool deleted = false;
                vector2 boxLocalPoint = Vector2(localPointerPos.x - originX, 
                                                localPointerPos.y - originY);
                for (int i = 0; i < editor->numCurrentHitboxes; ++i) {
                    rectangle currentBox = editor->hitboxes[i];
                    if (rectangleContainsPoint(currentBox, boxLocalPoint.x, boxLocalPoint.y)) {
                        deleted = true;
                        editor->hitboxes[i] = editor->hitboxes[editor->numCurrentHitboxes - 1];
                        --editor->numCurrentHitboxes;
                        break;
                    }
                }
                if (!deleted) {
                    for (int i = 0; i < editor->numCurrentHurtboxes; ++i) {
                        rectangle currentBox = editor->hurtboxes[i];
                        if (rectangleContainsPoint(currentBox, boxLocalPoint.x, boxLocalPoint.y)) {
                            deleted = true;
                            editor->hurtboxes[i] = editor->hurtboxes[editor->numCurrentHurtboxes - 1];
                            --editor->numCurrentHurtboxes;
                            break;
                        }
                    }
                }

            }
        } break;
        case HITBOX_EDITOR_STATE_DRAGGING_SPRITE: {
            additionalOffsetX = (int)localPointerPos.x - editor->dragStartX;
            additionalOffsetY = (int)localPointerPos.y - editor->dragStartY;
            offsetX += additionalOffsetX;
            offsetY += additionalOffsetY;
            if (!input->pointerDown) {
                editor->offsetX = offsetX;
                editor->offsetY = offsetY;

                vector2 newOffset = Vector2((float)additionalOffsetX, (float)additionalOffsetY);
                // shift hitboxes over by the new offset
                for (int i = 0; i < editor->numCurrentHitboxes; ++i) {
                    rectangle *box = &editor->hitboxes[i];
                    box->min += newOffset;
                    box->max += newOffset;
                }
                for (int i = 0; i < editor->numCurrentHurtboxes; ++i) {
                    rectangle *box = &editor->hurtboxes[i];
                    box->min += newOffset;
                    box->max += newOffset;
                }
                additionalOffsetX = 0;
                additionalOffsetY = 0;

                editor->state = HITBOX_EDITOR_STATE_NORMAL;
            }
        } break;
        case HITBOX_EDITOR_STATE_DRAGGING_BOX: {
            int otherCornerX = (int)localPointerPos.x - (int)originX; 
            int otherCornerY = (int)localPointerPos.y - (int)originY; 
            int minX = otherCornerX <= editor->dragStartX ? otherCornerX : editor->dragStartX;
            int maxX = otherCornerX >= editor->dragStartX ? otherCornerX : editor->dragStartX;
            int minY = otherCornerY <= editor->dragStartY ? otherCornerY : editor->dragStartY;
            int maxY = otherCornerY >= editor->dragStartY ? otherCornerY : editor->dragStartY;

            addText(6, 100, appendString("minX: ", numToString(minX, &stringMemory), &stringMemory), assets, TEXTURE_KEY_FONT, spriteList);
            addText(6, 110, appendString("minY: ", numToString(minY, &stringMemory), &stringMemory), assets, TEXTURE_KEY_FONT, spriteList);
            addText(6, 120, appendString("maxX: ", numToString(maxX, &stringMemory), &stringMemory), assets, TEXTURE_KEY_FONT, spriteList);
            addText(6, 130, appendString("maxY: ", numToString(maxY, &stringMemory), &stringMemory), assets, TEXTURE_KEY_FONT, spriteList);

            currentEditingBox.min = Vector2((float)minX, (float)minY);
            currentEditingBox.max = Vector2((float)maxX, (float)maxY);

            if (input->pointerJustDown) {
                if (currentEditingBox.max.x - currentEditingBox.min.x > 0 &&
                    currentEditingBox.max.y - currentEditingBox.min.y > 0) 
                {
                    // save box
                    rectangle *box;
                    if (editor->boxTypeIsHitbox) {
                        box = &editor->hitboxes[editor->numCurrentHitboxes];
                        ++editor->numCurrentHitboxes;
                    }
                    else {
                        box = &editor->hurtboxes[editor->numCurrentHurtboxes];
                        ++editor->numCurrentHurtboxes;
                    }
                    *box = currentEditingBox;
                }
                editor->state = HITBOX_EDITOR_STATE_NORMAL;
            }
        } break;
    }

    char *currentSpriteText;
    atlas_asset *gameAtlas = assets->atlases[ATLAS_KEY_GAME];
    atlas_frame *frame;
    if (editor->currentAtlasIndex >= 0) {
        frame = &gameAtlas->map.entries[editor->atlasIndices[editor->currentAtlasIndex]];
        currentSpriteText = frame->key;
        addSprite(originX + offsetX, originY + offsetY, assets, ATLAS_KEY_GAME, frame->key, spriteList, 0.0f, 1.0f);
    }
    else {
        currentSpriteText = "none";
    }
    addText(84, 16, currentSpriteText, assets, TEXTURE_KEY_FONT, spriteList);

    addText(6, 160, appendString("X: ", numToString((int)localPointerPos.x - (int)originX, &stringMemory), &stringMemory), assets, TEXTURE_KEY_FONT, spriteList);
    addText(6, 170, appendString("Y: ", numToString((int)localPointerPos.y - (int)originY, &stringMemory), &stringMemory), assets, TEXTURE_KEY_FONT, spriteList);
    addText(6, 180, appendString("offsetX: ", numToString(offsetX, &stringMemory), &stringMemory), assets, TEXTURE_KEY_FONT, spriteList);
    addText(6, 190, appendString("offsetY: ", numToString(offsetY, &stringMemory), &stringMemory), assets, TEXTURE_KEY_FONT, spriteList);

    addSprite(originX - 4, originY - 4, assets, ATLAS_KEY_HITBOX_EDITOR, "origin_reticule", spriteList, 0.0f, 0.0f);


    if (editor->state == HITBOX_EDITOR_STATE_DRAGGING_BOX) {
        char *boxKey;
        if (!editor->boxTypeIsHitbox) {
            boxKey = "hurtbox_frame_data";
        }
        else {
            boxKey = "hitbox_frame_data";
        }
        drawBox(spriteList, originX, originY, currentEditingBox, assets, boxKey, additionalOffsetX, additionalOffsetY);
    }

    for (int i = 0; i < editor->numCurrentHitboxes; ++i) {
        rectangle box = editor->hitboxes[i];
        char *boxKey = "hitbox_frame_data";
        drawBox(spriteList, originX, originY, box, assets, boxKey, additionalOffsetX, additionalOffsetY);
    }
    for (int i = 0; i < editor->numCurrentHurtboxes; ++i) {
        rectangle box = editor->hurtboxes[i];
        char *boxKey = "hurtbox_frame_data";
        drawBox(spriteList, originX, originY, box, assets, boxKey, additionalOffsetX, additionalOffsetY);
    }

    character_frame_data *frameData = &editor->animationData.frames[editor->currentFrame];
    //int frameTextLength;
    //for (frameTextLength = 0; frameTextLength < MAX_FRAME_KEY_LENGTH; ++frameTextLength) {
    //    if (currentSpriteText[frameTextLength] != 0) {
    //        frameData->frameKey[frameTextLength] = currentSpriteText[frameTextLength];
    //    }
    //    else {
    //        break;
    //    }
    //}
    //frameData->frameKey[frameTextLength] = 0;
    frameData->frameKey = currentSpriteText;

    frameData->xOffset = editor->offsetX;
    frameData->yOffset = editor->offsetY;
    editor->frameAtlasIndices[editor->currentFrame] = editor->currentAtlasIndex;

    for (int i = 0; i < editor->numCurrentHitboxes; ++i) {
        frameData->hitboxes[i] = editor->hitboxes[i];
    }
    frameData->numHitboxes = editor->numCurrentHitboxes;

    for (int i = 0; i < editor->numCurrentHurtboxes; ++i) {
        frameData->hurtboxes[i] = editor->hurtboxes[i];
    }
    frameData->numHurtboxes = editor->numCurrentHurtboxes;
    frameData->duration = editor->currentDuration;

    char *currentFrameIndexText = numToString(editor->currentFrame, &stringMemory);
    addText(185, 190, currentFrameIndexText, assets, TEXTURE_KEY_FONT, spriteList);

    if (editor->playback) {
        editor->playbackFrame++;
        if (editor->playbackFrame > editor->currentDuration) {
            loadFrame = true;
            nextFrame = editor->currentFrame + 1;
            editor->playbackFrame = 0;
            if (nextFrame >= 50 || editor->frameAtlasIndices[nextFrame] == -1) {
                if (!editor->loop) {
                    editor->playback = false;
                }
                nextFrame = 0;
            }
        }
    }
    if (startPlayback) {
        loadFrame = true;
        nextFrame = 0;
        editor->playback = true;
        editor->playbackFrame = 0;
    }

    if (duplicate) {
        for (int i = 50 - 1; i > editor->currentFrame; --i) {
            character_frame_data *prevFrameData = &editor->animationData.frames[i-1];
            character_frame_data *currFrameData = &editor->animationData.frames[i];
            *currFrameData = *prevFrameData;
            editor->frameAtlasIndices[i] = editor->frameAtlasIndices[i-1];
        }
        loadFrame = true;
        nextFrame = editor->currentFrame + 1;
    }
    if (remove) {
        for (int i = editor->currentFrame; i < 50 - 1; ++i) {
            character_frame_data *nextFrameData = &editor->animationData.frames[i+1];
            character_frame_data *currFrameData = &editor->animationData.frames[i];
            *currFrameData = *nextFrameData;
            editor->frameAtlasIndices[i] = editor->frameAtlasIndices[i+1];
        }
        editor->animationData.frames[50 -1] = {};
        loadFrame = true;
        nextFrame = editor->currentFrame;
    }

    if (loadFrame) {
        editor->currentFrame = nextFrame;
        character_frame_data *newFrameData = &editor->animationData.frames[editor->currentFrame];
        editor->offsetX = newFrameData->xOffset;
        editor->offsetY = newFrameData->yOffset;
        editor->numCurrentHitboxes = newFrameData->numHitboxes;
        for (int i = 0; i < editor->numCurrentHitboxes; ++i) {
            editor->hitboxes[i] = newFrameData->hitboxes[i];
        }
        editor->numCurrentHurtboxes = newFrameData->numHurtboxes;
        for (int i = 0; i < editor->numCurrentHurtboxes; ++i) {
            editor->hurtboxes[i] = newFrameData->hurtboxes[i];
        }
        editor->currentAtlasIndex = editor->frameAtlasIndices[editor->currentFrame];
        editor->currentDuration = newFrameData->duration;
    }
    //popSpriteMatrix(spriteList);
}
