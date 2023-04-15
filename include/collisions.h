#ifndef COLLISIONS_H
#define COLLISIONS_H

#include <intersections.h>

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

        // * ====================================================
        // * Helper Functions for 3D Box Collision Manifolds
        // * ====================================================

        // * Enums used for denotating the edges of the cubes
        enum Axis {
            FACE_A_X,
            FACE_A_Y,
            FACE_A_Z,
            FACE_B_X,
            FACE_B_Y,
            FACE_B_Z
        };

        struct ClipVertex {
            ZMath::Vec3D vec;

            char inEdge1;
            char outEdge1;
            char inEdge2;
            char outEdge2;
            char inEdge3;
            char outEdge3;
            // ! unsure how many we will actually need so we'll revisit this when I have a better understanding
        };

        // ! determine what exactly this does to write documentation and implement
        static void computeIncidentEdge(ClipVertex c[2], const ZMath::Vec3D& halfsize, const ZMath::Vec3D& pos, 
                                        const ZMath::Mat3D& rot, const ZMath::Vec3D& normal) {

        };

        // ! determine what exactly this does to write documentation and implement
        int clipSegmentToLine();

        CollisionManifold findCollisionFeatures(Primitives::AABB const &aabb1, Primitives::AABB const &aabb2);

        CollisionManifold findCollisionFeatures(Primitives::AABB const &aabb, Primitives::Cube const &cube);

        CollisionManifold findCollisionFeatures(Primitives::Cube const &cube1, Primitives::Cube const &cube2) {
            CollisionManifold result;

            // half size of cube a and b respectively
            ZMath::Vec3D hA = cube1.getHalfSize(), hB = cube2.getHalfSize();

            // * determine the rotation matrices of A and B

            // rotate anything in A's local space to global space
            ZMath::Mat3D rotA = ZMath::Mat3D::generateRotationMatrix(cube1.rb.theta, cube1.rb.phi);

            // rotate anything in B's local space to global space
            ZMath::Mat3D rotB = ZMath::Mat3D::generateRotationMatrix(cube2.rb.theta, cube2.rb.phi);

            // rotate anything from global space to A's local space
            ZMath::Mat3D rotAT = rotA.transpose();

            // rotate anything from gobal space to B's local space
            ZMath::Mat3D rotBT = rotB.transpose();

            // determine the difference between the positions
            ZMath::Vec3D dP = cube2.rb.pos - cube1.rb.pos;
            ZMath::Vec3D dA = rotAT * dP;
            ZMath::Vec3D dB = rotBT * dP;

            // * rotation matrices for switching between local spaces
            
            // Rotate anything from B's local space into A's
            ZMath::Mat3D C = ZMath::abs(rotAT * rotB);

            // Rotate anything from A's local space into B's
            ZMath::Mat3D CT = C.transpose();

            // * Check for intersections with the separating axis theorem

            // amount of penetration along A's axes
            ZMath::Vec3D faceA = ZMath::abs(dA) - hA - C * hB;
            if (faceA.x > 0 || faceA.y > 0 || faceA.z > 0) {
                result.hit = 0;
                return result;
            }

            // amount of penetration along B's axes
            ZMath::Vec3D faceB = ZMath::abs(dB) - hB - C * hA;
            if (faceB.x > 0 || faceB.y > 0 || faceB.z > 0) {
                result.hit = 0;
                return result;
            }

            // * Find the best axis (i.e. the axis with the least amount of penetration)

            // Assume A's x-axis is the best axis first
            Axis axis = FACE_A_X;
            float separation = faceA.x;
            ZMath::Vec3D normal = dA.x > 0.0f ? -rotA.c1 : rotA.c1;

            // tolerance values
            float relativeTol = 0.95f;
            float absoluteTol = 0.01f;

            // check if there is another axis better than A's x axis
            if (faceA.y > relativeTol * separation + absoluteTol * hA.y) {
                axis = FACE_A_Y;
                separation = faceA.y;
                normal = dA.y > 0.0f ? -rotA.c2 : rotA.c2;
            }

            if (faceA.z > relativeTol * separation + absoluteTol * hA.z) {
                axis = FACE_A_Z;
                separation = faceA.z;
                normal = dA.z > 0.0f ? -rotA.c3 : rotA.c3;
            }

            // B's axes
            if (faceB.x > relativeTol * separation + absoluteTol * hB.x) {
                axis = FACE_B_X;
                separation = faceB.x;
                normal = dB.x > 0.0f ? -rotB.c1 : rotB.c1;
            }

            if (faceB.y > relativeTol * separation + absoluteTol * hB.y) {
                axis = FACE_B_Y;
                separation = faceB.y;
                normal = dB.y > 0.0f ? -rotB.c2 : rotB.c2;
            }

            if (faceB.z > relativeTol * separation + absoluteTol * hB.z) {
                axis = FACE_B_Z;
                separation = faceB.z;
                normal = dB.z > 0.0f ? -rotB.c3 : rotB.c3;
            }

            // * Setup clipping plane data based on the best axis

            ZMath::Vec3D frontNormal, sideNormal;
            ClipVertex incidentEdge[2];
            float front, negSide, posSide;

            // * Compute the clipping lines and line segment to be clipped

            switch(axis) {
                case FACE_A_X:
                    // * Project onto the best axis (so in this case A's x-axis)
                    // * We now know the min and max values for the remaining face must be contained in the vertices; therefore, we can cosntruct our min and max vectors for the face
                    // * We can then use this to solve the problem further

                    // ? Current roadblock: Need to find a way to determine distance to the side but there are two sides to do so with
                    // ? This doesn't allow for the same simple check as in 2D
                    // ? Therefore, we can either solve this new problem as a 2D one or find a more elegant solution
                    // ? I will try to find the latter but will resort to the former if needed

                    // ! go through this and math and draw it out to fully understand it and correct any errors
                    frontNormal = normal;
                    front = cube1.rb.pos * frontNormal + hA.x;
                    sideNormal = rotA.c2;
                    float side = cube1.rb.pos * sideNormal;
                    /*negSide = -side + hA.y;
                    posSide = side + hA.y;*/

                    // todo compute the pos and neg sides for y and z

                    computeIncidentEdge(incidentEdge, hB, cube2.rb.pos, rotB, frontNormal);
                    break;
                
                case FACE_A_Y:
                    break;

                case FACE_A_Z:
                    break;

                case FACE_B_X:
                    break;
                
                case FACE_B_Y:
                    break;

                case FACE_B_Z:
                    break;
            }
        };
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
                    manifold.normal = -manifold.normal; // flip the direction as the original order passed in was reversed
                    return manifold;
                }

                if (c2.type == Primitives::AABB_COLLIDER) { return findCollisionFeatures(c1.aabb, c2.aabb); }
                if (c2.type == Primitives::CUBE_COLLIDER) { return findCollisionFeatures(c1.sphere, c2.cube); }

            case Primitives::CUBE_COLLIDER:
                if (c2.type == Primitives::CUBE_COLLIDER) {
                    CollisionManifold manifold = findCollisionFeatures(c2.sphere, c1.cube);
                    manifold.normal = -manifold.normal; // flip the direction as the original order passed in was reversed
                    return manifold;
                }

                if (c2.type == Primitives::AABB_COLLIDER) {
                    CollisionManifold manifold = findCollisionFeatures(c2.aabb, c1.cube);
                    manifold.normal = -manifold.normal; // flip the direction as the original order passed in was reversed
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
