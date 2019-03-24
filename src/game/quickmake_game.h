#ifndef QUICKMAKE_GAME_H
#define QUICKMAKE_GAME_H

#include "quickmake_platform.h"
#include "quickmake_math.cpp"
#include "quickmake_util.h"
#include "quickmake_assets.cpp"

#define DELTA_TIME (1.0f / 60.0f)

struct debug_camera {
    vector3 pos;
    quaternion rotation;
    int lastPointerX;
    int lastPointerY;
    vector3 lookAtTarget;
    vector3 up;
};

// TODO(ebuchholz): make stretchy
#define MAX_SPRITES_PER_FRAME 2000

struct sprite {
    vector2 pos;
    vector2 anchor;
    float width;
    float height;
    float scale;
    float rotation;
    float alpha;
    unsigned int tint;
    texture_key textureKey;
};

struct sprite_list {
    sprite *sprites;
    int numSprites;
};

struct game_state {
    memory_arena memory;
    memory_arena tempMemory; // cleared every frame
    bool assetsInitialized;
    game_assets assets;
    bool gameInitialized;

    debug_camera debugCamera;
};

#endif
