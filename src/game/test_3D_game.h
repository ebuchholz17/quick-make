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

struct test_3D_game {
    debug_camera debugCamera;
    bool initialized;
    float cubeT;
};

#endif
