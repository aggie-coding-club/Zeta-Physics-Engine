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
};

#endif // !COLLISIONS_H
