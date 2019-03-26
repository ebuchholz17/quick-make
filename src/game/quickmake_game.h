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

// TODO(ebuchholz): make stretchy? these are arbitrary numbers
#define MAX_SPRITES_PER_FRAME 2000
#define MAX_TRANSFORM_GROUPS 300

struct sprite {
    matrix3x3 parentTransform;
    vector2 pos;
    vector2 anchor;
    vector2 frameCorners[4];
    float width;
    float height;
    float scale;
    float rotation;
    float alpha;
    unsigned int tint;
    texture_key textureKey;
};

#define MATRIX_STACK_CAPACITY 20

struct sprite_list {
    matrix3x3 matrixStack[MATRIX_STACK_CAPACITY];
    sprite *sprites;

    int numSprites;
    unsigned int matrixStackIndex;
};

//enum transform_group_entry_type {
//    TRANSFORM_GROUP_ENTRY_TYPE_GROUP,
//    TRANSFORM_GROUP_ENTRY_TYPE_SPRITE
//};
//
//struct transform_group_entry {
//    transform_group_entry_type type;
//    unsigned int id;
//};
//
//struct transform_group_entry_list {
//    transform_group_entry *entries;
//    unsigned int length;
//    unsigned int capacity;
//};
//
//struct transform_group {
//    transform_group_entry_list children;
//    vector2 pos;
//    float scale;
//    float rotation;
//};
//
//struct transform_group_list {
//    transform_group *groups;
//    int numGroups;
//};

struct game_state {
    memory_arena memory;
    memory_arena tempMemory; // cleared every frame
    bool assetsInitialized;
    game_assets assets;
    bool gameInitialized;

    debug_camera debugCamera;
};

#endif
