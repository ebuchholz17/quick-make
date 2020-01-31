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

enum animated_mesh_key {
    ANIM_MESH_KEY_TUBE_SNAKE
};

struct animated_mesh_asset {
    animated_mesh_key key;
    // maybe save a copy of vertices and things like that
};

enum texture_key {
    TEXTURE_KEY_FONT,
    TEXTURE_KEY_GAME_ATLAS,
    TEXTURE_KEY_HITBOX_EDITOR_ATLAS
};

struct texture_asset {
    texture_key key;
    int width;
    int height;
    // maybe save pixels, but maybe don't need it after uploaded to GPU
};

enum atlas_key {
    ATLAS_KEY_GAME,
    ATLAS_KEY_HITBOX_EDITOR
};

enum sound_key {
    SOUND_KEY_MENU_BUTTON
};

struct sound_asset {
    sound_key key;
    // info about file size, duration, etc?
    short *samples;
    int numSamples;
};

enum data_key {
    DATA_KEY_HITBOX_DATA
};

struct data_asset {
    data_key key;
    void *data;
    unsigned int size;
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

// TODO(ebuchholz): maybe pack just the transforms together separately?
struct skeleton_bone {
    matrix4x4 transform;
    quaternion localRotation;
    vector3 localPos;
    int parentID;
};


struct skeleton_bone_pose {
    quaternion localRotation;
    vector3 localPos;
    int boneID; // TODO(ebuchholz): determine if this is needed
};

// TODO(ebuchholz): allocate the exact number of bones needed
// TODO(ebuchholz): force animations to correspond to their model? or allow many
// models to use the same animations
struct skeleton_pose {
    skeleton_bone_pose *bonePoses;
    int numBones;
};
// TODO(ebuchholz): allocate the exact number of keyframes when the asset is
// loaded
struct skeleton_keyframe {
    int poseID;
    float t;
};

struct skeleton_animation {
    // TODO(ebuchholz): store keyframes elsewhere? separate from animation struct?
    // TODO(ebuchholz): something better than string keys?
    skeleton_keyframe *keyframes;
    int numKeyFrames;
    float duration;
    char *key;
    int id;
};

enum animation_data_key {
    ANIMATION_DATA_KEY_LEGS,
    ANIMATION_DATA_KEY_SNAKE,
    ANIMATION_DATA_KEY_MULTI_ELBOW
};

struct animation_data {
    int *boneHierarchy; // parent IDs, should be something like: [-1, 0, 0, 1, 2, 3, 4]
    matrix4x4 *inverseRestTransforms;
    int numBones;

    skeleton_pose *poses;
    int numPoses;

    // TODO(ebuchholz): determine how to store these. One table in the game with all animations? string based map or 
    // hash map stored with animation?
    skeleton_animation *animations;
    int numAnimations;

    animation_data_key key;
};

#pragma pack(push, 1)
struct wav_header {
    unsigned char riff[4];
    unsigned int fileSize;
    unsigned char fileType[4];
    unsigned char formatChunkMarker[4];
    unsigned int formatLength;
    unsigned short formatType;
    unsigned short numChannels;
    unsigned int sampleRate;
    unsigned int byteRate;
    unsigned short blockAlign;
    unsigned short bitsPerSample;
    unsigned char dataMarker[4];
    unsigned int dataSize;
};
#pragma pack(pop)

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

// TODO(ebuchholz): pick reasonable numbers, or make it more dynamic
#define MAX_NUM_MESHES 100
#define MAX_NUM_ANIMATED_MESHES 30
#define MAX_NUM_TEXTURES 100
#define MAX_NUM_SOUNDS 100
#define MAX_NUM_ATLASES 20
#define MAX_NUM_DATA_ASSETS 100

#define MAX_NUM_ANIMATION_DATA 10

// TODO(ebuchholz): resizable arrays?
struct game_assets {
    memory_arena assetMemory;

    // TODO(ebuchholz): remove pointer arrays, make them regular arrays, don't remember why they are pointers
    mesh_asset *meshes[MAX_NUM_MESHES];
    int numMeshes;

    animated_mesh_asset *animatedMeshes[MAX_NUM_ANIMATED_MESHES];
    int numAnimatedMeshes;

    texture_asset *textures[MAX_NUM_TEXTURES];
    int numTextures;
    
    sound_asset *sounds[MAX_NUM_SOUNDS];
    int numSounds;

    atlas_asset *atlases[MAX_NUM_ATLASES];
    int numAtlases;

    animation_data *animationData[MAX_NUM_ANIMATION_DATA];
    int numAnimationData;

    data_asset *dataAssets[MAX_NUM_DATA_ASSETS];
    int numDataAssets;
};

#endif
