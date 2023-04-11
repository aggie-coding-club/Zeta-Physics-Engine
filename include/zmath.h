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
            Vec3D operator - (float c) const { return Vec3D(x - c, y - c, z - c); };

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

    // * Get the absolute value of all components in a 2D vector.
    Vec2D abs(Vec2D &vec) {
        vec.x = std::fabs(vec.x);
        vec.y = std::fabs(vec.y);
    };

    // * Get the absolute value of all components in a 3D vector.
    Vec3D abs(Vec3D &vec) {
        vec.x = std::fabs(vec.x);
        vec.y = std::fabs(vec.y);
        vec.z = std::fabs(vec.z);
    };

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


    // todo remove the unnecessary function calls in the matrix class wrappers

    // * Class modeling a 2x2 Matrix stored in column major order.
    class Mat2D {
        public:
            // Matrix columns.
            Vec2D c1, c2;

            // Does nothing (for performance).
            Mat2D() = default;

            // Create a 2D matrix from another 2D matrix.
            Mat2D (const Mat2D &mat) {
                c1.set(mat.c1);
                c2.set(mat.c2);
            };

            // Create a 2D matrix from 2 column vectors.
            Mat2D (const Vec2D &col1, const Vec2D &col2) {
                c1.set(col1);
                c2.set(col2);
            };

            // Create a 2D matrix from 4 scalars.
            Mat2D (float a11, float a12, float a21, float a22) {
                c1.set(a11, a21);
                c2.set(a12, a22);
            };

            // Set this matrix's components equal to that of another.
            void set (const Mat2D &mat) {
                c1.set(mat.c1);
                c2.set(mat.c2);
            };

            // Set this matrix's columns equal to those passed in.
            void set (const Vec2D &col1, const Vec2D &col2) {
                c1.set(col1);
                c2.set(col2);
            };

            // Set this matrix's elements equal to those passed in.
            void set(float a11, float a12, float a21, float a22) {
                c1.set(a11, a21);
                c2.set(a12, a22);
            };

            // Set all elements equal to 0.
            void zero() {
                c1.zero();
                c2.zero();
            };

            Mat2D operator + (const Mat2D &mat) const { return Mat2D(c1 + mat.c1, c2 + mat.c2); };
            Mat2D operator - (const Mat2D &mat) const { return Mat2D(c1 - mat.c1, c2 - mat.c2); };
            Mat2D operator * (const Mat2D &mat) const {
                return Mat2D(
                    c1.x*mat.c1.x + c1.x*mat.c1.y, c1.x*mat.c2.x + c2.x*mat.c2.y,
                    c1.y*mat.c1.x + c2.y*mat.c1.y, c1.y*mat.c2.x + c2.y*mat.c2.y
                );
            };

            Mat2D operator * (float c) const { return Mat2D(c1*c, c2*c); };
            Vec2D operator * (const Vec2D &vec) const { return Vec2D(c1.x*vec.x + c2.x*vec.y, c1.y*vec.x + c2.y*vec.y); };
            Mat2D operator + (float c) const { return Mat2D(c1 + c, c2 + c); };
            Mat2D operator - (float c) const { return Mat2D(c1 - c, c2 - c); };

            Mat2D& operator = (const Mat2D &mat) {
                c1.set(mat.c1);
                c2.set(mat.c2);
            };

            Mat2D& operator += (const Mat2D &mat) {
                c1.x += mat.c1.x;
                c1.y += mat.c1.y;
                c2.x += mat.c2.x;
                c2.y += mat.c2.y;

                return (*this);
            };

            Mat2D& operator += (float c) {
                c1.x += c;
                c1.y += c;
                c2.x += c;
                c2.y += c;

                return (*this);
            };

            Mat2D& operator -= (const Mat2D &mat) {
                c1.x -= mat.c1.x;
                c1.y -= mat.c1.y;
                c2.x -= mat.c2.x;
                c2.y -= mat.c2.y;

                return (*this);
            };

            Mat2D& operator -= (float c) {
                c1.x -= c;
                c1.y -= c;
                c2.x -= c;
                c2.y -= c;

                return (*this);
            };

            Mat2D& operator *= (const Mat2D &mat) {
                c1.x = c1.x * mat.c1.x + c2.x * mat.c1.y;
                c1.y = c1.x * mat.c2.x + c2.x * mat.c2.y;
                c2.x = c1.y * mat.c1.x + c2.y * mat.c1.y;
                c2.y = c1.y * mat.c2.x + c2.y * mat.c2.y;

                return (*this);
            };

            Mat2D& operator *= (float c) {
                c1.x *= c;
                c1.y *= c;
                c2.x *= c;
                c2.y *= c;

                return (*this);
            };

            // Return the inverse of this matrix.
            // Note this may experience issues for matrices with determinants nearly equal to 0.
            Mat2D inverse() const {
                float det = c1.x * c2.y - c2.x * c1.y;
                Mat2D mat(*this);

                if (compare(det, 0)) { return mat; }; // singular matrix -- doesn't have an inverse.

                det = 1.0f/det;

                mat.set(c2.y * det, c2.x * -det, c1.x * -det, c1.x * det);
                return mat;
            };

            // Return the transpose of this matrix.
            Mat2D transpose() const { return Mat2D(c1.x, c1.y, c2.x, c2.y); };


            // * ===============================
            // * Utility Matrix Functions
            // * ===============================

            // * Get the 2x2 identity matrix.
            Mat2D identity() { return Mat2D(1, 0, 0, 1); };

            // * Generate the 2D rotation matrix given a specified angle.
            // * The angle should be in degrees.
            Mat2D rotationMat(float theta) {
                float s = sin(toRadians(theta));
                float c = cos(toRadians(theta));

                return Mat2D(c, -s, s, c);
            };
    };


    // * Class modeling a 3x3 Matrix stored in column major order.
    class Mat3D {
        public:
            Vec3D c1, c2, c3;

            // Does nothing (for performance).
            Mat3D() = default;

            // Create a 3D matrix from another 3D matrix.
            Mat3D (const Mat3D &mat) {
                c1.set(mat.c1);
                c2.set(mat.c2);
                c3.set(mat.c3);
            };

            // Create a 3D matrix from 2 column vectors.
            Mat3D (const Vec3D &col1, const Vec3D &col2, const Vec3D &col3) {
                c1.set(col1);
                c2.set(col2);
                c3.set(col3);
            };

            // Create a 3D matrix from 9 scalars.
            Mat3D (float a11, float a12, float a13, float a21, float a22, float a23, float a31, float a32, float a33) {
                c1.set(a11, a21, a31);
                c2.set(a12, a22, a32);
                c3.set(a13, a23, a33);
            };

            // Set this matrix's components equal to that of another.
            void set (const Mat3D &mat) {
                c1.set(mat.c1);
                c2.set(mat.c2);
                c3.set(mat.c3);
            };

            // Set this matrix's columns equal to those passed in.
            void set (const Vec3D &col1, const Vec3D &col2, const Vec3D &col3) {
                c1.set(col1);
                c2.set(col2);
                c3.set(col3);
            };

            // Set this matrix's elements equal to those passed in.
            void set(float a11, float a12, float a13, float a21, float a22, float a23, float a31, float a32, float a33) {
                c1.set(a11, a21, a31);
                c2.set(a12, a22, a32);
                c3.set(a13, a23, a33);
            };

            // Set all elements equal to 0.
            void zero() {
                c1.zero();
                c2.zero();
                c3.zero();
            };

            Mat3D operator + (const Mat3D &mat) const { return Mat3D(c1 + mat.c1, c2 + mat.c2, c3 + mat.c3); };
            Mat3D operator - (const Mat3D &mat) const { return Mat3D(c1 - mat.c1, c2 - mat.c2, c3 - mat.c3); };

            Mat3D operator * (const Mat3D &mat) const {
                return Mat3D(
                    // row 1
                    c1.x*mat.c1.x + c2.x*mat.c1.y + c3.x*mat.c1.z,
                    c1.x*mat.c2.x + c2.x*mat.c2.y + c3.x*mat.c2.z,
                    c1.x*mat.c3.x + c2.x*mat.c3.y + c3.x*mat.c3.z,

                    // row 2
                    c1.y*mat.c1.x + c2.y*mat.c1.y + c3.y*mat.c1.z,
                    c1.y*mat.c2.x + c2.y*mat.c2.y + c3.y*mat.c2.z,
                    c1.y*mat.c3.x + c2.y*mat.c3.y + c3.y*mat.c3.z,

                    // row 3
                    c1.z*mat.c1.x + c2.z*mat.c1.y + c3.z*mat.c1.z,
                    c1.z*mat.c2.x + c2.z*mat.c2.y + c3.z*mat.c2.z,
                    c1.z*mat.c3.x + c2.z*mat.c3.y + c3.z*mat.c3.z
                );
            };

            Mat3D operator * (float c) const { return Mat3D(c1*c, c2*c, c3*c); };

            Vec3D operator * (const Vec3D &vec) const {
                return Vec3D(
                    c1.x*vec.x + c2.x*vec.y + c3.x*vec.z,
                    c1.y*vec.x + c2.y*vec.y + c3.y*vec.z,
                    c1.z*vec.x + c2.z*vec.y + c3.z*vec.z
                );
            };

            Mat3D operator + (float c) const { return Mat3D(c1 + c, c2 + c, c3 + c); };
            Mat3D operator - (float c) const { return Mat3D(c1 - c, c2 - c, c3 - c); };

            Mat3D& operator = (const Mat3D &mat) {
                c1.set(mat.c1);
                c2.set(mat.c2);
                c3.set(mat.c3);
            };

            Mat3D& operator += (const Mat3D &mat) {
                c1.x += mat.c1.x;
                c1.y += mat.c1.y;
                c1.z += mat.c1.z;
                c2.x += mat.c2.x;
                c2.y += mat.c2.y;
                c2.z += mat.c2.z;
                c3.x += mat.c3.x;
                c3.y += mat.c3.y;
                c3.z += mat.c3.z;

                return (*this);
            };

            Mat3D& operator += (float c) {
                c1.x += c;
                c1.y += c;
                c1.z += c;
                c2.x += c;
                c2.y += c;
                c2.z += c;
                c3.x += c;
                c3.y += c;
                c3.z += c;

                return (*this);
            };

            Mat3D& operator -= (const Mat3D &mat) {
                c1.x -= mat.c1.x;
                c1.y -= mat.c1.y;
                c1.z -= mat.c1.z;
                c2.x -= mat.c2.x;
                c2.y -= mat.c2.y;
                c2.z -= mat.c2.z;
                c3.x -= mat.c3.x;
                c3.y -= mat.c3.y;
                c3.z -= mat.c3.z;

                return (*this);
            };

            Mat3D& operator -= (float c) {
                c1.x -= c;
                c1.y -= c;
                c1.z -= c;
                c2.x -= c;
                c2.y -= c;
                c2.z -= c;
                c3.x -= c;
                c3.y -= c;
                c3.z -= c;

                return (*this);
            };

            Mat3D& operator *= (const Mat3D &mat) {
                // row 1
                c1.x = c1.x*mat.c1.x + c2.x*mat.c1.y + c3.x*mat.c1.z;
                c2.x = c1.x*mat.c2.x + c2.x*mat.c2.y + c3.x*mat.c2.z;
                c3.x = c1.x*mat.c3.x + c2.x*mat.c3.y + c3.x*mat.c3.z;

                // row 2
                c1.y = c1.y*mat.c1.x + c2.y*mat.c1.y + c3.y*mat.c1.z;
                c2.y = c1.y*mat.c2.x + c2.y*mat.c2.y + c3.y*mat.c2.z;
                c3.y = c1.y*mat.c3.x + c2.y*mat.c3.y + c3.y*mat.c3.z;

                // row 3
                c1.z = c1.z*mat.c1.x + c2.z*mat.c1.y + c3.z*mat.c1.z;
                c2.z = c1.z*mat.c2.x + c2.z*mat.c2.y + c3.z*mat.c2.z;
                c3.z = c1.z*mat.c3.x + c2.z*mat.c3.y + c3.z*mat.c3.z;

                return (*this);
            };

            Mat3D& operator *= (float c) {
                c1.x *= c;
                c1.y *= c;
                c1.z *= c;
                c2.x *= c;
                c2.y *= c;
                c2.z *= c;
                c3.x *= c;
                c3.y *= c;
                c3.z *= c;

                return (*this);
            };

            // Return the transpose of this Matrix.
            Mat3D transpose() const { return Mat3D(c1.x, c1.y, c1.z, c2.x, c2.y, c2.z, c3.x, c3.y, c3.z); };

            // Return the inverse of this Matrix.
            // Note this may experience issues for matrices with determinants nearly equal to 0.
            Mat3D inverse() const {
                // for optimization
                float qz = c2.y*c3.z;
                float rx = c3.y*c1.z;
                float py = c1.y*c2.z;
                float ry = c3.y*c2.z;
                float pz = c1.y*c3.z;
                float qx = c2.y*c1.z;

                // |a b c|
                // |p q r|
                // |x y z|
                // determinant = aqz + brx + cpy - ary - bpz - cqx
                float det = c1.x*qz + c2.x*rx + c3.x*py - (c1.x*ry + c2.x*pz + c3.x*qx);
                Mat3D mat(*this);

                if (compare(det, 0)) { return mat; } // singular matrix -- no inverse

                det = 1.0f/det;
                mat.set(
                    (qz - ry) * det, (-(pz - rx)) * det, (py - qx) * det,
                    (-(c2.x*c3.z - c3.x*c1.z)) * det, (c1.x*c3.z - c3.x*c1.z) * det, (-(c1.x*c2.z - c2.x*c1.z)) * det,
                    (c2.x*c3.y - c3.x*c2.y) * det, (-(c1.x*c3.y - c3.x*c1.y)) * det, (c1.x*c2.y - c2.x*c1.y) * det
                );

                return mat.transpose();
            };

            // * ===============================
            // * Utility Matrix Functions
            // * ===============================

            // * Get the 2x2 identity matrix.
            Mat3D identity() { return Mat3D(1, 0, 0, 0, 1, 0, 0, 0, 1); };

            /*// * Generate the 2D rotation matrix given a specified angle.
            // * The angle should be in degrees.
            Mat2D rotationMat(float theta) {
                float s = sin(toRadians(theta));
                float c = cos(toRadians(theta));

                return Mat2D(c, -s, s, c);
            };*/
    };
}

#endif // ! ZMATH_H
