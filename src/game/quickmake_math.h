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

 matrix3x3 operator* (matrix3x3 a, matrix3x3 b) ;

 matrix3x3 operator* (float a, matrix3x3 b) ;

 matrix3x3 transpose (matrix3x3 a) ;
// TODO(ebuchholz): maybe a 3x3 inverse transpose for tranforming normals

// TODO(ebuchholz): implement
 matrix3x3 inverse (matrix3x3 a) ;

 matrix3x3 identityMatrix3x3 () ;

 matrix3x3 translationMatrix (float x, float y) ;

 matrix3x3 scaleMatrix3x3 (float s) ;

 matrix3x3 scaleMatrix3x3 (float x, float y) ;

 matrix3x3 rotationMatrix3x3 (float angle) ;

 matrix4x4 operator* (matrix4x4 a, matrix4x4 b) ;

 matrix4x4 operator* (float a, matrix4x4 b) ;

 matrix4x4 transpose (matrix4x4 a) ;

// TODO(ebuchholz): fix this travesty
 matrix4x4 inverse (matrix4x4 a) ;

 matrix4x4 identityMatrix4x4 () ;

 matrix4x4 translationMatrix (float x, float y, float z) ;

 matrix4x4 scaleMatrix4x4 (float s) ;

 matrix4x4 scaleMatrix4x4 (float x, float y, float z) ;
//public static createViewMatrix (rotation: Quaternion, x: number, y: number, z: number): number[] ;



 float square (float a) ;

// float max (float a, float b) ;
//
// float min (float a, float b) ;

// vector2
 vector2 Vector2 (float x , float y ) ;;

 vector2 operator+ (vector2 a, vector2 b) ;

 vector2 & operator+= (vector2 &a, vector2 b) ;

 vector2 operator- (vector2 a, vector2 b) ;

 vector2 operator- (vector2 a) ;


 vector2 & operator-= (vector2 &a, vector2 b) ;

 vector2 operator* (float a, vector2 b) ;

 vector2 operator* (vector2 a, float b) ;

 vector2 & operator*= (vector2 &a, float b) ;

// think this is called the hadamard product
 vector2 operator* (vector2 a, vector2 b) ;

 vector2 & operator*= (vector2 &a, vector2 b) ;

 float dotProduct (vector2 a, vector2 b) ;

 vector2 operator* (matrix3x3 m, vector2 v);

 vector2 transformPoint (matrix3x3 m, vector2 v, float* z);

// Multiplies the vector like a direction
// TODO(ebuchholz): make a separate point data type?
 vector2 transformDirection (matrix3x3 m, vector2 v) ;

// vector3
 vector3 Vector3 (float x , float y , float z ) ;;

 vector3 operator+ (vector3 a, vector3 b) ;

 vector3 & operator+= (vector3 &a, vector3 b) ;

 vector3 operator- (vector3 a, vector3 b) ;

 vector3 operator- (vector3 a) ;


 vector3 & operator-= (vector3 &a, vector3 b) ;

 vector3 operator* (float a, vector3 b) ;

 vector3 operator* (vector3 a, float b) ;

 vector3 & operator*= (vector3 &a, float b) ;

// think this is called the hadamard product
 vector3 operator* (vector3 a, vector3 b) ;

 vector3 & operator*= (vector3 &a, vector3 b) ;

 float dotProduct (vector3 a, vector3 b) ;

 vector3 crossProduct (vector3 a, vector3 b) ;

 vector3 operator* (matrix3x3 m, vector3 v);

// Multiplies the vector like a point
 vector3 operator* (matrix4x4 m, vector3 v);

 vector3 transformPoint (matrix4x4 m, vector3 v, float* w);

// Multiplies the vector like a direction
// TODO(ebuchholz): make a separate point data type?
 vector3 transformDirection (matrix4x4 m, vector3 v) ;

 matrix4x4 translationMatrix (vector3 position) ;

 float lengthSquared (vector3 a) ;

 float length (vector3 a) ;

 vector3 normalize (vector3 a) ;

 quaternion Quaternion (float w , float x , float y , float z ) ;

 quaternion conjugate (quaternion a) ;

 quaternion normalize (quaternion a) ;

 quaternion operator+ (quaternion a, quaternion b) ;

// TODO(ebuchholz): triple check this one
 quaternion operator* (quaternion a, quaternion b) ;

 quaternion operator* (float a, quaternion b) ;

 quaternion operator* (quaternion a, float b) ;

 quaternion operator- (quaternion a) ;

 quaternion & operator*= (quaternion &a, quaternion b) ;

// TODO(ebuchholz): quadruple check this one
 vector3 rotateVectorByQuaternion (vector3 v, quaternion q) ;

 quaternion quaternionFromAxisAngle (vector3 axis, float angle) ;

 float dotProduct (quaternion a, quaternion b) ;

//TODO(ebuchholz): quintuple-check this one, may need to transpose
 matrix4x4 matrix4x4FromQuaternion (quaternion q) ;

 matrix4x4 rotationMatrixFromAxisAngle (vector3 axis, float angle) ;

// TODO(ebuchholz): check this one a few times
 quaternion lerp (quaternion a, quaternion b, float t) ;

// from wikipedia: quadruple check
// quaternion slerp (quaternion a, quaternion b, float t) ;

 quaternion quaternionFromRotationMatrix (matrix4x4 m) ;


// TODO(ebuchholz): Test this, I haven't used this yet
 matrix4x4 createLookAtMatrix (float camX, float camY, float camZ, 
                                     float lookX, float lookY, float lookZ,
                                     float upX, float upY, float upZ) 
;

// TODO(ebuchholz): make sure this is always right side up
 quaternion createLookAtQuaternion (float camX, float camY, float camZ, 
                                          float lookX, float lookY, float lookZ)
;

 matrix4x4 createViewMatrix (quaternion rotation, float x, float y, float z) ;

 matrix4x4 createPerspectiveMatrix (float nearPlane, float farPlane, float aspectRatio, float fov) ;


 matrix4x4 createOrthgraphicMatrix (float nearPlane, float farPlane, float l, float r, float t, float b) ;

 matrix4x4 createOrthgraphicMatrix (float nearPlane, float farPlane, float width, float height) ;

aabb getTriangleBounds (vector3 p0, vector3 p1, vector3 p2) ;

aabb unionAABB (aabb a, aabb b) ;

bool aabbIntersection (aabb a, aabb b) ;

float aabbSurfaceArea (aabb a) ;

int aabbAxisWithMaximumExtent (aabb a) ;

rectangle unionRectangle (rectangle a, rectangle b) ;

bool rectangleIntersection (rectangle a, rectangle b) ;

bool rectangleContainsPoint (rectangle rect, float x, float y) ;

 sphere Sphere (vector3 pos, float radius) ;

 plane Plane (vector3 normal, float distance) ;

 bool pointIsInFrontOfPlane (vector3 point, plane p) ;

 frustum_corners FrustumCorners (frustum f) ;

 frustum_corners FrustumCorners (vector3 origin, quaternion orientation, 
                                       float nearPlane, float farPlane, float fov, float ratio) 
;

#endif
