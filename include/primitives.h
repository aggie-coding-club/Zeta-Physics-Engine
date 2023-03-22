#ifndef PRIMITIVES_H
#define PRIMITIVES_H

#include "rigidbody.h"
#include "staticbody.h"

// todo might want to make some of the attributes public.
// todo refactor to do what I stated above.

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

            // todo create documentation

            Plane(ZMath::Vec2D const &min, ZMath::Vec2D const &max, float z) 
                    : halfSize((max - min) * 0.5f), sb(ZMath::Vec3D(min.x + halfSize.x, min.y + halfSize.y, z), 0.0f, 0.0f) {};
            Plane(ZMath::Vec2D const &min, ZMath::Vec2D const &max, float z, float angXY, float angXZ) 
                    : halfSize((max - min) * 0.5f), sb(ZMath::Vec3D(min.x + halfSize.x, min.y + halfSize.y, z), angXY, angXZ) {};
            
            ZMath::Vec3D getNormal();

            ZMath::Vec3D getLocalMin();
            ZMath::Vec3D getLocalMax();
            ZMath::Vec2D getHalfsize();

            // Get the vertices of the plane in terms of global coordinates.
            // Remember to use delete[] on the object you assign this to afterwards to free the memory.
            ZMath::Vec3D* getVertices();

            ZMath::Vec3D getNormal() const;

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
            Sphere() : r(1.0f), rb(ZMath::Vec3D()){};

            // @brief Create a Sphere with an arbitrary radius and center.
            //
            // @param rho (float) Radius of the sphere.
            // @param center (Vec3D) Center of the sphere.
            Sphere(float rho, ZMath::Vec3D const &center) : r(rho), rb(center){};
    };

    class AABB {
        private:
            ZMath::Vec3D halfSize;

        public:
            RigidBody3D rb; // rigid body representing the AABB -- stores the center point

            // @brief Instantiate a 3D AABB.
            // 
            // @param max (Vec3D) Min vertex of the AABB.
            // @param min (Vec3D) Max vertex of the AABB.
            AABB(ZMath::Vec3D const &max, ZMath::Vec3D const &min) 
                    : halfSize((max - min) * 0.5f), rb(min + halfSize) {};

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
            Cube() : rb(RigidBody3D(ZMath::Vec3D(0), 45.0f, 45.0f)), halfSize(ZMath::Vec3D(1)) {};

            // @brief Create a cube rotated by an arbitrary angle with arbitrary min and max vertices.
            //
            // @param p1 Min vertex of the cube as if it was not rotated.
            // @param p2 Max vertex of the cube as if it was not rotated.
            // @param angXY Angle the cube is rotated by with respect to the XY plane in degrees.
            // @param angXZ Angle the cube is rotated by with respect to the XZ plane in degrees.
            Cube(ZMath::Vec3D const &min, ZMath::Vec3D const &max, float angXY, float angXZ) 
                    : halfSize((max - min) * 0.5f), rb(RigidBody3D(min + halfSize, angXY, angXZ)) {};

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
