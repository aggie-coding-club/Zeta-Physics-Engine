#ifndef COLLISIONS_H
#define COLLISIONS_H

#include "intersections.h"

// todo move all of the collision related stuff (intersections and manifolds) into here
// todo move the intersection detection check into the findCollisionFeatures stuff

// We can use the normals for each as possible separation axes
// We have to account for certain edge cases when moving this to 3D

namespace Collisions {
    // * =========================
    // * Collision Manifolds
    // * =========================

    // ? Note: if we have objects A and B colliding, the collison normal will point towards A and away from B.

    struct CollisionManifold {
        ZMath::Vec3D normal; // collision normal
        ZMath::Vec3D* contactPoints; // contact points of the collision
        float pDist; // penetration distance
        int numPoints; // number of contact points
        bool hit; // do they intersect
    };

    namespace {
        // * ===================================
        // * Collision Manifold Calculators
        // * ===================================

        // todo test these functions

        CollisionManifold findCollisionFeatures(Primitives::Sphere const &sphere1, Primitives::Sphere const &sphere2) {
            CollisionManifold result;

            float r = sphere1.r + sphere2.r;

            if (sphere1.rb.pos.distSq(sphere2.rb.pos) > r*r) {
                result.hit = 0;
                return result;
            }

            ZMath::Vec3D sphereDiff = sphere1.rb.pos - sphere2.rb.pos;
            float d = sphereDiff.mag(); // allows us to only take the sqrt once

            result.pDist = (sphere1.r + sphere2.r - d) * 0.5f;
            result.normal = sphereDiff * (1.0f/d);
            result.hit = 1;

            // determine the contact point
            result.numPoints = 1;
            result.contactPoints = new ZMath::Vec3D[result.numPoints];
            result.contactPoints[0] = sphere2.rb.pos + (result.normal * (sphere2.r - result.pDist));

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

            // The closest point to the sphere's center will be our contact point.
            // Therefore, we just set our contact point to closest.

            result.numPoints = 1;
            result.contactPoints = new ZMath::Vec3D[1];
            result.contactPoints[0] = closest;

            // determine the penetration distance and collision normal

            ZMath::Vec3D diff = sphere.rb.pos - closest;
            float d = diff.mag(); // allows us to only take the sqrt once
            result.pDist = sphere.r - d;
            result.normal = diff * (1.0f/d);

            return result;
        };

        CollisionManifold findCollisionFeatures(Primitives::Sphere const &sphere, Primitives::Cube const &cube) {
            CollisionManifold result;

            ZMath::Vec3D center = sphere.rb.pos;
            ZMath::Vec3D min = cube.getLocalMin(), max = cube.getLocalMax();

            // rotate the center of the sphere into the UVW coordinates of our cube
            ZMath::rotateXZ(center, cube.rb.pos, 360 - cube.rb.phi);
            ZMath::rotateXY(center, cube.rb.pos, 360 - cube.rb.theta);
            
            // perform the check as if it was an AABB vs Sphere
            ZMath::Vec3D closest(center);

            closest.x = ZMath::clamp(closest.x, min.x, max.x);
            closest.y = ZMath::clamp(closest.y, min.y, max.y);
            closest.z = ZMath::clamp(closest.z, min.z, max.z);

            result.hit = closest.distSq(center) <= sphere.r*sphere.r;

            if (!result.hit) { return result; }

            // the closest point to the sphere's center will be our contact point rotated back into UVW coordinates

            ZMath::rotateXY(closest, cube.rb.pos, cube.rb.theta);
            ZMath::rotateXZ(closest, cube.rb.pos, cube.rb.phi);

            result.numPoints = 1;
            result.contactPoints = new ZMath::Vec3D[1];
            result.contactPoints[0] = closest;

            // determine the penetration distance and the collision normal

            ZMath::Vec3D diff = sphere.rb.pos - closest;
            float d = diff.mag(); // allows us to only take the sqrt once
            result.pDist = sphere.r - d;
            result.normal = diff * (1.0f/d);

            return result;
        };

        CollisionManifold findCollisionFeatures(Primitives::AABB const &aabb1, Primitives::AABB const &aabb2);

        CollisionManifold findCollisionFeatures(Primitives::AABB const &aabb, Primitives::Cube const &cube);

        CollisionManifold findCollisionFeatures(Primitives::Cube const &cube1, Primitives::Cube const &cube2);
    }

    // Find the collision features and resolve the impulse between two arbitrary primitives.
    CollisionManifold findCollisionFeatures(Primitives::Collider3D const &c1, Primitives::Collider3D const &c2) {
        switch (c1.type) {
            case Primitives::SPHERE_COLLIDER:
                if (c2.type == Primitives::SPHERE_COLLIDER) { return findCollisionFeatures(c1.sphere, c2.sphere); }
                if (c2.type == Primitives::AABB_COLLIDER) { return findCollisionFeatures(c1.sphere, c2.aabb); }
                if (c2.type == Primitives::CUBE_COLLIDER) { return findCollisionFeatures(c1.sphere, c2.cube); }

            case Primitives::AABB_COLLIDER:
                if (c2.type == Primitives::SPHERE_COLLIDER) {
                    CollisionManifold manifold = findCollisionFeatures(c2.sphere, c1.aabb);
                    manifold.normal *= -1; // flip the direction as the original order passed in was reversed
                    return manifold;
                }

                if (c2.type == Primitives::AABB_COLLIDER) { return findCollisionFeatures(c1.aabb, c2.aabb); }
                if (c2.type == Primitives::CUBE_COLLIDER) { return findCollisionFeatures(c1.sphere, c2.cube); }

            case Primitives::CUBE_COLLIDER:
                if (c2.type == Primitives::CUBE_COLLIDER) {
                    CollisionManifold manifold = findCollisionFeatures(c2.sphere, c1.cube);
                    manifold.normal *= -1; // flip the direction as the original order passed in was reversed
                    return manifold;
                }

                if (c2.type == Primitives::AABB_COLLIDER) {
                    CollisionManifold manifold = findCollisionFeatures(c2.aabb, c1.cube);
                    manifold.normal *= -1; // flip the direction as the original order passed in was reversed
                    return manifold;
                }

                if (c2.type == Primitives::CUBE_COLLIDER) { return findCollisionFeatures(c1.cube, c2.cube); }

            default:
                // * User defined types go here.
                break;
        }

        return (CollisionManifold) {ZMath::Vec3D(), nullptr, -1.0f, 0, 0};
    };
};

#endif // !COLLISIONS_H
