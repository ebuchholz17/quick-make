// NOTE(ebuchholz): seeing if we can get away without using any of these functions in the
// opengl/webgl renderers (only in game code)

#include "quickmake_math.h"

inline matrix3x3 operator* (matrix3x3 a, matrix3x3 b) {
    matrix3x3 result;

    result.m[0] = a.m[0]*b.m[0] + a.m[1]*b.m[3] + a.m[2]*b.m[6];
    result.m[1] = a.m[0]*b.m[1] + a.m[1]*b.m[4] + a.m[2]*b.m[7];
    result.m[2] = a.m[0]*b.m[2] + a.m[1]*b.m[5] + a.m[2]*b.m[8];

    result.m[3] = a.m[3]*b.m[0] + a.m[4]*b.m[3] + a.m[5]*b.m[6]; 
    result.m[4] = a.m[3]*b.m[1] + a.m[4]*b.m[4] + a.m[5]*b.m[7]; 
    result.m[5] = a.m[3]*b.m[2] + a.m[4]*b.m[5] + a.m[5]*b.m[8];

    result.m[6] = a.m[6]*b.m[0] + a.m[7]*b.m[3] + a.m[8]*b.m[6]; 
    result.m[7] = a.m[6]*b.m[1] + a.m[7]*b.m[4] + a.m[8]*b.m[7]; 
    result.m[8] = a.m[6]*b.m[2] + a.m[7]*b.m[5] + a.m[8]*b.m[8];

    return result;
}

inline matrix3x3 operator* (float a, matrix3x3 b) {
    matrix3x3 result;

    result.m[0] = a*b.m[0];
    result.m[1] = a*b.m[1];
    result.m[2] = a*b.m[2];
    result.m[3] = a*b.m[3];
    result.m[4] = a*b.m[4];
    result.m[5] = a*b.m[5];
    result.m[6] = a*b.m[6];
    result.m[7] = a*b.m[7];
    result.m[8] = a*b.m[8];

    return result;
}

inline matrix3x3 transpose (matrix3x3 a) {
    matrix3x3 result;

    result.m[0] = a.m[0];
    result.m[1] = a.m[3];
    result.m[2] = a.m[6];
    result.m[3] = a.m[1];
    result.m[4] = a.m[4];
    result.m[5] = a.m[7];
    result.m[6] = a.m[2];
    result.m[7] = a.m[5];
    result.m[8] = a.m[8];

    return result;
}
// TODO(ebuchholz): maybe a 3x3 inverse transpose for tranforming normals

// TODO(ebuchholz): implement
inline matrix3x3 inverse (matrix3x3 a) {
    // 0 1 2
    // 3 4 5
    // 6 7 8

    matrix3x3 minors = {};

    minors.m[0] = a.m[4]*a.m[8] - a.m[5]*a.m[7];
    minors.m[1] = -(a.m[3]*a.m[8] - a.m[5]*a.m[6]);
    minors.m[2] = a.m[3]*a.m[7] - a.m[4]*a.m[6];

    minors.m[3] = -(a.m[1]*a.m[8] - a.m[2]*a.m[7]);
    minors.m[4] = a.m[0]*a.m[8] - a.m[2]*a.m[6];
    minors.m[5] = -(a.m[0]*a.m[7] - a.m[1]*a.m[6]);

    minors.m[6] = a.m[1]*a.m[5] - a.m[2]*a.m[4];
    minors.m[7] = -(a.m[0]*a.m[5] - a.m[2]*a.m[3]);
    minors.m[8] = a.m[0]*a.m[4] - a.m[1]*a.m[3];

    float determinant = a.m[0]*minors.m[0] + a.m[1]*minors.m[1] + a.m[2]*minors.m[2]; // Check for 0?
    float ood = 1.0f / determinant;

    // need * operator for float * matri3x3
    matrix3x3 result = ood * transpose(minors);

    return result;
}

inline matrix3x3 identityMatrix3x3 () {
    matrix3x3 result = {};

    result.m[0] = 1.0f;
    result.m[4] = 1.0f;
    result.m[8] = 1.0f;

    return result;
}

inline matrix3x3 translationMatrix (float x, float y) {
    matrix3x3 result = identityMatrix3x3();

    result.m[2] = x;
    result.m[5] = y;

    return result;
}

inline matrix3x3 scaleMatrix3x3 (float s) {
    matrix3x3 result = {};

    result.m[0] = s;
    result.m[4] = s;
    result.m[8] = 1.0f;

    return result;
}

inline matrix3x3 scaleMatrix3x3 (float x, float y) {
    matrix3x3 result = {};

    result.m[0] = x;
    result.m[4] = y;
    result.m[8] = 1.0f;

    return result;
}

inline matrix3x3 rotationMatrix3x3 (float angle) {
    matrix3x3 result = identityMatrix3x3();

    result.m[0] = cosf(angle);
    result.m[1] = -sinf(angle);
    result.m[3] = sinf(angle);
    result.m[4] = cosf(angle);

    return result;
}

inline matrix4x4 operator* (matrix4x4 a, matrix4x4 b) {
    matrix4x4 result;

    result.m[0] = a.m[0]*b.m[0] + a.m[1]*b.m[4] + a.m[2]*b.m[8]  + a.m[3]*b.m[12];
    result.m[1] = a.m[0]*b.m[1] + a.m[1]*b.m[5] + a.m[2]*b.m[9]  + a.m[3]*b.m[13];
    result.m[2] = a.m[0]*b.m[2] + a.m[1]*b.m[6] + a.m[2]*b.m[10] + a.m[3]*b.m[14];
    result.m[3] = a.m[0]*b.m[3] + a.m[1]*b.m[7] + a.m[2]*b.m[11] + a.m[3]*b.m[15];

    result.m[4] = a.m[4]*b.m[0] + a.m[5]*b.m[4] + a.m[6]*b.m[8]  + a.m[7]*b.m[12];
    result.m[5] = a.m[4]*b.m[1] + a.m[5]*b.m[5] + a.m[6]*b.m[9]  + a.m[7]*b.m[13];
    result.m[6] = a.m[4]*b.m[2] + a.m[5]*b.m[6] + a.m[6]*b.m[10] + a.m[7]*b.m[14];
    result.m[7] = a.m[4]*b.m[3] + a.m[5]*b.m[7] + a.m[6]*b.m[11] + a.m[7]*b.m[15];

    result.m[8]  = a.m[8]*b.m[0] + a.m[9]*b.m[4] + a.m[10]*b.m[8]  + a.m[11]*b.m[12];
    result.m[9]  = a.m[8]*b.m[1] + a.m[9]*b.m[5] + a.m[10]*b.m[9]  + a.m[11]*b.m[13];
    result.m[10] = a.m[8]*b.m[2] + a.m[9]*b.m[6] + a.m[10]*b.m[10] + a.m[11]*b.m[14];
    result.m[11] = a.m[8]*b.m[3] + a.m[9]*b.m[7] + a.m[10]*b.m[11] + a.m[11]*b.m[15];

    result.m[12] = a.m[12]*b.m[0] + a.m[13]*b.m[4] + a.m[14]*b.m[8]  + a.m[15]*b.m[12];
    result.m[13] = a.m[12]*b.m[1] + a.m[13]*b.m[5] + a.m[14]*b.m[9]  + a.m[15]*b.m[13];
    result.m[14] = a.m[12]*b.m[2] + a.m[13]*b.m[6] + a.m[14]*b.m[10] + a.m[15]*b.m[14];
    result.m[15] = a.m[12]*b.m[3] + a.m[13]*b.m[7] + a.m[14]*b.m[11] + a.m[15]*b.m[15];

    return result;
}

inline matrix4x4 operator* (float a, matrix4x4 b) {
    matrix4x4 result;

    result.m[0] = a*b.m[0];
    result.m[1] = a*b.m[1];
    result.m[2] = a*b.m[2];
    result.m[3] = a*b.m[3];
    result.m[4] = a*b.m[4];
    result.m[5] = a*b.m[5];
    result.m[6] = a*b.m[6];
    result.m[7] = a*b.m[7];
    result.m[8] = a*b.m[8];
    result.m[9] = a*b.m[9];
    result.m[10] = a*b.m[10];
    result.m[11] = a*b.m[11];
    result.m[12] = a*b.m[12];
    result.m[13] = a*b.m[13];
    result.m[14] = a*b.m[14];
    result.m[15] = a*b.m[15];

    return result;
}

inline matrix4x4 transpose (matrix4x4 a) {
    matrix4x4 result;

    result.m[0] = a.m[0];
    result.m[1] = a.m[4];
    result.m[2] = a.m[8];
    result.m[3] = a.m[12];
    result.m[4] = a.m[1];
    result.m[5] = a.m[5];
    result.m[6] = a.m[9];
    result.m[7] = a.m[13];
    result.m[8] = a.m[2];
    result.m[9] = a.m[6];
    result.m[10] = a.m[10];
    result.m[11] = a.m[14];
    result.m[12] = a.m[3];
    result.m[13] = a.m[7];
    result.m[14] = a.m[11];
    result.m[15] = a.m[15];

    return result;
}

// TODO(ebuchholz): fix this travesty
inline matrix4x4 inverse (matrix4x4 a) {
    matrix4x4 minors = {};

    float a0m0 = a.m[10]*a.m[15] - a.m[11]*a.m[14];
    float a0m1 = a.m[9]*a.m[15] - a.m[11]*a.m[13];
    float a0m2 = a.m[9]*a.m[14] - a.m[10]*a.m[13];
    minors.m[0] = a.m[5]*a0m0 - a.m[6]*a0m1 + a.m[7]*a0m2;

    float a1m0 = a.m[10]*a.m[15] - a.m[11]*a.m[14];
    float a1m1 = a.m[8]*a.m[15] - a.m[11]*a.m[12];
    float a1m2 = a.m[8]*a.m[14] - a.m[10]*a.m[12];
    minors.m[1] = -(a.m[4]*a1m0 - a.m[6]*a1m1 + a.m[7]*a1m2);

    float a2m0 = a.m[9]*a.m[15] - a.m[11]*a.m[13];
    float a2m1 = a.m[8]*a.m[15] - a.m[11]*a.m[12];
    float a2m2 = a.m[8]*a.m[13] - a.m[9]*a.m[12];
    minors.m[2] = a.m[4]*a2m0 - a.m[5]*a2m1 + a.m[7]*a2m2;

    float a3m0 = a.m[9]*a.m[14] - a.m[10]*a.m[13];
    float a3m1 = a.m[8]*a.m[14] - a.m[10]*a.m[12];
    float a3m2 = a.m[8]*a.m[13] - a.m[9]*a.m[12];
    minors.m[3] = -(a.m[4]*a3m0 - a.m[5]*a3m1 + a.m[6]*a3m2);


    float a4m0 = a.m[10]*a.m[15] - a.m[11]*a.m[14];
    float a4m1 = a.m[9]*a.m[15] - a.m[11]*a.m[13];
    float a4m2 = a.m[9]*a.m[14] - a.m[10]*a.m[13];
    minors.m[4] = -(a.m[1]*a4m0 - a.m[2]*a4m1 + a.m[3]*a4m2);

    float a5m0 = a.m[10]*a.m[15] - a.m[11]*a.m[14];
    float a5m1 = a.m[8]*a.m[15] - a.m[11]*a.m[12];
    float a5m2 = a.m[8]*a.m[14] - a.m[10]*a.m[12];
    minors.m[5] = a.m[0]*a5m0 - a.m[2]*a5m1 + a.m[3]*a5m2;

    float a6m0 = a.m[9]*a.m[15] - a.m[11]*a.m[13];
    float a6m1 = a.m[8]*a.m[15] - a.m[11]*a.m[12];
    float a6m2 = a.m[8]*a.m[13] - a.m[9]*a.m[12];
    minors.m[6] = -(a.m[0]*a6m0 - a.m[1]*a6m1 + a.m[3]*a6m2);

    float a7m0 = a.m[9]*a.m[14] - a.m[10]*a.m[13];
    float a7m1 = a.m[8]*a.m[14] - a.m[10]*a.m[12];
    float a7m2 = a.m[8]*a.m[13] - a.m[9]*a.m[12];
    minors.m[7] = a.m[0]*a7m0 - a.m[1]*a7m1 + a.m[2]*a7m2;


    float a8m0 = a.m[6]*a.m[15] - a.m[7]*a.m[14];
    float a8m1 = a.m[5]*a.m[15] - a.m[7]*a.m[13];
    float a8m2 = a.m[5]*a.m[14] - a.m[6]*a.m[13];
    minors.m[8] = a.m[1]*a8m0 - a.m[2]*a8m1 + a.m[3]*a8m2;

    float a9m0 = a.m[6]*a.m[15] - a.m[7]*a.m[14];
    float a9m1 = a.m[4]*a.m[15] - a.m[7]*a.m[12];
    float a9m2 = a.m[4]*a.m[14] - a.m[6]*a.m[12];
    minors.m[9] = -(a.m[0]*a9m0 - a.m[2]*a9m1 + a.m[3]*a9m2);

    float a10m0 = a.m[5]*a.m[15] - a.m[7]*a.m[13];
    float a10m1 = a.m[4]*a.m[15] - a.m[7]*a.m[12];
    float a10m2 = a.m[4]*a.m[13] - a.m[5]*a.m[12];
    minors.m[10] = a.m[0]*a10m0 - a.m[1]*a10m1 + a.m[3]*a10m2;

    float a11m0 = a.m[5]*a.m[14] - a.m[6]*a.m[13];
    float a11m1 = a.m[8]*a.m[14] - a.m[6]*a.m[12];
    float a11m2 = a.m[8]*a.m[13] - a.m[5]*a.m[12];
    minors.m[11] = -(a.m[0]*a11m0 - a.m[1]*a11m1 + a.m[2]*a11m2);

    //  0  1  2  3
    //  4  5  6  7
    //  8  9 10 11
    // 12 13 14 15

    float a12m0 = a.m[6]*a.m[11] - a.m[7]*a.m[10];
    float a12m1 = a.m[5]*a.m[11] - a.m[7]*a.m[9];
    float a12m2 = a.m[5]*a.m[10] - a.m[6]*a.m[9];
    minors.m[12] = -(a.m[1]*a12m0 - a.m[2]*a12m1 + a.m[3]*a12m2);

    float a13m0 = a.m[6]*a.m[11] - a.m[7]*a.m[10];
    float a13m1 = a.m[4]*a.m[11] - a.m[7]*a.m[8];
    float a13m2 = a.m[4]*a.m[10] - a.m[6]*a.m[8];
    minors.m[13] = a.m[0]*a13m0 - a.m[2]*a13m1 + a.m[3]*a13m2;

    float a14m0 = a.m[5]*a.m[11] - a.m[7]*a.m[9];
    float a14m1 = a.m[4]*a.m[11] - a.m[7]*a.m[8];
    float a14m2 = a.m[4]*a.m[9] - a.m[5]*a.m[8];
    minors.m[14] = -(a.m[0]*a14m0 - a.m[1]*a14m1 + a.m[3]*a14m2);

    float a15m0 = a.m[5]*a.m[10] - a.m[6]*a.m[9];
    float a15m1 = a.m[4]*a.m[10] - a.m[6]*a.m[8];
    float a15m2 = a.m[4]*a.m[9] - a.m[5]*a.m[8];
    minors.m[15] = a.m[0]*a15m0 - a.m[1]*a15m1 + a.m[2]*a15m2;

    float determinant = a.m[0]*minors.m[0] + a.m[1]*minors.m[1] +
                        a.m[2]*minors.m[2] + a.m[3]*minors.m[3];
    float ood = 1.0f / determinant;

    matrix4x4 result = ood * transpose(minors);

    return result;
}

inline matrix4x4 identityMatrix4x4 () {
    matrix4x4 result = {};

    result.m[0] = 1.0f;
    result.m[5] = 1.0f;
    result.m[10] = 1.0f;
    result.m[15] = 1.0f;

    return result;
}

inline matrix4x4 translationMatrix (float x, float y, float z) {
    matrix4x4 result = identityMatrix4x4();

    result.m[3] = x;
    result.m[7] = y;
    result.m[11] = z;

    return result;
}

inline matrix4x4 scaleMatrix4x4 (float s) {
    matrix4x4 result = {};

    result.m[0] = s;
    result.m[5] = s;
    result.m[10] = s;
    result.m[15] = 1.0f;

    return result;
}

inline matrix4x4 scaleMatrix4x4 (float x, float y, float z) {
    matrix4x4 result = {};

    result.m[0] = x;
    result.m[5] = y;
    result.m[10] = z;
    result.m[15] = 1.0f;

    return result;
}
//public static createViewMatrix (rotation: Quaternion, x: number, y: number, z: number): number[] {
//    let viewMatrix = MathUtil.quaternionMatrix(rotation);
//    viewMatrix = MathUtil.matrixTranspose4x4(viewMatrix);
//
//    let newCamX = -MathUtil.dotProduct([x, y, z], [viewMatrix[0], viewMatrix[4], viewMatrix[8]]);
//    let newCamY = -MathUtil.dotProduct([x, y, z], [viewMatrix[1], viewMatrix[5], viewMatrix[9]]);
//    let newCamZ = -MathUtil.dotProduct([x, y, z], [viewMatrix[2], viewMatrix[6], viewMatrix[10]]);
//    viewMatrix[12] = newCamX;
//    viewMatrix[13] = newCamY;
//    viewMatrix[14] = newCamZ;
//
//    return viewMatrix;
//}



inline float square (float a) {
    return a * a;
}

//inline float max (float a, float b) {
//    return a > b ? a : b;
//}
//
//inline float min (float a, float b) {
//    return a < b ? a : b;
//}

// vector2
inline vector2 Vector2 (float x = 0.0f, float y = 0.0f) {
    vector2 result;

    result.x = x;
    result.y = y;

    return result;
};

inline vector2 operator+ (vector2 a, vector2 b) {
    vector2 result;

    result.x = a.x + b.x;
    result.y = a.y + b.y;

    return result;
}

inline vector2 & operator+= (vector2 &a, vector2 b) {
    a = a + b;
    return a;
}

inline vector2 operator- (vector2 a, vector2 b) {
    vector2 result;

    result.x = a.x - b.x;
    result.y = a.y - b.y;

    return result;
}

inline vector2 operator- (vector2 a) {
    vector2 result;

    result.x = -a.x;
    result.y = -a.y;

    return result;
}


inline vector2 & operator-= (vector2 &a, vector2 b) {
    a = a - b;
    return a;
}

inline vector2 operator* (float a, vector2 b) {
    vector2 result;

    result.x = a * b.x;
    result.y = a * b.y;

    return result;
}

inline vector2 operator* (vector2 a, float b) {
    vector2 result = b * a;
    return result;
}

inline vector2 & operator*= (vector2 &a, float b) {
    a = b * a;
    return a;
}

// think this is called the hadamard product
inline vector2 operator* (vector2 a, vector2 b) {
    vector2 result;

    result.x = a.x * b.x;
    result.y = a.y * b.y;

    return result;
}

inline vector2 & operator*= (vector2 &a, vector2 b) {
    a = a * b;
    return a;
}

inline float dotProduct (vector2 a, vector2 b) {
    return a.x * b.x + a.y * b.y;
}

inline vector2 operator* (matrix3x3 m, vector2 v){
    vector2 result;

    result.x = m.m[0]*v.x + m.m[1]*v.y + m.m[2]*1.0f;
    result.y = m.m[3]*v.x + m.m[4]*v.y + m.m[5]*1.0f;
    // w component would be here
    // TODO(ebuchholz): check if this is a good idea

    return result;
}

inline vector2 transformPoint (matrix3x3 m, vector2 v, float* z){
    vector2 result;

    result.x = m.m[0]*v.x + m.m[1]*v.y + m.m[2]*(*z);
    result.y = m.m[3]*v.x + m.m[4]*v.y + m.m[5]*(*z);

    // z as an out parameter so we don't have to make a vector3
    *z = m.m[6]*v.x + m.m[7]*v.y + m.m[8]*1.0f;

    return result;
}

// Multiplies the vector like a direction
// TODO(ebuchholz): make a separate point data type?
inline vector2 transformDirection (matrix3x3 m, vector2 v) {
    vector2 result;

    result.x = m.m[0]*v.x + m.m[1]*v.y + m.m[2]*0.0f;
    result.y = m.m[3]*v.x + m.m[4]*v.y + m.m[5]*0.0f;
    // w component would be here
    // TODO(ebuchholz): check if this is a good idea

    return result;
}

// vector3
inline vector3 Vector3 (float x = 0.0f, float y = 0.0f, float z = 0.0f) {
    vector3 result;

    result.x = x;
    result.y = y;
    result.z = z;

    return result;
};

inline vector3 operator+ (vector3 a, vector3 b) {
    vector3 result;

    result.x = a.x + b.x;
    result.y = a.y + b.y;
    result.z = a.z + b.z;

    return result;
}

inline vector3 & operator+= (vector3 &a, vector3 b) {
    a = a + b;
    return a;
}

inline vector3 operator- (vector3 a, vector3 b) {
    vector3 result;

    result.x = a.x - b.x;
    result.y = a.y - b.y;
    result.z = a.z - b.z;

    return result;
}

inline vector3 operator- (vector3 a) {
    vector3 result;

    result.x = -a.x;
    result.y = -a.y;
    result.z = -a.z;

    return result;
}


inline vector3 & operator-= (vector3 &a, vector3 b) {
    a = a - b;
    return a;
}

inline vector3 operator* (float a, vector3 b) {
    vector3 result;

    result.x = a * b.x;
    result.y = a * b.y;
    result.z = a * b.z;

    return result;
}

inline vector3 operator* (vector3 a, float b) {
    vector3 result = b * a;
    return result;
}

inline vector3 & operator*= (vector3 &a, float b) {
    a = b * a;
    return a;
}

// think this is called the hadamard product
inline vector3 operator* (vector3 a, vector3 b) {
    vector3 result;

    result.x = a.x * b.x;
    result.y = a.y * b.y;
    result.z = a.z * b.z;

    return result;
}

inline vector3 & operator*= (vector3 &a, vector3 b) {
    a = a * b;
    return a;
}

inline float dotProduct (vector3 a, vector3 b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

inline vector3 crossProduct (vector3 a, vector3 b) {
    vector3 result;

    result.x = -b.y * a.z + a.y * b.z;
    result.y = -b.z * a.x + a.z * b.x;
    result.z = -b.x * a.y + a.x * b.y;

    return result; // TODO(ebuchholz): double check
}

inline vector3 operator* (matrix3x3 m, vector3 v){
    vector3 result;

    result.x = m.m[0]*v.x + m.m[1]*v.y + m.m[2]*v.z;
    result.y = m.m[3]*v.x + m.m[4]*v.y + m.m[5]*v.z;
    result.z = m.m[6]*v.x + m.m[7]*v.y + m.m[8]*v.z;

    return result;
}

// Multiplies the vector like a point
inline vector3 operator* (matrix4x4 m, vector3 v){
    vector3 result;

    result.x = m.m[0]*v.x + m.m[1]*v.y +  m.m[2]*v.z +  m.m[3]*1.0f;
    result.y = m.m[4]*v.x + m.m[5]*v.y +  m.m[6]*v.z +  m.m[7]*1.0f;
    result.z = m.m[8]*v.x + m.m[9]*v.y + m.m[10]*v.z + m.m[11]*1.0f;
    // w component would be here
    // TODO(ebuchholz): check if this is a good idea

    return result;
}

inline vector3 transformPoint (matrix4x4 m, vector3 v, float* w){
    vector3 result;

    result.x = m.m[0]*v.x + m.m[1]*v.y +  m.m[2]*v.z +  m.m[3]*(*w);
    result.y = m.m[4]*v.x + m.m[5]*v.y +  m.m[6]*v.z +  m.m[7]*(*w);
    result.z = m.m[8]*v.x + m.m[9]*v.y + m.m[10]*v.z + m.m[11]*(*w);

    // w as an out parameter so we don't have to make a vector4
    *w = m.m[12]*v.x + m.m[13]*v.y + m.m[14]*v.z + m.m[15]*1.0f;

    return result;
}

// Multiplies the vector like a direction
// TODO(ebuchholz): make a separate point data type?
inline vector3 transformDirection (matrix4x4 m, vector3 v) {
    vector3 result;

    result.x = m.m[0]*v.x + m.m[1]*v.y +  m.m[2]*v.z +  m.m[3]*0.0f;
    result.y = m.m[4]*v.x + m.m[5]*v.y +  m.m[6]*v.z +  m.m[7]*0.0f;
    result.z = m.m[8]*v.x + m.m[9]*v.y + m.m[10]*v.z + m.m[11]*0.0f;
    // w component would be here
    // TODO(ebuchholz): check if this is a good idea

    return result;
}

inline matrix4x4 translationMatrix (vector3 position) {
    return translationMatrix(position.x, position.y, position.z);
}

inline float lengthSquared (vector3 a) {
    return dotProduct(a, a);
}

inline float length (vector3 a) {
    return sqrtf(lengthSquared(a));
}

inline vector3 normalize (vector3 a) {
    return a * (1.0f / length(a));
}

inline quaternion Quaternion (float w = 1.0f, float x = 0.0f, float y = 0.0f, float z = 0.0f) {
    quaternion result;

    result.w = w;
    result.x = x;
    result.y = y;
    result.z = z;

    return result;
}

inline quaternion conjugate (quaternion a) {
    quaternion result;

    result.w = a.w;
    result.x = -a.x;
    result.y = -a.y;
    result.z = -a.z;

    return result;
}

inline quaternion normalize (quaternion a) {
    quaternion result;

    float magnitude = sqrtf(a.w * a.w + a.x * a.x + a.y * a.y + a.z * a.z);
    float oneOverMag = 1.0f / magnitude;
    result.w = a.w * oneOverMag;
    result.x = a.x * oneOverMag;
    result.y = a.y * oneOverMag;
    result.z = a.z * oneOverMag;

    return result;
}

inline quaternion operator+ (quaternion a, quaternion b) {
    quaternion result;

    result.x = a.x + b.x;
    result.y = a.y + b.y;
    result.z = a.z + b.z;
    result.w = a.w + b.w;

    return result;
}

// TODO(ebuchholz): triple check this one
inline quaternion operator* (quaternion a, quaternion b) {
    quaternion result;

    result.w = a.w*b.w - a.x*b.x - a.y*b.y - a.z*b.z;
    result.x = a.w*b.x + a.x*b.w + a.y*b.z - a.z*b.y;
    result.y = a.w*b.y - a.x*b.z + a.y*b.w + a.z*b.x;
    result.z = a.w*b.z + a.x*b.y - a.y*b.x + a.z*b.w;

    return result;
}

inline quaternion operator* (float a, quaternion b) {
    quaternion result;

    result.x = a * b.x;
    result.y = a * b.y;
    result.z = a * b.z;
    result.w = a * b.w;

    return result;
}

inline quaternion operator* (quaternion a, float b) {
    quaternion result = b * a;
    return result;
}

inline quaternion operator- (quaternion a) {
    quaternion result;

    result.x = -a.x;
    result.y = -a.y;
    result.z = -a.z;
    result.w = -a.w;

    return result;
}

inline quaternion & operator*= (quaternion &a, quaternion b) {
    a = a * b;
    return a;
}

// TODO(ebuchholz): quadruple check this one
inline vector3 rotateVectorByQuaternion (vector3 v, quaternion q) {
    quaternion qConjugate = conjugate(q);

    quaternion quaternionVersionOfVector;
    quaternionVersionOfVector.w = 0.0f;
    quaternionVersionOfVector.x = v.x;
    quaternionVersionOfVector.y = v.y;
    quaternionVersionOfVector.z = v.z;

    quaternion newQ = q * quaternionVersionOfVector * qConjugate;

    return Vector3(newQ.x, newQ.y, newQ.z);
}

inline quaternion quaternionFromAxisAngle (vector3 axis, float angle) {
    quaternion result;
    float angleOverTwo = angle / 2.0f;
    float sinAngleOverTwo = sinf(angleOverTwo);

    result.w = cosf(angleOverTwo);
    result.x = axis.x * sinAngleOverTwo;
    result.y = axis.y * sinAngleOverTwo;
    result.z = axis.z * sinAngleOverTwo;

    return result;
}

inline float dotProduct (quaternion a, quaternion b) {
    return a.w*b.w + a.x*b.x + a.y*b.y + a.z*b.z;
}

//TODO(ebuchholz): quintuple-check this one, may need to transpose
inline matrix4x4 matrix4x4FromQuaternion (quaternion q) {
    matrix4x4 result;

    result.m[0] = 1.0f - (2.0f*square(q.y)) - (2.0f*square(q.z));
    result.m[1] = (2.0f*q.x*q.y) - (2.0f*q.w*q.z);
    result.m[2] = (2.0f*q.x*q.z) + (2.0f*q.w*q.y);
    result.m[3] = 0.0f;

    result.m[4] = (2.0f*q.x*q.y) + (2.0f*q.w*q.z);
    result.m[5] = 1.0f - (2.0f*square(q.x)) - (2.0f*square(q.z));
    result.m[6] = (2.0f*q.y*q.z) - (2.0f*q.w*q.x);
    result.m[7] = 0.0f;

    result.m[8] = (2.0f*q.x*q.z) - (2.0f*q.w*q.y);
    result.m[9] = (2.0f*q.y*q.z) + (2.0f*q.w*q.x);
    result.m[10] = 1.0f - (2.0f*square(q.x)) - (2.0f*square(q.y));
    result.m[11] = 0.0f;

    result.m[12] = 0.0f;
    result.m[13] = 0.0f;
    result.m[14] = 0.0f;
    result.m[15] = 1.0f;

    return result;
}

inline matrix4x4 rotationMatrixFromAxisAngle (vector3 axis, float angle) {
    matrix4x4 result;

    quaternion axisAngleQuaternion = quaternionFromAxisAngle(axis, angle);
    result = matrix4x4FromQuaternion(axisAngleQuaternion);

    return result;
}

// TODO(ebuchholz): check this one a few times
inline quaternion lerp (quaternion a, quaternion b, float t) {
    quaternion result;
    float oneMinusT = 1.0f - t;

    result.w = oneMinusT*a.w + t*b.w;
    result.x = oneMinusT*a.x + t*b.x;
    result.y = oneMinusT*a.y + t*b.y;
    result.z = oneMinusT*a.z + t*b.z;

    result = normalize(result);
    return result;
}

// from wikipedia: quadruple check
//inline quaternion slerp (quaternion a, quaternion b, float t) {
//    // assume quaternions are normalized
//
//    float dot = dotProduct(a, b);
//    if (dot < 0.0f) {
//        a = -a;
//        dot = -dot;
//    }
//
//    if (dot > 0.9995f) {
//        return lerp(a, b, t);
//    }
//
//    float theta0 = acosf(dot);
//    float theta = theta0 * t;
//    float sinTheta = sinf(theta);
//    float sinTheta0 = sinf(theta0);
//
//    float s0 = cosf(theta) - dot * sinTheta / sinTheta0;
//    float s1 = sinTheta / sinTheta0;
//
//    return (s0 * a) + (s1 * b);
//}

inline quaternion quaternionFromRotationMatrix (matrix4x4 m) {
    quaternion result;
    float t;
    if (m.m[10] > 0) {
        if (m.m[0] > m.m[5]) {
            t = 1.0f + m.m[0] - m.m[5] - m.m[10];
            result = Quaternion(m.m[4] + m.m[1], m.m[2] + m.m[8], m.m[9] - m.m[6], t);
        }
        else {
            t = 1.0f - m.m[0] + m.m[5] - m.m[10];
            result = Quaternion(t, m.m[9] + m.m[6], m.m[2] - m.m[8], m.m[4] + m.m[1]);
        }
    }
    else {
        if (m.m[0] < - m.m[5]) { 
            t = 1.0f - m.m[0] - m.m[5] + m.m[10];
            result = Quaternion(m.m[9] + m.m[6], t, m.m[4] - m.m[1], m.m[2] + m.m[8]);
        }
        else {
            t = 1.0f + m.m[0] + m.m[5] + m.m[10];
            result = Quaternion(m.m[2] - m.m[8], m.m[4] - m.m[1], t, m.m[9] - m.m[6]);
        }
    }
    float k = 0.5f / sqrtf(t);
    result = Quaternion(result.w * k, result.x * k, result.y * k, result.z * k);
    return result;
}


// TODO(ebuchholz): Test this, I haven't used this yet
inline matrix4x4 createLookAtMatrix (float camX, float camY, float camZ, 
                                     float lookX, float lookY, float lookZ,
                                     float upX, float upY, float upZ) 
{
    matrix4x4 result = identityMatrix4x4();

    vector3 zVector = normalize(Vector3(camX, camY, camZ) - Vector3(lookX, lookY, lookZ));
    vector3 up = Vector3(upX, upY, upZ);
    vector3 xVector = normalize(crossProduct(up, zVector));
    vector3 yVector = normalize(crossProduct(zVector, xVector));

    result.m[0] = xVector.x;
    result.m[1] = xVector.y;
    result.m[2] = xVector.z;
    
    result.m[4] = yVector.x;
    result.m[5] = yVector.y;
    result.m[6] = yVector.z;

    result.m[8] = zVector.x;
    result.m[9] = zVector.y;
    result.m[10] = zVector.z;

    //result = transpose(result);

    float newCamX = dotProduct(Vector3(camX, camY, camZ), Vector3(result.m[0], result.m[1], result.m[2]));
    float newCamY = dotProduct(Vector3(camX, camY, camZ), Vector3(result.m[4], result.m[5], result.m[6]));
    float newCamZ = dotProduct(Vector3(camX, camY, camZ), Vector3(result.m[8], result.m[9], result.m[10]));
    result.m[3] = -newCamX;
    result.m[7] = -newCamY;
    result.m[11] = -newCamZ;

    return result;
}

// TODO(ebuchholz): make sure this is always right side up
inline quaternion createLookAtQuaternion (float camX, float camY, float camZ, 
                                          float lookX, float lookY, float lookZ)
{
    // find axis/angle and create quaternion from it
    //vector3 zVector = normalize(Vector3(lookX, lookY, lookZ) - Vector3(camX, camY, camZ));
    //vector3 forward = Vector3(0.0f, 0.0f, 1.0f);

    //vector3 axis = normalize(crossProduct(forward, zVector));
    //float angle = acosf(dotProduct(zVector, forward));

    //return quaternionFromAxisAngle(axis, angle);

    matrix4x4 lookAtMatrix = createLookAtMatrix(camX, camY, camZ, lookX, lookY, lookZ, 0.0f, 1.0f, 0.0f);
    return quaternionFromRotationMatrix(transpose(lookAtMatrix));
}

inline matrix4x4 createViewMatrix (quaternion rotation, float x, float y, float z) {
    matrix4x4 result = matrix4x4FromQuaternion(rotation);
    result = transpose(result);

    float newCamX = dotProduct(Vector3(x, y, z), Vector3(result.m[0], result.m[1], result.m[2]));
    float newCamY = dotProduct(Vector3(x, y, z), Vector3(result.m[4], result.m[5], result.m[6]));
    float newCamZ = dotProduct(Vector3(x, y, z), Vector3(result.m[8], result.m[9], result.m[10]));
    result.m[3] = -newCamX;
    result.m[7] = -newCamY;
    result.m[11] = -newCamZ;

    return result;
}

inline matrix4x4 createPerspectiveMatrix (float nearPlane, float farPlane, float aspectRatio, float fov) {
    float fovy = 2.0f * atanf(tanf(fov * (PI / 180.0f) / 2.0f) * (1.0f / aspectRatio));
    float f = 1.0f / tanf(fovy / 2.0f);
    float nf = 1.0f / (farPlane - nearPlane);

    matrix4x4 result = identityMatrix4x4();

    result.m[0] = f / aspectRatio;
    result.m[5] = f;
    result.m[10] = -(farPlane + nearPlane) * nf;
    result.m[11] = -2 * (farPlane * nearPlane) * nf;
    result.m[14] = -1.0f;
    result.m[15] = 0;

    return result;
}


inline matrix4x4 createOrthgraphicMatrix (float nearPlane, float farPlane, float l, float r, float t, float b) {
    matrix4x4 result = identityMatrix4x4();

    result.m[0] = 2.0f / (r - l);
    result.m[3] = -(r + l)/(r - l);
    result.m[5] = 2.0f / (t - b);
    result.m[7] = -(t + b)/(t - b);
    result.m[10] = -2.0f / (farPlane - nearPlane);
    result.m[11] = -(farPlane + nearPlane)/(farPlane - nearPlane);

    return result;
}

inline matrix4x4 createOrthgraphicMatrix (float nearPlane, float farPlane, float width, float height) {
    float halfWidth = width / 2.0f;
    float l = -halfWidth;
    float r = halfWidth;
    float halfHeight = height / 2.0f;
    float b = -halfHeight;
    float t = halfHeight;

    return createOrthgraphicMatrix(nearPlane, farPlane, l, r, t, b);
}

aabb getTriangleBounds (vector3 p0, vector3 p1, vector3 p2) {
    aabb result;

    result.min.x = p0.x < p1.x ? p0.x : p1.x;
    result.min.y = p0.y < p1.y ? p0.y : p1.y;
    result.min.z = p0.z < p1.z ? p0.z : p1.z;

    result.max.x = p0.x > p1.x ? p0.x : p1.x;
    result.max.y = p0.y > p1.y ? p0.y : p1.y;
    result.max.z = p0.z > p1.z ? p0.z : p1.z;

    result.min.x = result.min.x < p2.x ? result.min.x : p2.x;
    result.min.y = result.min.y < p2.y ? result.min.y : p2.y;
    result.min.z = result.min.z < p2.z ? result.min.z : p2.z;

    result.max.x = result.max.x > p2.x ? result.max.x : p2.x;
    result.max.y = result.max.y > p2.y ? result.max.y : p2.y;
    result.max.z = result.max.z > p2.z ? result.max.z : p2.z;

    return result;
}

aabb unionAABB (aabb a, aabb b) {
    aabb result;

    result.min.x = a.min.x < b.min.x ? a.min.x : b.min.x;
    result.min.y = a.min.y < b.min.y ? a.min.y : b.min.y;
    result.min.z = a.min.z < b.min.z ? a.min.z : b.min.z;

    result.max.x = a.max.x > b.max.x ? a.max.x : b.max.x;
    result.max.y = a.max.y > b.max.y ? a.max.y : b.max.y;
    result.max.z = a.max.z > b.max.z ? a.max.z : b.max.z;

    return result;
}

bool aabbIntersection (aabb a, aabb b) {
    if (a.max.x < b.min.x) { return false; }
    if (a.min.x > b.max.x) { return false; }
    if (a.max.y < b.min.y) { return false; }
    if (a.min.y > b.max.y) { return false; }
    if (a.max.z < b.min.z) { return false; }
    if (a.min.z > b.max.z) { return false; }
    return true;
}

float aabbSurfaceArea (aabb a) {
    float width = a.max.x - a.min.x;
    float depth = a.max.z - a.min.z;
    float height = a.max.y - a.min.y;

    return 2.0f * (width * depth + width * height + depth * height);
}

int aabbAxisWithMaximumExtent (aabb a) {
    int max = 0;
    if (a.max.y - a.min.y > a.max.x - a.min.x) {
        max = 1;
        if (a.max.z - a.min.z > a.max.y - a.min.y) {
            max = 2;
        }
    }
    else {
        if (a.max.z - a.min.z > a.max.x - a.min.x) {
            max = 2;
        }
    }

    return max;
}

rectangle unionRectangle (rectangle a, rectangle b) {
    rectangle result;

    result.min.x = a.min.x < b.min.x ? a.min.x : b.min.x;
    result.min.y = a.min.y < b.min.y ? a.min.y : b.min.y;

    result.max.x = a.max.x > b.max.x ? a.max.x : b.max.x;
    result.max.y = a.max.y > b.max.y ? a.max.y : b.max.y;

    return result;
}

bool rectangleIntersection (rectangle a, rectangle b) {
    if (a.max.x < b.min.x) { return false; }
    if (a.min.x > b.max.x) { return false; }
    if (a.max.y < b.min.y) { return false; }
    if (a.min.y > b.max.y) { return false; }
    return true;
}

bool rectangleContainsPoint (rectangle rect, float x, float y) {
    if (x >= rect.min.x && x < rect.max.x && y >= rect.min.y && y < rect.max.y) {
        return true;
    }
    return false;
}

inline sphere Sphere (vector3 pos, float radius) {
    sphere result;

    result.pos = pos;
    result.radius = radius;

    return result;
}

inline plane Plane (vector3 normal, float distance) {
    plane result = {};

    result.normal = normal;
    result.distance = distance;

    return result;
}

inline bool pointIsInFrontOfPlane (vector3 point, plane p) {
    vector3 pointOnPlane = p.normal * p.distance;
    vector3 planeToPoint = point - pointOnPlane;
    return dotProduct(p.normal, planeToPoint) > 0;
}

inline frustum_corners FrustumCorners (frustum f) {
    frustum_corners result = {};

    float fovy = 2.0f * atanf(tanf(f.fov * (PI / 180.0f) / 2.0f) * (1.0f / f.ratio));

    float nearHalfHeight = f.nearPlane * tanf(fovy / 2.0f);
    float nearHalfWidth = nearHalfHeight * f.ratio;
    float farHalfHeight = f.farPlane * tanf(fovy / 2.0f);
    float farHalfWidth = farHalfHeight * f.ratio;

    vector3 camDir = rotateVectorByQuaternion(Vector3(0.0f, 0.0f, -1.0f), f.orientation);
    vector3 up = rotateVectorByQuaternion(Vector3(0.0f, 1.0f, 0.0f), f.orientation);
    vector3 camRight = crossProduct(camDir, up);

    result.nearUpperLeft = f.origin + camDir * f.nearPlane + nearHalfHeight * up - nearHalfWidth * camRight;
    result.nearUpperRight = f.origin + camDir * f.nearPlane + nearHalfHeight * up + nearHalfWidth * camRight;
    result.nearLowerLeft = f.origin + camDir * f.nearPlane - nearHalfHeight * up - nearHalfWidth * camRight;
    result.nearLowerRight = f.origin + camDir * f.nearPlane - nearHalfHeight * up + nearHalfWidth * camRight;
    result.farUpperLeft = f.origin + camDir * f.farPlane + farHalfHeight * up - farHalfWidth * camRight;
    result.farUpperRight = f.origin + camDir * f.farPlane + farHalfHeight * up + farHalfWidth * camRight;
    result.farLowerLeft = f.origin + camDir * f.farPlane - farHalfHeight * up - farHalfWidth * camRight;
    result.farLowerRight = f.origin + camDir * f.farPlane - farHalfHeight * up + farHalfWidth * camRight;

    return result;
}

inline frustum_corners FrustumCorners (vector3 origin, quaternion orientation, 
                                       float nearPlane, float farPlane, float fov, float ratio) 
{
    frustum f;
    f.origin = origin;
    f.orientation = orientation;
    f.nearPlane = nearPlane;
    f.farPlane = farPlane;
    f.fov = fov;
    f.ratio = ratio;
    return FrustumCorners(f);
}
