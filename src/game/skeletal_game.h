#ifndef SKELETAL_GAME_H
#define SKELETAL_GAME_H

#define MAX_NUM_CHILD_BONES 4
#define MAX_NUM_KEYFRAMES 30

struct debug_camera {
    vector3 pos;
    quaternion rotation;
    int lastPointerX;
    int lastPointerY;
    vector3 lookAtTarget;
    vector3 up;
};

struct animation_state {
    int animationID;
    int currentKeyframe;
    int nextKeyframe;
    float t;
};

struct combined_animation_state {
    animation_state animationState;
    animation_state prevAnimationState;
    float blendT;
};

struct walking_legs {
    combined_animation_state animState;
    float animationSwitchTimer;
    float x;
};

struct curling_elbows {
    combined_animation_state animState;
    float animationSwitchTimer;
    float x;
};

struct skeletal_game {
    debug_camera debugCamera;

    walking_legs walkingLegs[6];
    curling_elbows curlingElbows[6];

    combined_animation_state snakeAnimationState;
};

#endif
