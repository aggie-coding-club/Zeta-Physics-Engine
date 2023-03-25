#ifndef COLLISIONS_H
#define COLLISIONS_H

#include "primitives.h"

// todo move all of the collision related stuff (intersections and manifolds) into here

// ! handle with a broad phase and narrow phase
// ! retrieve data in the narrow phase and prune which objects to check in the broad phase

// We can use the normals for each as possible separation axes
// We have to account for certain edge cases when moving this to 3D

namespace Collisions {
    // ? Note: if we have objects A and B colliding, the collison normal will point away from A and towards B.

    class CollisionManifold {
        public:
            // ! look through a refresher for the rule of 5 and add in the stuff

            ZMath::Vec3D normal; // collision normal
            float p; // penetration distance
            ZMath::Vec3D* contactPoints; // contact points of the collision
            int numPoints; // number of contact points

            CollisionManifold(ZMath::Vec3D const &n, float dist, ZMath::Vec3D* contactPoints, int numPoints) : normal(n), p(dist) {
                this->contactPoints = new ZMath::Vec3D[numPoints];
                for (int i = 0; i < numPoints; i++) { this->contactPoints[i] = contactPoints[i]; }
                this->numPoints = numPoints;
            };

            ~CollisionManifold() { delete[] contactPoints; };
    };

    // Should we bother checking for collisions between the objects?
    // Should not be called on spheres as sphere intersection detection functions are fast.
    void isCollision();

    // Determine the collision normal and penetration distance for two spheres.
    // Should only be called if there is a collision.
    ZMath::Vec3D collisionNormal(Primitives::Sphere const &sphere1, Primitives::Sphere const &sphere2, float &dist);
    
    // Determine the collision normal and penetration distance for a sphere and an AABB.
    // Should only be called if there is a collision.
    ZMath::Vec3D collisionNormal(Primitives::Sphere const &sphere, Primitives::AABB const &aabb, float &dist);

    // Determine the collision normal and penetration distance for a sphere and a cube.
    // Should only be called if there is a collision.
    ZMath::Vec3D collisionNormal(Primitives::Sphere const &sphere, Primitives::Cube const &cube, float &dist);
    
    // Determine the collision normal and penetration distance for two AABBs.
    // Should only be called if there is a collision.
    ZMath::Vec3D collisionNormal(Primitives::AABB const &aabb1, Primitives::AABB const &aabb2, float &dist);

    // Determine the collision normal and penetration distance for an AABB and a cube.
    // Should only be called if there is a collision.
    ZMath::Vec3D collisionNormal(Primitives::AABB const &aabb, Primitives::Cube const &cube, float &dist);

    // Determine the collision normal and penetration distance for two cubes.
    // Should only be called if there is a collision.
    ZMath::Vec3D collisionNormal(Primitives::Cube const &cube1, Primitives::Cube const &cube2, float &dist);

    namespace {
        // * ======================
        // * Private functions
        // * ======================

        CollisionManifold findCollisionFeatures(Primitives::Sphere const &sphere1, Primitives::Sphere const &sphere2);

        CollisionManifold findCollisionFeatures(Primitives::Sphere const &sphere, Primitives::AABB const &aabb);

        CollisionManifold findCollisionFeatures(Primitives::Sphere const &sphere, Primitives::Cube const &cube);

        CollisionManifold findCollisionFeatures(Primitives::AABB const &aabb1, Primitives::AABB const &aabb2);

        CollisionManifold findCollisionFeatures(Primitives::AABB const &aabb, Primitives::Cube const &cube);

        CollisionManifold findCollisionFeatures(Primitives::Cube const &cube1, Primitives::Cube const &cube2);
    }

    // Find the collision features between two arbitrary primitives.
    CollisionManifold findCollisionFeatures(Primitives::Collider3D const &c1, Primitives::Collider3D const &c2);
};

#endif // !COLLISIONS_H
