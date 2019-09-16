#include "skeletal_game.h"
#include "skeletal_pose_setup.cpp"


void debugCameraMovement (debug_camera *debugCamera, game_input *input) {
    const float CAMERA_SPEED = 3.0f;
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

void initSkeletalGame (memory_arena *memory, skeletal_game* skeletalGame) {
    setRNGSeed(0); // TODO(ebuchholz): seed with time?

    *skeletalGame = {};
    debug_camera *debugCamera = &skeletalGame->debugCamera;
    debugCamera->pos = {};
    debugCamera->pos.x = 3.0f;
    debugCamera->pos.y = 3.0f;
    debugCamera->pos.z = 3.0f;
    debugCamera->rotation = 
        quaternionFromAxisAngle(Vector3(0.0f, 1.0f, 0.0f), 45.0f * (PI / 180.0f)) *
        quaternionFromAxisAngle(Vector3(1.0f, 0.0f, 0.0f), -33.0f * (PI / 180.0f));
    debugCamera->lastPointerX = 0;
    debugCamera->lastPointerY = 0;

    for (int i = 0; i < NUM_DEBUG_BONES; ++i) {
        skeleton_bone *bone = &skeletalGame->bones[i];
        *bone = {};
        bone->parentID = -1;
    }

    setupSkeleton(skeletalGame);
    
    // define curled up pose
    //skeleton_pose *curledPose = &skeletalGame->poses[1];
    //bonePose = &curledPose->bonePoses[0];
    //bonePose->boneID = 0;
    //bonePose->localPos = Vector3(0.0f, 1.0f, 0.0f);
    //bonePose->localRotation = Quaternion();

    //bonePose = &curledPose->bonePoses[1];
    //bonePose->boneID = 1;
    //bonePose->localPos = Vector3(0.0f, 0.0f, 1.0f);
    //bonePose->localRotation = quaternionFromAxisAngle(Vector3(1.0f, 0.0f, 0.0f), (-PI / 2.0f));
    //
    //bonePose = &curledPose->bonePoses[2];
    //bonePose->boneID = 2;
    //bonePose->localPos = Vector3(0.0f, 0.0f, 1.0f);
    //bonePose->localRotation = quaternionFromAxisAngle(Vector3(1.0f, 0.0f, 0.0f), (-PI / 2.0f));
    
    // calculate inverse rest transforms
    // TODO(ebuchholz): determine if bones should have their own rotation/position properties
    skeleton_pose *currentPose = &skeletalGame->poses[0]; // assume 0 is rest pose
    for (int i = 0; i < skeletalGame->numBones; ++i) {
        skeleton_bone *currentBone = &skeletalGame->bones[i];
        skeleton_bone_pose *currentBonePose = &currentPose->bonePoses[i];
        assert(i == currentBonePose->boneID);

        matrix4x4 localTransform = matrix4x4FromQuaternion(currentBonePose->localRotation);
        localTransform = translationMatrix(currentBonePose->localPos) * localTransform;

        if (currentBone->parentID == -1) {
            currentBone->transform = localTransform;
        }
        else {
            skeleton_bone *parentBone = &skeletalGame->bones[currentBone->parentID];
            currentBone->transform = parentBone->transform * localTransform;
        }
        currentBone->inverseRestTransform = inverse(currentBone->transform);
    }
}

void calculateBoneTransforms (skeletal_game *skeletalGame) {
    for (int i = 0; i < skeletalGame->numBones; ++i) {
        skeleton_bone *currentBone = &skeletalGame->bones[i];

        matrix4x4 localTransform = matrix4x4FromQuaternion(currentBone->localRotation);
        localTransform = translationMatrix(currentBone->localPos) * localTransform;

        if (currentBone->parentID == -1) {
            currentBone->transform = localTransform;
        }
        else {
            skeleton_bone *parentBone = &skeletalGame->bones[currentBone->parentID];
            currentBone->transform = parentBone->transform * localTransform;
        }
    }
}

void playAnimation (int animationID, animation_state *animationState, skeleton_bone *bones, 
                    skeleton_animation *animations, skeleton_pose *poses) 
{

} 

void updateSkeletalGame (memory_arena *memory, memory_arena *tempMemory, game_assets *assets, game_input *input, 
                         skeletal_game *skeletalGame, sprite_list *spriteList, render_command_list * renderCommands)
{
    // memory for dynamically created strings
    memory_arena stringMemory = {};
    stringMemory.capacity = 512 * 1024;
    stringMemory.base = allocateMemorySize(tempMemory, stringMemory.capacity);

    debugCameraMovement(&skeletalGame->debugCamera, input);

    float screenWidth = (float)renderCommands->windowWidth;
    float screenHeight = (float)renderCommands->windowHeight;
    matrix4x4 projMatrix = createPerspectiveMatrix(0.1f, 1000.0f, (screenWidth / screenHeight), 80.0f);
    matrix4x4 viewMatrix = createViewMatrix(skeletalGame->debugCamera.rotation, 
                                            skeletalGame->debugCamera.pos.x,
                                            skeletalGame->debugCamera.pos.y,
                                            skeletalGame->debugCamera.pos.z);

    render_command_set_camera *setCameraCommand = 
        (render_command_set_camera *)pushRenderCommand(renderCommands,
                                                       RENDER_COMMAND_SET_CAMERA,
                                                       sizeof(render_command_set_camera));
    setCameraCommand->viewMatrix = viewMatrix;
    setCameraCommand->projMatrix = projMatrix;

    // hack together an arm animation
    static float t = 0.0f;
    t += DELTA_TIME * 1.7f;

    float blendT = 0.0f;
    skeleton_pose *currentPose;
    skeleton_pose *nextPose;

    if (t > 2.0f) { t -= 2.0f; }
    if (t < 0.25f) {
        blendT = t / 0.25f;
        currentPose = &skeletalGame->poses[1];
        nextPose = &skeletalGame->poses[2];
    }
    else if (t >= 0.25f && t < 0.5f) {
        blendT = (t - 0.25f) / 0.25f;
        currentPose = &skeletalGame->poses[2];
        nextPose = &skeletalGame->poses[3];
    }
    else if (t >= 0.5f && t < 0.75f) {
        blendT = (t - 0.5f) / 0.25f;
        currentPose = &skeletalGame->poses[3];
        nextPose = &skeletalGame->poses[4];
    }
    else if (t >= 0.75f && t < 1.0f) {
        blendT = (t - 0.75f) / 0.25f;
        currentPose = &skeletalGame->poses[4];
        nextPose = &skeletalGame->poses[5];
    }
    else if (t >= 1.0f && t < 1.25f) {
        blendT = (t - 1.0f) / 0.25f;
        currentPose = &skeletalGame->poses[5];
        nextPose = &skeletalGame->poses[6];
    }
    else if (t >= 1.25f && t < 1.5f) {
        blendT = (t - 1.25f) / 0.25f;
        currentPose = &skeletalGame->poses[6];
        nextPose = &skeletalGame->poses[7];
    }
    else if (t >= 1.5f && t < 1.75f) {
        blendT = (t - 1.5f) / 0.25f;
        currentPose = &skeletalGame->poses[7];
        nextPose = &skeletalGame->poses[8];
    }
    else {//if (t >= 1.75f && t < 2.0f) {
        blendT = (t - 1.75f) / 0.25f;
        currentPose = &skeletalGame->poses[8];
        nextPose = &skeletalGame->poses[1];
    }

    // calculate interpolated positions/rotations
    for (int i = 0; i < skeletalGame->numBones; ++i) {
        skeleton_bone *bone = &skeletalGame->bones[i];
        skeleton_bone_pose *currentBonePose = &currentPose->bonePoses[i];
        skeleton_bone_pose *nextBonePose = &nextPose->bonePoses[i];
        assert(i == currentBonePose->boneID);
        assert(i == nextBonePose->boneID);

        bone->localPos = currentBonePose->localPos * (1.0f - blendT) + blendT * (nextBonePose->localPos);
        bone->localRotation = lerp(currentBonePose->localRotation, nextBonePose->localRotation, blendT);
    }

    calculateBoneTransforms(skeletalGame);

    matrix4x4 jointMatrix = scaleMatrix4x4(0.25f);
    matrix4x4 lengthMatrix = scaleMatrix4x4(0.125f, 0.125f, 0.5f);
    lengthMatrix = translationMatrix(0.0f, 0.0f, 0.5f) * lengthMatrix;

    // NOTE(ebuchholz): skip root for now so it doesn't look... suggestive
    for (int i = 1; i < skeletalGame->numBones; ++i) {
        skeleton_bone *bone = &skeletalGame->bones[i];
        matrix4x4 boneTransform = bone->transform;

        matrix4x4 jointTransform = boneTransform * jointMatrix;
        drawModel(MESH_KEY_CUBE, TEXTURE_KEY_BLUE, jointTransform, renderCommands);

        matrix4x4 lengthTransform = boneTransform * lengthMatrix;
        drawModel(MESH_KEY_CUBE, TEXTURE_KEY_BLUE, lengthTransform, renderCommands);
    }
}
