#include "test_game.h"

static void drawBox (sprite_list *spriteList, float originX, float originY, rectangle box, game_assets *assets, char *boxKey, int additionalOffsetX, int additionalOffsetY) {
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

void updateTestGame (memory_arena *memory, memory_arena *tempMemory, 
                     game_assets *assets, game_input *input, sprite_list *spriteList, game_sounds *gameSounds)
{
    static bool initialized = false;
    static character_animation_data animation;
    static int frame = 0;
    static int frameStep = 0;
    if (!initialized) {
        initialized = true;

        data_asset *hitboxData = getTextData(assets, "data");
        loadHitboxData(assets, hitboxData->data, &animation);

        playBGM(MIDI_KEY_TEST, gameSounds, assets);
    }
    // memory for dynamically created strings
    memory_arena stringMemory = {};
    stringMemory.capacity = 512 * 1024;
    stringMemory.base = allocateMemorySize(tempMemory, stringMemory.capacity);

    // zoom out a bit
    //pushSpriteTransform(spriteList, Vector2(GAME_WIDTH/2.0f, GAME_HEIGHT/2.0f), 0.5f);

    //popSpriteMatrix(spriteList);
    //character_frame_data *currentFrame = &animation.frames[frame];
    //addSprite(40.0f + currentFrame->xOffset, 40.0f + currentFrame->yOffset, assets, ATLAS_KEY_GAME, currentFrame->frameKey, spriteList, 0.0f, 1.0f);

    //for (int i = 0; i < currentFrame->numHitboxes; ++i) {
    //    rectangle box = currentFrame->hitboxes[i];
    //    char *boxKey = "hitbox_frame_data";
    //    drawBox(spriteList, 40, 40, box, assets, boxKey, 0, 0);
    //}
    //for (int i = 0; i < currentFrame->numHurtboxes; ++i) {
    //    rectangle box = currentFrame->hurtboxes[i];
    //    char *boxKey = "hurtbox_frame_data";
    //    drawBox(spriteList, 40, 40, box, assets, boxKey, 0, 0);
    //}

    //++frameStep;
    //if (frameStep >= currentFrame->duration) {
    //    frameStep = 0;
    //    ++frame;
    //    if (frame == animation.numFrames) {
    //        frame = 0;
    //    }
    //}
}
