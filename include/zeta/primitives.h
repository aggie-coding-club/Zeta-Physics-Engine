#pragma once

#include "zmath.h"

namespace Zeta {
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
            inline ZMath::Vec3D getMin() const { return ZMath::Vec3D(ZMath::min(start.x, end.x), ZMath::min(start.y, end.y), ZMath::min(start.z, end.z)); };

            // A vector with greatest value of x, y, and z the line segment reaches.
            inline ZMath::Vec3D getMax() const { return ZMath::Vec3D(ZMath::max(start.x, end.x), ZMath::max(start.y, end.y), ZMath::max(start.z, end.z)); };
    };

    // Models a rectangular, finite plane in 3D space.
    // Planes should only be used as colliders for static bodies.
    // This should be used to model death planes, borders, etc. as planes are not affected by forces and impulse.
    class Plane {
        private:
            ZMath::Vec2D halfSize; // todo maybe change to be a vec3D with z = 0.0f. This would allow for easier access and use in manifolds and collision detection

        public:
            ZMath::Vec3D pos; // Center of the plane.
            ZMath::Vec3D normal; // Normal vector to the plane in global coordinates. Cached for efficiency.
            ZMath::Mat3D rot; // Rotate anything from global space to this plane's local space. Cache this value for efficiency.

            float theta; // Angle rotated with respect to the XY plane.
            float phi; // Angle rotated with respect to the XZ plane.

            /**
             * @brief Create a rotated plane.
             * 
             * @param min (Vec2D) The min vertex of the plane as if it was unrotated.
             * @param max (Vec2D) The max vertex of the plane as if it was unrotated.
             * @param z (float) The z-level the plane lies on.
             * @param angXY (float) The angle, in degrees, the plane is rotated with respect to the XY plane. Default is 0 degrees.
             * @param angXZ (float) The angle, in degrees, the plane is rotated with respect to the XZ plane. Default is 0 degrees.
             */
            Plane(ZMath::Vec2D const &min, ZMath::Vec2D const &max, float z, float angXY = 0.0f, float angXZ = 0.0f) {
                halfSize = (max - min) * 0.5f;
                pos = ZMath::Vec3D(min.x + halfSize.x, min.y + halfSize.y, z);

                theta = angXY;
                phi = angXZ;

                ZMath::Vec3D v1 = ZMath::Vec3D(-halfSize.x, -halfSize.y, 0.0f);
                ZMath::Vec3D v2 = ZMath::Vec3D(halfSize.x, -halfSize.y, 0.0f);

                rot = ZMath::Mat3D::generateRotationMatrix(angXY, angXZ);

                // Rotate the points to find the normal.
                // We do not need to add back the positions as it would be subtracted again from both when taking the cross.
                // // ZMath::Mat3D rotT = rot.transpose();

                v1 = rot * v1;
                v2 = rot * v2;

                normal = v2.cross(v1);
            };

            ZMath::Vec2D getLocalMin() const { return ZMath::Vec2D(pos.x - halfSize.x, pos.y - halfSize.y); };
            ZMath::Vec2D getLocalMax() const { return ZMath::Vec2D(pos.x + halfSize.x, pos.y + halfSize.y); };
            ZMath::Vec2D getHalfSize() const { return halfSize; };

            // Get the vertices of the plane in terms of global coordinates.
            // Remember to use delete[] on the object you assign this to afterwards to free the memory.
            ZMath::Vec3D* getVertices() const {
                ZMath::Vec3D* v = new ZMath::Vec3D[4]; // 4 as it is rectangular

                v[0] = ZMath::Vec3D(-halfSize.x, -halfSize.y, 0.0f);
                v[1] = ZMath::Vec3D(-halfSize.x, halfSize.y, 0.0f);
                v[2] = ZMath::Vec3D(halfSize.x, halfSize.y, 0.0f);
                v[3] = ZMath::Vec3D(halfSize.x, -halfSize.y, 0.0f);

                // rotate each vertex
                for (int i = 0; i < 4; i++) { v[i] = rot*v[i] + pos; }

                return v;
            };
    };

    class Sphere {
        public:
            float r; // radius
            ZMath::Vec3D c; // centerpoint

            // @brief Create a Sphere with an arbitrary radius and center.
            //
            // @param center (Vec3D) Center of the sphere.
            // @param rho (float) Radius of the sphere.
            Sphere(ZMath::Vec3D const &center, float rho) : r(rho), c(center) {};
    };

    class AABB {
        private:
            ZMath::Vec3D halfSize;

        public:
            ZMath::Vec3D pos; // Centerpoint of the AABB.

            /** 
             * @brief Instantiate a 3D unrotated Cube.
             * 
             * @param min (Vec3D) Min vertex of the AABB.
             * @param max (Vec3D) Max vertex of the AABB.
             */
            AABB(ZMath::Vec3D const &min, ZMath::Vec3D const &max) : halfSize((max - min) * 0.5f), pos(min + halfSize) {};

            inline ZMath::Vec3D getMin() const { return pos - halfSize; };
            inline ZMath::Vec3D getMax() const { return pos + halfSize; };
            inline ZMath::Vec3D getHalfSize() const { return halfSize; };

            ZMath::Vec3D* getVertices() const{
                ZMath::Vec3D* vertices = new ZMath::Vec3D[8];

                // todo fix order to match OpenGL bindings

                vertices[0] = {pos.x - halfSize.x, pos.y - halfSize.y, pos.z + halfSize.z}; // bottom left
                vertices[1] = {pos.x - halfSize.x, pos.y + halfSize.y, pos.z + halfSize.z}; // top left
                vertices[2] = {pos.x + halfSize.x, pos.y + halfSize.y, pos.z + halfSize.z}; // top right
                vertices[3] = {pos.x + halfSize.x, pos.y - halfSize.y, pos.z + halfSize.z};  // bottom right
                
                vertices[4] = {pos.x - halfSize.x, pos.y - halfSize.y, pos.z - halfSize.z}; // bottom left
                vertices[5] = {pos.x - halfSize.x, pos.y + halfSize.y, pos.z - halfSize.z}; // top left
                vertices[6] = {pos.x + halfSize.x, pos.y + halfSize.y, pos.z - halfSize.z}; // top right
                vertices[7] = {pos.x + halfSize.x, pos.y - halfSize.y, pos.z - halfSize.z}; // bottom right

                return vertices;
            };
    };

    class Cube {
        private:
            ZMath::Vec3D halfSize;

        public:
            ZMath::Vec3D pos; // Centerpoint of the Cube.
            ZMath::Mat3D rot; // Rotate anything from global space to this cube's local space. Cache this value for efficiency.

            float theta; // Rotation with respect to the XY plane.
            float phi; // Rotation with respect to the XZ plane.

            // @brief Create a cube rotated by an arbitrary angle with arbitrary min and max vertices.
            //
            // @param min Min vertex of the cube as if it was not rotated.
            // @param max Max vertex of the cube as if it was not rotated.
            // @param angXY Angle the cube is rotated by with respect to the XY plane in degrees.
            // @param angXZ Angle the cube is rotated by with respect to the XZ plane in degrees.
            Cube(ZMath::Vec3D const &min, ZMath::Vec3D const &max, float angXY, float angXZ){
                halfSize = ((max - min) * 0.5f);
                pos = min + halfSize;
                theta = angXY;
                phi = angXZ;
                rot = ZMath::Mat3D::generateRotationMatrix(theta, phi);
            }
            
            // Get the min vertex in the cube's UVW coordinates.
            inline ZMath::Vec3D getLocalMin() const { return pos - halfSize; };

            // Get the max vertex in the cube's UVW coordinates.
            inline ZMath::Vec3D getLocalMax() const { return pos + halfSize; };

            // Get half the distance between the cube's min and max vertices.
            inline ZMath::Vec3D getHalfSize() const { return halfSize; };

            // Get the vertices of the cube in terms of global coordinates.
            // Remeber to use delete[] on the variable you assign this after use to free the memory.
            ZMath::Vec3D* getVertices() const {
                ZMath::Vec3D* v = new ZMath::Vec3D[8];

                // todo reorder to match OpenGL bindings
                v[0] = -halfSize;
                v[1] = ZMath::Vec3D(-halfSize.x, -halfSize.y, halfSize.z);
                v[2] = ZMath::Vec3D(halfSize.x, -halfSize.y, halfSize.z);
                v[3] = ZMath::Vec3D(halfSize.x, -halfSize.y, -halfSize.z);
                v[4] = ZMath::Vec3D(-halfSize.x, halfSize.y, halfSize.z);
                v[5] = ZMath::Vec3D(-halfSize.x, halfSize.y, -halfSize.z);
                v[6] = ZMath::Vec3D(halfSize.x, halfSize.y, -halfSize.z);
                v[7] = halfSize;

                // Rotate the vertices.
                for (int i = 0; i < 8; i++) { v[i] = rot*v[i] + pos; }

                return v;
            }; 
    };
    
    class TriangularPyramid {
        private: 
            float radius; // distance from the centerpoint to a vertex
            // Constants that help with calculating vertices
            const float c1 = 0.6123724357f; // (sqrt(3)/(2*sqrt(2)))
            const float c2 = 0.5929270613f; // (3*sqrt(5)/(8*sqrt(2)))
            const float c3 = 0.1530931089f; // (sqrt(3)/(8*sqrt(2)))
            const float c4 = 0.2964635306f; // (3*sqrt(5)/(16*sqrt(2)))
        public:
            ZMath::Vec3D pos; // Center point
            ZMath::Mat3D rot; // Rotation matrix

            float sideLength; // Side length, all side lengths are the same in a triangular pyramid
            float theta; // Angle with respect to the XY plane in degrees, is 0 when single point points +x
            float phi; // Angle with respect to XZ plane in degrees, pyramid points straight up in +z direction by default

            // @param ctr center point; equidistant from all vertices
            // @param sl side length; all side lengths are equal
            // @param th angle with respect to the XY plane
            // @param f angle with respect to the XZ plane
            TriangularPyramid(ZMath::Vec3D const &ctr, float sl, float th = 0.0f, float f = 0.0f): theta(th), phi(f), sideLength(sl) {
                this->pos = ctr;
                this->radius = c1 * sl;
                this->rot = ZMath::Mat3D::generateRotationMatrix(th, f);
            };
            // Returns all 4 vertices of the tetrahedron in global coordinates
            // Must use delete[] to free memory used by the value returned
            ZMath::Vec3D* getVertices() const {
                ZMath::Vec3D* v = new ZMath::Vec3D[4];
                v[0] = ZMath::Vec3D(pos.x, pos.y, pos.z + (c1 * this->sideLength));
                v[1] = ZMath::Vec3D(pos.x + (c2 * this->sideLength), pos.y, pos.z - (c3 * this->sideLength));
                v[2] = ZMath::Vec3D(pos.x - (c4 * this->sideLength), pos.y - (0.5 * this->sideLength), pos.z - (c3 * this->sideLength));
                v[3] = ZMath::Vec3D(pos.x - (c4 * this->sideLength), pos.y + (0.5 * this->sideLength), pos.z - (c3 * this->sideLength));
                // Applies rotation to each vertex
                for(int i = 0; i < 4; ++i) {
                    v[i] = pos + (rot * v[i]);
                }
                return v;
            };
    };
} // namespace Primitives
