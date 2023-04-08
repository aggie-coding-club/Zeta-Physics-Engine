#ifndef ZMATH_H
#define ZMATH_H

#include <cmath>

namespace ZMath {
    // * ============================================
    // * ZMath Constants
    // * ============================================

    // * Pi constant
    #define PI 3.1415926535897932L

    // * Default tolerance value for a floating point comparison
    #define EPSILON 0.0005


    // * Class modeling a 2D Vector.
    class Vec2D {
        public:
            // * ===========================
            // * Vector Components
            // * ===========================

            // * x and y components.
            float x, y;

            // * ============================
            // * Constructors
            // * ============================

            // * Instantiate a Vec2D object with all components set to 0.
            Vec2D() : x(0), y(0) {};

            // * Instantiate a Vec2D object with all components set to the same value.
            Vec2D(float d) : x(d), y(d) {};

            // * Instantiate a Vec3D object with each component assigned.
            Vec2D(float i, float j) : x(i), y(j) {};

            // * Instantiate a copy of the Vec3D object passed in.
            Vec2D(const Vec2D &vec) : x(vec.x), y(vec.y) {};

            // * ============================
            // * Functions
            // * ============================

            // * Zero this vector.
            void zero() {
                x = 0;
                y = 0;
            };

            // * Set this vector's components equal to another.
            void set (Vec2D const &vec) {
                this->x = vec.x;
                this->y = vec.y;
            };

            // * Set all components of this vector to the same value.
            void set (float d) {
                x = d;
                y = d;
            };

            // * Set each component of this vector.
            // * Less expensive than creating a new Vec3D object.
            void set (float i, float j) {
                x = i;
                y = j;
            };

            const Vec2D operator + (Vec2D const &vec) const { return Vec2D(x + vec.x, y + vec.y); };
            const Vec2D operator - (Vec2D const &vec) const { return Vec2D(x - vec.x, y - vec.y); };
            const Vec2D operator * (float c) const { return Vec2D(c*x, c*y); };
            float operator * (Vec2D const &vec) const { return x * vec.x + y * vec.y; };

            // * Add a constant to each vector component.
            const Vec2D operator + (float c) const { return Vec2D(x + c, y + c); };

            bool operator != (Vec2D const &vec) const { return x != vec.x || y != vec.y; };
            bool operator == (Vec2D const &vec) const { return x == vec.x && y == vec.y; };

            const Vec2D operator += (Vec2D const &vec) const { return Vec2D(x + vec.x, y + vec.y); };
            const Vec2D operator -= (Vec2D const &vec) const { return Vec2D(x - vec.x, y - vec.y); };
            const Vec2D operator *= (float c) const { return Vec2D(x*c, y*c); };

            // * Get the cross product of this and another vector.
            const Vec2D cross (Vec2D const &vec) const { return x*vec.y - y*vec.x; };

            // * Get the magnitude.
            float mag() const { return sqrtf(x*x + y*y); };

            // * Get the magnitude squared.
            // * This should be used over mag() when possible as it is less expensive.
            float magSq() const { return x*x + y*y; };

            // * Get the vector projection of another vector onto this vector (Parameter onto this).
            const Vec2D proj (Vec2D const &vec) const { return (*this) * ((x*vec.x + y*vec.y)/(x*x + y*y)); };

            // * Get the distance between this and another vector.
            float dist (Vec2D const &vec) const { return sqrtf((x - vec.x) * (x - vec.x) + (y - vec.y) * (y - vec.y)); };

            // * Get the distance squared between this and another vector.
            // * This should be used over dist() when possible as it is less expensive.
            float distSq (Vec2D const &vec) const { return (x - vec.x) * (x - vec.x) + (y - vec.y) * (y - vec.y); };

            // * Get the normal vector. This is used to determine the direction a vector is pointing in.
            const Vec2D normalize() const { return (*this) * (1.0f/sqrtf(x*x + y*y)); };

            // * Get the angle between the vectors in radians.
            // * Keep in mind range restrictions for arccos.
            // * This function is very expensive. Only call if absolutely needed.
            float angle (Vec2D const &vec) const { return acos((x*vec.x + y*vec.y)/(sqrtf(x*x + y*y) * sqrtf(vec.x*vec.x + vec.y*vec.y))); };

            // * Get the value of cos^2(theta) between the vectors.
            float cos2Ang (Vec2D const &vec) const {
                float d = x*vec.x + y*vec.y;
                return (d*d)/((x*x + y*y)*(vec.x*vec.x + vec.y*vec.y));
            };
    };


    // * Class modeling a 3D Vector.
    class Vec3D {
        public:
            // * ============================
            // * Vector Components
            // * ============================

            // * x, y, and z components. These are public for ease of access and assignment.
            float x, y, z;

            // * ============================
            // * Constructors
            // * ============================

            // * Instantiate a Vec3D object with all components set to 0.
            Vec3D() : x(0), y(0), z(0) {};

            // * Instantiate a Vec3D object with all components set to the same value.
            Vec3D(float d) : x(d), y(d), z(d) {};

            // * Instantiate a Vec3D object with each component assigned.
            Vec3D(float i, float j, float k) : x(i), y(j), z(k) {};

            // * Instantiate a copy of the Vec3D object passed in.
            Vec3D(const Vec3D &vec) : x(vec.x), y(vec.y), z(vec.z) {};

            // * ============================
            // * Functions
            // * ============================

            // * Zero this vector.
            void zero() {
                x = 0;
                y = 0;
                z = 0;
            };

            // * Set this vector's components equal to another.
            void set (Vec3D const &vec) {
                this->x = vec.x;
                this->y = vec.y;
                this->z = vec.z;
            };

            // * Set all components of this vector to the same value.
            void set (float d) {
                x = d;
                y = d;
                z = d;
            };

            // * Set each component of this vector.
            // * Less expensive than creating a new Vec3D object.
            void set (float i, float j, float k) {
                x = i;
                y = j;
                z = k;
            };

            Vec3D operator + (Vec3D const &vec) const { return Vec3D(x + vec.x, y + vec.y, z + vec.z); };
            Vec3D operator - (Vec3D const &vec) const { return Vec3D(x - vec.x, y - vec.y, z - vec.z); };
            Vec3D operator * (float c) const { return Vec3D(c*x, c*y, c*z); };
            float operator * (Vec3D const &vec) const { return x * vec.x + y * vec.y + z * vec.z; };

            // * Add a constant to each vector component.
            Vec3D operator + (float c) const { return Vec3D(x + c, y + c, z + c); };

            bool operator != (Vec3D const &vec) const { return x != vec.x || y != vec.y || z != vec.z; };
            bool operator == (Vec3D const &vec) const { return x == vec.x && y == vec.y && z == vec.z; };

            Vec3D operator += (Vec3D const &vec) const { return Vec3D(x + vec.x, y + vec.y, z + vec.z); };
            Vec3D operator -= (Vec3D const &vec) const { return Vec3D(x - vec.x, y - vec.y, z - vec.z); };
            Vec3D operator *= (float c) const { return Vec3D(x*c, y*c, z*c); };

            // * Get the cross product of this and another vector.
            Vec3D cross (Vec3D const &vec) const { return Vec3D(y*vec.z - z*vec.y, -(x*vec.z - z*vec.x), x*vec.y - y*vec.x); };

            // * Get the magnitude.
            float mag() const { return sqrtf(x*x + y*y + z*z); };

            // * Get the magnitude squared.
            // * This should be used over mag() when possible as it is less expensive.
            float magSq() const { return x*x + y*y + z*z; };

            // * Get the vector projection of another vector onto this vector (Parameter onto this).
            Vec3D proj (Vec3D const &vec) const { return (*this) * ((x*vec.x + y*vec.y + z*vec.z)/(x*x + y*y + z*z)); };

            // * Get the distance between this and another vector.
            float dist (Vec3D const &vec) const { return sqrtf((x - vec.x) * (x - vec.x) + (y - vec.y) * (y - vec.y) + (z - vec.z) * (z - vec.z)); };

            // * Get the distance squared between this and another vector.
            // * This should be used over dist() when possible as it is less expensive.
            float distSq (Vec3D const &vec) const { return (x - vec.x) * (x - vec.x) + (y - vec.y) * (y - vec.y) + (z - vec.z) * (z - vec.z); };

            // * Get the normal vector. This is used to determine the direction a vector is pointing in.
            Vec3D normalize() const { return (*this) * (1.0f/sqrtf(x*x + y*y + z*z)); };

            // * Get the angle between the vectors in radians.
            // * Keep in mind range restrictions for arccos.
            // * This function is very expensive. Only call if absolutely needed.
            float angle (Vec3D const &vec) const { return acos((x*vec.x + y*vec.y + z*vec.z)/(sqrtf(x*x + y*y + z*z) * sqrtf(vec.x*vec.x + vec.y*vec.y + vec.z*vec.z))); };

            // * Get the value of cos^2(theta) between the vectors.
            float cos2Ang (Vec3D const &vec) const {
                float d = x*vec.x + y*vec.y + z*vec.z;
                return (d*d)/((x*x + y*y + z*z)*(vec.x*vec.x + vec.y*vec.y + vec.z*vec.z));
            };
    };


    // * ============================================
    // * Utility Functions
    // * ============================================

    // * Get the max value of two floats.
    float max(float a, float b) { return a > b ? a : b; };

    // * Get the min value of two floats.
    float min(float a, float b) { return a < b ? a : b; };

    // * Convert an angle in degrees to radians.
    float toRadians(float degrees) { return (degrees/180) * PI; };

    // * @brief Rotate a point in 3D space with respect to the XY-plane about an origin.
    // * 
    // * @param point The point in 3D space to rotate.
    // * @param origin The origin the point will be rotated about.
    // * @param angle The angle, in degrees, to rotate the point by.
    void rotateXY(Vec3D &point, const Vec3D &origin, float angle) {
        float x = point.x - origin.x, y = point.y - origin.y;

        float cosine = cos(toRadians(angle));
        float sine = sin(toRadians(angle));

        // compute the new point -- z component is unchanged
        point.x = x*cosine - y*sine + origin.x;
        point.y = x*sine + y*cosine + origin.y;
    };

    // * @brief Rotate a point in 3D space with respect to the XZ-plane about an origin.
    // * 
    // * @param point The point in 3D space to rotate.
    // * @param origin The origin the point will be rotated about.
    // * @param angle The angle, in degrees, to rotate the point by.
    void rotateXZ(Vec3D &point, const Vec3D &origin, float angle) {
        float x = point.x - origin.x, z = point.z - origin.z;

        float cosine = cos(toRadians(angle));
        float sine = sin(toRadians(angle));

        // compute the new point -- y component is unchanged
        point.x = x*cosine - z*sine + origin.x;
        point.z = x*sine + z*cosine + origin.z;
    };

    // * Handle tolerance for floating point numbers.
    // * If no epsilon is specified, the default of 5 * 10^-4 will be used.
    bool compare(float a, float b, float epsilon = EPSILON) { return std::fabs(a - b) <= epsilon; };

    // * Handler tolerance for 3D vectors of floating point numbers.
    // * If no epsilon is specified, the default of 5 * 10^-4 will be used.
    bool compare(Vec3D u, Vec3D v, float epsilon = EPSILON) { return std::fabs(u.x - v.x) <= epsilon && std::fabs(u.y - v.y) <= epsilon && std::fabs(u.z - v.z) <= epsilon; };

    // * Clamp a float between a min and max.
    float clamp(float n, float min, float max) { return ZMath::max(ZMath::min(n, max), min); };
}

#endif // ! ZMATH_H
