#ifndef PRIMITIVES_H
#define PRIMITIVES_H

#include "bodies.h"

// todo add masses for each of the primitives for their rigidbodies

namespace Primitives {
    class Ray3D {
        public:
            ZMath::Vec3D origin;
            ZMath::Vec3D dir; // normalized direction of the ray

            // @brief Construct a new Ray3D object
            // 
            // @param position The origin of the ray.
            // @param direction The direction of the ray as a normalized vector.
            Ray3D(ZMath::Vec3D origin, ZMath::Vec3D direction) : origin(origin), dir(direction) {};
    };

    class Line3D {
        public:
            ZMath::Vec3D start, end;

            // @brief Create a line between two points.
            //
            // @param p1 (Vec3D) Starting point.
            // @param p2 (Vec3D) Ending point.
            Line3D(ZMath::Vec3D const &p1, ZMath::Vec3D const &p2) : start(p1), end(p2) {};

            // A vector with the lowest value of x, y, and z the line segment reaches.
            ZMath::Vec3D getMin() const { return ZMath::Vec3D(ZMath::min(start.x, end.x), ZMath::min(start.y, end.y), ZMath::min(start.z, end.z)); };

            // A vector with greatest value of x, y, and z the line segment reaches.
            ZMath::Vec3D getMax() const { return ZMath::Vec3D(ZMath::max(start.x, end.x), ZMath::max(start.y, end.y), ZMath::max(start.z, end.z)); };
    };

    // Models a rectangular, finite plane in 3D space.
    // This should be used to model death planes, borders, etc. as planes are not affected by forces and impulse.
    class Plane {
        private:
            ZMath::Vec2D halfSize;

        public:
            // static body representing the plane -- stores angles and the centerpoint.
            // We use a static body for a plane as it should not be affected by forces and impulse.
            StaticBody3D sb;
            ZMath::Vec3D normal; // Normal vector to the plane in global coordinates.

            /**
             * @brief Create an unrotated plane.
             * 
             * @param min (Vec2D) The min vertex of the plane.
             * @param max (Vec2D) The max vertex of the plane.
             * @param z (float) The z-level the plane lies on.
             */
            Plane(ZMath::Vec2D const &min, ZMath::Vec2D const &max, float z) 
                    : halfSize((max - min) * 0.5f), sb({ZMath::Vec3D(min.x + halfSize.x, min.y + halfSize.y, z), 0.0f, 0.0f}) {
                
                ZMath::Vec3D v1 = ZMath::Vec3D(sb.pos.x - halfSize.x, sb.pos.y - halfSize.y, sb.pos.z);
                ZMath::Vec3D v2 = ZMath::Vec3D(sb.pos.x + halfSize.x, sb.pos.y - halfSize.y, sb.pos.z);

                normal = (v2 - sb.pos).cross(v1 - sb.pos);
            };

            /**
             * @brief Create a rotated plane.
             * 
             * @param min (Vec2D) The min vertex of the plane as if it was unrotated.
             * @param max (Vec2D) The max vertex of the plane as if it was unrotated.
             * @param z (float) The z-level the plane lies on.
             * @param angXY (float) The angle, in degrees, the plane is rotated with respect to the XY plane.
             * @param angXZ (float) The angle, in degrees, the plane is rotated with respect to the XZ plane.
             */
            Plane(ZMath::Vec2D const &min, ZMath::Vec2D const &max, float z, float angXY, float angXZ) 
                    : halfSize((max - min) * 0.5f), sb({ZMath::Vec3D(min.x + halfSize.x, min.y + halfSize.y, z), angXY, angXZ}) {

                ZMath::Vec3D v1 = ZMath::Vec3D(sb.pos.x - halfSize.x, sb.pos.y - halfSize.y, sb.pos.z);
                ZMath::Vec3D v2 = ZMath::Vec3D(sb.pos.x + halfSize.x, sb.pos.y - halfSize.y, sb.pos.z);

                ZMath::rotateXY(v1, sb.pos, sb.theta);
                ZMath::rotateXZ(v1, sb.pos, sb.phi);
                ZMath::rotateXY(v2, sb.pos, sb.theta);
                ZMath::rotateXZ(v2, sb.pos, sb.phi);

                normal = (v2 - sb.pos).cross(v1 - sb.pos);
            };

            ZMath::Vec2D getLocalMin() const { return ZMath::Vec2D(sb.pos.x - halfSize.x, sb.pos.y - halfSize.y); };
            ZMath::Vec2D getLocalMax() const { return ZMath::Vec2D(sb.pos.x + halfSize.x, sb.pos.y + halfSize.y); };
            ZMath::Vec2D getHalfSize() const { return halfSize; };

            // Get the vertices of the plane in terms of global coordinates.
            // Remember to use delete[] on the object you assign this to afterwards to free the memory.
            ZMath::Vec3D* getVertices() const {
                ZMath::Vec3D* v = new ZMath::Vec3D[4]; // 4 as it is rectangular

                v[0] = ZMath::Vec3D(sb.pos.x - halfSize.x, sb.pos.y - halfSize.y, sb.pos.z);
                v[1] = ZMath::Vec3D(sb.pos.x - halfSize.x, sb.pos.y + halfSize.y, sb.pos.z);
                v[2] = ZMath::Vec3D(sb.pos.x + halfSize.x, sb.pos.y + halfSize.y, sb.pos.z);
                v[3] = ZMath::Vec3D(sb.pos.x + halfSize.x, sb.pos.y - halfSize.y, sb.pos.z);

                // rotate each vertex
                for (int i = 0; i < 4; i++) {
                    ZMath::rotateXY(v[i], sb.pos, sb.theta);
                    ZMath::rotateXZ(v[i], sb.pos, sb.phi);
                }

                return v;
            };
    };

    class Sphere {
        public:
            float r; // radius
            RigidBody3D rb; // rigidbody representing the sphere -- stores its centerpoint

            // @brief Create a Sphere with an arbitrary radius and center.
            //
            // @param rho (float) Radius of the sphere.
            // @param center (Vec3D) Center of the sphere.
            Sphere(float rho, ZMath::Vec3D const &center) : r(rho) {
                rb.pos = center;
                rb.theta = 0.0f;
                rb.phi = 0.0f;
            };
    };

    class AABB {
        private:
            ZMath::Vec3D halfSize;

        public:
            RigidBody3D rb; // rigid body representing the AABB -- stores the center point

            /** 
             * @brief Instantiate a 3D unrotated Cube.
             * 
             * @param min (Vec3D) Min vertex of the AABB.
             * @param max (Vec3D) Max vertex of the AABB.
             */
            AABB(ZMath::Vec3D const &min, ZMath::Vec3D const &max) : halfSize((max - min) * 0.5f) {
                rb.pos = min + halfSize;
                rb.theta = 0.0f;
                rb.phi = 0.0f;
            };

            ZMath::Vec3D getMin() const { return rb.pos - halfSize; };
            ZMath::Vec3D getMax() const { return rb.pos + halfSize; };
            ZMath::Vec3D getHalfSize() const { return halfSize; };

            ZMath::Vec3D* getVertices() const{
                ZMath::Vec3D* vertices = new ZMath::Vec3D[4];

                vertices[0] = {rb.pos.x - halfSize.x, rb.pos.y - halfSize.y, rb.pos.z + halfSize.z}; // bottom left
                vertices[1] = {rb.pos.x - halfSize.x, rb.pos.y + halfSize.y, rb.pos.z + halfSize.z}; // top left
                vertices[2] = {rb.pos.x + halfSize.x, rb.pos.y + halfSize.y, rb.pos.z + halfSize.z}; // top right
                vertices[3] = {rb.pos.x + halfSize.x, rb.pos.y - halfSize.y, rb.pos.z + halfSize.z};  // bottom right
                
                vertices[4] = {rb.pos.x - halfSize.x, rb.pos.y - halfSize.y, rb.pos.z - halfSize.z}; // bottom left
                vertices[5] = {rb.pos.x - halfSize.x, rb.pos.y + halfSize.y, rb.pos.z - halfSize.z}; // top left
                vertices[6] = {rb.pos.x + halfSize.x, rb.pos.y + halfSize.y, rb.pos.z - halfSize.z}; // top right
                vertices[7] = {rb.pos.x + halfSize.x, rb.pos.y - halfSize.y, rb.pos.z - halfSize.z}; // bottom right

                return vertices;
            };
    };

    class Cube {
        private:
            ZMath::Vec3D halfSize;

        public:
            // todo cache a rotation matrix for the cube instead of storing its angles.

            RigidBody3D rb; // rigid body representing the cube -- stores the angles rotated and the center point

            // @brief Create a cube rotated by an arbitrary angle with arbitrary min and max vertices.
            //
            // @param min Min vertex of the cube as if it was not rotated.
            // @param max Max vertex of the cube as if it was not rotated.
            // @param angXY Angle the cube is rotated by with respect to the XY plane in degrees.
            // @param angXZ Angle the cube is rotated by with respect to the XZ plane in degrees.
            Cube(ZMath::Vec3D const &min, ZMath::Vec3D const &max, float angXY, float angXZ) : halfSize((max - min) * 0.5f) {
                rb.pos = min + halfSize;
                rb.theta = angXY;
                rb.phi = angXZ;
            };

            // Get the min vertex in the cube's UVW coordinates.
            ZMath::Vec3D getLocalMin() const { return rb.pos - halfSize; };

            // Get the max vertex in the cube's UVW coordinates.
            ZMath::Vec3D getLocalMax() const { return rb.pos + halfSize; };

            // Get half the distance between the cube's min and max vertices.
            ZMath::Vec3D getHalfSize() const { return halfSize; };

            // Get the vertices of the cube in terms of global coordinates.
            // Remeber to use delete[] on the variable you assign this after use to free the memory.
            ZMath::Vec3D* getVertices() const {
                ZMath::Vec3D* v = new ZMath::Vec3D[8];

                // todo maybe reorder
                v[0] = rb.pos - halfSize;
                v[1] = ZMath::Vec3D(rb.pos.x - halfSize.x, rb.pos.y - halfSize.y, rb.pos.z +  halfSize.z);
                v[2] = ZMath::Vec3D(rb.pos.x + halfSize.x, rb.pos.y - halfSize.y, rb.pos.z + halfSize.z);
                v[3] = ZMath::Vec3D(rb.pos.x + halfSize.x, rb.pos.y - halfSize.y, rb.pos.z - halfSize.z);
                v[4] = ZMath::Vec3D(rb.pos.x - halfSize.x, rb.pos.y + halfSize.y, rb.pos.z + halfSize.z);
                v[5] = ZMath::Vec3D(rb.pos.x - halfSize.x, rb.pos.y + halfSize.y, rb.pos.z - halfSize.z);
                v[6] = ZMath::Vec3D(rb.pos.x + halfSize.x, rb.pos.y + halfSize.y, rb.pos.z - halfSize.z);
                v[7] = rb.pos + halfSize;

                for (int i = 0; i < 8; i++) {
                    ZMath::rotateXY(v[i], rb.pos, rb.theta);
                    ZMath::rotateXZ(v[i], rb.pos, rb.phi);
                }

                return v;
            }; 
    };

    // * ==================
    // * Colliders
    // * ==================

    // ! Current design for this is bad; refactor later.
    // ! This is just used to have some form of easy system to begin connecting everything together

    enum {
        SPHERE_COLLIDER,
        AABB_COLLIDER,
        CUBE_COLLIDER,
        CUSTOM_COLLIDER, // ? allows users to model their own colliders
        NONE // ? No collider. Used for initializing a value without actually assigning a collider.
    };

    struct Collider3D {
        // todo later overload the assignment operator and copy the union value by using memcpy.

        Collider3D() {}; // default constructor to make the compiler happy

        int type = NONE;
        union {
            Sphere sphere;
            AABB aabb;
            Cube cube;
        };
    };

} // namespace Primitives

#endif // !PRIMITIVES_H
