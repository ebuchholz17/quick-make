#ifndef RUN_AROUND_GAME_H
#define RUN_AROUND_GAME_H

#include "run_around_platform.h"
#include "run_around_math.cpp"

#define DELTA_TIME (1.0f / 60.0f)

// TODO(ebuchholz): some convenient structure to access assets
enum mesh_key {
    MESH_KEY_CUBE
    //MESH_KEY_FLAG,
    //MESH_KEY_SPIKES,
    //MESH_KEY_SPHERE,
    //MESH_KEY_THUMBSTICK,
    //MESH_KEY_BUTTON,
    //MESH_KEY_BUMP,
    //MESH_KEY_YOUWIN,

    //MESH_KEY_HILL,
    //MESH_KEY_LOOP,
    //MESH_KEY_LOOP_RAMP,
    //MESH_KEY_LOOP_RAMP2,
    //MESH_KEY_GROUND,
    //MESH_KEY_TRICKY_AREA,
    //MESH_KEY_CLIFF,
    //MESH_KEY_TOWERS,
    //MESH_KEY_UNDER_LOOP
};

struct mesh_asset {
    mesh_key key;
    // maybe save a copy of vertices and things like that
};

enum texture_key {
    TEXTURE_KEY_BLUE
    //TEXTURE_KEY_GREEN,
    //TEXTURE_KEY_WHITE,
    //TEXTURE_KEY_FLAG,
    //TEXTURE_KEY_BLACK_TEXTURE
};

struct texture_asset {
    texture_key key;
    // maybe save pixels, but maybe don't need it after uploaded to GPU
};

#pragma pack(push, 1)
struct bitmap_header {
    unsigned short type;
    unsigned int fileSize;
    unsigned short reserved1;
    unsigned short reserved2;
    unsigned int bitmapOffset;
    unsigned int structSize;
    int width;
    int height;
    unsigned short planes;
    unsigned short bitsPerPixel;
    unsigned int compression;
    unsigned int imageSize;
    int xResolution;
    int yResolution;
    unsigned int numColors;
    unsigned int numImportantColors;
};
#pragma pack(pop)

#define MAX_NUM_MESHES 100
#define MAX_NUM_TEXTURES 100

struct game_assets {
    memory_arena assetMemory;

    mesh_asset *meshes[MAX_NUM_MESHES];
    int numMeshes;

    texture_asset *textures[MAX_NUM_TEXTURES];
    int numTextures;
};

struct debug_camera {
    vector3 pos;
    quaternion rotation;
    int lastPointerX;
    int lastPointerY;
    vector3 lookAtTarget;
    vector3 up;
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
