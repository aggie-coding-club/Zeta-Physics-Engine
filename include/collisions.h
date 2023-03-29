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

        CollisionManifold findCollisionFeatures(Primitives::Sphere const &sphere, Primitives::Cube const &cube);

        CollisionManifold findCollisionFeatures(Primitives::AABB const &aabb1, Primitives::AABB const &aabb2);

        CollisionManifold findCollisionFeatures(Primitives::AABB const &aabb, Primitives::Cube const &cube);

        CollisionManifold findCollisionFeatures(Primitives::Cube const &cube1, Primitives::Cube const &cube2);

        // * =========================
        // * Impulse Resolution
        // * =========================

        // Resolve a collision between two spheres
        void collisionResponse(Primitives::Sphere const &sphere1, Primitives::Sphere const &sphere2, CollisionManifold const &manifold) {};

        // Resolve a collision between a sphere and an AABB
        void collisionResponse(Primitives::Sphere const &sphere, Primitives::AABB const &aabb, CollisionManifold const &manifold) {
            // delta v = J/m
            // For this calculation we need to acocunt for the relative velocity between the two objects
            // v_r = v_1 - v_2
            // To determine the total velocity of the collision: v_j = -(1 + coeffOfRestitution)v_r dot collisionNormal
            // Impulse then equals: J = v_j/(invMass_1 + invMass_2)
            // v_1' = v_1 + invMass_1 * J * collisionNormal
            // v_2' = v_2 - invMass_2 * J * collisionNormal. Note the - is to account for the direction which the normal is pointing.
            // It's opposite for one of the two objects.

            // todo signs might be messed up

            float J = (((sphere.rb.velocity - aabb.rb.velocity) * -(1 + sphere.rb.coeffOfRestitution * aabb.rb.coeffOfRestitution)) 
                    * manifold.normal)/(sphere.rb.invMass + aabb.rb.invMass);

            sphere.rb.velocity += manifold.normal * (sphere.rb.invMass * J);
            aabb.rb.velocity -= manifold.normal * (aabb.rb.invMass * J);
        };

        // Resolve a collision between a sphere and a cube
        void collisionResponse(Primitives::Sphere const &sphere, Primitives::Cube const &cube, CollisionManifold const &manifold) {};

        // Resolve a collision between two AABBs
        void collisionResponse(Primitives::AABB const &aabb1, Primitives::AABB const &aabb2, CollisionManifold const &manifold) {};

        // Resolve a collision between an AABB and a cube
        void collisionResponse(Primitives::AABB const &aabb, Primitives::Cube const &cube, CollisionManifold const &manifold) {
            float J = (((aabb.rb.velocity - cube.rb.velocity) * -(1 + aabb.rb.coeffOfRestitution * cube.rb.coeffOfRestitution)) 
                    * manifold.normal)/(aabb.rb.invMass + cube.rb.invMass);

            aabb.rb.velocity += manifold.normal * (aabb.rb.invMass * J);
            cube.rb.velocity -= manifold.normal * (cube.rb.invMass * J);
        };

        // Resolve a collision between two cubes
        void collisionResponse(Primitives::Cube const &cube1, Primitives::Cube const &cube2, CollisionManifold const &manifold) {
            float J = (((cube1.rb.velocity - cube2.rb.velocity) * -(1 + cube1.rb.coeffOfRestitution * cube2.rb.coeffOfRestitution)) 
                    * manifold.normal)/(cube1.rb.invMass + cube2.rb.invMass);

            cube1.rb.velocity += manifold.normal * (cube1.rb.invMass * J);
            cube2.rb.velocity -= manifold.normal * (cube2.rb.invMass * J);
        };
    }

    // * ====================
    // * Main Handler
    // * ====================

    // Find the collision features and resolve the impulse between two arbitrary primitives.
    void resolveCollision(Primitives::Collider3D const &c1, Primitives::Collider3D const &c2) {
        if (c1.type == Primitives::SPHERE_COLLIDER && c2.type == Primitives::SPHERE_COLLIDER) {
            CollisionManifold manifold = findCollisionFeatures(c1.sphere, c2.sphere);
            if (manifold.hit) { collisionResponse(c1.sphere, c2.sphere, manifold); }
            return;
        }

        if (c1.type == Primitives::SPHERE_COLLIDER && c2.type == Primitives::AABB_COLLIDER) {
            CollisionManifold manifold = findCollisionFeatures(c1.sphere, c2.aabb);
            if (manifold.hit) { collisionResponse(c1.sphere, c2.sphere, manifold); }
            return;
        }

        if (c1.type == Primitives::SPHERE_COLLIDER && c2.type == Primitives::CUBE_COLLIDER) {
            CollisionManifold manifold = findCollisionFeatures(c1.sphere, c2.cube);
            if (manifold.hit) { collisionResponse(c1.sphere, c2.sphere, manifold); }
            return;
        }

        if (c1.type == Primitives::AABB_COLLIDER && c2.type == Primitives::SPHERE_COLLIDER) {
            CollisionManifold manifold = findCollisionFeatures(c2.sphere, c1.aabb);
            manifold.normal *= -1; // flip the direction as the original order passed in was reversed
            if (manifold.hit) { collisionResponse(c1.sphere, c2.sphere, manifold); }
            return;
        }

        if (c1.type == Primitives::AABB_COLLIDER && c2.type == Primitives::AABB_COLLIDER) {
            CollisionManifold manifold = findCollisionFeatures(c1.aabb, c2.aabb);
            if (manifold.hit) { collisionResponse(c1.sphere, c2.sphere, manifold); }
            return;
        }

        if (c1.type == Primitives::AABB_COLLIDER && c2.type == Primitives::CUBE_COLLIDER) {
            CollisionManifold manifold = findCollisionFeatures(c1.aabb, c2.cube);
            if (manifold.hit) { collisionResponse(c1.sphere, c2.sphere, manifold); }
            return;
        }

        if (c1.type == Primitives::CUBE_COLLIDER && c2.type == Primitives::SPHERE_COLLIDER) {
            CollisionManifold manifold = findCollisionFeatures(c2.sphere, c1.cube);
            manifold.normal *= -1; // flip the direction as the original order passed in was reversed
            if (manifold.hit) { collisionResponse(c1.sphere, c2.sphere, manifold); }
            return;
        }

        if (c1.type == Primitives::CUBE_COLLIDER && c2.type == Primitives::AABB_COLLIDER) {
            CollisionManifold manifold = findCollisionFeatures(c2.aabb, c1.cube);
            manifold.normal *= -1; // flip the direction as the original order passed in was reversed
            if (manifold.hit) { collisionResponse(c1.sphere, c2.sphere, manifold); }
            return;
        }

        if (c1.type == Primitives::CUBE_COLLIDER && c2.type == Primitives::CUBE_COLLIDER) {
            CollisionManifold manifold = findCollisionFeatures(c1.cube, c2.cube);
            if (manifold.hit) { collisionResponse(c1.sphere, c2.sphere, manifold); }
            return;
        }

        // * User defined types begin here.
    };
};

#endif // !COLLISIONS_H
