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

            float d = sphere1.rb.pos.dist(sphere2.rb.pos);
            result.pDist = (sphere1.r + sphere2.r - d) * 0.5f;
            result.normal = (sphere1.rb.pos - sphere2.rb.pos).normalize();
            result.hit = 1;

            // determine the contact point
            result.numPoints = 1;
            result.contactPoints = new ZMath::Vec3D[result.numPoints];
            result.contactPoints[0] = sphere1.rb.pos + (result.normal * (sphere1.r - result.pDist));

            return result;
        };

        CollisionManifold findCollisionFeatures(Primitives::Sphere const &sphere, Primitives::AABB const &aabb) {
            CollisionManifold result;

            // ? We know a sphere and AABB would intersect if the distance from the closest point to the center on the AABB
            // ?  from the center is less than or equal to the radius of the sphere.
            // ? We can determine the closet point by clamping the value of the sphere's center between the min and max of the AABB.
            // ? From here, we can check the distance from this point to the sphere's center.

            ZMath::Vec3D closest(sphere.rb.pos);
            ZMath::Vec3D min = aabb.getMin(), max = aabb.getMax();

            closest.x = ZMath::clamp(closest.x, min.x, max.x);
            closest.y = ZMath::clamp(closest.y, min.y, max.y);
            closest.z = ZMath::clamp(closest.z, min.z, max.z);

            result.hit = closest.distSq(sphere.rb.pos) <= sphere.r*sphere.r;

            if (!result.hit) { return result; }

            return result;
        };

        CollisionManifold findCollisionFeatures(Primitives::Sphere const &sphere, Primitives::Cube const &cube);

        CollisionManifold findCollisionFeatures(Primitives::AABB const &aabb1, Primitives::AABB const &aabb2);

        CollisionManifold findCollisionFeatures(Primitives::AABB const &aabb, Primitives::Cube const &cube);

        CollisionManifold findCollisionFeatures(Primitives::Cube const &cube1, Primitives::Cube const &cube2);
    }

    // Find the collision features between two arbitrary primitives.
    CollisionManifold findCollisionFeatures(Primitives::Collider3D const &c1, Primitives::Collider3D const &c2) {
        if (c1.type == Primitives::Collider3D::SPHERE_COLLIDER && c2.type == Primitives::Collider3D::SPHERE_COLLIDER) {
            // ! set up user defined casting
            // ! alternatively, handle the manifold stuff directly in here.
            // todo maybe store the rigidbody in the collider
            // ! I'll fix this later
        }
    };
};

#endif // !COLLISIONS_H
