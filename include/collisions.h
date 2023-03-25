#ifndef COLLISIONS_H
#define COLLISIONS_H

#include "intersections.h"

// todo move all of the collision related stuff (intersections and manifolds) into here
// todo move the intersection detection check into the findCollisionFeatures stuff

// We can use the normals for each as possible separation axes
// We have to account for certain edge cases when moving this to 3D

namespace Collisions {
    // ? Note: if we have objects A and B colliding, the collison normal will point away from A and towards B.

    struct CollisionManifold {
        ZMath::Vec3D normal; // collision normal
        float pDist; // penetration distance
        ZMath::Vec3D* contactPoints; // contact points of the collision
        int numPoints; // number of contact points
        bool hit; // do they intersect
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

        CollisionManifold findCollisionFeatures(Primitives::Sphere const &sphere1, Primitives::Sphere const &sphere2) {
            CollisionManifold result;

            float r = sphere1.r + sphere2.r;

            if (sphere1.rb.pos.distSq(sphere2.rb.pos) > r*r) {
                result.hit = 0;
                return result;
            }

            float d = abs(sphere1.rb.pos.dist(sphere2.rb.pos));
            result.pDist = (sphere1.r + sphere2.r - d) * 0.5f;
            result.normal = (sphere1.rb.pos - sphere2.rb.pos).normalize();
            result.hit = 1;

            // determine the contact point
            result.numPoints = 1;
            result.contactPoints = new ZMath::Vec3D[result.numPoints];
            result.contactPoints[0] = sphere1.rb.pos + (result.normal * (sphere1.r - result.pDist));

            return result;
        };

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
