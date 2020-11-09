#ifndef QUICKMAKE_ASSETS_H
#define QUICKMAKE_ASSETS_H

#pragma pack(push, 1)
struct qmpack_file_header {
    char name[50];
    unsigned int size;
    asset_type type;
};
#pragma pack(pop)


// TODO(ebuchholz): some convenient structure to access assets
enum mesh_key {
    MESH_KEY_CUBE
};

struct mesh_asset {
    unsigned int id;
    // maybe save a copy of vertices and things like that
};

typedef mesh_asset *mesh_asset_ptr;
#define HASH_MAP_TYPE mesh_asset_ptr
#include "hash_map.h"

enum animated_mesh_key {
    ANIM_MESH_KEY_TUBE_SNAKE
};

struct animated_mesh_asset {
    animated_mesh_key key;
    // maybe save a copy of vertices and things like that
};

struct texture_asset {
    unsigned int id;
    int width;
    int height;
    // maybe save pixels, but maybe don't need it after uploaded to GPU
};

typedef texture_asset *texture_asset_ptr;
#define HASH_MAP_TYPE texture_asset_ptr
#include "hash_map.h"

struct sound_asset {
    char *key;
    // info about file size, duration, etc?
    short *samples;
    int numSamples;
};

typedef sound_asset *sound_asset_ptr;
#define HASH_MAP_TYPE sound_asset_ptr
#include "hash_map.h"

enum data_key {
    DATA_KEY_HITBOX_DATA
};

struct data_asset {
    void *data;
    unsigned int size;
};

typedef data_asset *data_asset_ptr;
#define HASH_MAP_TYPE data_asset_ptr
#include "hash_map.h"

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
    int textureID;
    int width;
    int height;
};

typedef atlas_asset *atlas_asset_ptr;
#define HASH_MAP_TYPE atlas_asset_ptr
#include "hash_map.h"

enum midi_key {
    MIDI_KEY_TEST
};

enum midi_format {
    MIDI_FORMAT_SINGLE_TRACK,
    MIDI_FORMAT_MULTI_TRACK_SIMULTANEOUS,
    MIDI_FORMAT_MULTI_TRACK_ASYNCHRONOUS
};

enum midi_div_mode {
    MIDI_DIV_MODE_TICKS_PER_QUARTER_NOTE,
    MIDI_DIV_MODE_TICKS_PER_FRAME
};

#define MAX_MIDI_TRACKS 256

struct midi_track {
    char *data;
    unsigned int length;
};

struct midi_asset {
    midi_format format;
    midi_div_mode divisionMode;
    int ticksPerQuarterNote;
    double tickDuration;

    int numTracks;
    midi_track tracks[MAX_MIDI_TRACKS];
};

typedef midi_asset *midi_asset_ptr;
#define HASH_MAP_TYPE midi_asset_ptr
#include "hash_map.h"

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
#define MAX_NUM_MIDIS 10

#define MAX_NUM_ANIMATION_DATA 10

// TODO(ebuchholz): resizable arrays?
struct game_assets {
    memory_arena assetMemory;

    // TODO(ebuchholz): consider storing values in these hash maps instead of pointers
    mesh_asset_ptr_hash_map meshes;
    unsigned int numMeshes; // used to assign integer id's to meshes, which renderer can use

    animated_mesh_asset *animatedMeshes[MAX_NUM_ANIMATED_MESHES];
    int numAnimatedMeshes;

    sound_asset_ptr_hash_map sounds;
    midi_asset_ptr_hash_map midis;
    atlas_asset_ptr_hash_map atlases;
    data_asset_ptr_hash_map dataAssets;

    texture_asset_ptr_hash_map textures;
    unsigned int numTextures; // used to assign integer id's to textures, which renderer can use

    animation_data *animationData[MAX_NUM_ANIMATION_DATA];
    int numAnimationData;
};

#endif
