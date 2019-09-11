#ifndef SKELETAL_GAME_H
#define SKELETAL_GAME_H

#define MAX_NUM_CHILD_BONES 4
#define NUM_DEBUG_BONES 16

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

struct skeleton_pose {
    skeleton_bone_pose bonePoses[NUM_DEBUG_BONES];
};

struct skeletal_game {
    debug_camera debugCamera;

    int numBones;
    skeleton_bone bones[NUM_DEBUG_BONES];
    skeleton_pose poses[2];
};

#endif
