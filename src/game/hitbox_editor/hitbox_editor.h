#ifndef HITBOX_EDITOR_H
#define HITBOX_EDITOR_H

enum hitbox_editor_state {
    HITBOX_EDITOR_STATE_NORMAL,
    HITBOX_EDITOR_STATE_DRAGGING_SPRITE,
    HITBOX_EDITOR_STATE_DRAGGING_BOX
};

struct hitbox_editor {
    character_animation_data animationData;
    int frameAtlasIndices[50];
    int currentFrame;

    int atlasIndices[500];
    int numAtlasIndices;
    int currentAtlasIndex;

    hitbox_editor_state state;

    int dragStartX;
    int dragStartY;
    int offsetX;
    int offsetY;

    int currentDuration;

    int numCurrentHitboxes;
    rectangle hitboxes[MAX_NUM_HITBOXES];
    int numCurrentHurtboxes;
    rectangle hurtboxes[MAX_NUM_HURTBOXES];

    bool boxTypeIsHitbox;
    bool playback;
    bool loop;
    int playbackFrame;

    bool requestFileLoad;
    bool requestFileSave;
    void *fileToSaveData;
    unsigned int fileToSaveSize;
};

#endif
