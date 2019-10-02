#include "quickmake_assets.h"

void parseOBJ (void *objData, game_assets *assets, int key, memory_arena *workingMemory) {
    int numMeshes = assets->numMeshes;
    assert(numMeshes < MAX_NUM_MESHES);

    mesh_asset *meshAsset = (mesh_asset *)allocateMemorySize(&assets->assetMemory, sizeof(mesh_asset));
    assets->meshes[key] = meshAsset;
    assets->numMeshes++;
    meshAsset->key = (mesh_key)key;

    loaded_mesh_asset *loadedMesh = (loaded_mesh_asset *)allocateMemorySize(workingMemory, sizeof(loaded_mesh_asset));
    loadedMesh->key = key;

    char *start, *end;
    start = (char *)objData;

    unsigned int numPositions = 0;
    unsigned int numNormals = 0;
    unsigned int numTexCoords = 0;
    unsigned int numTriangles = 0;

    while (true) {
        readLine(start, &start, &end);

        char *wordStart = start;
        char *wordEnd = wordStart;
        while (*wordEnd != ' ' && wordEnd != end) { ++wordEnd; }
        int length = (int)(wordEnd - wordStart);

        if (stringsAreEqual(length, wordStart, "v")) {
            ++numPositions;
        }
        else if (stringsAreEqual(length, wordStart, "vt")) {
            ++numTexCoords;
        }
        else if (stringsAreEqual(length, wordStart, "vn")) {
            ++numNormals;
        }
        else if (stringsAreEqual(length, wordStart, "f")) {
            ++numTriangles;
        }

        start = end;
        if (*start == 0) {
            break;
        }
        else {
            if (*start == '\r') {
                start += 2;
            }
            else {
                start++;
            }
        }
    }

    float (*tempPositions)[3] = (float (*)[3])allocateMemorySize(workingMemory, sizeof(float) * 3 * numPositions);
    float (*tempTexCoords)[2] = (float (*)[2])allocateMemorySize(workingMemory, sizeof(float) * 2 * numTexCoords);
    float (*tempNormals)[3] = (float (*)[3])allocateMemorySize(workingMemory, sizeof(float) * 3 * numNormals);

    loadedMesh->positions.count = 3 * 3 * numTriangles;
    loadedMesh->texCoords.count = 2 * 3 * numTriangles;
    loadedMesh->normals.count = 3 * 3 * numTriangles;
    loadedMesh->indices.count =  3 * numTriangles;

    loadedMesh->positions.values = allocateMemorySize(workingMemory, sizeof(float) * loadedMesh->positions.count);
    loadedMesh->texCoords.values = allocateMemorySize(workingMemory, sizeof(float) * loadedMesh->texCoords.count);
    loadedMesh->normals.values = allocateMemorySize(workingMemory, sizeof(float) * loadedMesh->normals.count);
    loadedMesh->indices.values = allocateMemorySize(workingMemory, sizeof(int) * loadedMesh->indices.count);

    start = (char *)objData;
    int tempPositionIndex = 0;
    int tempTexCoordIndex = 0;
    int tempNormalIndex = 0;

    int positionIndex = 0;
    int uvIndex = 0;
    int normalIndex = 0;
    int indexIndex = 0;
    while (true) {
        readLine(start, &start, &end);

        char *wordStart = start;
        char *wordEnd = wordStart;
        while (*wordEnd != ' ' && wordEnd != end) { ++wordEnd; }
        int length = (int)(wordEnd - wordStart);

        if (stringsAreEqual(length, wordStart, "v")) {
            float (*tempPosition)[3] = &tempPositions[tempPositionIndex];
            wordEnd++;
            wordStart = wordEnd;
            while (*wordEnd != ' ' && wordEnd != end) { ++wordEnd; }
            (*tempPosition)[0] = stringToFloat(wordStart, wordEnd - 1);

            wordEnd++;
            wordStart = wordEnd;
            while (*wordEnd != ' ' && wordEnd != end) { ++wordEnd; }
            (*tempPosition)[1] = stringToFloat(wordStart, wordEnd - 1);

            wordEnd++;
            wordStart = wordEnd;
            while (*wordEnd != ' ' && wordEnd != end) { ++wordEnd; }
            (*tempPosition)[2] = stringToFloat(wordStart, wordEnd - 1);

            tempPositionIndex++;
        }
        else if (stringsAreEqual(length, wordStart, "vt")) {
            float (*tempTexCoord)[2] = &tempTexCoords[tempTexCoordIndex];
            wordEnd++;
            wordStart = wordEnd;
            while (*wordEnd != ' ' && wordEnd != end) { ++wordEnd; }
            (*tempTexCoord)[0] = stringToFloat(wordStart, wordEnd - 1);

            wordEnd++;
            wordStart = wordEnd;
            while (*wordEnd != ' ' && wordEnd != end) { ++wordEnd; }
            (*tempTexCoord)[1] = stringToFloat(wordStart, wordEnd - 1);

            tempTexCoordIndex++;
        }
        else if (stringsAreEqual(length, wordStart, "vn")) {
            float (*tempNormal)[3] = &tempNormals[tempNormalIndex];
            wordEnd++;
            wordStart = wordEnd;
            while (*wordEnd != ' ' && wordEnd != end) { ++wordEnd; }
            (*tempNormal)[0] = stringToFloat(wordStart, wordEnd - 1);

            wordEnd++;
            wordStart = wordEnd;
            while (*wordEnd != ' ' && wordEnd != end) { ++wordEnd; }
            (*tempNormal)[1] = stringToFloat(wordStart, wordEnd - 1);

            wordEnd++;
            wordStart = wordEnd;
            while (*wordEnd != ' ' && wordEnd != end) { ++wordEnd; }
            (*tempNormal)[2] = stringToFloat(wordStart, wordEnd - 1);

            tempNormalIndex++;
        }
        else if (stringsAreEqual(length, wordStart, "f")) {
            // TODO(ebuchholz): handle the case where text coords and/or normals are missing
            for (int i = 0; i < 3; ++i) {
                wordEnd++;
                wordStart = wordEnd;
                while (*wordEnd != ' ' && wordEnd != end) { ++wordEnd; }

                char *indexStart = wordStart;
                char *indexEnd = indexStart;
                while (*indexEnd != ' ' && indexEnd != end && *indexEnd != '/') { ++indexEnd; }
                if (indexStart != indexEnd) {
                    int index = stringToInt(indexStart, indexEnd - 1) - 1;
                    float *positionsPointer = (float *)loadedMesh->positions.values + (positionIndex * 3);
                    positionsPointer[0] = tempPositions[index][0];
                    positionsPointer[1] = tempPositions[index][1];
                    positionsPointer[2] = tempPositions[index][2];
                    ++positionIndex;
                }
                indexEnd++;
                indexStart = indexEnd;

                while (*indexEnd != ' ' && indexEnd != end && *indexEnd != '/') { ++indexEnd; }
                if (indexStart != indexEnd) {
                    int index = stringToInt(indexStart, indexEnd - 1) - 1;
                    float *texCoordsPointer = (float *)loadedMesh->texCoords.values + (uvIndex * 2);
                    texCoordsPointer[0] = tempTexCoords[index][0];
                    texCoordsPointer[1] = tempTexCoords[index][1];
                    uvIndex++;
                }
                indexEnd++;
                indexStart = indexEnd;

                while (*indexEnd != ' ' && indexEnd != end && *indexEnd != '/') { ++indexEnd; }
                if (indexStart != indexEnd) {
                    int index = stringToInt(indexStart, indexEnd - 1) - 1;
                    float *normalsPointer = (float *)loadedMesh->normals.values + (normalIndex * 3);
                    normalsPointer[0] = tempNormals[index][0];
                    normalsPointer[1] = tempNormals[index][1];
                    normalsPointer[2] = tempNormals[index][2];
                    ++normalIndex;
                }

                ((int *)(loadedMesh->indices.values))[indexIndex] = indexIndex;
                ++indexIndex;
            }
        }

        start = end;
        if (*start == 0) {
            break;
        }
        else {
            if (*start == '\r') {
                start += 2;
            }
            else {
                start++;
            }
        }
    }
    //for (int i = 0; i < (int)numTriangles; ++i) {
    //    int temp = ((int *)loadedMesh->indices.values)[i*3+1];
    //    ((int *)loadedMesh->indices.values)[i*3+1] = ((int *)loadedMesh->indices.values)[i*3+2];
    //    ((int *)loadedMesh->indices.values)[i*3+2] = temp;
    //}
}

// TODO(ebuchholz): actually parse a custom file type, rather than this hard coded stuff
void parseQMM (void *fileData, game_assets *assets, int key, memory_arena *workingMemory) {
    int numAnimatedMeshes = assets->numAnimatedMeshes;
    assert(numAnimatedMeshes < MAX_NUM_ANIMATED_MESHES);

    animated_mesh_asset *animatedMeshAsset = (animated_mesh_asset *)allocateMemorySize(&assets->assetMemory, sizeof(animated_mesh_asset));
    assets->animatedMeshes[key] = animatedMeshAsset;
    assets->numAnimatedMeshes++;
    animatedMeshAsset->key = (animated_mesh_key)key;

    loaded_animated_mesh_asset *loadedMesh = (loaded_animated_mesh_asset *)allocateMemorySize(workingMemory, sizeof(loaded_animated_mesh_asset));
    loadedMesh->key = key;

    switch (key) {
        default: {
            assert(false);
        } break;
        case ANIM_MESH_KEY_TUBE_SNAKE: {
            // generate a long cylindrical tube
            int numSegments = 10;
            float segmentLength = 1.0f;
            float totalLength = numSegments * segmentLength;
            int numRingsPerSegment = 4;
            float distBetweenRings = segmentLength / (float)numRingsPerSegment;

            int numVerticesPerRing = 8; // +1, duplicated on bottom
            int numRings = numRingsPerSegment * numSegments + 1;

            loadedMesh->positions.count = (numVerticesPerRing + 1) * 3 * numRings;
            loadedMesh->texCoords.count = (numVerticesPerRing + 1) * 2 * numRings;
            loadedMesh->normals.count = (numVerticesPerRing + 1) * 3 * numRings;
            loadedMesh->boneIndices.count =  (numVerticesPerRing + 1) * 4 * numRings;
            loadedMesh->boneWeights.count =  (numVerticesPerRing + 1) * 4 * numRings;
            loadedMesh->indices.count =  6 * numVerticesPerRing * (numRings-1);

            loadedMesh->positions.values = allocateMemorySize(workingMemory, sizeof(float) * loadedMesh->positions.count);
            loadedMesh->texCoords.values = allocateMemorySize(workingMemory, sizeof(float) * loadedMesh->texCoords.count);
            loadedMesh->normals.values = allocateMemorySize(workingMemory, sizeof(float) * loadedMesh->normals.count);
            loadedMesh->boneIndices.values = allocateMemorySize(workingMemory, sizeof(float) * loadedMesh->boneIndices.count);
            loadedMesh->boneWeights.values = allocateMemorySize(workingMemory, sizeof(float) * loadedMesh->boneWeights.count);
            loadedMesh->indices.values = allocateMemorySize(workingMemory, sizeof(int) * loadedMesh->indices.count);

            for (int ringIndex = 0; ringIndex < numRings; ++ringIndex) {
                for (int vertexIndex = 0; vertexIndex < numVerticesPerRing + 1; ++vertexIndex) {
                    float angle = 2.0f * PI * ((float)vertexIndex / (float)numVerticesPerRing);
                    vector3 pos = Vector3();
                    pos.x = -sinf(angle) * 0.125f;
                    pos.y = -cosf(angle) * 0.125f;
                    pos.z = ((float)ringIndex / (float)numRingsPerSegment) * segmentLength;

                    float u = (float)ringIndex / (float)(numRings-1);
                    float v = (float)vertexIndex / (float)numVerticesPerRing;

                    vector3 normal = pos;
                    normal.z = 0.0f;
                    normal = normalize(normal);

                    vector4 boneIndices;
                    float floatBoneIndex = (float)ringIndex / (float)numRingsPerSegment;
                    boneIndices.x = floorf(floatBoneIndex - 0.5f);
                    boneIndices.y = ceilf(floatBoneIndex - 0.5f);

                    if (boneIndices.x < 0.0f) { boneIndices.x = 0.0f; }
                    if (boneIndices.x > (float)(numSegments - 1)) { boneIndices.x = (float)(numSegments - 1); }
                    if (boneIndices.y > (float)(numSegments - 1)) { boneIndices.y = (float)(numSegments - 1); }
                    boneIndices.z = 0.0f;
                    boneIndices.w = 0.0f;

                    vector4 boneWeights;
                    boneWeights.y = (floatBoneIndex - 0.5f) - boneIndices.x;
                    if (boneWeights.y < 0.0f) { boneWeights.y = 0.0f; }
                    boneWeights.x = 1.0f - boneWeights.y;
                    boneWeights.z = 0.0f;
                    boneWeights.w = 0.0f;

                    float *positions = (float *)loadedMesh->positions.values + 3 * (ringIndex * (numVerticesPerRing + 1) + vertexIndex);
                    positions[0] = pos.x;
                    positions[1] = pos.y;
                    positions[2] = pos.z;

                    float *texCoords = (float *)loadedMesh->texCoords.values + 2 * (ringIndex * (numVerticesPerRing + 1) + vertexIndex);
                    texCoords[0] = u;
                    texCoords[1] = v;

                    float *normals = (float *)loadedMesh->normals.values + 3 * (ringIndex * (numVerticesPerRing + 1) + vertexIndex);
                    normals[0] = normal.x;
                    normals[1] = normal.y;
                    normals[2] = normal.z;

                    float *boneIndexArray = (float *)loadedMesh->boneIndices.values + 4 * (ringIndex * (numVerticesPerRing + 1) + vertexIndex);
                    boneIndexArray[0] = (float)boneIndices.x;
                    boneIndexArray[1] = (float)boneIndices.y;
                    boneIndexArray[2] = (float)boneIndices.z;
                    boneIndexArray[3] = (float)boneIndices.w;

                    float *boneWeightArray = (float *)loadedMesh->boneWeights.values + 4 * (ringIndex * (numVerticesPerRing + 1) + vertexIndex);
                    boneWeightArray[0] = (float)boneWeights.x;
                    boneWeightArray[1] = (float)boneWeights.y;
                    boneWeightArray[2] = (float)boneWeights.z;
                    boneWeightArray[3] = (float)boneWeights.w;
                }
            }
            
            int numIndices = 0;
            int *indices = (int *)loadedMesh->indices.values;
            for (int ringIndex = 0; ringIndex < numRings - 1; ++ringIndex) {
                for (int vertexIndex = 0; vertexIndex < numVerticesPerRing; ++vertexIndex) {
                    int vertexNum = (numVerticesPerRing + 1) * ringIndex + vertexIndex;
                    indices[numIndices++] = vertexNum;
                    indices[numIndices++] = vertexNum + 10;
                    indices[numIndices++] = vertexNum + 1;
                    indices[numIndices++] = vertexNum + 0;
                    indices[numIndices++] = vertexNum + 9;
                    indices[numIndices++] = vertexNum + 10;
                }
            }
        } break;
    }
}

void parseBitmap (void *fileData, game_assets *assets, int key, memory_arena *workingMemory) {
    int numTextures = assets->numTextures;
    assert(numTextures < MAX_NUM_TEXTURES);

    texture_asset *textureAsset = (texture_asset *)allocateMemorySize(&assets->assetMemory, sizeof(texture_asset)); 
    assets->textures[key] = textureAsset;
    assets->numTextures++;

    loaded_texture_asset *loadedBitmap = (loaded_texture_asset *)allocateMemorySize(workingMemory, sizeof(loaded_texture_asset));
    loadedBitmap->key = key;

    bitmap_header *header = (bitmap_header *)fileData;    
    unsigned int *pixels = (unsigned int *)((char *)fileData + header->bitmapOffset);

    int width = header->width;
    int height = header->height;
    textureAsset->width = width;
    textureAsset->height = height;
    loadedBitmap->width = width;
    loadedBitmap->height = height;
    unsigned char *bitmapPixels = (unsigned char *)allocateMemorySize(workingMemory, sizeof(unsigned char) * 4 * width * height);
    loadedBitmap->pixels = (unsigned int *)bitmapPixels;

    assert(header->bitsPerPixel == 32 && header->compression == 3);

    unsigned int alphaMask = ~(header->redMask | header->greenMask | header->blueMask);

    bool foundLeastSignificantDigit = false;
    unsigned int redMaskDigit = findLeastSignificantBit(header->redMask, &foundLeastSignificantDigit);
    assert(foundLeastSignificantDigit);
    unsigned int greenMaskDigit = findLeastSignificantBit(header->greenMask, &foundLeastSignificantDigit);
    assert(foundLeastSignificantDigit);
    unsigned int blueMaskDigit = findLeastSignificantBit(header->blueMask, &foundLeastSignificantDigit);
    assert(foundLeastSignificantDigit);
    unsigned int alphaMaskDigit = findLeastSignificantBit(alphaMask, &foundLeastSignificantDigit);
    assert(foundLeastSignificantDigit);

    unsigned int numPixelValues = width * height;
    unsigned int numBitmapValues = 0;
    for (unsigned int i = 0; i < numPixelValues; i++) {
        unsigned int color = pixels[i];

        unsigned char red = (unsigned char)((color & header->redMask) >> redMaskDigit);
        unsigned char green = (unsigned char)((color & header->greenMask) >> greenMaskDigit);
        unsigned char blue = (unsigned char)((color & header->blueMask) >> blueMaskDigit);
        unsigned char alpha = (unsigned char)((color & alphaMask) >> alphaMaskDigit);

        bitmapPixels[numBitmapValues] = red;
        bitmapPixels[numBitmapValues+1] = green;
        bitmapPixels[numBitmapValues+2] = blue;
        bitmapPixels[numBitmapValues+3] = alpha;
        numBitmapValues += 4;
    }
}

void parseAnimationData (void *fileData, game_assets *assets, int key, memory_arena *workingMemory) {
    // ignore file data for now
    int numAnimationData = assets->numAnimationData;
    assert(numAnimationData < MAX_NUM_ANIMATION_DATA);

    animation_data *animationDataAsset = (animation_data *)allocateMemorySize(&assets->assetMemory, sizeof(animation_data)); 
    assets->animationData[key] = animationDataAsset;
    assets->numAnimationData++;
    animationDataAsset->key = (animation_data_key)key;

    // TODO(ebuchholz): replace with actual file parsing
    switch (key) {
        case ANIMATION_DATA_KEY_LEGS: {
            // set up bone hierarchy
            int numBones = 9;
            animationDataAsset->numBones = numBones;
            animationDataAsset->boneHierarchy = (int *)allocateMemorySize(&assets->assetMemory, sizeof(int) * numBones);
            animationDataAsset->inverseRestTransforms = (matrix4x4 *)allocateMemorySize(&assets->assetMemory, sizeof(matrix4x4) * numBones);

            int *boneHierarchy = animationDataAsset->boneHierarchy;
            boneHierarchy[0] = -1;
            boneHierarchy[1] = 0;
            boneHierarchy[2] = 0;
            boneHierarchy[3] = 1;
            boneHierarchy[4] = 2;
            boneHierarchy[5] = 3;
            boneHierarchy[6] = 4;
            boneHierarchy[7] = 5;
            boneHierarchy[8] = 6;

            // set up poses, t-pose and walking animation poses
            int numPoses = 9;
            animationDataAsset->numPoses = numPoses;
            animationDataAsset->poses = (skeleton_pose *)allocateMemorySize(&assets->assetMemory, sizeof(skeleton_pose) * numPoses);

            skeleton_pose *poses = animationDataAsset->poses;
            for (int i = 0; i < numPoses; ++i) {
                skeleton_pose *pose = &poses[i];
                pose->numBones = numBones;
                pose->bonePoses = (skeleton_bone_pose *)allocateMemorySize(&assets->assetMemory, sizeof(skeleton_bone_pose) * numBones);
            }

            // rest pose
            skeleton_pose *restPose = &poses[0];
            skeleton_bone_pose *bonePose = &restPose->bonePoses[0];
            bonePose->boneID = 0;
            bonePose->localPos = Vector3(0.0f, 0.5f, 0.0f);
            bonePose->localRotation = Quaternion();

            // hips
            bonePose = &restPose->bonePoses[1];
            bonePose->boneID = 1;
            bonePose->localPos = Vector3(0.0f, 0.0f, 0.0f);
            bonePose->localRotation = quaternionFromAxisAngle(Vector3(0.0f, 1.0f, 0.0f), -PI/2.0f);
            
            bonePose = &restPose->bonePoses[2];
            bonePose->boneID = 2;
            bonePose->localPos = Vector3(0.0f, 0.0f, 0.0f);
            bonePose->localRotation = quaternionFromAxisAngle(Vector3(0.0f, 1.0f, 0.0f), +PI/2.0f);

            // thighs
            bonePose = &restPose->bonePoses[3];
            bonePose->boneID = 3;
            bonePose->localPos = Vector3(0.0f, 0.0f, 1.0f);
            bonePose->localRotation = quaternionFromAxisAngle(Vector3(0.0f, 1.0f, 0.0f), +PI/2.0f) *
                                      quaternionFromAxisAngle(Vector3(1.0f, 0.0f, 0.0f), +PI/2.0f);

            bonePose = &restPose->bonePoses[4];
            bonePose->boneID = 4;
            bonePose->localPos = Vector3(0.0f, 0.0f, 1.0f);
            bonePose->localRotation = quaternionFromAxisAngle(Vector3(0.0f, 1.0f, 0.0f), -PI/2.0f) *
                                      quaternionFromAxisAngle(Vector3(1.0f, 0.0f, 0.0f), +PI/2.0f);
            
            // shins
            bonePose = &restPose->bonePoses[5];
            bonePose->boneID = 5;
            bonePose->localPos = Vector3(0.0f, 0.0f, 1.0f);
            bonePose->localRotation = Quaternion();
            
            bonePose = &restPose->bonePoses[6];
            bonePose->boneID = 6;
            bonePose->localPos = Vector3(0.0f, 0.0f, 1.0f);
            bonePose->localRotation = Quaternion();

            // feet
            bonePose = &restPose->bonePoses[7];
            bonePose->boneID = 7;
            bonePose->localPos = Vector3(0.0f, 0.0f, 1.0f);
            bonePose->localRotation = quaternionFromAxisAngle(Vector3(1.0f, 0.0f, 0.0f), -PI/2.0f);

            bonePose = &restPose->bonePoses[8];
            bonePose->boneID = 8;
            bonePose->localPos = Vector3(0.0f, 0.0f, 1.0f);
            bonePose->localRotation = quaternionFromAxisAngle(Vector3(1.0f, 0.0f, 0.0f), -PI/2.0f);

            // calculate inverse rest transforms for bones
            // TODO(ebuchholz): determine if bones should have their own rotation/position properties
            matrix4x4 *boneTransforms = (matrix4x4 *)allocateMemorySize(workingMemory, sizeof(matrix4x4) * numBones);
            for (int i = 0; i < numBones; ++i) {
                matrix4x4 *currentBoneMatrix = &boneTransforms[i];
                skeleton_bone_pose *currentBonePose = &restPose->bonePoses[i];
                assert(i == currentBonePose->boneID);

                matrix4x4 localTransform = matrix4x4FromQuaternion(currentBonePose->localRotation);
                localTransform = translationMatrix(currentBonePose->localPos) * localTransform;

                int parentID = boneHierarchy[i];
                if (parentID == -1) {
                    *currentBoneMatrix = localTransform;
                }
                else {
                    matrix4x4 *parentMatrix = &boneTransforms[parentID];
                    *currentBoneMatrix = *parentMatrix * localTransform;
                }
                animationDataAsset->inverseRestTransforms[i] = inverse(*currentBoneMatrix);
            }

            // left foot passing right
            // root
            skeleton_pose *currentPose = &poses[1];
            bonePose = &currentPose->bonePoses[0];
            bonePose->boneID = 0;
            bonePose->localPos = Vector3(0.0f, 0.5f, 0.0f);
            bonePose->localRotation = Quaternion();

            // hips
            bonePose = &currentPose->bonePoses[1];
            bonePose->boneID = 1;
            bonePose->localPos = Vector3(0.0f, 0.0f, 0.0f);
            bonePose->localRotation = quaternionFromAxisAngle(Vector3(0.0f, 1.0f, 0.0f), -PI/2.0f);
            
            bonePose = &currentPose->bonePoses[2];
            bonePose->boneID = 2;
            bonePose->localPos = Vector3(0.0f, 0.0f, 0.0f);
            bonePose->localRotation = quaternionFromAxisAngle(Vector3(0.0f, 1.0f, 0.0f), +PI/2.0f);

            // thighs
            bonePose = &currentPose->bonePoses[3];
            bonePose->boneID = 3;
            bonePose->localPos = Vector3(0.0f, 0.0f, 1.0f);
            bonePose->localRotation = quaternionFromAxisAngle(Vector3(0.0f, 1.0f, 0.0f), +PI/2.0f) *
                                      quaternionFromAxisAngle(Vector3(1.0f, 0.0f, 0.0f), +PI/2.0f + 0.1f);

            bonePose = &currentPose->bonePoses[4];
            bonePose->boneID = 4;
            bonePose->localPos = Vector3(0.0f, 0.0f, 1.0f);
            bonePose->localRotation = quaternionFromAxisAngle(Vector3(0.0f, 1.0f, 0.0f), -PI/2.0f) *
                                      quaternionFromAxisAngle(Vector3(1.0f, 0.0f, 0.0f), +PI/2.0f - 0.25f);
            
            // shins
            bonePose = &currentPose->bonePoses[5];
            bonePose->boneID = 5;
            bonePose->localPos = Vector3(0.0f, 0.0f, 1.0f);
            bonePose->localRotation = Quaternion();
            
            bonePose = &currentPose->bonePoses[6];
            bonePose->boneID = 6;
            bonePose->localPos = Vector3(0.0f, 0.0f, 1.0f);
            bonePose->localRotation = quaternionFromAxisAngle(Vector3(1.0f, 0.0f, 0.0f), 1.25f);

            // feet
            bonePose = &currentPose->bonePoses[7];
            bonePose->boneID = 7;
            bonePose->localPos = Vector3(0.0f, 0.0f, 1.0f);
            bonePose->localRotation = quaternionFromAxisAngle(Vector3(1.0f, 0.0f, 0.0f), -PI/2.0f - 0.1f);

            bonePose = &currentPose->bonePoses[8];
            bonePose->boneID = 8;
            bonePose->localPos = Vector3(0.0f, 0.0f, 1.0f);
            bonePose->localRotation = quaternionFromAxisAngle(Vector3(1.0f, 0.0f, 0.0f), -PI/2.0f + 0.3f);

            // left foot up frame
            currentPose = &poses[2];
            bonePose = &currentPose->bonePoses[0];
            bonePose->boneID = 0;
            bonePose->localPos = Vector3(0.0f, 0.6f, 0.0f);
            bonePose->localRotation = Quaternion();

            // hips
            bonePose = &currentPose->bonePoses[1];
            bonePose->boneID = 1;
            bonePose->localPos = Vector3(0.0f, 0.0f, 0.0f);
            bonePose->localRotation = quaternionFromAxisAngle(Vector3(0.0f, 1.0f, 0.0f), -PI/2.0f);
            
            bonePose = &currentPose->bonePoses[2];
            bonePose->boneID = 2;
            bonePose->localPos = Vector3(0.0f, 0.0f, 0.0f);
            bonePose->localRotation = quaternionFromAxisAngle(Vector3(0.0f, 1.0f, 0.0f), +PI/2.0f);

            // thighs
            bonePose = &currentPose->bonePoses[3];
            bonePose->boneID = 3;
            bonePose->localPos = Vector3(0.0f, 0.0f, 1.0f);
            bonePose->localRotation = quaternionFromAxisAngle(Vector3(0.0f, 1.0f, 0.0f), +PI/2.0f) *
                                      quaternionFromAxisAngle(Vector3(1.0f, 0.0f, 0.0f), +PI/2.0f + 0.4f);

            bonePose = &currentPose->bonePoses[4];
            bonePose->boneID = 4;
            bonePose->localPos = Vector3(0.0f, 0.0f, 1.0f);
            bonePose->localRotation = quaternionFromAxisAngle(Vector3(0.0f, 1.0f, 0.0f), -PI/2.0f) *
                                      quaternionFromAxisAngle(Vector3(1.0f, 0.0f, 0.0f), +PI/2.0f - 0.55f);
            
            // shins
            bonePose = &currentPose->bonePoses[5];
            bonePose->boneID = 5;
            bonePose->localPos = Vector3(0.0f, 0.0f, 1.0f);
            bonePose->localRotation = Quaternion();
            
            bonePose = &currentPose->bonePoses[6];
            bonePose->boneID = 6;
            bonePose->localPos = Vector3(0.0f, 0.0f, 1.0f);
            bonePose->localRotation = quaternionFromAxisAngle(Vector3(1.0f, 0.0f, 0.0f), 0.8f);

            // feet
            bonePose = &currentPose->bonePoses[7];
            bonePose->boneID = 7;
            bonePose->localPos = Vector3(0.0f, 0.0f, 1.0f);
            bonePose->localRotation = quaternionFromAxisAngle(Vector3(1.0f, 0.0f, 0.0f), -PI/2.0f - 0.1f);

            bonePose = &currentPose->bonePoses[8];
            bonePose->boneID = 8;
            bonePose->localPos = Vector3(0.0f, 0.0f, 1.0f);
            bonePose->localRotation = quaternionFromAxisAngle(Vector3(1.0f, 0.0f, 0.0f), -PI/2.0f + 0.1f);
            // left foot contact
            currentPose = &poses[3];
            bonePose = &currentPose->bonePoses[0];
            bonePose->boneID = 0;
            bonePose->localPos = Vector3(0.0f, 0.5f, 0.0f);
            bonePose->localRotation = Quaternion();

            // hips
            bonePose = &currentPose->bonePoses[1];
            bonePose->boneID = 1;
            bonePose->localPos = Vector3(0.0f, 0.0f, 0.0f);
            bonePose->localRotation = quaternionFromAxisAngle(Vector3(0.0f, 1.0f, 0.0f), -PI/2.0f);
            
            bonePose = &currentPose->bonePoses[2];
            bonePose->boneID = 2;
            bonePose->localPos = Vector3(0.0f, 0.0f, 0.0f);
            bonePose->localRotation = quaternionFromAxisAngle(Vector3(0.0f, 1.0f, 0.0f), +PI/2.0f);

            // thighs
            bonePose = &currentPose->bonePoses[3];
            bonePose->boneID = 3;
            bonePose->localPos = Vector3(0.0f, 0.0f, 1.0f);
            bonePose->localRotation = quaternionFromAxisAngle(Vector3(0.0f, 1.0f, 0.0f), +PI/2.0f) *
                                      quaternionFromAxisAngle(Vector3(1.0f, 0.0f, 0.0f), +PI/2.0f + 0.65f);

            bonePose = &currentPose->bonePoses[4];
            bonePose->boneID = 4;
            bonePose->localPos = Vector3(0.0f, 0.0f, 1.0f);
            bonePose->localRotation = quaternionFromAxisAngle(Vector3(0.0f, 1.0f, 0.0f), -PI/2.0f) *
                                      quaternionFromAxisAngle(Vector3(1.0f, 0.0f, 0.0f), +PI/2.0f - 0.65f);
            
            // shins
            bonePose = &currentPose->bonePoses[5];
            bonePose->boneID = 5;
            bonePose->localPos = Vector3(0.0f, 0.0f, 1.0f);
            bonePose->localRotation = Quaternion();
            
            bonePose = &currentPose->bonePoses[6];
            bonePose->boneID = 6;
            bonePose->localPos = Vector3(0.0f, 0.0f, 1.0f);
            bonePose->localRotation = Quaternion();

            // feet
            bonePose = &currentPose->bonePoses[7];
            bonePose->boneID = 7;
            bonePose->localPos = Vector3(0.0f, 0.0f, 1.0f);
            bonePose->localRotation = quaternionFromAxisAngle(Vector3(1.0f, 0.0f, 0.0f), -PI/2.0f);

            bonePose = &currentPose->bonePoses[8];
            bonePose->boneID = 8;
            bonePose->localPos = Vector3(0.0f, 0.0f, 1.0f);
            bonePose->localRotation = quaternionFromAxisAngle(Vector3(1.0f, 0.0f, 0.0f), -PI/2.0f);

            // left foot down
            currentPose = &poses[4];
            bonePose = &currentPose->bonePoses[0];
            bonePose->boneID = 0;
            bonePose->localPos = Vector3(0.0f, 0.4f, 0.0f);
            bonePose->localRotation = Quaternion();

            // hips
            bonePose = &currentPose->bonePoses[1];
            bonePose->boneID = 1;
            bonePose->localPos = Vector3(0.0f, 0.0f, 0.0f);
            bonePose->localRotation = quaternionFromAxisAngle(Vector3(0.0f, 1.0f, 0.0f), -PI/2.0f);
            
            bonePose = &currentPose->bonePoses[2];
            bonePose->boneID = 2;
            bonePose->localPos = Vector3(0.0f, 0.0f, 0.0f);
            bonePose->localRotation = quaternionFromAxisAngle(Vector3(0.0f, 1.0f, 0.0f), +PI/2.0f);

            // thighs
            bonePose = &currentPose->bonePoses[3];
            bonePose->boneID = 3;
            bonePose->localPos = Vector3(0.0f, 0.0f, 1.0f);
            bonePose->localRotation = quaternionFromAxisAngle(Vector3(0.0f, 1.0f, 0.0f), +PI/2.0f) *
                                      quaternionFromAxisAngle(Vector3(1.0f, 0.0f, 0.0f), +PI/2.0f + 0.6f);

            bonePose = &currentPose->bonePoses[4];
            bonePose->boneID = 4;
            bonePose->localPos = Vector3(0.0f, 0.0f, 1.0f);
            bonePose->localRotation = quaternionFromAxisAngle(Vector3(0.0f, 1.0f, 0.0f), -PI/2.0f) *
                                      quaternionFromAxisAngle(Vector3(1.0f, 0.0f, 0.0f), +PI/2.0f - 0.7f);
            
            // shins
            bonePose = &currentPose->bonePoses[5];
            bonePose->boneID = 5;
            bonePose->localPos = Vector3(0.0f, 0.0f, 1.0f);
            bonePose->localRotation = quaternionFromAxisAngle(Vector3(1.0f, 0.0f, 0.0f), +1.1f);
            
            bonePose = &currentPose->bonePoses[6];
            bonePose->boneID = 6;
            bonePose->localPos = Vector3(0.0f, 0.0f, 1.0f);
            bonePose->localRotation = quaternionFromAxisAngle(Vector3(1.0f, 0.0f, 0.0f), +0.8f);

            // feet
            bonePose = &currentPose->bonePoses[7];
            bonePose->boneID = 7;
            bonePose->localPos = Vector3(0.0f, 0.0f, 1.0f);
            bonePose->localRotation = quaternionFromAxisAngle(Vector3(1.0f, 0.0f, 0.0f), -PI/2.0f - 0.2f);

            bonePose = &currentPose->bonePoses[8];
            bonePose->boneID = 8;
            bonePose->localPos = Vector3(0.0f, 0.0f, 1.0f);
            bonePose->localRotation = quaternionFromAxisAngle(Vector3(1.0f, 0.0f, 0.0f), -PI/2.0f - 0.1f);

            // right foot passing left
            // root
            currentPose = &poses[5];
            bonePose = &currentPose->bonePoses[0];
            bonePose->boneID = 0;
            bonePose->localPos = Vector3(0.0f, 0.5f, 0.0f);
            bonePose->localRotation = Quaternion();

            // hips
            bonePose = &currentPose->bonePoses[1];
            bonePose->boneID = 1;
            bonePose->localPos = Vector3(0.0f, 0.0f, 0.0f);
            bonePose->localRotation = quaternionFromAxisAngle(Vector3(0.0f, 1.0f, 0.0f), -PI/2.0f);
            
            bonePose = &currentPose->bonePoses[2];
            bonePose->boneID = 2;
            bonePose->localPos = Vector3(0.0f, 0.0f, 0.0f);
            bonePose->localRotation = quaternionFromAxisAngle(Vector3(0.0f, 1.0f, 0.0f), +PI/2.0f);

            // thighs
            bonePose = &currentPose->bonePoses[3];
            bonePose->boneID = 3;
            bonePose->localPos = Vector3(0.0f, 0.0f, 1.0f);
            bonePose->localRotation = quaternionFromAxisAngle(Vector3(0.0f, 1.0f, 0.0f), +PI/2.0f) *
                                      quaternionFromAxisAngle(Vector3(1.0f, 0.0f, 0.0f), +PI/2.0f - 0.25f);

            bonePose = &currentPose->bonePoses[4];
            bonePose->boneID = 4;
            bonePose->localPos = Vector3(0.0f, 0.0f, 1.0f);
            bonePose->localRotation = quaternionFromAxisAngle(Vector3(0.0f, 1.0f, 0.0f), -PI/2.0f) *
                                      quaternionFromAxisAngle(Vector3(1.0f, 0.0f, 0.0f), +PI/2.0f + 0.1f);
            
            // shins
            bonePose = &currentPose->bonePoses[5];
            bonePose->boneID = 5;
            bonePose->localPos = Vector3(0.0f, 0.0f, 1.0f);
            bonePose->localRotation = quaternionFromAxisAngle(Vector3(1.0f, 0.0f, 0.0f), 1.2f);
            
            bonePose = &currentPose->bonePoses[6];
            bonePose->boneID = 6;
            bonePose->localPos = Vector3(0.0f, 0.0f, 1.0f);
            bonePose->localRotation = Quaternion();

            // feet
            bonePose = &currentPose->bonePoses[7];
            bonePose->boneID = 7;
            bonePose->localPos = Vector3(0.0f, 0.0f, 1.0f);
            bonePose->localRotation = quaternionFromAxisAngle(Vector3(1.0f, 0.0f, 0.0f), -PI/2.0f + 0.3f);

            bonePose = &currentPose->bonePoses[8];
            bonePose->boneID = 8;
            bonePose->localPos = Vector3(0.0f, 0.0f, 1.0f);
            bonePose->localRotation = quaternionFromAxisAngle(Vector3(1.0f, 0.0f, 0.0f), -PI/2.0f - 0.1f);

            // right foot up frame
            currentPose = &poses[6];
            bonePose = &currentPose->bonePoses[0];
            bonePose->boneID = 0;
            bonePose->localPos = Vector3(0.0f, 0.6f, 0.0f);
            bonePose->localRotation = Quaternion();

            // hips
            bonePose = &currentPose->bonePoses[1];
            bonePose->boneID = 1;
            bonePose->localPos = Vector3(0.0f, 0.0f, 0.0f);
            bonePose->localRotation = quaternionFromAxisAngle(Vector3(0.0f, 1.0f, 0.0f), -PI/2.0f);
            
            bonePose = &currentPose->bonePoses[2];
            bonePose->boneID = 2;
            bonePose->localPos = Vector3(0.0f, 0.0f, 0.0f);
            bonePose->localRotation = quaternionFromAxisAngle(Vector3(0.0f, 1.0f, 0.0f), +PI/2.0f);

            // thighs
            bonePose = &currentPose->bonePoses[3];
            bonePose->boneID = 3;
            bonePose->localPos = Vector3(0.0f, 0.0f, 1.0f);
            bonePose->localRotation = quaternionFromAxisAngle(Vector3(0.0f, 1.0f, 0.0f), +PI/2.0f) *
                                      quaternionFromAxisAngle(Vector3(1.0f, 0.0f, 0.0f), +PI/2.0f - 0.55f);

            bonePose = &currentPose->bonePoses[4];
            bonePose->boneID = 4;
            bonePose->localPos = Vector3(0.0f, 0.0f, 1.0f);
            bonePose->localRotation = quaternionFromAxisAngle(Vector3(0.0f, 1.0f, 0.0f), -PI/2.0f) *
                                      quaternionFromAxisAngle(Vector3(1.0f, 0.0f, 0.0f), +PI/2.0f + 0.4f);
            
            // shins
            bonePose = &currentPose->bonePoses[5];
            bonePose->boneID = 5;
            bonePose->localPos = Vector3(0.0f, 0.0f, 1.0f);
            bonePose->localRotation = quaternionFromAxisAngle(Vector3(1.0f, 0.0f, 0.0f), 0.8f);
            
            bonePose = &currentPose->bonePoses[6];
            bonePose->boneID = 6;
            bonePose->localPos = Vector3(0.0f, 0.0f, 1.0f);
            bonePose->localRotation = Quaternion();

            // feet
            bonePose = &currentPose->bonePoses[7];
            bonePose->boneID = 7;
            bonePose->localPos = Vector3(0.0f, 0.0f, 1.0f);
            bonePose->localRotation = quaternionFromAxisAngle(Vector3(1.0f, 0.0f, 0.0f), -PI/2.0f + 0.1f);

            bonePose = &currentPose->bonePoses[8];
            bonePose->boneID = 8;
            bonePose->localPos = Vector3(0.0f, 0.0f, 1.0f);
            bonePose->localRotation = quaternionFromAxisAngle(Vector3(1.0f, 0.0f, 0.0f), -PI/2.0f - 0.1f);
            // right foot contact
            currentPose = &poses[7];
            bonePose = &currentPose->bonePoses[0];
            bonePose->boneID = 0;
            bonePose->localPos = Vector3(0.0f, 0.5f, 0.0f);
            bonePose->localRotation = Quaternion();

            // hips
            bonePose = &currentPose->bonePoses[1];
            bonePose->boneID = 1;
            bonePose->localPos = Vector3(0.0f, 0.0f, 0.0f);
            bonePose->localRotation = quaternionFromAxisAngle(Vector3(0.0f, 1.0f, 0.0f), -PI/2.0f);
            
            bonePose = &currentPose->bonePoses[2];
            bonePose->boneID = 2;
            bonePose->localPos = Vector3(0.0f, 0.0f, 0.0f);
            bonePose->localRotation = quaternionFromAxisAngle(Vector3(0.0f, 1.0f, 0.0f), +PI/2.0f);

            // thighs
            bonePose = &currentPose->bonePoses[3];
            bonePose->boneID = 3;
            bonePose->localPos = Vector3(0.0f, 0.0f, 1.0f);
            bonePose->localRotation = quaternionFromAxisAngle(Vector3(0.0f, 1.0f, 0.0f), +PI/2.0f) *
                                      quaternionFromAxisAngle(Vector3(1.0f, 0.0f, 0.0f), +PI/2.0f - 0.65f);

            bonePose = &currentPose->bonePoses[4];
            bonePose->boneID = 4;
            bonePose->localPos = Vector3(0.0f, 0.0f, 1.0f);
            bonePose->localRotation = quaternionFromAxisAngle(Vector3(0.0f, 1.0f, 0.0f), -PI/2.0f) *
                                      quaternionFromAxisAngle(Vector3(1.0f, 0.0f, 0.0f), +PI/2.0f + 0.65f);
            
            // shins
            bonePose = &currentPose->bonePoses[5];
            bonePose->boneID = 5;
            bonePose->localPos = Vector3(0.0f, 0.0f, 1.0f);
            bonePose->localRotation = Quaternion();
            
            bonePose = &currentPose->bonePoses[6];
            bonePose->boneID = 6;
            bonePose->localPos = Vector3(0.0f, 0.0f, 1.0f);
            bonePose->localRotation = Quaternion();

            // feet
            bonePose = &currentPose->bonePoses[7];
            bonePose->boneID = 7;
            bonePose->localPos = Vector3(0.0f, 0.0f, 1.0f);
            bonePose->localRotation = quaternionFromAxisAngle(Vector3(1.0f, 0.0f, 0.0f), -PI/2.0f);

            bonePose = &currentPose->bonePoses[8];
            bonePose->boneID = 8;
            bonePose->localPos = Vector3(0.0f, 0.0f, 1.0f);
            bonePose->localRotation = quaternionFromAxisAngle(Vector3(1.0f, 0.0f, 0.0f), -PI/2.0f);

            // right foot down
            currentPose = &poses[8];
            bonePose = &currentPose->bonePoses[0];
            bonePose->boneID = 0;
            bonePose->localPos = Vector3(0.0f, 0.4f, 0.0f);
            bonePose->localRotation = Quaternion();

            // hips
            bonePose = &currentPose->bonePoses[1];
            bonePose->boneID = 1;
            bonePose->localPos = Vector3(0.0f, 0.0f, 0.0f);
            bonePose->localRotation = quaternionFromAxisAngle(Vector3(0.0f, 1.0f, 0.0f), -PI/2.0f);
            
            bonePose = &currentPose->bonePoses[2];
            bonePose->boneID = 2;
            bonePose->localPos = Vector3(0.0f, 0.0f, 0.0f);
            bonePose->localRotation = quaternionFromAxisAngle(Vector3(0.0f, 1.0f, 0.0f), +PI/2.0f);

            // thighs
            bonePose = &currentPose->bonePoses[3];
            bonePose->boneID = 3;
            bonePose->localPos = Vector3(0.0f, 0.0f, 1.0f);
            bonePose->localRotation = quaternionFromAxisAngle(Vector3(0.0f, 1.0f, 0.0f), +PI/2.0f) *
                                      quaternionFromAxisAngle(Vector3(1.0f, 0.0f, 0.0f), +PI/2.0f - 0.7f);

            bonePose = &currentPose->bonePoses[4];
            bonePose->boneID = 4;
            bonePose->localPos = Vector3(0.0f, 0.0f, 1.0f);
            bonePose->localRotation = quaternionFromAxisAngle(Vector3(0.0f, 1.0f, 0.0f), -PI/2.0f) *
                                      quaternionFromAxisAngle(Vector3(1.0f, 0.0f, 0.0f), +PI/2.0f + 0.6f);
            
            // shins
            bonePose = &currentPose->bonePoses[5];
            bonePose->boneID = 5;
            bonePose->localPos = Vector3(0.0f, 0.0f, 1.0f);
            bonePose->localRotation = quaternionFromAxisAngle(Vector3(1.0f, 0.0f, 0.0f), +0.8f);
            
            bonePose = &currentPose->bonePoses[6];
            bonePose->boneID = 6;
            bonePose->localPos = Vector3(0.0f, 0.0f, 1.0f);
            bonePose->localRotation = quaternionFromAxisAngle(Vector3(1.0f, 0.0f, 0.0f), +1.1f);

            // feet
            bonePose = &currentPose->bonePoses[7];
            bonePose->boneID = 7;
            bonePose->localPos = Vector3(0.0f, 0.0f, 1.0f);
            bonePose->localRotation = quaternionFromAxisAngle(Vector3(1.0f, 0.0f, 0.0f), -PI/2.0f - 0.1f);

            bonePose = &currentPose->bonePoses[8];
            bonePose->boneID = 8;
            bonePose->localPos = Vector3(0.0f, 0.0f, 1.0f);
            bonePose->localRotation = quaternionFromAxisAngle(Vector3(1.0f, 0.0f, 0.0f), -PI/2.0f - 0.2f);

            // set up animations, idle and walking
            int numAnimations = 2;
            animationDataAsset->numAnimations = numAnimations;
            animationDataAsset->animations = (skeleton_animation *)allocateMemorySize(&assets->assetMemory, sizeof(skeleton_animation) * numAnimations);

            skeleton_animation *animations = animationDataAsset->animations;

            // idle
            skeleton_animation *animation = &animations[0];
            animation->numKeyFrames = 1;
            animation->keyframes = (skeleton_keyframe *)allocateMemorySize(&assets->assetMemory, sizeof(skeleton_keyframe) * animation->numKeyFrames);
            animation->duration = 0.0f;
            animation->key = allocateString("idle", &assets->assetMemory);
            animation->id = 0;

            skeleton_keyframe *keyframe = &animation->keyframes[0];
            keyframe->t = 0.0f;
            keyframe->poseID = 0;

            // walking
            animation = &animations[1];
            animation->numKeyFrames = 8;
            animation->keyframes = (skeleton_keyframe *)allocateMemorySize(&assets->assetMemory, sizeof(skeleton_keyframe) * animation->numKeyFrames);
            animation->duration = 0.72f;
            animation->key = allocateString("walking", &assets->assetMemory);
            animation->id = 1;

            keyframe = &animation->keyframes[0];
            keyframe->t = 0.0f;
            keyframe->poseID = 1;

            keyframe = &animation->keyframes[1];
            keyframe->t = 0.09f;
            keyframe->poseID = 2;

            keyframe = &animation->keyframes[2];
            keyframe->t = 0.18f;
            keyframe->poseID = 3;

            keyframe = &animation->keyframes[3];
            keyframe->t = 0.27f;
            keyframe->poseID = 4;

            keyframe = &animation->keyframes[4];
            keyframe->t = 0.36f;
            keyframe->poseID = 5;

            keyframe = &animation->keyframes[5];
            keyframe->t = 0.45f;
            keyframe->poseID = 6;

            keyframe = &animation->keyframes[6];
            keyframe->t = 0.54f;
            keyframe->poseID = 7;

            keyframe = &animation->keyframes[7];
            keyframe->t = 0.63f;
            keyframe->poseID = 8;

        } break;
        case ANIMATION_DATA_KEY_MULTI_ELBOW: {
            // set up bone hierarchy
            int numBones = 10;
            animationDataAsset->numBones = numBones;
            animationDataAsset->boneHierarchy = (int *)allocateMemorySize(&assets->assetMemory, sizeof(int) * numBones);
            animationDataAsset->inverseRestTransforms = (matrix4x4 *)allocateMemorySize(&assets->assetMemory, sizeof(matrix4x4) * numBones);

            int *boneHierarchy = animationDataAsset->boneHierarchy;
            boneHierarchy[0] = -1;
            boneHierarchy[1] = 0;
            boneHierarchy[2] = 1;
            boneHierarchy[3] = 2;
            boneHierarchy[4] = 3;
            boneHierarchy[5] = 4;
            boneHierarchy[6] = 5;
            boneHierarchy[7] = 6;
            boneHierarchy[8] = 7;
            boneHierarchy[9] = 8;

            // set up poses, t-pose and walking animation poses
            int numPoses = 2;
            animationDataAsset->numPoses = numPoses;
            animationDataAsset->poses = (skeleton_pose *)allocateMemorySize(&assets->assetMemory, sizeof(skeleton_pose) * numPoses);

            skeleton_pose *poses = animationDataAsset->poses;
            for (int i = 0; i < numPoses; ++i) {
                skeleton_pose *pose = &poses[i];
                pose->numBones = numBones;
                pose->bonePoses = (skeleton_bone_pose *)allocateMemorySize(&assets->assetMemory, sizeof(skeleton_bone_pose) * numBones);
            }

            // rest pose
            skeleton_pose *restPose = &poses[0];
            skeleton_bone_pose *bonePose = &restPose->bonePoses[0];
            bonePose->boneID = 0;
            bonePose->localPos = Vector3(0.0f, 0.0f, 0.0f);
            bonePose->localRotation = Quaternion();

            bonePose = &restPose->bonePoses[1];
            bonePose->boneID = 1;
            bonePose->localPos = Vector3(0.0f, 0.0f, 1.0f);
            bonePose->localRotation = Quaternion();

            bonePose = &restPose->bonePoses[2];
            bonePose->boneID = 2;
            bonePose->localPos = Vector3(0.0f, 0.0f, 1.0f);
            bonePose->localRotation = Quaternion();

            bonePose = &restPose->bonePoses[3];
            bonePose->boneID = 3;
            bonePose->localPos = Vector3(0.0f, 0.0f, 1.0f);
            bonePose->localRotation = Quaternion();

            bonePose = &restPose->bonePoses[4];
            bonePose->boneID = 4;
            bonePose->localPos = Vector3(0.0f, 0.0f, 1.0f);
            bonePose->localRotation = Quaternion();

            bonePose = &restPose->bonePoses[5];
            bonePose->boneID = 5;
            bonePose->localPos = Vector3(0.0f, 0.0f, 1.0f);
            bonePose->localRotation = Quaternion();

            bonePose = &restPose->bonePoses[6];
            bonePose->boneID = 6;
            bonePose->localPos = Vector3(0.0f, 0.0f, 1.0f);
            bonePose->localRotation = Quaternion();

            bonePose = &restPose->bonePoses[7];
            bonePose->boneID = 7;
            bonePose->localPos = Vector3(0.0f, 0.0f, 1.0f);
            bonePose->localRotation = Quaternion();

            bonePose = &restPose->bonePoses[8];
            bonePose->boneID = 8;
            bonePose->localPos = Vector3(0.0f, 0.0f, 1.0f);
            bonePose->localRotation = Quaternion();

            bonePose = &restPose->bonePoses[9];
            bonePose->boneID = 9;
            bonePose->localPos = Vector3(0.0f, 0.0f, 1.0f);
            bonePose->localRotation = Quaternion();

            matrix4x4 *boneTransforms = (matrix4x4 *)allocateMemorySize(workingMemory, sizeof(matrix4x4) * numBones);
            for (int i = 0; i < numBones; ++i) {
                matrix4x4 *currentBoneMatrix = &boneTransforms[i];
                skeleton_bone_pose *currentBonePose = &restPose->bonePoses[i];
                assert(i == currentBonePose->boneID);

                matrix4x4 localTransform = matrix4x4FromQuaternion(currentBonePose->localRotation);
                localTransform = translationMatrix(currentBonePose->localPos) * localTransform;

                int parentID = boneHierarchy[i];
                if (parentID == -1) {
                    *currentBoneMatrix = localTransform;
                }
                else {
                    matrix4x4 *parentMatrix = &boneTransforms[parentID];
                    *currentBoneMatrix = *parentMatrix * localTransform;
                }
                animationDataAsset->inverseRestTransforms[i] = inverse(*currentBoneMatrix);
            }

            // curled
            // root
            quaternion r = quaternionFromAxisAngle(Vector3(0.0f, 1.0f, 0.0f), (-PI / 2.0f) / 1.5f) *
                           quaternionFromAxisAngle(Vector3(1.0f, 0.0f, 0.0f), (-PI / 2.0f) / 1.5f);
            skeleton_pose *currentPose = &poses[1];
            bonePose = &currentPose->bonePoses[0];
            bonePose->boneID = 0;
            bonePose->localPos = Vector3(0.0f, 0.0f, 0.0f);
            bonePose->localRotation = r;

            bonePose = &currentPose->bonePoses[1];
            bonePose->boneID = 1;
            bonePose->localPos = Vector3(0.0f, 0.0f, 1.0f);
            bonePose->localRotation = r;

            bonePose = &currentPose->bonePoses[2];
            bonePose->boneID = 2;
            bonePose->localPos = Vector3(0.0f, 0.0f, 1.0f);
            bonePose->localRotation = r;

            bonePose = &currentPose->bonePoses[3];
            bonePose->boneID = 3;
            bonePose->localPos = Vector3(0.0f, 0.0f, 1.0f);
            bonePose->localRotation = r;

            bonePose = &currentPose->bonePoses[4];
            bonePose->boneID = 4;
            bonePose->localPos = Vector3(0.0f, 0.0f, 1.0f);
            bonePose->localRotation = r;

            bonePose = &currentPose->bonePoses[5];
            bonePose->boneID = 5;
            bonePose->localPos = Vector3(0.0f, 0.0f, 1.0f);
            bonePose->localRotation = r;

            bonePose = &currentPose->bonePoses[6];
            bonePose->boneID = 6;
            bonePose->localPos = Vector3(0.0f, 0.0f, 1.0f);
            bonePose->localRotation = r;

            bonePose = &currentPose->bonePoses[7];
            bonePose->boneID = 7;
            bonePose->localPos = Vector3(0.0f, 0.0f, 1.0f);
            bonePose->localRotation = r;

            bonePose = &currentPose->bonePoses[8];
            bonePose->boneID = 8;
            bonePose->localPos = Vector3(0.0f, 0.0f, 1.0f);
            bonePose->localRotation = r;

            bonePose = &currentPose->bonePoses[9];
            bonePose->boneID = 9;
            bonePose->localPos = Vector3(0.0f, 0.0f, 1.0f);
            bonePose->localRotation = r;

            // set up animations, idle and walking
            int numAnimations = 1;
            animationDataAsset->numAnimations = numAnimations;
            animationDataAsset->animations = (skeleton_animation *)allocateMemorySize(&assets->assetMemory, sizeof(skeleton_animation) * numAnimations);

            skeleton_animation *animations = animationDataAsset->animations;

            // curling
            skeleton_animation *animation = &animations[0];
            animation->numKeyFrames = 2;
            animation->keyframes = (skeleton_keyframe *)allocateMemorySize(&assets->assetMemory, sizeof(skeleton_keyframe) * animation->numKeyFrames);
            animation->duration = 10.0f;
            animation->key = allocateString("curling", &assets->assetMemory);
            animation->id = 0;

            skeleton_keyframe *keyframe = &animation->keyframes[0];
            keyframe->t = 0.0f;
            keyframe->poseID = 0;

            keyframe = &animation->keyframes[1];
            keyframe->t = 5.0f;
            keyframe->poseID = 1;
        } break;
        case ANIMATION_DATA_KEY_SNAKE: {
            // set up bone hierarchy
            int numBones = 10;
            animationDataAsset->numBones = numBones;
            animationDataAsset->boneHierarchy = (int *)allocateMemorySize(&assets->assetMemory, sizeof(int) * numBones);
            animationDataAsset->inverseRestTransforms = (matrix4x4 *)allocateMemorySize(&assets->assetMemory, sizeof(matrix4x4) * numBones);

            int *boneHierarchy = animationDataAsset->boneHierarchy;
            boneHierarchy[0] = -1;
            boneHierarchy[1] = 0;
            boneHierarchy[2] = 1;
            boneHierarchy[3] = 2;
            boneHierarchy[4] = 3;
            boneHierarchy[5] = 4;
            boneHierarchy[6] = 5;
            boneHierarchy[7] = 6;
            boneHierarchy[8] = 7;
            boneHierarchy[9] = 8;

            // set up poses 
            int numPoses = 9;
            animationDataAsset->numPoses = numPoses;
            animationDataAsset->poses = (skeleton_pose *)allocateMemorySize(&assets->assetMemory, sizeof(skeleton_pose) * numPoses);

            skeleton_pose *poses = animationDataAsset->poses;
            for (int i = 0; i < numPoses; ++i) {
                skeleton_pose *pose = &poses[i];
                pose->numBones = numBones;
                pose->bonePoses = (skeleton_bone_pose *)allocateMemorySize(&assets->assetMemory, sizeof(skeleton_bone_pose) * numBones);
            }

            // rest pose
            skeleton_pose *restPose = &poses[0];
            skeleton_bone_pose *bonePose = &restPose->bonePoses[0];
            bonePose->boneID = 0;
            bonePose->localPos = Vector3(0.0f, 0.0f, 0.0f);
            bonePose->localRotation = Quaternion();
            for (int i = 1; i < numBones; ++i) {
                bonePose = &restPose->bonePoses[i];
                bonePose->boneID = i;
                bonePose->localPos = Vector3(0.0f, 0.0f, 1.0f);
                bonePose->localRotation = Quaternion();
            }

            matrix4x4 *boneTransforms = (matrix4x4 *)allocateMemorySize(workingMemory, sizeof(matrix4x4) * numBones);
            for (int i = 0; i < numBones; ++i) {
                matrix4x4 *currentBoneMatrix = &boneTransforms[i];
                skeleton_bone_pose *currentBonePose = &restPose->bonePoses[i];
                assert(i == currentBonePose->boneID);

                matrix4x4 localTransform = matrix4x4FromQuaternion(currentBonePose->localRotation);
                localTransform = translationMatrix(currentBonePose->localPos) * localTransform;

                int parentID = boneHierarchy[i];
                if (parentID == -1) {
                    *currentBoneMatrix = localTransform;
                }
                else {
                    matrix4x4 *parentMatrix = &boneTransforms[parentID];
                    *currentBoneMatrix = *parentMatrix * localTransform;
                }
                animationDataAsset->inverseRestTransforms[i] = inverse(*currentBoneMatrix);
            }

            // curled
            // root
            for (int poseIndex = 1; poseIndex < numPoses; ++poseIndex) {
                int tempPose = 1;
                skeleton_pose *currentPose = &poses[poseIndex];
                for (int boneIndex = 0; boneIndex < numBones; ++boneIndex) {
                    bonePose = &currentPose->bonePoses[boneIndex];
                    bonePose->boneID = boneIndex;
                    bonePose->localPos = Vector3(0.0f, 0.0f, 1.0f);

                    bonePose->localRotation = quaternionFromAxisAngle(Vector3(0.0f, 1.0f, 0.0f), 
                            sinf((float)(poseIndex-1)/8.0f * 2.0f * PI + (((float)(boneIndex) / (float)(numBones)) * 2.0f * PI)) * (PI/4.0f));
                    /*
                    bonePose->localRotation = quaternionFromAxisAngle(Vector3(0.0f, 1.0f, 0.0f), 
                            sinf((float)(poseIndex-1)/8.0f * 2.0f * PI + 
                                 (((float)boneIndex / (float)(numBones-1)) * 2.0f * PI)) * (PI/8.0f));
                    */

                }
                currentPose->bonePoses[0].localRotation = 
                    quaternionFromAxisAngle(Vector3(0.0f, 1.0f, 0.0f), 
                            sinf((float)(poseIndex-1)/8.0f * 2.0f * PI) * 0.3f) *
                    quaternionFromAxisAngle(Vector3(0.0f, 1.0f, 0.0f), 
                            -cosf((float)(poseIndex-1)/8.0f * 2.0f * PI) * 1.2f);
                currentPose->bonePoses[0].localPos.x = -sinf((float)(poseIndex-1)/8.0f * 2.0f * PI) * 2.0f;
                //currentPose->bonePoses[0].localPos.z = -cosf((float)(poseIndex-1)/8.0f * 2.0f * PI * 4.0f) * 0.4f;
            }
            // set up animations, idle and walking
            int numAnimations = 1;
            animationDataAsset->numAnimations = numAnimations;
            animationDataAsset->animations = (skeleton_animation *)allocateMemorySize(&assets->assetMemory, sizeof(skeleton_animation) * numAnimations);

            skeleton_animation *animations = animationDataAsset->animations;

            // curling
            skeleton_animation *animation = &animations[0];
            animation->numKeyFrames = 8;
            animation->keyframes = (skeleton_keyframe *)allocateMemorySize(&assets->assetMemory, sizeof(skeleton_keyframe) * animation->numKeyFrames);
            animation->duration = 2.0f;
            animation->key = allocateString("snaking", &assets->assetMemory);
            animation->id = 0;

            for (int i = 0; i < animation->numKeyFrames; ++i) {
                skeleton_keyframe *keyframe = &animation->keyframes[i];
                keyframe->t = ((float)i/ 8.0f) * 2.0f;
                keyframe->poseID = i+1;
            }
        } break;
    }
}

animation_data *getAnimationData (animation_data_key animationDataKey, game_assets *assets) {
    assert(animationDataKey < assets->numAnimationData);
    animation_data *animationDataAsset = assets->animationData[animationDataKey]; 
    return animationDataAsset;
}

matrix4x4 *getInverseTransformsForAnimatedModel (animation_data_key animationDataKey, game_assets *assets) {
    animation_data *animationDataAsset = getAnimationData(animationDataKey, assets);
    return animationDataAsset->inverseRestTransforms;
}

skeleton_animation *getAnimationFromData (animation_data *animationData, char *animationKey) {
    skeleton_animation *result = 0;
    for (int i = 0; i < animationData->numAnimations; ++i) {
        skeleton_animation *animation = &animationData->animations[i];
        char *currentKey = animation->key;
        if (stringsAreEqual(animationKey, currentKey)) {
            result = animation;
            break;
        }
    }

    assert(result != 0);
    return result;
}

// get animation function

// djb2
// http://www.cse.yorku.ca/~oz/hash.html
unsigned int hashString (char *string) {
    unsigned int hash = 5381;
    int c;
    assert(string);
    do {
        c = *string++;
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
    } while (c);

    return hash;
}

void parseAtlas (void *atlasData, game_assets *assets, int atlasKey, int textureKey, memory_arena *workingMemory) {
    int numAtlases = assets->numAtlases;
    assert(numAtlases < MAX_NUM_ATLASES);

    atlas_asset *atlasAsset = (atlas_asset *)allocateMemorySize(&assets->assetMemory, sizeof(atlas_asset)); 
    assets->atlases[atlasKey] = atlasAsset;
    assets->numAtlases++;
    atlasAsset->atlasKey = (atlas_key)atlasKey;
    atlasAsset->textureKey = (texture_key)textureKey;

    // texture packer libgdx format
    char *start, *end, *nextLineStart;
    start = (char *)atlasData;
    readLine(start, &start, &end, &nextLineStart); // skip file name
    start = nextLineStart;
    readLine(start, &start, &end, &nextLineStart);

    char *wordStart = start;
    char *wordEnd = wordStart;
    while (*wordEnd != ' ' && wordEnd != end) { ++wordEnd; }
    ++wordEnd;
    wordStart = wordEnd;
    while (*wordEnd != ',' && wordEnd != end) { ++wordEnd; }
    atlasAsset->width = stringToInt(wordStart, wordEnd-1);

    wordEnd+=2;
    wordStart = wordEnd;
    while (*wordEnd != '\n' && *wordEnd != '\r' && wordEnd != end) { ++wordEnd; }
    atlasAsset->height = stringToInt(wordStart, wordEnd-1);

    start = nextLineStart;
    // skip format, filter, repeat
    readLine(start, &start, &end, &nextLineStart);
    start = nextLineStart;
    readLine(start, &start, &end, &nextLineStart);
    start = nextLineStart;
    readLine(start, &start, &end, &nextLineStart);
    start = nextLineStart;

    char *frameInfoStart = start;
    int numFrames = 0;
    while (true) {
        for (int i = 0; i < 7; ++i) {
            readLine(start, &start, &end, &nextLineStart);
            start = nextLineStart;
        }
        numFrames++;
        if (*(nextLineStart) == 0) { break; }
    }

    // allocate enough atlas_frames
    assert(numFrames < 500);
    for (int i = 0; i < 500; ++i) {
        atlasAsset->map.entries[i] = {};
    }
    // TODO(ebuchholz): get rid of this first scan or change structure of map

    // TODO(ebuchholz): more convenient file reading/string copying functions
    start = frameInfoStart;
    for (int i = 0; i < numFrames; ++i) {
        atlas_frame frame;
        readLine(start, &start, &end, &nextLineStart);
        int length = (int)(end - start);
        char *frameName = (char *)allocateMemorySize(&assets->assetMemory, 
                                                     sizeof(char) * length+1);
        for (int j = 0; j < length; ++j) {
            frameName[j] = start[j];
        }
        frameName[length] = 0;
        frame.key = frameName;

        start = nextLineStart;
        readLine(start, &start, &end, &nextLineStart);
        start = nextLineStart;
        readLine(start, &start, &end, &nextLineStart);

        start += 6;
        wordStart = start;
        wordEnd = wordStart;
        while (*wordEnd != ',' && wordEnd != end) { ++wordEnd; }
        int frameX = stringToInt(wordStart, wordEnd-1);

        wordEnd += 2;
        wordStart = wordEnd;
        while (*wordEnd != '\n' && *wordEnd != '\r' && wordEnd != end) { ++wordEnd; }
        int frameY = stringToInt(wordStart, wordEnd-1);

        start = nextLineStart;
        readLine(start, &start, &end, &nextLineStart);

        start += 8;
        wordStart = start;
        wordEnd = wordStart;
        while (*wordEnd != ',' && wordEnd != end) { ++wordEnd; }
        int frameWidth = stringToInt(wordStart, wordEnd-1);

        wordEnd += 2;
        wordStart = wordEnd;
        while (*wordEnd != '\n' && *wordEnd != '\r' && wordEnd != end) { ++wordEnd; }
        int frameHeight = stringToInt(wordStart, wordEnd-1);

        // skip the rest of the frame info
        start = nextLineStart;
        readLine(start, &start, &end, &nextLineStart);
        start = nextLineStart;
        readLine(start, &start, &end, &nextLineStart);
        start = nextLineStart;
        readLine(start, &start, &end, &nextLineStart);
        start = nextLineStart;

        frame.frameWidth = frameWidth;
        frame.frameHeight = frameHeight;

        float uvWidth = (float)frameWidth / (float)atlasAsset->width;
        float uvHeight = (float)frameHeight / (float)atlasAsset->height;
        frame.frameCorners[0] = Vector2((float)frameX / (float)atlasAsset->width,
                                        1.0f - (float)frameY / (float)atlasAsset->height);
        frame.frameCorners[1] = Vector2(frame.frameCorners[0].x + uvWidth, frame.frameCorners[0].y);
        frame.frameCorners[2] = Vector2(frame.frameCorners[0].x, frame.frameCorners[0].y - uvHeight);
        frame.frameCorners[3] = Vector2(frame.frameCorners[0].x + uvWidth, frame.frameCorners[0].y - uvHeight);

        unsigned int hash = hashString(frameName);
        unsigned int mapIndex = hash % 500;
        atlas_frame *existingFrame = &atlasAsset->map.entries[mapIndex];
        while (existingFrame->key != 0) {
            mapIndex = (mapIndex + 1) % 500;
            existingFrame = &atlasAsset->map.entries[mapIndex];
        }
        atlasAsset->map.entries[mapIndex] = frame;
    }
}

atlas_frame *getAtlasFrame (game_assets *assets, int atlasKey, char *frameName) {
    atlas_frame *result;

    atlas_asset *textureAtlas = assets->atlases[atlasKey];
    assert(textureAtlas);

    unsigned int hash = hashString(frameName);
    unsigned int mapIndex = hash % 500;
    unsigned int originalMapIndex = mapIndex; // check that we actually find it after looping all the way around
    while (true) {
        result = &textureAtlas->map.entries[mapIndex];
        if (stringsAreEqual(frameName, result->key)) {
            break;
        }
        else {
            mapIndex = (mapIndex + 1) % 500;
            assert(mapIndex != originalMapIndex);
        }
    }

    return result;
}

// get from atlas map function: calc hash, get item from array, check key, if conflict, get next and check key again

void pushAsset (asset_list *assetList, char *path, asset_type type, int key) {
    assert(assetList->numAssetsToLoad < assetList->maxAssetsToLoad);
    asset_to_load *assetToLoad = assetList->assetsToLoad + assetList->numAssetsToLoad;
    assetList->numAssetsToLoad++;
    assetToLoad->path = path;
    assetToLoad->type = type;
    assetToLoad->key = key;
    assetToLoad->secondKey = -1;
}

void pushAsset (asset_list *assetList, char *path, asset_type type, int key, int secondKey) {
    assert(assetList->numAssetsToLoad < assetList->maxAssetsToLoad);
    asset_to_load *assetToLoad = assetList->assetsToLoad + assetList->numAssetsToLoad;
    assetList->numAssetsToLoad++;
    assetToLoad->path = path;
    assetToLoad->type = type;
    assetToLoad->key = key;
    assetToLoad->secondKey = secondKey;
}
