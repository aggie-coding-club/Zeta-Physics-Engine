#ifndef PRIMITIVES_H
#define PRIMITIVES_H

// todo create a collider3D class

#include "bodies.h"

namespace Primitives {
    class Ray3D {
        public:
            ZMath::Vec3D origin;
            ZMath::Vec3D dir; // normalized direction of the ray

            // @brief Construct a new Ray3D object
            // 
            // @param position The origin of the ray.
            // @param direction The direction of the ray as a normalized vector.
            Ray3D(ZMath::Vec3D position, ZMath::Vec3D direction) : origin(position), dir(direction) {};
    };

    class Line3D {
        public:
            ZMath::Vec3D start, end;

            // @brief Create a line between two points.
            //
            // @param p1 (Vec3D) Starting point.
            // @param p2 (Vec3D) Ending point.
            Line3D(ZMath::Vec3D const &p1, ZMath::Vec3D const &p2) : start(p1), end(p2) {};
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
            ZMath::Vec3D normal; // normal vector to the plane. Stored as it is commonly used in computations.

            /**
             * @brief Create an unrotated plane.
             * 
             * @param min (Vec2D) The min vertex of the plane.
             * @param max (Vec2D) The max vertex of the plane.
             * @param z (float) The z-level the plane lies on.
             */
            Plane(ZMath::Vec2D const &min, ZMath::Vec2D const &max, float z) 
                    : halfSize((max - min) * 0.5f), sb(ZMath::Vec3D(min.x + halfSize.x, min.y + halfSize.y, z), 0.0f, 0.0f) {
                
                ZMath::Vec3D v1 = ZMath::Vec3D(sb.pos.x - halfSize.x, sb.pos.y - halfSize.y, sb.pos.z);
                ZMath::Vec3D v2 = ZMath::Vec3D(sb.pos.x + halfSize.x, sb.pos.y + halfSize.y, sb.pos.z);

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
                    : halfSize((max - min) * 0.5f), sb(ZMath::Vec3D(min.x + halfSize.x, min.y + halfSize.y, z), angXY, angXZ) {

                ZMath::Vec3D v1 = ZMath::Vec3D(sb.pos.x - halfSize.x, sb.pos.y - halfSize.y, sb.pos.z);
                ZMath::Vec3D v2 = ZMath::Vec3D(sb.pos.x + halfSize.x, sb.pos.y - halfSize.y, sb.pos.z);

                ZMath::rotateXY(v1, sb.pos, sb.theta);
                ZMath::rotateXZ(v1, sb.pos, sb.phi);
                ZMath::rotateXY(v2, sb.pos, sb.theta);
                ZMath::rotateXZ(v2, sb.pos, sb.phi);

                normal = (v2 - sb.pos).cross(v1 - sb.pos);
            };

            ZMath::Vec3D getLocalMin();
            ZMath::Vec3D getLocalMax();
            ZMath::Vec2D getHalfsize();

            // Get the vertices of the plane in terms of global coordinates.
            // Remember to use delete[] on the object you assign this to afterwards to free the memory.
            ZMath::Vec3D* getVertices();

            ZMath::Vec3D getLocalMin() const;
            ZMath::Vec3D getLocalMax() const;
            ZMath::Vec2D getHalfSize() const;

            // Get the vertices of the plane in terms of global coordinates.
            // Remember to use delete[] on the object you assign this to afterwards to free the memory.
            ZMath::Vec3D* getVertices() const;
    };

    class Sphere {
        public:
            float r; // radius
            RigidBody3D rb; // rigidbody representing the sphere -- stores its centerpoint

            // @brief Create a Sphere centered at (0, 0, 0) with a radius of 1.
            Sphere() : r(1.0f), rb(ZMath::Vec3D(), 1.0f){};

            // @brief Create a Sphere with an arbitrary radius and center.
            //
            // @param rho (float) Radius of the sphere.
            // @param center (Vec3D) Center of the sphere.
            Sphere(float rho, ZMath::Vec3D const &center) : r(rho), rb(center, 1.0f){};
    };

    class AABB {
        private:
            ZMath::Vec3D halfSize;

        public:
            RigidBody3D rb; // rigid body representing the AABB -- stores the center point

            // @brief Instantiate a 3D AABB.
            // 
            // @param min (Vec3D) Min vertex of the AABB.
            // @param max (Vec3D) Max vertex of the AABB.
            AABB(ZMath::Vec3D const &min, ZMath::Vec3D const &max) 
                    : halfSize((max - min) * 0.5f), rb(min + halfSize, 1.0f) {};

            ZMath::Vec3D getMin();
            ZMath::Vec3D getMax();
            ZMath::Vec3D getHalfSize();

            ZMath::Vec3D* getVertices();

            ZMath::Vec3D getMin() const;
            ZMath::Vec3D getMax() const;
            ZMath::Vec3D getHalfSize() const;

            ZMath::Vec3D* getVertices() const;
    };

    class Cube {
        private:
            ZMath::Vec3D halfSize;

        public:
            RigidBody3D rb; // rigid body representing the cube -- stores the angles rotated and the center point

            // @brief Create a cube rotated by 45 degrees with respect to both the XY and XZ planes, 
            //         its center at (0, 0, 0), and its halfsize as 1.
            Cube() : rb(RigidBody3D(ZMath::Vec3D(0), 1.0f, 45.0f, 45.0f)), halfSize(ZMath::Vec3D(1)) {};

            // @brief Create a cube rotated by an arbitrary angle with arbitrary min and max vertices.
            //
            // @param p1 Min vertex of the cube as if it was not rotated.
            // @param p2 Max vertex of the cube as if it was not rotated.
            // @param angXY Angle the cube is rotated by with respect to the XY plane in degrees.
            // @param angXZ Angle the cube is rotated by with respect to the XZ plane in degrees.
            Cube(ZMath::Vec3D const &min, ZMath::Vec3D const &max, float angXY, float angXZ) 
                    : halfSize((max - min) * 0.5f), rb(RigidBody3D(min + halfSize, 1.0f, angXY, angXZ)) {};

            // Get the min vertex in the cube's UVW coordinates.
            ZMath::Vec3D getLocalMin();

            // Get the max vertex in the cube's UVW coordinates.
            ZMath::Vec3D getLocalMax();

            // Get half the distance between the cube's min and max vertices.
            ZMath::Vec3D getHalfSize();

            // Get the vertices of the cube in terms of global coordinates.
            // Remeber to use delete[] on the variable you assign this after use to free the memory.
            ZMath::Vec3D* getVertices();

            // Get the min vertex in the cube's UVW coordinates.
            ZMath::Vec3D getLocalMin() const;

            // Get the max vertex in the cube's UVW coordinates.
            ZMath::Vec3D getLocalMax() const;

            // Get half the distance between the cube's min and max vertices.
            ZMath::Vec3D getHalfSize() const;

            // Get the vertices of the cube in terms of global coordinates.
            // Remeber to use delete[] on the variable you assign this after use to free the memory.
            ZMath::Vec3D* getVertices() const;
    };
} // namespace Primitives

#endif // !PRIMITIVES_H
