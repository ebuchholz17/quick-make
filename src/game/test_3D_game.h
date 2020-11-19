#ifndef TEST_3D_GAME_H
#define TEST_3D_GAME_H

struct debug_camera {
    vector3 pos;
    quaternion rotation;
    int lastPointerX;
    int lastPointerY;
    vector3 lookAtTarget;
    vector3 up;
};

enum voxel_data_flags {
    VOXEL_DATA_FLAGS_ACTIVE = 1
};

struct voxel_data {
    char data;
};

struct chunk_mesh_id_node {
    chunk_mesh_id_node *next;
    unsigned int meshID;
};

#define MAX_VOXEL_CHUNK_MESH_IDS 500

struct chunk_mesh_id_list {
    chunk_mesh_id_node nodes[MAX_VOXEL_CHUNK_MESH_IDS];
    chunk_mesh_id_node *firstFree;
};

struct voxel_chunk {
    chunk_mesh_id_node *meshIDNode;
    bool dirty;
    bool loaded;
    bool meshGenerated;
    voxel_data *voxels;
    unsigned int x;
    unsigned int y;
    unsigned int z;
};

struct voxel_chunk_hash_map {
    unsigned int capacity;
    voxel_chunk *chunks;
};


#define VOXEL_CHUNK_WIDTH 32
#define VOXEL_CHUNK_DEPTH 32
#define VOXEL_CHUNK_HEIGHT 32

struct test_3D_game {
    chunk_mesh_id_list meshIDList;
    voxel_chunk_hash_map chunkMap;

    debug_camera debugCamera;
    bool initialized;

    unsigned int playerChunkX;
    unsigned int playerChunkY;
    unsigned int playerChunkZ;
    vector3 playerPos;

    bool showTestCamera;
};

#endif
