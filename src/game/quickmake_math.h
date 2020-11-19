#ifndef QUICKMAKE_MATH_H
#define QUICKMAKE_MATH_H

#include <math.h> // TODO(ebuchholz): see if we can remove this later

// TODO(ebuchholz): Better constants
#define PI 3.14159265359f
#define EPSILON 0.0000001f

union matrix3x3 {
    float m[9];
    // TODO(ebuchholz): see if we can add some other ways to access the elements
};

union matrix4x4 {
    float m[16];

    // TODO(ebuchholz): 2 dimensional array? struct with individual members? [] operator?
};

union vector2 {
    struct {
        float x;
        float y;
    };
    float c[2];
};

union vector3 {
    struct {
        float x;
        float y;
        float z;
    };
    struct {
        float r;
        float g;
        float b;
    };
    float c[3];
};

union vector4 {
    struct {
        float x;
        float y;
        float z;
        float w;
    };
    struct {
        float r;
        float g;
        float b;
        float a;
    };
    float c[4];
};

// TODO(ebuchholz): double check all of this quaternion stuff
struct quaternion {
    float w;
    float x;
    float y;
    float z;
};

struct rectangle {
    vector2 min;
    vector2 max;
};

struct aabb {
    vector3 min;
    vector3 max;
};

struct sphere {
    vector3 pos;
    float radius;
};

struct plane {
    vector3 normal;
    float distance;
};

struct triangle {
    vector3 p0;
    vector3 p1;
    vector3 p2;
};

struct triangleWithNormals {
    vector3 p0;
    vector3 p1;
    vector3 p2;
    vector3 n0;
    vector3 n1;
    vector3 n2;
};

struct line {
    vector3 a;
    vector3 b;
};

struct frustum {
    vector3 origin;
    quaternion orientation;
    float nearPlane;
    float farPlane;
    float fov;
    float ratio;
};

struct frustum_corners {
    vector3 nearUpperLeft;
    vector3 nearUpperRight;
    vector3 nearLowerLeft;
    vector3 nearLowerRight;
    vector3 farUpperLeft;
    vector3 farUpperRight;
    vector3 farLowerLeft;
    vector3 farLowerRight;
};

#endif
