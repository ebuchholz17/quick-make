#include "hitbox.h"

void loadHitboxData (game_assets *assets, void *fileData, character_animation_data *animationData) {
    char *start;
    char *end;
    char *nextLineStart;
    char *file = (char *)fileData;

    readLine(file, &start, &end, &nextLineStart);

    int numFrames = stringToInt(start, end-1);
    animationData->numFrames = numFrames;

    start = nextLineStart;

    for (int i = 0; i < numFrames; ++i) {
        character_frame_data *frameData = &animationData->frames[i];

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
        //int mapIndex = (int)getAtlasFrameIndex(assets, ATLAS_KEY_GAME, frameName);
        //for (int j = 0; j < 500; ++j) {
        //    if (editor->atlasIndices[j] == mapIndex) {
        //        editor->frameAtlasIndices[i] = j;
        //        break;
        //    }
        //}
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
}
