#ifndef QUICKMAKE_ASSETS_H
#define QUICKMAKE_ASSETS_H

// TODO(ebuchholz): some convenient structure to access assets
enum mesh_key {
    MESH_KEY_CUBE
};

struct mesh_asset {
    mesh_key key;
    // maybe save a copy of vertices and things like that
};

enum texture_key {
    TEXTURE_KEY_GOLFMAN,
    TEXTURE_KEY_GAME_ATLAS
};

struct texture_asset {
    texture_key key;
    int width;
    int height;
    // maybe save pixels, but maybe don't need it after uploaded to GPU
};

enum atlas_key {
    ATLAS_KEY_GAME
};

struct atlas_frame {
    // assumes unrotated
    vector2 frameCorners[4]; // 0.0-1.0 texture coords, not pixel width/height
    char *key;
    int frameWidth;
    int frameHeight;
};

struct atlas_map {
    atlas_frame entries[500];
};

struct atlas_asset {
    atlas_map map;
    atlas_key atlasKey;
    texture_key textureKey;
    int width;
    int height;
};

// assuming a particular bmp format (the one that gets exported from gimp: 32 bit, rgba, compression mode 3)
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

    unsigned int redMask;
    unsigned int greenMask;
    unsigned int blueMask;
};
#pragma pack(pop)

#define MAX_NUM_MESHES 100
#define MAX_NUM_TEXTURES 100
#define MAX_NUM_ATLASES 20

struct game_assets {
    memory_arena assetMemory;

    mesh_asset *meshes[MAX_NUM_MESHES];
    int numMeshes;

    texture_asset *textures[MAX_NUM_TEXTURES];
    int numTextures;

    atlas_asset *atlases[MAX_NUM_ATLASES];
    int numAtlases;
};

#endif
