#include "skeletal_game.h"


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

    // state for animated models
    for (int i = 0; i < 6; ++i) {
        skeletalGame->walkingLegs[i] = {};
        skeletalGame->walkingLegs[i].animState.animationState.animationID = -1;
        skeletalGame->walkingLegs[i].animState.prevAnimationState.animationID = -1;
    }
    skeletalGame->walkingLegs[0].animationSwitchTimer = 0.0f;
    skeletalGame->walkingLegs[0].x = 0.0f;
    skeletalGame->walkingLegs[1].animationSwitchTimer = 1.0f;
    skeletalGame->walkingLegs[1].x = 3.0f;
    skeletalGame->walkingLegs[2].animationSwitchTimer = 2.0f;
    skeletalGame->walkingLegs[2].x = 6.0f;
    skeletalGame->walkingLegs[3].animationSwitchTimer = 3.0f;
    skeletalGame->walkingLegs[3].x = 9.0f;
    skeletalGame->walkingLegs[4].animationSwitchTimer = 4.0f;
    skeletalGame->walkingLegs[4].x = 12.0f;
    skeletalGame->walkingLegs[5].animationSwitchTimer = 5.0f;
    skeletalGame->walkingLegs[5].x = 15.0f;

    for (int i = 0; i < 6; ++i) {
        skeletalGame->curlingElbows[i] = {};
        skeletalGame->curlingElbows[i].animState.animationState.animationID = 0;
        skeletalGame->curlingElbows[i].animState.animationState.t = i * 0.33f;
        skeletalGame->curlingElbows[i].animState.animationState.nextKeyframe = 1;
        skeletalGame->curlingElbows[i].animState.prevAnimationState.animationID = -1;
    }
    skeletalGame->curlingElbows[0].x = 0.0f;
    skeletalGame->curlingElbows[1].x = 3.0f;
    skeletalGame->curlingElbows[2].x = 6.0f;
    skeletalGame->curlingElbows[3].x = 9.0f;
    skeletalGame->curlingElbows[4].x = 12.0f;
    skeletalGame->curlingElbows[5].x = 15.0f;

    skeletalGame->snakeAnimationState = {};
    skeletalGame->snakeAnimationState.animationState.animationID = -1;
    skeletalGame->snakeAnimationState.prevAnimationState.animationID = -1;
}

void calculateBoneTransforms (skeleton_bone *bones, int numBones) {
    for (int i = 0; i < numBones; ++i) {
        skeleton_bone *currentBone = &bones[i];

        matrix4x4 localTransform = matrix4x4FromQuaternion(currentBone->localRotation);
        localTransform = translationMatrix(currentBone->localPos) * localTransform;

        if (currentBone->parentID == -1) {
            currentBone->transform = localTransform;
        }
        else {
            skeleton_bone *parentBone = &bones[currentBone->parentID];
            currentBone->transform = parentBone->transform * localTransform;
        }
    }
}

// TODO(ebuchholz): clean up parameters
bool calcBonesForAnimation (skeleton_animation *animation, animation_state *animationState, bool loop,
                            skeleton_pose *poses, skeleton_bone *bones, int numBones) 
{
    bool animationComplete = false;

    skeleton_pose *currentPose;
    if (animation->numKeyFrames > 1) {
        animationState->t += DELTA_TIME;

        skeleton_keyframe *nextKeyframe = &animation->keyframes[animationState->nextKeyframe];
        float nextT;
        if (animationState->nextKeyframe == 0) {
            nextT = animation->duration;
        }
        else {
            nextT = nextKeyframe->t;
        }
        if (animationState->t >= nextT) {
            if (loop) {
                while (animationState->t >= animation->duration) {
                    animationState->t -= animation->duration;
                }
            }
            else {
                if (animationState->t >= animation->duration) {
                    animationState->t = animation->duration;
                    animationComplete = true;
                }
            }

            if (!animationComplete) {
                animationState->currentKeyframe = animationState->nextKeyframe;
                animationState->nextKeyframe = (animationState->currentKeyframe + 1) % animation->numKeyFrames;
                nextKeyframe = &animation->keyframes[animationState->nextKeyframe];
            }
        }

        skeleton_keyframe *currentKeyframe = &animation->keyframes[animationState->currentKeyframe];
        float startT = currentKeyframe->t;
        float endT = animationState->nextKeyframe == 0 ? animation->duration : nextKeyframe->t;
        float blendT = (animationState->t - startT) / (endT - startT);

        currentPose = &poses[currentKeyframe->poseID];
        skeleton_pose *nextPose = &poses[nextKeyframe->poseID];
        // calculate interpolated positions/rotations
        for (int i = 0; i < numBones; ++i) {
            skeleton_bone *bone = &bones[i];
            skeleton_bone_pose *currentBonePose = &currentPose->bonePoses[i];
            skeleton_bone_pose *nextBonePose = &nextPose->bonePoses[i];
            assert(i == currentBonePose->boneID);
            assert(i == nextBonePose->boneID);

            bone->localPos = currentBonePose->localPos * (1.0f - blendT) + blendT * (nextBonePose->localPos);
            bone->localRotation = lerp(currentBonePose->localRotation, nextBonePose->localRotation, blendT);
        }
    }
    else {
        skeleton_keyframe *keyframe = &animation->keyframes[0];
        currentPose = &poses[keyframe->poseID];

        // just match the single keyframe
        for (int i = 0; i < numBones; ++i) {
            skeleton_bone *bone = &bones[i];
            skeleton_bone_pose *currentBonePose = &currentPose->bonePoses[i];
            assert(i == currentBonePose->boneID);

            bone->localPos = currentBonePose->localPos;
            bone->localRotation = currentBonePose->localRotation;
        }
    }

    return animationComplete;
} 

inline void initAnimationState (animation_state *animState, int animationID, skeleton_animation *animation) {
    animState->animationID = animationID;
    animState->t = 0.0f;
    animState->currentKeyframe = 0;
    if (animation->numKeyFrames == 1) {
        animState->nextKeyframe = 0;
    }
    else {
        animState->nextKeyframe = 1;
    }
}

bool playAnimation (animation_data_key animationDataKey, char *animationKey, combined_animation_state *combinedAnimationState, bool loop,
                    game_assets *assets, memory_arena *tempMemory, skeleton_bone **resultBones, int *numResultBones) 
{
    // TODO(ebuchholz): clean up the way this generates bones- maybe defer that part til later
    bool animationComplete = false;
    animation_data *animationData = getAnimationData(animationDataKey, assets);
    int *boneHierarchy = animationData->boneHierarchy;
    int numBones = animationData->numBones;

    skeleton_animation *animation = getAnimationFromData(animationData, animationKey);
    int animationID = animation->id;

    animation_state *currentAnimState = &combinedAnimationState->animationState;
    animation_state *prevAnimationState = &combinedAnimationState->prevAnimationState;

    // TODO(ebuchholz): make sure id, bones, poses, etc are valid
    if (currentAnimState->animationID == -1) {
        initAnimationState(currentAnimState, animationID, animation);
    }
    else if (currentAnimState->animationID != animationID) {
        // setup blend animations
        prevAnimationState->animationID = currentAnimState->animationID;
        prevAnimationState->t = currentAnimState->t;
        prevAnimationState->currentKeyframe = currentAnimState->currentKeyframe;
        prevAnimationState->nextKeyframe = currentAnimState->nextKeyframe;

        combinedAnimationState->blendT = 0.0f;

        initAnimationState(currentAnimState, animationID, animation);
    }

    // allocate some bones from temporary memory
    // TODO(ebuchholz): should probably remove this bit
    unsigned int skeletonSize = sizeof(skeleton_bone) * numBones;
    skeleton_bone *bones = (skeleton_bone *)allocateMemorySize(tempMemory, skeletonSize);
    for (int i = 0; i < numBones; ++i) {
        skeleton_bone *bone = &bones[i];
        *bone = {};
        bone->parentID = animationData->boneHierarchy[i];
    }
    *resultBones = bones;
    *numResultBones = numBones;

    // if prev animation id != -1 -- make -1 after blended, or after previous complete
    if (prevAnimationState->animationID != -1) {
        // TODO(ebuchholz): maybe use some temporary memory for intermediate bones
        // TODO(ebuchholz): nice way of temprorary allocating memory and freeing it
        skeleton_bone *prevBones = (skeleton_bone *)allocateMemorySize(tempMemory, skeletonSize);
        skeleton_bone *nextBones = (skeleton_bone *)allocateMemorySize(tempMemory, skeletonSize);
        for (int i = 0; i < numBones; ++i) {
            prevBones[i] = bones[i];
            nextBones[i] = bones[i];
        }
        skeleton_animation *prevAnimation = &animationData->animations[prevAnimationState->animationID];
        calcBonesForAnimation(prevAnimation, prevAnimationState, loop, animationData->poses, prevBones, numBones);
        animationComplete = calcBonesForAnimation(animation, currentAnimState, loop, animationData->poses, nextBones, numBones);

        bool prevAnimComplete = false;
        combinedAnimationState->blendT += DELTA_TIME;
        float animTransitionTime = 0.1f;
        if (combinedAnimationState->blendT >= animTransitionTime) {
            combinedAnimationState->blendT = animTransitionTime;
            prevAnimComplete = true;
        }

        for (int i = 0; i < numBones; ++i) {
            skeleton_bone *bone = &bones[i];
            skeleton_bone *prevBone = &prevBones[i];
            skeleton_bone *nextBone = &nextBones[i];

            float animBlendT = combinedAnimationState->blendT / animTransitionTime;

            bone->localPos = prevBone->localPos * (1.0f - animBlendT) + animBlendT * (nextBone->localPos);
            bone->localRotation = lerp(prevBone->localRotation, nextBone->localRotation, animBlendT);
        }

        freeMemorySize(tempMemory, skeletonSize);
        freeMemorySize(tempMemory, skeletonSize);

        if (prevAnimComplete) {
            prevAnimationState->animationID = -1;
        }
    }
    else {
        animationComplete = calcBonesForAnimation(animation, currentAnimState, loop, animationData->poses, bones, numBones);
    }

    return animationComplete;
}

void updateWalkingLegs (walking_legs *walkingLegs, memory_arena *tempMemory, game_assets *assets, render_command_list *renderCommands) {
    // test animation blending
    float walkingT = walkingLegs->animationSwitchTimer;
    bool walking = true;
    walkingT += DELTA_TIME;
    if (walkingT > 6.0f) {
        walkingT -= 6.0f;
        walking = true;
    }
    else if (walkingT > 3.0f) {
        walking = false;
    }
    walkingLegs->animationSwitchTimer = walkingT;

    skeleton_bone *bones = 0;
    int numBones = 0;

    if (walking) {
        bool animComplete = playAnimation(ANIMATION_DATA_KEY_LEGS, "walking", &walkingLegs->animState,
                                          true, assets, tempMemory, &bones, &numBones);
    }
    else {
        playAnimation(ANIMATION_DATA_KEY_LEGS, "idle", &walkingLegs->animState,
                      false, assets, tempMemory, &bones, &numBones);
    }

    calculateBoneTransforms(bones, numBones);

    // TODO(ebuchholz): somewhere, precompute inverse rest transforms
    matrix4x4 jointMatrix = scaleMatrix4x4(0.25f);
    matrix4x4 lengthMatrix = scaleMatrix4x4(0.125f, 0.125f, 0.5f);
    lengthMatrix = translationMatrix(0.0f, 0.0f, 0.5f) * lengthMatrix;
    matrix4x4 wholeModelTranslate = translationMatrix(walkingLegs->x + 15.0f, 0.0f, 0.0f);

    // NOTE(ebuchholz): skip root for now so it doesn't look... suggestive
    for (int i = 1; i < numBones; ++i) {
        skeleton_bone *bone = &bones[i];
        matrix4x4 boneTransform = bone->transform;

        matrix4x4 jointTransform = wholeModelTranslate * boneTransform * jointMatrix;
        drawModel(MESH_KEY_CUBE, TEXTURE_KEY_BLUE, jointTransform, renderCommands);

        matrix4x4 lengthTransform = wholeModelTranslate * boneTransform * lengthMatrix;
        drawModel(MESH_KEY_CUBE, TEXTURE_KEY_BLUE, lengthTransform, renderCommands);
    }
}

void updateCurlingElbows (curling_elbows *curlingElbows, memory_arena *tempMemory, game_assets *assets, render_command_list *renderCommands) {
    // test animation blending
    skeleton_bone *bones = 0;
    int numBones = 0;

    playAnimation(ANIMATION_DATA_KEY_MULTI_ELBOW, "curling", &curlingElbows->animState,
                  true, assets, tempMemory, &bones, &numBones);

    calculateBoneTransforms(bones, numBones);

    // TODO(ebuchholz): somewhere, precompute inverse rest transforms
    matrix4x4 jointMatrix = scaleMatrix4x4(0.25f);
    matrix4x4 lengthMatrix = scaleMatrix4x4(0.125f, 0.125f, 0.5f);
    lengthMatrix = translationMatrix(0.0f, 0.0f, 0.5f) * lengthMatrix;
    matrix4x4 wholeModelTranslate = translationMatrix(curlingElbows->x + 15.0f, 0.0f, -10.0f);

    for (int i = 0; i < numBones; ++i) {
        skeleton_bone *bone = &bones[i];
        matrix4x4 boneTransform = bone->transform;

        matrix4x4 jointTransform = wholeModelTranslate * boneTransform * jointMatrix;
        drawModel(MESH_KEY_CUBE, TEXTURE_KEY_BLUE, jointTransform, renderCommands);

        matrix4x4 lengthTransform = wholeModelTranslate * boneTransform * lengthMatrix;
        drawModel(MESH_KEY_CUBE, TEXTURE_KEY_BLUE, lengthTransform, renderCommands);
    }
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

    for (int i = 0; i < 6; ++i) {
        updateWalkingLegs(&skeletalGame->walkingLegs[i], tempMemory, assets, renderCommands);
    }
    for (int i = 0; i < 6; ++i) {
        updateCurlingElbows(&skeletalGame->curlingElbows[i], tempMemory, assets, renderCommands);
    }

    matrix4x4 *inverseRestTransforms = getInverseTransformsForAnimatedModel(ANIMATION_DATA_KEY_MULTI_ELBOW, assets);

    skeleton_bone *bones = 0;
    int numBones = 0;

    playAnimation(ANIMATION_DATA_KEY_SNAKE, "snaking", &skeletalGame->snakeAnimationState,
                  true, assets, tempMemory, &bones, &numBones);

    calculateBoneTransforms(bones, numBones);
    drawAnimatedModel(ANIM_MESH_KEY_TUBE_SNAKE, TEXTURE_KEY_SNAKE_PATTERN, translationMatrix(0.0f, 0.0f, -9.0f), bones, inverseRestTransforms, numBones, renderCommands);
}
