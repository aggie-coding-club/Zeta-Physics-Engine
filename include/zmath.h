#ifndef ZMATH_H
#define ZMATH_H

namespace ZMath {
    // * ============================================
    // * ZMath Constants
    // * ============================================

    // * Pi constant
    #define PI 3.1415926535897932L

    // * Default tolerance value for a floating point comparison
    #define EPSILON 0.0000000001

    // todo go through all of ZMath and check where overloaded functions can be replaced by default parameters


    class Vec3D {
        public:
            // * ============================================
            // * Vector Components
            // * ============================================

            // * x, y, and z components. These are public for ease of access and assignment.
            float x, y, z;

            // * ============================================
            // * Dynamic Object Functions
            // * ============================================

            // * Instantiate a Vec3D object with all components set to 0.
            Vec3D() : x(0), y(0), z(0) {};

            // * Instantiate a Vec3D object with all components set to the same value.
            Vec3D(float d) : x(d), y(d), z(d) {};

            // * Instantiate a Vec3D object with each component assigned.
            Vec3D(float i, float j, float k) : x(i), y(j), z(k) {};

            // * Instantiate a copy of the Vec3D object passed in.
            Vec3D(const Vec3D &vec) : x(vec.x), y(vec.y), z(vec.z) {};

            Vec3D operator + (Vec3D const &vec) { return Vec3D(x + vec.x, y + vec.y, z + vec.z); };
            Vec3D operator - (Vec3D const &vec) { return Vec3D(x - vec.x, y - vec.y, z - vec.z); };
            Vec3D operator * (float c) { return Vec3D(c*x, c*y, c*z); };
            float operator * (Vec3D const &vec) { return x * vec.x + y * vec.y + z * vec.z; };

            // * Add a constant to each vector component.
            Vec3D operator + (float c) { return Vec3D(x + c, y + c, z + c); };

            bool operator != (Vec3D const &vec) { return x != vec.x || y != vec.y || z != vec.z; };
            bool operator == (Vec3D const &vec) { return x == vec.x && y == vec.y && z == vec.z; };

            Vec3D operator += (Vec3D const &vec) { return Vec3D(x + vec.x, y + vec.y, z + vec.z); };
            Vec3D operator -= (Vec3D const &vec) { return Vec3D(x - vec.x, y - vec.y, z - vec.z); };
            Vec3D operator *= (float c) { return Vec3D(x*c, y*c, z*c); };
            float operator *= (Vec3D const &vec) { return x * vec.x + y * vec.y + z * vec.z; };

            // * Get the cross product of this and another vector.
            Vec3D cross (Vec3D const &vec) { return Vec3D(y*vec.z - z*vec.y, -(x*vec.z - z*vec.x), x*vec.y - y*vec.x); };

            // * Get the magnitude.
            float mag();

            // * Get the magnitude squared.
            // * This should be used over mag() when possible as it is less expensive.
            float magSq();

            // * Get the vector projection of another vector onto this vector (Parameter onto this).
            Vec3D proj (Vec3D const &vec) { return (*this) * ((x*vec.x + y*vec.y + z*vec.z)/(x*x + y*y + z*z)); };

            // * Get the distance between this and another vector.
            float dist (Vec3D const &vec);

            // * Get the distance squared between this and another vector.
            // * This should be used over dist() when possible as it is less expensive.
            float distSq (Vec3D const &vec) { return (x - vec.x) * (x - vec.x) + (y - vec.y) * (y - vec.y) + (z - vec.z) * (z - vec.z); };

            // * Get the normal vector. This is used to determine the direction a vector is pointing in.
            Vec3D normalize();

            // * Get the angle between the vectors in radians.
            // * Keep in mind range restrictions for arccos.
            float angle (Vec3D const &vec);

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

            // * ============================================
            // * Const Object Functions
            // * ============================================

            Vec3D operator + (Vec3D const &vec) const { return Vec3D(x + vec.x, y + vec.y, z + vec.z); };
            Vec3D operator - (Vec3D const &vec) const { return Vec3D(x - vec.x, y - vec.y, z - vec.z); };
            Vec3D operator * (float c) const { return Vec3D(c*x, c*y, c*z); };

            // * Add a constant to each vector component.
            Vec3D operator + (float c) const { return Vec3D(x + c, y + c, z + c); };

            Vec3D operator += (Vec3D const &vec) const { return Vec3D(x + vec.x, y + vec.y, z + vec.z); };
            Vec3D operator -= (Vec3D const &vec) const { return Vec3D(x - vec.x, y - vec.y, z - vec.z); };
            Vec3D operator *= (float c) const { return Vec3D(x*c, y*c, z*c); };

            // * Get the cross product of this and another vector.
            Vec3D cross (Vec3D const &vec) const { return Vec3D(y*vec.z - z*vec.y, -(x*vec.z - z*vec.x), x*vec.y - y*vec.x); };

            // * Get the vector projection of another vector onto this vector (Parameter onto this).
            Vec3D proj (Vec3D const &vec) const { return (*this) * ((x*vec.x + y*vec.y + z*vec.z)/(x*x + y*y + z*z)); };

            // * Get the normal vector. This is used to determine the direction a vector is pointing in.
            Vec3D normalize() const;
    };


    // * ============================================
    // * Utility Functions
    // * ============================================

    // * Get the max value of two floats.
    float max(float a, float b);

    // * Get the min value of two floats.
    float min(float a, float b);

    // * Convert an angle in degrees to radians.
    float toRadians(float degrees);

    // * @brief Rotate a point in 3D space with respect to the XY-plane about an origin.
    // * 
    // * @param point The point in 3D space to rotate.
    // * @param origin The origin the point will be rotated about.
    // * @param angle The angle, in degrees, to rotate the point by.
    void rotateXY(Vec3D &point, const Vec3D &origin, float angle);

    // * @brief Rotate a point in 3D space with respect to the XZ-plane about an origin.
    // * 
    // * @param point The point in 3D space to rotate.
    // * @param origin The origin the point will be rotated about.
    // * @param angle The angle, in degrees, to rotate the point by.
    void rotateXZ(Vec3D &point, const Vec3D &origin, float angle);

    // * Handle tolerance for floating point numbers.
    bool compare(float a, float b, float epsilon);

    // * Handle tolerance for floating point numbers using the default tolerance of 10^-10.
    bool compare(float a, float b);

    // * Clamp a float between a min and max.
    float clamp(float n, float min, float max);

    // * Fast algorithm for computing 1/sqrt(n). This is not super accurate but is sufficient for a physics engine.
    // * Primarily used for computing directional vectors.
    float invSqrt(float n);
}

#endif // ! ZMATH_H
