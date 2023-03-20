#ifndef PRIMITIVES_H
#define PRIMITIVES_H

#include "rigidbody.h"

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

    // ! update to fix -- should store a single point on the plane and the normal (radius of a plane doesn't exist)
    class Plane {
        private:
            ZMath::Vec3D normal;
            float radius;

        public:
            Plane(ZMath::Vec3D const &norm, float r) : normal(norm), radius(r){};

            Plane(ZMath::Vec3D const &min, ZMath::Vec3D const &max) : normal(min.cross(max)), radius((max - min).mag()){};
    };

    class Sphere {
        public:
            // @brief Create a Sphere centered at (0, 0, 0) with a radius of 1.
            Sphere() : rho(1.0f), center(ZMath::Vec3D()){};

            // @brief Create a Sphere with an arbitrary radius and center.
            //
            // @param r (float) Radius of the sphere.
            // @param c (Vec3D) Center of the sphere.
            Sphere(float r, ZMath::Vec3D const &c) : rho(r), center(c){};

            void setRadius(float r);
            void setCenter(ZMath::Vec3D const &c);

            float getRadius();
            ZMath::Vec3D getCenter();

            float getRadius() const;
            ZMath::Vec3D getCenter() const;

        private:
            float rho; // radius
            ZMath::Vec3D center;
    };

    // ! update to implement getVertices (potentially could implement a rigidbody3D too to make computations with forces easier later)
    class AABB {
        private:
            ZMath::Vec3D pos;
            ZMath::Vec3D halfSize;

        public:
            AABB(ZMath::Vec3D const &max, ZMath::Vec3D const &min) : pos(max - min), halfSize(pos * 0.5f){};

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
            RigidBody3D rb; // rigid body representing the cube -- stores the angles rotated and the center point
            ZMath::Vec3D halfSize;

        public:
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
                    : rb(RigidBody3D(max - min, angXY, angXZ)), halfSize((max - min) * 0.5f) {};

            // Set the angle the cube is rotated with respect to the XY plane.
            void setTheta(float theta);

            // Set the angle the cube is rotated with respect to the XZ plane.
            void setPhi(float phi);

            // Get the angle the cube is rotated with respect to the XY plane.
            float getTheta();

            // Get the angle the cube is rotated with respect to the XZ plane.
            float getPhi();

            // Get the pos of the cube
            ZMath::Vec3D getPos();

            // Get the min vertex in the cube's UVW coordinates.
            ZMath::Vec3D getLocalMin();

            // Get the max vertex in the cube's UVW coordinates.
            ZMath::Vec3D getLocalMax();

            // Get half the distance between the cube's min and max vertices.
            ZMath::Vec3D getHalfSize();

            // Get the vertices of the cube in terms of global coordinates.
            // Remeber to use delete[] on the variable you assign this after use to free the memory.
            ZMath::Vec3D* getVertices();

            // Get the angle the cube is rotated with respect to the XY plane.
            float getTheta() const;

            // Get the angle the cube is rotated with respect to the XZ plane.
            float getPhi() const;

            // Get the pos of the cube
            ZMath::Vec3D getPos() const;

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
