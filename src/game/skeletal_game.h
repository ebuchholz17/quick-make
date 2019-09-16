#ifndef SKELETAL_GAME_H
#define SKELETAL_GAME_H

#define MAX_NUM_CHILD_BONES 4
#define NUM_DEBUG_BONES 16
#define MAX_NUM_KEYFRAMES 30

struct debug_camera {
    vector3 pos;
    quaternion rotation;
    int lastPointerX;
    int lastPointerY;
    vector3 lookAtTarget;
    vector3 up;
};

struct skeleton_bone {
    matrix4x4 transform;
    matrix4x4 inverseRestTransform;
    quaternion localRotation;
    vector3 localPos;
    int parentID;
};

struct skeleton_bone_pose {
    quaternion localRotation;
    vector3 localPos;
    int boneID;
};

// TODO(ebuchholz): allocate the exact number of bones needed
// TODO(ebuchholz): force animations to correspond to their model? or allow many
// models to use the same animations
struct skeleton_pose {
    skeleton_bone_pose bonePoses[NUM_DEBUG_BONES];
};

//         0
//        1|2
//       o-o-o 
//     3 |   | 4
//       o   o 
//     5 |   | 6
//     --o   o--
//       7   8

// TODO(ebuchholz): allocate the exact number of keyframes when the asset is
// loaded
struct skeleton_keyframe {
    int poseID;
    float t;
};

struct skeleton_animation {
    // TODO(ebuchholz): store keyframes elsewhere? separate from animation struct?
    skeleton_keyframe keyframes[MAX_NUM_KEYFRAMES];
    int numKeyFrames;
    float duration;
};

struct animation_state {
    int animationID;
    int currentKeyframe;
    int nextKeyframe;
    float t;
    bool loop;
};

struct skeletal_game {
    debug_camera debugCamera;

    int numBones;
    skeleton_bone bones[NUM_DEBUG_BONES];
    skeleton_pose poses[9];

    skeleton_animation animations[2];
    animation_state currentAnimation;
};

#endif
