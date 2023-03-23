// ? Define functions for our ZMath library.

#include "zmath.h"
#include <cmath>

// * ============================================
// * Vec 2D Functions
// * ============================================

// todo change the method by which I am obtaining the angle as it is ridiculously expensive

float ZMath::Vec2D::mag() { return sqrt(x*x + y*y); };
float ZMath::Vec2D::magSq() { return x*x + y*y; };
float ZMath::Vec2D::dist(Vec2D const &vec) { return sqrt((x - vec.x) * (x - vec.x) + (y - vec.y) * (y - vec.y)); };
float ZMath::Vec2D::angle(Vec2D const &vec) { return acos((x*vec.x + y*vec.y)/(sqrt(x*x + y*y) * sqrt(vec.x*vec.x + vec.y*vec.y))); };

ZMath::Vec2D ZMath::Vec2D::normalize() { return (*this) * (1.0f/sqrt(x*x + y*y)); };
const ZMath::Vec2D ZMath::Vec2D::normalize() const { return (*this) * (1.0f/sqrt(x*x + y*y)); };

float ZMath::Vec2D::mag() const { return sqrt(x*x + y*y); };
float ZMath::Vec2D::magSq() const { return x*x + y*y; };

float ZMath::Vec2D::dist(Vec2D const &vec) const { return sqrt((x - vec.x) * (x - vec.x) + (y - vec.y) * (y - vec.y)); };

// * ============================================
// * Vec 3D Functions
// * ============================================

// todo check my work for Vec3D::angle() -- also change because this method currently is ridicolously expensive

float ZMath::Vec3D::mag() { return sqrt(x*x + y*y + z*z); };
float ZMath::Vec3D::magSq() { return x*x + y*y + z*z; };
float ZMath::Vec3D::dist(Vec3D const &vec) { return sqrt((x - vec.x) * (x - vec.x) + (y - vec.y) * (y - vec.y) + (z - vec.z) * (z - vec.z)); };
float ZMath::Vec3D::angle(Vec3D const &vec) { return acos((x*vec.x + y*vec.y + z*vec.z)/(sqrt(x*x + y*y + z*z) * sqrt(vec.x*vec.x + vec.y*vec.y + vec.z*vec.z))); };

ZMath::Vec3D ZMath::Vec3D::normalize() { return (*this) * (1.0f/sqrt(x*x + y*y + z*z)); };
const ZMath::Vec3D ZMath::Vec3D::normalize() const { return (*this) * (1.0f/sqrt(x*x + y*y + z*z)); };

float ZMath::Vec3D::mag() const { return sqrt(x*x + y*y + z*z); };
float ZMath::Vec3D::magSq() const { return x*x + y*y + z*z; };

float ZMath::Vec3D::dist(Vec3D const &vec) const { return sqrt((x - vec.x) * (x - vec.x) + (y - vec.y) * (y - vec.y) + (z - vec.z) * (z - vec.z)); };

// * ============================================
// * ZMath Functions
// * ============================================

float ZMath::max(float a, float b) { return a > b ? a : b; };
float ZMath::min(float a, float b) { return a < b ? a : b; };
float ZMath::toRadians(float degrees) { return (degrees/180) * PI; };

void ZMath::rotateXY (Vec3D &point, const Vec3D &origin, float angle) {
    float x = point.x - origin.x, y = point.y - origin.y;

    float cosine = cos(toRadians(angle));
    float sine = sin(toRadians(angle));

    // compute the new point -- z component is unchanged
    point.x = x*cosine - y*sine + origin.x;
    point.y = x*sine + y*cosine + origin.y;
};

void ZMath::rotateXZ (Vec3D &point, const Vec3D &origin, float angle) {
    float x = point.x - origin.x, z = point.z - origin.z;

    float cosine = cos(toRadians(angle));
    float sine = sin(toRadians(angle));

    // compute the new point -- y component is unchanged
    point.x = x*cosine - z*sine + origin.x;
    point.z = x*sine + z*cosine + origin.z;
};

bool ZMath::compare (float a, float b, float epsilon) { return abs(a - b) <= epsilon; };
bool ZMath::compare (float a, float b) { return abs(a - b) <= EPSILON; };

float ZMath::clamp(float n, float min, float max) { return ZMath::max(ZMath::min(n, max), min); };
float ZMath::invSqrt(float n) {};
