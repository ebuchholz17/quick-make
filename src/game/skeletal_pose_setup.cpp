void setupSkeleton (skeletal_game *skeletalGame) {
    // TODO(ebuchholz): load from a file

//         0
//        1|2
//       o-o-o 
//     3 |   | 4
//       o   o 
//     5 |   | 6
//     --o   o--
//       7   8

    skeletalGame->numBones = 9;
    skeleton_bone *bone = &skeletalGame->bones[0];
    bone->parentID = -1;

    bone = &skeletalGame->bones[1];
    bone->parentID = 0;

    bone = &skeletalGame->bones[2];
    bone->parentID = 0;

    bone = &skeletalGame->bones[3];
    bone->parentID = 1;

    bone = &skeletalGame->bones[4];
    bone->parentID = 2;

    bone = &skeletalGame->bones[5];
    bone->parentID = 3;

    bone = &skeletalGame->bones[6];
    bone->parentID = 4;

    bone = &skeletalGame->bones[7];
    bone->parentID = 5;

    bone = &skeletalGame->bones[8];
    bone->parentID = 6;

    // define "t-pose"
    // root
    skeleton_pose *restPose = &skeletalGame->poses[0];
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

    // left foot passing right
    // root
    skeleton_pose *currentPose = &skeletalGame->poses[1];
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
    currentPose = &skeletalGame->poses[2];
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
    currentPose = &skeletalGame->poses[3];
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
    currentPose = &skeletalGame->poses[4];
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
    currentPose = &skeletalGame->poses[5];
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
    currentPose = &skeletalGame->poses[6];
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
    currentPose = &skeletalGame->poses[7];
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
    currentPose = &skeletalGame->poses[8];
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

    // set up keyframes, animations
    // idle
    skeleton_animation *idleAnimation = &skeletalGame->animations[0];
    idleAnimation->numKeyFrames = 1;
    idleAnimation->duration = 0.0f;
    skeleton_keyframe *keyframe = &idleAnimation->keyframes[0];
    keyframe->t = 0.0f;
    keyframe->poseID = 0;

    // walking
    skeleton_animation *walkingAnimation = &skeletalGame->animations[1];
    walkingAnimation->numKeyFrames = 8;
    walkingAnimation->duration = 0.72f;

    keyframe = &walkingAnimation->keyframes[0];
    keyframe->t = 0.0f;
    keyframe->poseID = 1;

    keyframe = &walkingAnimation->keyframes[1];
    keyframe->t = 0.09f;
    keyframe->poseID = 2;

    keyframe = &walkingAnimation->keyframes[2];
    keyframe->t = 0.18f;
    keyframe->poseID = 3;

    keyframe = &walkingAnimation->keyframes[3];
    keyframe->t = 0.27f;
    keyframe->poseID = 4;

    keyframe = &walkingAnimation->keyframes[4];
    keyframe->t = 0.36f;
    keyframe->poseID = 5;

    keyframe = &walkingAnimation->keyframes[5];
    keyframe->t = 0.45f;
    keyframe->poseID = 6;

    keyframe = &walkingAnimation->keyframes[6];
    keyframe->t = 0.54f;
    keyframe->poseID = 7;

    keyframe = &walkingAnimation->keyframes[7];
    keyframe->t = 0.63f;
    keyframe->poseID = 8;
}
