#ifndef QUICKMAKE_PLATFORM_H
#define QUICKMAKE_PLATFORM_H

#define assert(expression) if (!(expression)) { *(int *)0 = 0; }

// TODO(ebuchholz): figure out how to organize includes a little better
#include "quickmake_math.h"

struct memory_arena {
    void *base;
    unsigned int size;
    unsigned int capacity;
};

// asset loading
enum asset_type {
    ASSET_TYPE_OBJ,
    ASSET_TYPE_BMP,
    ASSET_TYPE_ATLAS,
    ASSET_TYPE_ANIMATION_DATA,
    ASSET_TYPE_QMM,
    ASSET_TYPE_WAV
};

struct asset_to_load {
    char *path;
    asset_type type;
    int key;
    int secondKey;
};

struct asset_list {
    int numAssetsToLoad;
    int maxAssetsToLoad;
    asset_to_load *assetsToLoad;
};

// Makes it easier to wrap up in WebIDL
// NOTE(ebuchholz): emscripten+idl can't handle pointers to primitives, so these have to be void
// pointers
struct float_mesh_attribute {
    void *values;
    int count;
};

struct int_mesh_attribute {
    void *values;
    int count;
};

struct loaded_mesh_asset {
    int key;
    float_mesh_attribute positions;
    float_mesh_attribute texCoords;
    float_mesh_attribute normals;
    int_mesh_attribute indices;
};

struct loaded_animated_mesh_asset {
    int key;
    float_mesh_attribute positions;
    float_mesh_attribute texCoords;
    float_mesh_attribute normals;
    // TODO(ebuchholz): multiple bone indices per vertex + weights
    float_mesh_attribute boneIndices;
    float_mesh_attribute boneWeights;
    int_mesh_attribute indices;
};

struct loaded_texture_asset {
    int key;
    int width;
    int height;
    void *pixels; // unsigned int, but void for compatibility with webidl
};

// same as texture asset, combine?
struct loaded_atlas_asset {
    int key;
    int width;
    int height;
    void *pixels; // unsigned int, but void for compatibility with webidl
};

enum render_command_type {
    RENDER_COMMAND_MODEL,
    RENDER_COMMAND_ANIMATED_MODEL,
    RENDER_COMMAND_LINES,
    RENDER_COMMAND_SPRITE,
    RENDER_COMMAND_SPRITE_LIST,
    RENDER_COMMAND_SET_CAMERA,
    RENDER_COMMAND_BACKGROUND_VISUALIZATION
};

struct render_command_header{
    render_command_type type;
};

struct render_command_model {
    int meshKey;
    int textureKey;
    matrix4x4 modelMatrix;
};

// TODO(ebuchholz): bone matrices
struct render_command_animated_model {
    matrix4x4 modelMatrix;
    matrix4x4 *boneMatrices;
    int animatedMeshKey;
    int textureKey;
    int numBones;
};

struct render_command_lines {
    line *lines;
    int numLines;
};

struct render_command_sprite {
    int textureKey;
    float x;
    float y;
    float width;
    float height;
};

struct render_sprite {
    vector2 pos[4];
    vector2 texCoord[4];
    vector4 color[4];
    int textureKey;
};

struct render_command_sprite_list {
    render_sprite *sprites;
    int numSprites;
};

struct render_command_set_camera {
    matrix4x4 viewMatrix;
    matrix4x4 projMatrix;
};

struct render_command_background_visualization {
    float t;
};

struct render_command_list {
    int windowWidth;
    int windowHeight;
    memory_arena memory;
};

struct input_key {
    bool down;
    bool justPressed;
};

struct game_input {
    input_key upKey;
    input_key downKey;
    input_key leftKey;
    input_key rightKey;

    // TODO(ebuchholz): add just pressed for other buttons, maybe button struct
    input_key aKey;
    input_key sKey;
    input_key dKey;
    input_key fKey;
    input_key gKey;
    input_key hKey;
    input_key jKey;
    input_key kKey;
    input_key wKey;
    input_key eKey;
    input_key tKey;
    input_key yKey;
    input_key uKey;

    bool pointerDown;
    bool pointerJustDown;
    int pointerX;
    int pointerY;

    bool pointer2Down;
    bool pointer2JustDown;
    int pointer2X;
    int pointer2Y;
};

struct sound_sample {
    float value;
};

struct game_sound_output {
    int samplesPerSecond;
    int sampleCount;
    sound_sample *samples;
};

struct game_memory {
    void *memory;
    unsigned int memoryCapacity;
    void *tempMemory;
    unsigned int tempMemoryCapacity;
};

#endif
