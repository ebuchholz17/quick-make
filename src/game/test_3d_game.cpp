#include "test_3D_game.h"

void debugCameraMovement (debug_camera *debugCamera, game_input *input) {
    const float CAMERA_SPEED = 10.0f;
    const float CAMERA_TURN_SPEED = 1.0f;

    // Position
    vector3 moveVector = {};
    if (input->upKey.down || input->wKey.down) {
        moveVector.z -= CAMERA_SPEED * DELTA_TIME;
    }
    if (input->downKey.down || input->sKey.down) {
        moveVector.z += CAMERA_SPEED * DELTA_TIME;
    }
    if (input->leftKey.down || input->aKey.down) {
        moveVector.x -= CAMERA_SPEED * DELTA_TIME;
    }
    if (input->rightKey.down || input->dKey.down) {
        moveVector.x += CAMERA_SPEED * DELTA_TIME;
    }

    // Rotation
    if (input->pointerJustDown) {
        debugCamera->lastPointerX = input->pointerX;
        debugCamera->lastPointerY = input->pointerY;
    }
    if (input->pointerDown) {
        int pointerDX = input->pointerX - debugCamera->lastPointerX;
        int pointerDY = input->pointerY - debugCamera->lastPointerY;

        float yaw = (float)pointerDX * 0.25f;
        float pitch = (float)pointerDY * 0.25f;

        debugCamera->rotation = quaternionFromAxisAngle(Vector3(0, 1, 0), -yaw * DELTA_TIME) * (debugCamera->rotation);
        debugCamera->rotation = (debugCamera->rotation) * quaternionFromAxisAngle(Vector3(1, 0, 0), -pitch * DELTA_TIME);

        debugCamera->lastPointerX = input->pointerX;
        debugCamera->lastPointerY = input->pointerY;
    }
    // Move in the direction of the current rotation
    moveVector = rotateVectorByQuaternion(moveVector, debugCamera->rotation);
    debugCamera->pos += moveVector;
}


inline void assignVoxelMeshPosition (vector3 *positions, unsigned int *numPositions, float x, float y, float z) {
    positions[*numPositions].x = x;
    positions[*numPositions].y = y;
    positions[*numPositions].z = z;
    (*numPositions)++;
}

inline void assignVoxelMeshNormal (vector3 *normals, unsigned int *numNormals, float x, float y, float z) {
    normals[*numNormals].x = x;
    normals[*numNormals].y = y;
    normals[*numNormals].z = z;
    (*numNormals)++;
}

inline void generateMeshForVoxel (vector3 *positions, unsigned int *numPositions, 
                                  vector2 *texCoords, unsigned int *numTexCoords,
                                  vector3 *normals, unsigned int *numNormals,
                                  unsigned int *indices, unsigned int *numIndices,
                                  int i, int j, int k)
{
    vector3 posOffset;
    posOffset.x = 1.0f * k;
    posOffset.y = 1.0f * i;
    posOffset.z = 1.0f * j;

    assignVoxelMeshPosition(positions, numPositions, posOffset.x + 0.0f, posOffset.y + 1.0f, posOffset.z + 1.0f);
    assignVoxelMeshPosition(positions, numPositions, posOffset.x + 0.0f, posOffset.y + 0.0f, posOffset.z + 1.0f);
    assignVoxelMeshPosition(positions, numPositions, posOffset.x + 1.0f, posOffset.y + 1.0f, posOffset.z + 1.0f);
    assignVoxelMeshPosition(positions, numPositions, posOffset.x + 1.0f, posOffset.y + 1.0f, posOffset.z + 1.0f);
    assignVoxelMeshPosition(positions, numPositions, posOffset.x + 0.0f, posOffset.y + 0.0f, posOffset.z + 1.0f);
    assignVoxelMeshPosition(positions, numPositions, posOffset.x + 1.0f, posOffset.y + 0.0f, posOffset.z + 1.0f);

    assignVoxelMeshPosition(positions, numPositions, posOffset.x + 1.0f, posOffset.y + 1.0f, posOffset.z + 1.0f);
    assignVoxelMeshPosition(positions, numPositions, posOffset.x + 1.0f, posOffset.y + 0.0f, posOffset.z + 1.0f);
    assignVoxelMeshPosition(positions, numPositions, posOffset.x + 1.0f, posOffset.y + 1.0f, posOffset.z + 0.0f);
    assignVoxelMeshPosition(positions, numPositions, posOffset.x + 1.0f, posOffset.y + 1.0f, posOffset.z + 0.0f);
    assignVoxelMeshPosition(positions, numPositions, posOffset.x + 1.0f, posOffset.y + 0.0f, posOffset.z + 1.0f);
    assignVoxelMeshPosition(positions, numPositions, posOffset.x + 1.0f, posOffset.y + 0.0f, posOffset.z + 0.0f);

    assignVoxelMeshPosition(positions, numPositions, posOffset.x + 1.0f, posOffset.y + 1.0f, posOffset.z + 0.0f);
    assignVoxelMeshPosition(positions, numPositions, posOffset.x + 1.0f, posOffset.y + 0.0f, posOffset.z + 0.0f);
    assignVoxelMeshPosition(positions, numPositions, posOffset.x + 0.0f, posOffset.y + 1.0f, posOffset.z + 0.0f);
    assignVoxelMeshPosition(positions, numPositions, posOffset.x + 0.0f, posOffset.y + 1.0f, posOffset.z + 0.0f);
    assignVoxelMeshPosition(positions, numPositions, posOffset.x + 1.0f, posOffset.y + 0.0f, posOffset.z + 0.0f);
    assignVoxelMeshPosition(positions, numPositions, posOffset.x + 0.0f, posOffset.y + 0.0f, posOffset.z + 0.0f);

    assignVoxelMeshPosition(positions, numPositions, posOffset.x + 0.0f, posOffset.y + 1.0f, posOffset.z + 0.0f);
    assignVoxelMeshPosition(positions, numPositions, posOffset.x + 0.0f, posOffset.y + 0.0f, posOffset.z + 0.0f);
    assignVoxelMeshPosition(positions, numPositions, posOffset.x + 0.0f, posOffset.y + 1.0f, posOffset.z + 1.0f);
    assignVoxelMeshPosition(positions, numPositions, posOffset.x + 0.0f, posOffset.y + 1.0f, posOffset.z + 1.0f);
    assignVoxelMeshPosition(positions, numPositions, posOffset.x + 0.0f, posOffset.y + 0.0f, posOffset.z + 0.0f);
    assignVoxelMeshPosition(positions, numPositions, posOffset.x + 0.0f, posOffset.y + 0.0f, posOffset.z + 1.0f);

    assignVoxelMeshPosition(positions, numPositions, posOffset.x + 0.0f, posOffset.y + 1.0f, posOffset.z + 0.0f);
    assignVoxelMeshPosition(positions, numPositions, posOffset.x + 0.0f, posOffset.y + 1.0f, posOffset.z + 1.0f);
    assignVoxelMeshPosition(positions, numPositions, posOffset.x + 1.0f, posOffset.y + 1.0f, posOffset.z + 0.0f);
    assignVoxelMeshPosition(positions, numPositions, posOffset.x + 1.0f, posOffset.y + 1.0f, posOffset.z + 0.0f);
    assignVoxelMeshPosition(positions, numPositions, posOffset.x + 0.0f, posOffset.y + 1.0f, posOffset.z + 1.0f);
    assignVoxelMeshPosition(positions, numPositions, posOffset.x + 1.0f, posOffset.y + 1.0f, posOffset.z + 1.0f);

    assignVoxelMeshPosition(positions, numPositions, posOffset.x + 0.0f, posOffset.y + 0.0f, posOffset.z + 1.0f);
    assignVoxelMeshPosition(positions, numPositions, posOffset.x + 0.0f, posOffset.y + 0.0f, posOffset.z + 0.0f);
    assignVoxelMeshPosition(positions, numPositions, posOffset.x + 1.0f, posOffset.y + 0.0f, posOffset.z + 1.0f);
    assignVoxelMeshPosition(positions, numPositions, posOffset.x + 1.0f, posOffset.y + 0.0f, posOffset.z + 1.0f);
    assignVoxelMeshPosition(positions, numPositions, posOffset.x + 0.0f, posOffset.y + 0.0f, posOffset.z + 0.0f);
    assignVoxelMeshPosition(positions, numPositions, posOffset.x + 1.0f, posOffset.y + 0.0f, posOffset.z + 0.0f);

    for (int texIndex = 0; texIndex < 6; ++texIndex) {
        texCoords[*numTexCoords].x = 0.0f;
        texCoords[*numTexCoords].y = 1.0f;
        (*numTexCoords)++;
        texCoords[*numTexCoords].x = 0.0f;
        texCoords[*numTexCoords].y = 0.0f;
        (*numTexCoords)++;
        texCoords[*numTexCoords].x = 1.0f;
        texCoords[*numTexCoords].y = 1.0f;
        (*numTexCoords)++;
        texCoords[*numTexCoords].x = 1.0f;
        texCoords[*numTexCoords].y = 1.0f;
        (*numTexCoords)++;
        texCoords[*numTexCoords].x = 0.0f;
        texCoords[*numTexCoords].y = 0.0f;
        (*numTexCoords)++;
        texCoords[*numTexCoords].x = 1.0f;
        texCoords[*numTexCoords].y = 0.0f;
        (*numTexCoords)++;
    }

    for (int normalIndex = 0; normalIndex < 6; ++normalIndex) {
        assignVoxelMeshNormal(normals, numNormals, 0.0f, 0.0f, 1.0f);
    }
    for (int normalIndex = 0; normalIndex < 6; ++normalIndex) {
        assignVoxelMeshNormal(normals, numNormals, 1.0f, 0.0f, 0.0f);
    }
    for (int normalIndex = 0; normalIndex < 6; ++normalIndex) {
        assignVoxelMeshNormal(normals, numNormals, 0.0f, 0.0f, -1.0f);
    }
    for (int normalIndex = 0; normalIndex < 6; ++normalIndex) {
        assignVoxelMeshNormal(normals, numNormals, -1.0f, 0.0f, 0.0f);
    }
    for (int normalIndex = 0; normalIndex < 6; ++normalIndex) {
        assignVoxelMeshNormal(normals, numNormals, 0.0f, 1.0f, 0.0f);
    }
    for (int normalIndex = 0; normalIndex < 6; ++normalIndex) {
        assignVoxelMeshNormal(normals, numNormals, 0.0f, -1.0f, 0.0f);
    }

    for (int indexIndex = 0; indexIndex < 36; ++indexIndex) {
        indices[(*numIndices)++] = *numIndices;
    }
}

void generateMeshForVoxelChunk (voxel_chunk *chunk, memory_arena *tempMemory, render_command_list *renderCommands) {
    // generate new mesh
    // NOTE(ebuchholz): allocating the maximum amount of space a chunk can use. Maybe better to try
    // to only allocate needed memory?
    unsigned int voxelsPerChunk = VOXEL_CHUNK_WIDTH * VOXEL_CHUNK_DEPTH * VOXEL_CHUNK_HEIGHT;
    unsigned int maxNumTriangles = voxelsPerChunk * 12;
    vector3 *positions = (vector3 *)allocateMemorySize(tempMemory, sizeof(vector3) * 3 * maxNumTriangles);
    unsigned int numPositions = 0;
    vector2 *texCoords = (vector2 *)allocateMemorySize(tempMemory, sizeof(vector2) * 3 * maxNumTriangles);
    unsigned int numTexCoords = 0;
    vector3 *normals = (vector3 *)allocateMemorySize(tempMemory, sizeof(vector3) * 3 * maxNumTriangles);
    unsigned int numNormals = 0;
    unsigned int *indices = (unsigned int *)allocateMemorySize(tempMemory, sizeof(unsigned int) * 3 * maxNumTriangles);
    unsigned int numIndices = 0;

    // simple voxel culling: cull if completely surrounded, otherwise produce all 6 faces
    // always produce faces for outer faces of the chunk
    // bottom
    for (int j = 0; j < VOXEL_CHUNK_DEPTH; ++j) {
        for (int k = 0; k < VOXEL_CHUNK_WIDTH; ++k) {
            char voxelValue = chunk->voxels[(0 * VOXEL_CHUNK_WIDTH * VOXEL_CHUNK_DEPTH) + j * VOXEL_CHUNK_WIDTH + k].data;
            if (voxelValue & VOXEL_DATA_FLAGS_ACTIVE) {
                generateMeshForVoxel(positions, &numPositions, texCoords, &numTexCoords, normals, &numNormals, indices, &numIndices, 0, j, k);
            }
        }
    }

    // top
    for (int j = 0; j < VOXEL_CHUNK_DEPTH; ++j) {
        for (int k = 0; k < VOXEL_CHUNK_WIDTH; ++k) {
            char voxelValue = chunk->voxels[((VOXEL_CHUNK_HEIGHT-1) * VOXEL_CHUNK_WIDTH * VOXEL_CHUNK_DEPTH) + j * VOXEL_CHUNK_WIDTH + k].data;
            if (voxelValue & VOXEL_DATA_FLAGS_ACTIVE) {
                generateMeshForVoxel(positions, &numPositions, texCoords, &numTexCoords, normals, &numNormals, indices, &numIndices, (VOXEL_CHUNK_HEIGHT - 1), j, k);
            }
        }
    }

    // front
    for (int i = 1; i < VOXEL_CHUNK_HEIGHT - 1; ++i) {
        for (int k = 0; k < VOXEL_CHUNK_WIDTH; ++k) {
            char voxelValue = chunk->voxels[(i * VOXEL_CHUNK_WIDTH * VOXEL_CHUNK_DEPTH) + 0 * VOXEL_CHUNK_WIDTH + k].data;
            if (voxelValue & VOXEL_DATA_FLAGS_ACTIVE) {
                generateMeshForVoxel(positions, &numPositions, texCoords, &numTexCoords, normals, &numNormals, indices, &numIndices, i, 0, k);
            }
        }
    }

    // back
    for (int i = 1; i < VOXEL_CHUNK_HEIGHT - 1; ++i) {
        for (int k = 0; k < VOXEL_CHUNK_WIDTH; ++k) {
            char voxelValue = chunk->voxels[(i * VOXEL_CHUNK_WIDTH * VOXEL_CHUNK_DEPTH) + (VOXEL_CHUNK_DEPTH - 1) * VOXEL_CHUNK_WIDTH + k].data;
            if (voxelValue & VOXEL_DATA_FLAGS_ACTIVE) {
                generateMeshForVoxel(positions, &numPositions, texCoords, &numTexCoords, normals, &numNormals, indices, &numIndices, i, (VOXEL_CHUNK_DEPTH - 1), k);
            }
        }
    }

    // left
    for (int i = 1; i < VOXEL_CHUNK_HEIGHT - 1; ++i) {
        for (int j = 1; j < VOXEL_CHUNK_DEPTH - 1; ++j) {
            char voxelValue = chunk->voxels[(i * VOXEL_CHUNK_WIDTH * VOXEL_CHUNK_DEPTH) + j * VOXEL_CHUNK_WIDTH + 0].data;
            if (voxelValue & VOXEL_DATA_FLAGS_ACTIVE) {
                generateMeshForVoxel(positions, &numPositions, texCoords, &numTexCoords, normals, &numNormals, indices, &numIndices, i, j, 0);
            }
        }
    }

    // right
    for (int i = 1; i < VOXEL_CHUNK_HEIGHT - 1; ++i) {
        for (int j = 1; j < VOXEL_CHUNK_DEPTH - 1; ++j) {
            char voxelValue = chunk->voxels[(i * VOXEL_CHUNK_WIDTH * VOXEL_CHUNK_DEPTH) + j * VOXEL_CHUNK_WIDTH + (VOXEL_CHUNK_WIDTH - 1)].data;
            if (voxelValue & VOXEL_DATA_FLAGS_ACTIVE) {
                generateMeshForVoxel(positions, &numPositions, texCoords, &numTexCoords, normals, &numNormals, indices, &numIndices, i, j, (VOXEL_CHUNK_WIDTH - 1));
            }
        }
    }

    // center
    for (int i = 1; i < VOXEL_CHUNK_HEIGHT - 1; ++i) {
        for (int j = 1; j < VOXEL_CHUNK_DEPTH - 1; ++j) {
            for (int k = 1; k < VOXEL_CHUNK_WIDTH - 1; ++k) {
                char voxelValue = chunk->voxels[(i * VOXEL_CHUNK_WIDTH * VOXEL_CHUNK_DEPTH) + j * VOXEL_CHUNK_WIDTH + k].data;

                if ((voxelValue & VOXEL_DATA_FLAGS_ACTIVE)) {
                    bool aboveVoxelSolid = chunk->voxels[((i + 1) * VOXEL_CHUNK_WIDTH * VOXEL_CHUNK_DEPTH) + j * VOXEL_CHUNK_WIDTH + k].data & VOXEL_DATA_FLAGS_ACTIVE;
                    bool belowVoxelSolid = chunk->voxels[((i - 1) * VOXEL_CHUNK_WIDTH * VOXEL_CHUNK_DEPTH) + j * VOXEL_CHUNK_WIDTH + k].data & VOXEL_DATA_FLAGS_ACTIVE;
                    bool frontVoxelSolid = chunk->voxels[(i * VOXEL_CHUNK_WIDTH * VOXEL_CHUNK_DEPTH) + j * VOXEL_CHUNK_WIDTH + (k + 1)].data & VOXEL_DATA_FLAGS_ACTIVE;
                    bool backVoxelSolid = chunk->voxels[(i * VOXEL_CHUNK_WIDTH * VOXEL_CHUNK_DEPTH) + j * VOXEL_CHUNK_WIDTH + (k - 1)].data & VOXEL_DATA_FLAGS_ACTIVE;
                    bool leftVoxelSolid = chunk->voxels[(i * VOXEL_CHUNK_WIDTH * VOXEL_CHUNK_DEPTH) + (j - 1) * VOXEL_CHUNK_WIDTH + k].data & VOXEL_DATA_FLAGS_ACTIVE;
                    bool rightVoxelSolid = chunk->voxels[(i * VOXEL_CHUNK_WIDTH * VOXEL_CHUNK_DEPTH) + (j + 1) * VOXEL_CHUNK_WIDTH + k].data & VOXEL_DATA_FLAGS_ACTIVE;

                    if (!(aboveVoxelSolid && belowVoxelSolid && leftVoxelSolid && rightVoxelSolid && frontVoxelSolid && backVoxelSolid)) {
                        generateMeshForVoxel(positions, &numPositions, texCoords, &numTexCoords, normals, &numNormals, indices, &numIndices, i, j, k);
                    }
                }
            }
        }
    }

    float_mesh_attribute positionAttribute = {};
    positionAttribute.values = (float *)positions;
    positionAttribute.count = numPositions * 3;
    float_mesh_attribute texCoordAttribute = {};
    texCoordAttribute.values = (float *)texCoords;
    texCoordAttribute.count = numTexCoords * 2;
    float_mesh_attribute normalAttribute = {};
    normalAttribute.values = (float *)normals;
    normalAttribute.count = numNormals * 3;
    int_mesh_attribute indexAttribute = {};
    indexAttribute.values = (unsigned int *)indices;
    indexAttribute.count = numIndices;

    generateDynamicModel(chunk->meshIDNode->meshID, positionAttribute, texCoordAttribute, normalAttribute, indexAttribute, 
                         renderCommands);
}

voxel_chunk *getChunkByXYZAndAllocate (test_3D_game *test3DGame, memory_arena *memory, unsigned int x, unsigned int y, unsigned int z) {
    // TODO(ebuchholz): better hash function
    voxel_chunk_hash_map *chunkMap = &test3DGame->chunkMap;
    unsigned int hashValue = (x * 100 + y * 27 + z * 3) % chunkMap->capacity;
    unsigned int originalHashValue = hashValue;

    voxel_chunk *chunk;
    while (true) {
        chunk = &chunkMap->chunks[hashValue];
        if (chunk->voxels == 0) {
            // TODO(ebuchholz): use some kind of free list or something instead of potentially allocating voxels for 
            // every entry in the hash map
            chunk->voxels = (voxel_data *)allocateMemorySize(memory, sizeof(voxel_data) * VOXEL_CHUNK_WIDTH * 
                                                                     VOXEL_CHUNK_DEPTH * VOXEL_CHUNK_HEIGHT);
            chunk->x = x;
            chunk->y = y;
            chunk->z = z;

            break;
        }
        else {
            if (!chunk->loaded) {
                chunk->x = x;
                chunk->y = y;
                chunk->z = z;
                break;
            }
            else {
                if (chunk->x == x && chunk->y == y && chunk->z == z) {
                    break;
                }
                else {
                    hashValue = (hashValue + 1) % chunkMap->capacity;
                    assert(hashValue != originalHashValue);
                }
            }
        }
    }

    return chunk;
}

voxel_chunk *getLoadedChunkByXYZ (test_3D_game *test3DGame, unsigned int x, unsigned int y, unsigned int z) {
    // TODO(ebuchholz): better hash function
    voxel_chunk_hash_map *chunkMap = &test3DGame->chunkMap;
    unsigned int hashValue = (x * 100 + y * 27 + z * 3) % chunkMap->capacity;
    unsigned int originalHashValue = hashValue;

    voxel_chunk *chunk;
    while (true) {
        chunk = &chunkMap->chunks[hashValue];
        if (chunk->voxels == 0) {
            return 0;
        }
        else {
            if (!chunk->loaded) {
                return 0;
            }
            else {
                if (chunk->x == x && chunk->y == y && chunk->z == z) {
                    return chunk;
                }
                else {
                    hashValue = (hashValue + 1) % chunkMap->capacity;
                    assert(hashValue != originalHashValue);
                    return 0; // should be unreachable if asserts are enabled
                }
            }
        }
    }
}

void freeChunkMeshIDNode (test_3D_game *test3DGame, voxel_chunk *chunk) {
    chunk->meshIDNode->next = test3DGame->meshIDList.firstFree;
    test3DGame->meshIDList.firstFree = chunk->meshIDNode;

    chunk->meshIDNode = 0;
}

bool updateChunkCoords (vector3 *pos, unsigned int *chunkX, unsigned int *chunkY, unsigned int *chunkZ) {
    bool result = false;
    while (pos->x >= 32.0f) {
        pos->x -= 32.0f;
        (*chunkX)++;
        result = true;
    }
    while (pos->x < 0.0f) {
        pos->x += 32.0f;
        (*chunkX)--;
        result = true;
    }
    while (pos->y >= 32.0f) {
        pos->y -= 32.0f;
        (*chunkY)++;
        result = true;
    }
    while (pos->y < 0.0f) {
        pos->y += 32.0f;
        (*chunkY)--;
        result = true;
    }
    while (pos->z >= 32.0f) {
        pos->z -= 32.0f;
        (*chunkZ)++;
        result = true;
    }
    while (pos->z < 0.0f) {
        pos->z += 32.0f;
        (*chunkZ)--;
        result = true;
    }

    return result;
}

bool updateChunkCoords (int *voxelX, int *voxelY, int *voxelZ, unsigned int *chunkX, unsigned int *chunkY, unsigned int *chunkZ) {
    bool result = false;
    while ((*voxelX) >= 32) {
        (*voxelX) -= 32;
        (*chunkX)++;
        result = true;
    }
    while ((*voxelX) < 0) {
        (*voxelX) += 32;
        (*chunkX)--;
        result = true;
    }
    while ((*voxelY) >= 32) {
        (*voxelY) -= 32;
        (*chunkY)++;
        result = true;
    }
    while ((*voxelY) < 0) {
        (*voxelY) += 32;
        (*chunkY)--;
        result = true;
    }
    while ((*voxelZ) >= 32) {
        (*voxelZ) -= 32;
        (*chunkZ)++;
        result = true;
    }
    while ((*voxelZ) < 0) {
        (*voxelZ) += 32;
        (*chunkZ)--;
        result = true;
    }

    return result;
}

plane frustumPlaneFromCorners (vector3 sharedPoint, vector3 firstPoint, vector3 secondPoint) {
    plane result = {};
    result.normal = normalize(crossProduct(firstPoint - sharedPoint, secondPoint - sharedPoint));
    result.distance = dotProduct(result.normal, sharedPoint);
    return result;
}

bool chunkIntersectsFrustumPlane (vector3 chunkOffset, plane frustumPlane) {
    vector3 chunkMin = chunkOffset;
    vector3 chunkMax = chunkMin + Vector3(32.0f, 32.0f, 32.0f);

    vector3 positiveTestPoint = chunkMin;
    if (frustumPlane.normal.x >= 0) {
        positiveTestPoint.x = chunkMax.x;
    }
    if (frustumPlane.normal.y >= 0) {
        positiveTestPoint.y = chunkMax.y;
    }
    if (frustumPlane.normal.z >= 0) {
        positiveTestPoint.z = chunkMax.z;
    }

    vector3 negativeTestPoint = chunkMax;
    if (frustumPlane.normal.x >= 0) {
        negativeTestPoint.x = chunkMin.x;
    }
    if (frustumPlane.normal.y >= 0) {
        negativeTestPoint.y = chunkMin.y;
    }
    if (frustumPlane.normal.z >= 0) {
        negativeTestPoint.z = chunkMin.z;
    }

    if (pointIsInFrontOfPlane(negativeTestPoint, frustumPlane)) {
        return false;
    } 
    return true;
}

void assignMeshToChunk (voxel_chunk *chunk, test_3D_game *test3DGame) {
    chunk->meshIDNode = test3DGame->meshIDList.firstFree;
    assert(chunk->meshIDNode);

    test3DGame->meshIDList.firstFree = test3DGame->meshIDList.firstFree->next;
    chunk->meshIDNode->next = 0;
}

void loadChunkWithSphereCavePattern (voxel_chunk *chunk, test_3D_game *test3DGame) {
    assignMeshToChunk(chunk, test3DGame);

    // TODO(ebuchholz): use some interesting noise function, or load from a file
    for (int i = 0; i < VOXEL_CHUNK_HEIGHT; ++i) {
        for (int j = 0; j < VOXEL_CHUNK_DEPTH; ++j) {
            for (int k = 0; k < VOXEL_CHUNK_WIDTH; ++k) {
                float worldI = 32.0f * (float)chunk->y + (float)i;
                float worldJ = 32.0f * (float)chunk->z + (float)j;
                float worldK = 32.0f * (float)chunk->x + (float)k;
                float crazyValue = sinf(worldI * 0.1f) + cosf(worldK * 0.1f) + sinf(worldJ * 0.1f);

                if (crazyValue > -0.3f) {
                    chunk->voxels[(i * VOXEL_CHUNK_WIDTH * VOXEL_CHUNK_DEPTH) + j * VOXEL_CHUNK_WIDTH + k].data = VOXEL_DATA_FLAGS_ACTIVE;
                }
                else {
                    chunk->voxels[(i * VOXEL_CHUNK_WIDTH * VOXEL_CHUNK_DEPTH) + j * VOXEL_CHUNK_WIDTH + k].data = 0;
                }
            }
        }
    }
    chunk->dirty = true;
    chunk->loaded = true;
}

void loadChunkWithEmptyVoxels (voxel_chunk * chunk, test_3D_game *test3DGame) {
    assignMeshToChunk(chunk, test3DGame);

    // TODO(ebuchholz): use some interesting noise function, or load from a file
    for (int i = 0; i < VOXEL_CHUNK_HEIGHT; ++i) {
        for (int j = 0; j < VOXEL_CHUNK_DEPTH; ++j) {
            for (int k = 0; k < VOXEL_CHUNK_WIDTH; ++k) {
                chunk->voxels[(i * VOXEL_CHUNK_WIDTH * VOXEL_CHUNK_DEPTH) + j * VOXEL_CHUNK_WIDTH + k].data = 0;
            }
        }
    }
    chunk->dirty = true;
    chunk->loaded = true;
}

void loadChunkWithFloorPattern (voxel_chunk * chunk, test_3D_game *test3DGame) {
    assignMeshToChunk(chunk, test3DGame);

    // TODO(ebuchholz): use some interesting noise function, or load from a file
    for (int i = 1; i < VOXEL_CHUNK_HEIGHT; ++i) {
        for (int j = 0; j < VOXEL_CHUNK_DEPTH; ++j) {
            for (int k = 0; k < VOXEL_CHUNK_WIDTH; ++k) {
                chunk->voxels[(i * VOXEL_CHUNK_WIDTH * VOXEL_CHUNK_DEPTH) + j * VOXEL_CHUNK_WIDTH + k].data = 0;
            }
        }
    }

    for (int j = 0; j < VOXEL_CHUNK_DEPTH; ++j) {
        for (int k = 0; k < VOXEL_CHUNK_WIDTH; ++k) {
            chunk->voxels[j * VOXEL_CHUNK_WIDTH + k].data = VOXEL_DATA_FLAGS_ACTIVE;
        }
    }
    chunk->dirty = true;
    chunk->loaded = true;
}

void update3DGame (memory_arena *memory, memory_arena *tempMemory, game_assets *assets, game_input *input, 
                   test_3D_game *test3DGame, sprite_list *spriteList, render_command_list * renderCommands)
{
    if (!test3DGame->initialized) {
        test3DGame->initialized =true;
        debug_camera *debugCamera = &test3DGame->debugCamera;
        debugCamera->pos = {};
        debugCamera->pos.x = 16.0f;
        debugCamera->pos.y = 2.0f;
        debugCamera->pos.z = 16.0f;
        debugCamera->rotation = quaternionFromAxisAngle(Vector3(0.0f, 1.0f, 0.0f), 0.0f);
        //debugCamera->rotation = 
        //    quaternionFromAxisAngle(Vector3(0.0f, 1.0f, 0.0f), 40.0f * (PI / 180.0f)) *
        //    quaternionFromAxisAngle(Vector3(1.0f, 0.0f, 0.0f), -33.0f * (PI / 180.0f));
        debugCamera->lastPointerX = 0;
        debugCamera->lastPointerY = 0;

        test3DGame->meshIDList = {};
        test3DGame->meshIDList.nodes[MAX_VOXEL_CHUNK_MESH_IDS - 1].meshID = MAX_VOXEL_CHUNK_MESH_IDS - 1;
        test3DGame->meshIDList.nodes[MAX_VOXEL_CHUNK_MESH_IDS - 1].next = 0;
        for (int i = MAX_VOXEL_CHUNK_MESH_IDS - 2; i >= 0; --i) {
            test3DGame->meshIDList.nodes[i].meshID = i;
            test3DGame->meshIDList.nodes[i].next = &test3DGame->meshIDList.nodes[i+1];
        }
        test3DGame->meshIDList.firstFree = &test3DGame->meshIDList.nodes[0];

        test3DGame->chunkMap = {};
        test3DGame->chunkMap.capacity = 2000;
        test3DGame->chunkMap.chunks = (voxel_chunk *)allocateMemorySize(memory, sizeof(voxel_chunk) * test3DGame->chunkMap.capacity);

        for (unsigned int i = 0; i < test3DGame->chunkMap.capacity; ++i) {
            test3DGame->chunkMap.chunks[i] = {};
        }

        test3DGame->playerChunkX = 4001;
        test3DGame->playerChunkY = 4001;
        test3DGame->playerChunkZ = 4001;
        test3DGame->playerPos = Vector3(16.0f, 16.0f, 16.0f);

        // init player chunk with a floor on the bottom
        //voxel_chunk *chunk = getChunkByXYZAndAllocate(test3DGame, memory, 
        //                                   test3DGame->playerChunkX, test3DGame->playerChunkY, test3DGame->playerChunkZ);
        //loadChunkWithFloorPattern(chunk, test3DGame);
    }

    // memory for dynamically created strings
    memory_arena stringMemory = {};
    stringMemory.capacity = 512 * 1024;
    stringMemory.base = allocateMemorySize(tempMemory, stringMemory.capacity);

    debugCameraMovement(&test3DGame->debugCamera, input);
    // use camera pos as player pos for now
    test3DGame->playerPos = test3DGame->debugCamera.pos;
    updateChunkCoords(&test3DGame->playerPos, &test3DGame->playerChunkX, &test3DGame->playerChunkY, &test3DGame->playerChunkZ);
    test3DGame->debugCamera.pos = test3DGame->playerPos;

    float screenWidth = (float)renderCommands->windowWidth;
    float screenHeight = (float)renderCommands->windowHeight;

    float nearViewDist = 0.1f;
    float farViewDist = 1000.0f;
    float hFOV = 80.0f;
    float viewRatio = screenWidth / screenHeight;

    float filmWidth = 2.0f * nearViewDist * tanf(((PI / 180.0f) * hFOV) / 2.0f);
    float filmHeight = filmWidth / viewRatio; 

    vector3 worldMousePos = Vector3((float)input->pointerX, (float)input->pointerY, -nearViewDist);
    worldMousePos.x *= 1.0f / renderCommands->windowWidth;
    worldMousePos.x -= 0.5f;
    worldMousePos.x *= filmWidth;

    worldMousePos.y *= 1.0f / renderCommands->windowHeight;
    worldMousePos.y -= 0.5f;
    worldMousePos.y *= -1;
    worldMousePos.y *= filmHeight;

    vector3 worldMouseDir = normalize(worldMousePos);
    worldMouseDir = rotateVectorByQuaternion(worldMouseDir, test3DGame->debugCamera.rotation);

    bool canPlaceBlock = false;
    int placeBlockX = 0;
    int placeBlockY = 0;
    int placeBlockZ = 0;
    int placeBlockChunkX = 0;
    int placeBlockChunkY = 0;
    int placeBlockChunkZ = 0;
    vector3 rayCastStart = test3DGame->debugCamera.pos + 0.1f * worldMouseDir;
    unsigned int chunkX = test3DGame->playerChunkX;
    unsigned int chunkY = test3DGame->playerChunkY;
    unsigned int chunkZ = test3DGame->playerChunkZ;
    updateChunkCoords(&rayCastStart, &chunkX, &chunkY, &chunkZ);
    voxel_chunk *rayCastChunk = getLoadedChunkByXYZ(test3DGame, chunkX, chunkY, chunkZ);
    if (rayCastChunk) {
        int voxelX = (int)floorf(rayCastStart.x);
        int voxelY = (int)floorf(rayCastStart.y);
        int voxelZ = (int)floorf(rayCastStart.z);
        int stepX = worldMouseDir.x > 0.0f ? 1 : -1;
        int stepY = worldMouseDir.y > 0.0f ? 1 : -1;
        int stepZ = worldMouseDir.z > 0.0f ? 1 : -1;
        float tMaxX = stepX > 0 ? (ceilf(rayCastStart.x) - rayCastStart.x) / worldMouseDir.x:
                                  (floorf(rayCastStart.x) - rayCastStart.x) / worldMouseDir.x;
        float tMaxY = stepY > 0 ? (ceilf(rayCastStart.y) - rayCastStart.y) / worldMouseDir.y:
                                  (floorf(rayCastStart.y) - rayCastStart.y) / worldMouseDir.y;
        float tMaxZ = stepZ > 0 ? (ceilf(rayCastStart.z) - rayCastStart.z) / worldMouseDir.z:
                                  (floorf(rayCastStart.z) - rayCastStart.z) / worldMouseDir.z;

        float tDeltaX = (float)stepX / worldMouseDir.x;
        float tDeltaY = (float)stepY / worldMouseDir.y;
        float tDeltaZ = (float)stepZ / worldMouseDir.z;
        do {
            placeBlockX = voxelX;
            placeBlockY = voxelY;
            placeBlockZ = voxelZ;
            placeBlockChunkX = chunkX;
            placeBlockChunkY = chunkY;
            placeBlockChunkZ = chunkZ;
            
            if (tMaxX < tMaxY) {
                if (tMaxX < tMaxZ) {
                    voxelX += stepX;
                    tMaxX += tDeltaX;
                }
                else {
                    voxelZ += stepZ;
                    tMaxZ += tDeltaZ;
                }
            }
            else {
                if (tMaxY < tMaxZ) {
                    voxelY += stepY;
                    tMaxY += tDeltaY;
                }
                else {
                    voxelZ += stepZ;
                    tMaxZ += tDeltaZ;
                }
            }
            bool changedChunks = updateChunkCoords(&voxelX, &voxelY, &voxelZ, &chunkX, &chunkY, &chunkZ);
            if (changedChunks) {
                rayCastChunk = getLoadedChunkByXYZ(test3DGame, chunkX, chunkY, chunkZ);
                if (!rayCastChunk) {
                    break; // went into chunk that wasn't loaded
                }
            }

            char voxelData = rayCastChunk->voxels[voxelY * VOXEL_CHUNK_DEPTH * VOXEL_CHUNK_WIDTH + voxelZ * VOXEL_CHUNK_WIDTH + voxelX].data;
            if (voxelData & VOXEL_DATA_FLAGS_ACTIVE) {
                if (input->fKey.justPressed) {
                    if (changedChunks) {
                        rayCastChunk = getLoadedChunkByXYZ(test3DGame, placeBlockChunkX, placeBlockChunkY, placeBlockChunkZ);
                        assert(rayCastChunk);
                    }
                    rayCastChunk->voxels[placeBlockY * VOXEL_CHUNK_DEPTH * VOXEL_CHUNK_WIDTH + placeBlockZ * VOXEL_CHUNK_WIDTH + placeBlockX].data = VOXEL_DATA_FLAGS_ACTIVE;
                    rayCastChunk->dirty = true;
                }
                else if (input->gKey.justPressed) {
                    rayCastChunk->voxels[voxelY * VOXEL_CHUNK_DEPTH * VOXEL_CHUNK_WIDTH + voxelZ * VOXEL_CHUNK_WIDTH + voxelX].data = 0;
                    rayCastChunk->dirty = true;
                }
                else {
                    canPlaceBlock = true;
                }
                break;
            }
        } while (true);
    }

    matrix4x4 projMatrix = createPerspectiveMatrix(nearViewDist, farViewDist, viewRatio, hFOV);
    matrix4x4 viewMatrix;
    if (input->kKey.justPressed) {
        test3DGame->showTestCamera = !test3DGame->showTestCamera;
    }
    if (!test3DGame->showTestCamera) {
        viewMatrix = createViewMatrix(test3DGame->debugCamera.rotation, 
                                                test3DGame->debugCamera.pos.x,
                                                test3DGame->debugCamera.pos.y,
                                                test3DGame->debugCamera.pos.z);
    }
    else {
        vector3 testCamPos = test3DGame->debugCamera.pos + Vector3(0.0f, 0.0f, 0.1f);
        viewMatrix = createLookAtMatrix(testCamPos.x,
                                        testCamPos.y,
                                        testCamPos.z,
                                        test3DGame->debugCamera.pos.x, 
                                        test3DGame->debugCamera.pos.y, 
                                        test3DGame->debugCamera.pos.z, 
                                        0.0f, 1.0f, 0.0f);
    }

    render_command_set_camera *setCameraCommand = 
        (render_command_set_camera *)pushRenderCommand(renderCommands,
                                                       RENDER_COMMAND_SET_CAMERA,
                                                       sizeof(render_command_set_camera));
    setCameraCommand->viewMatrix = viewMatrix;
    setCameraCommand->projMatrix = projMatrix;

    // frustum info for culling chunks
    frustum_corners fCorners = FrustumCorners(test3DGame->debugCamera.pos, test3DGame->debugCamera.rotation,
                                              nearViewDist, farViewDist, hFOV, viewRatio);
    plane frontFrustumPlane = frustumPlaneFromCorners(fCorners.nearLowerRight, fCorners.nearUpperRight, fCorners.nearLowerLeft);
    plane backFrustumPlane = frustumPlaneFromCorners(fCorners.farLowerLeft, fCorners.farUpperLeft, fCorners.farLowerRight);
    plane leftFrustumPlane = frustumPlaneFromCorners(fCorners.nearLowerLeft, fCorners.nearUpperLeft, fCorners.farLowerLeft); 
    plane rightFrustumPlane = frustumPlaneFromCorners(fCorners.farLowerRight, fCorners.farUpperRight, fCorners.nearLowerRight);
    plane topFrustumPlane = frustumPlaneFromCorners(fCorners.nearUpperRight, fCorners.farUpperRight, fCorners.nearUpperLeft);
    plane bottomFrustumPlane = frustumPlaneFromCorners(fCorners.nearLowerLeft, fCorners.farLowerLeft, fCorners.nearLowerRight);

    bool alreadyUpdatedChunk = false;
    int chunkViewDistance = 4;
    for (unsigned int y = test3DGame->playerChunkY - chunkViewDistance; y <= test3DGame->playerChunkY + chunkViewDistance; ++y) {
        for (unsigned int z = test3DGame->playerChunkZ - chunkViewDistance; z <= test3DGame->playerChunkZ + chunkViewDistance; ++z) {
            for (unsigned int x = test3DGame->playerChunkX - chunkViewDistance; x <= test3DGame->playerChunkX + chunkViewDistance; ++x) {
                vector3 chunkDistance = Vector3((float)(int)(x - test3DGame->playerChunkX),
                                                (float)(int)(y - test3DGame->playerChunkY),
                                                (float)(int)(z - test3DGame->playerChunkZ));

                if (length(chunkDistance) <= (float)chunkViewDistance) {
                    voxel_chunk *chunk = getChunkByXYZAndAllocate(test3DGame, memory, x, y, z);

                    if (!chunk->loaded) {
                        loadChunkWithSphereCavePattern(chunk, test3DGame);
                    }

                    vector3 chunkOffset = Vector3(chunkDistance.x * (float)VOXEL_CHUNK_WIDTH,
                                                  chunkDistance.y * (float)VOXEL_CHUNK_HEIGHT,
                                                  chunkDistance.z * (float)VOXEL_CHUNK_DEPTH);

                    if (chunk->dirty && !alreadyUpdatedChunk) {
                        generateMeshForVoxelChunk(chunk, tempMemory, renderCommands);
                        alreadyUpdatedChunk = true;
                        chunk->meshGenerated = true;
                        chunk->dirty = false;
                    }

                    if (chunk->meshGenerated) {
                        if (!chunkIntersectsFrustumPlane(chunkOffset, frontFrustumPlane)) {
                            continue;
                        } 
                        if (!chunkIntersectsFrustumPlane(chunkOffset, backFrustumPlane)) {
                            continue;
                        } 
                        if (!chunkIntersectsFrustumPlane(chunkOffset, leftFrustumPlane)) {
                            continue;
                        } 
                        if (!chunkIntersectsFrustumPlane(chunkOffset, rightFrustumPlane)) {
                            continue;
                        } 
                        if (!chunkIntersectsFrustumPlane(chunkOffset, topFrustumPlane)) {
                            continue;
                        } 
                        if (!chunkIntersectsFrustumPlane(chunkOffset, bottomFrustumPlane)) {
                            continue;
                        } 

                        drawDynamicModel(chunk->meshIDNode->meshID, "green_frame", translationMatrix(chunkOffset), renderCommands, assets);
                    }
                }

            }
        }
    }

    for (unsigned int i = 0; i < test3DGame->chunkMap.capacity; ++i) {
        voxel_chunk *chunk = &test3DGame->chunkMap.chunks[i];
        if (chunk->loaded) {
            vector3 chunkDistance = Vector3((float)(int)(chunk->x - test3DGame->playerChunkX),
                                            (float)(int)(chunk->y - test3DGame->playerChunkY),
                                            (float)(int)(chunk->z - test3DGame->playerChunkZ));

            if (length(chunkDistance) > (float)chunkViewDistance) {
                chunk->loaded = false;
                chunk->meshGenerated = false;
                freeChunkMeshIDNode(test3DGame, chunk);
            }
        }
    }

    if (canPlaceBlock) {
        // TODO(ebuchholz): helper functions for chunk coords
        vector3 chunkDistance = Vector3((float)(int)(placeBlockChunkX - test3DGame->playerChunkX),
                                        (float)(int)(placeBlockChunkY - test3DGame->playerChunkY),
                                        (float)(int)(placeBlockChunkZ - test3DGame->playerChunkZ));

        vector3 chunkOffset = Vector3(chunkDistance.x * (float)VOXEL_CHUNK_WIDTH + (float)placeBlockX,
                                      chunkDistance.y * (float)VOXEL_CHUNK_HEIGHT + (float)placeBlockY,
                                      chunkDistance.z * (float)VOXEL_CHUNK_DEPTH + (float)placeBlockZ);
        chunkOffset.z += 1.0f; // "voxel_cube" model is off by 1 (z's are negative but should be positive)


        matrix4x4 transform = translationMatrix(chunkOffset);
        drawModel("voxel_cube", "blue", transform, renderCommands, assets);
    }

    if (test3DGame->showTestCamera) {
        drawViewFrustum(test3DGame->debugCamera.pos, test3DGame->debugCamera.rotation, 
                        nearViewDist, farViewDist, hFOV, viewRatio, renderCommands);
    }

    //for (int i = 4; i < VOXEL_CHUNK_HEIGHT; ++i) {
    //    for (int j = 0; j < VOXEL_CHUNK_DEPTH; ++j) {
    //        for (int k = 0; k < VOXEL_CHUNK_WIDTH; ++k) {
    //            float sineHeight = (i-16) * (1.0f / 8.0f) - 1.0f;
    //            float sineVal = -0.5f + 0.5f * sinf(test3DGame->cubeT * 8.0f + (j / 8.0f)) + 0.5f * cosf(test3DGame->cubeT * 4.0f + 0.5f + (k / 8.0f));
    //            if (sineHeight < sineVal) {
    //                test3DGame->chunk.voxels[(i * VOXEL_CHUNK_WIDTH * VOXEL_CHUNK_DEPTH) + j * VOXEL_CHUNK_WIDTH + k].data = VOXEL_DATA_FLAGS_ACTIVE;
    //            }
    //            else {
    //                test3DGame->chunk.voxels[(i * VOXEL_CHUNK_WIDTH * VOXEL_CHUNK_DEPTH) + j * VOXEL_CHUNK_WIDTH + k].data = 0;
    //            }
    //        }
    //    }
    //}
    //test3DGame->chunk.dirty = true;

    //if (test3DGame->chunk.dirty) {
    //    generateMeshForVoxelChunk(&test3DGame->chunk, tempMemory, renderCommands);
    //    test3DGame->chunk.dirty = false;
    //}
    //drawDynamicModel(test3DGame->chunk.meshID, "green_frame", identityMatrix4x4(), renderCommands, assets);

    //voxel_data *voxels = test3DGame->chunk.voxels;
    //for (int i = 0; i < VOXEL_CHUNK_HEIGHT; ++i) {
    //    for (int j = 0; j < VOXEL_CHUNK_DEPTH; ++j) {
    //        for (int k = 0; k < VOXEL_CHUNK_WIDTH; ++k) {
    //            char voxelValue = voxels[(i * VOXEL_CHUNK_WIDTH * VOXEL_CHUNK_DEPTH) + j * VOXEL_CHUNK_WIDTH + k].data;
    //            if (voxelValue & VOXEL_DATA_FLAGS_ACTIVE) {
    //                matrix4x4 transform = translationMatrix(k * 1.0f, i * 1.0f, j * 1.0f);
    //                drawModel("voxel_cube", "green_frame", transform, renderCommands, assets);
    //            }
    //        }
    //    }
    //}

    //test3DGame->cubeT += DELTA_TIME;

    //matrix4x4 transform = translationMatrix(0.0f, 0.0f, -6.0f) *
    //                      rotationMatrixFromAxisAngle(Vector3(0.0f, 1.0f, 0.0f), test3DGame->cubeT);

    //drawModel("voxel_cube", "green_frame", transform, renderCommands, assets);
}
