#ifndef COLLISIONS_H
#define COLLISIONS_H

#include "intersections.h"

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

        // ! don't think we need this
        // struct ClipVertex {
        //     ZMath::Vec3D vec;

        //     char inEdge1;
        //     char outEdge1;
        //     char inEdge2;
        //     char outEdge2;
        //     char inEdge3;
        //     char outEdge3;
        //     // ! unsure how many we will actually need so we'll revisit this when I have a better understanding
        // };

        /**
         * @brief Determine the 4 vertices making up the incident face.
         * 
         * @param v Array which gets filled with the 4 vertices comprising the incident face.
         * @param h halfsize of the incident cube.
         * @param pos The position of the incident cube.
         * @param rot The rotation matrix of the incident cube.
         * @param normal The normal vector of the collision.
         */
        static void computeIncidentFace(ZMath::Vec3D v[4], const ZMath::Vec3D& h, const ZMath::Vec3D& pos, 
                                        const ZMath::Mat3D& rot, const ZMath::Vec3D& normal) {

            // todo will fs need to test this function
            // todo do some more math to make sure all the signs are correct

            // Rotate the normal to the incident cube's local space and flip the sign.
            ZMath::Vec3D n = -(rot.transpose() * normal); // ! may not need to flip the sign since our normal points towards A from B
            ZMath::Vec3D nAbs = ZMath::abs(n);

            // Determine the vertices in terms of halfsize.
            // Vertex array starts in bottom left corner when considering the face as a 2D box and goes around counterclockwise.
            if (nAbs.x > nAbs.y && nAbs.x > nAbs.z) { // x > y && x > z
                if (n.x > 0.0f) { // incident cube is intersecting on its -x side
                    v[0] = ZMath::Vec3D(-h.x, -h.y, -h.z);
                    v[1] = ZMath::Vec3D(-h.x, h.y, -h.z);
                    v[2] = ZMath::Vec3D(-h.x, h.y, h.z);
                    v[3] = ZMath::Vec3D(-h.x, -h.y, h.z);

                } else { // incident cube is intersecting on its +x side
                    v[0] = ZMath::Vec3D(h.x, -h.y, -h.z);
                    v[1] = ZMath::Vec3D(h.x, h.y, -h.z);
                    v[2] = ZMath::Vec3D(h.x, h.y, h.z);
                    v[3] = ZMath::Vec3D(h.x, -h.y, h.z);
                }

            } else if (nAbs.y > nAbs.z) { // y >= x && y > z
                if (n.y > 0.0f) { // incident cube is intersecting on its -y side
                    v[0] = ZMath::Vec3D(-h.x, -h.y, -h.z);
                    v[1] = ZMath::Vec3D(h.x, -h.y, -h.z);
                    v[2] = ZMath::Vec3D(h.x, -h.y, h.z);
                    v[3] = ZMath::Vec3D(-h.x, -h.y, h.z);

                } else { // incident cube is intersecting on its +y side
                    v[0] = ZMath::Vec3D(-h.x, h.y, -h.z);
                    v[1] = ZMath::Vec3D(h.x, h.y, -h.z);
                    v[2] = ZMath::Vec3D(h.x, h.y, h.z);
                    v[3] = ZMath::Vec3D(-h.x, h.y, h.z);
                }

            } else { // z >= y && z >= x
                if (n.z > 0.0f) { // incident cube is intersecting on its -z side
                    v[0] = ZMath::Vec3D(-h.x, -h.y, -h.z);
                    v[1] = ZMath::Vec3D(-h.x, h.y, -h.z);
                    v[2] = ZMath::Vec3D(h.x, h.y, -h.z);
                    v[3] = ZMath::Vec3D(h.x, -h.y, -h.z);

                } else { // incdient cube is intersecting on its +z side
                    v[0] = ZMath::Vec3D(-h.x, -h.y, h.z);
                    v[1] = ZMath::Vec3D(-h.x, h.y, h.z);
                    v[2] = ZMath::Vec3D(h.x, h.y, h.z);
                    v[3] = ZMath::Vec3D(h.x, -h.y, h.z);
                }
            }

            // rotate vertices back into global coordinates and translate them to their proper positions
            v[0] = pos + rot * v[0];
            v[1] = pos + rot * v[1];
            v[2] = pos + rot * v[2];
            v[3] = pos + rot * v[3];
        };

        /**
         * @brief // ! Write more when I fully know what it's doing
         * 
         * @param vOut 
         * @param vIn The vertices for the incident face.
         * @param normal 
         * @param offset 
         * @return (int) Number of vertices 
         */
        int clipSegmentToLine(ZMath::Vec3D vOut[4], ZMath::Vec3D vIn[4], const ZMath::Vec3D &normal, float offset) {
            // begin with 0 output points
            int np = 0;

            // todo fs need to test this function extensively

            // calculate the distance
            // ! it doesn't work quite like this for 3D probs
            float dist0 = normal * vIn[0] - offset;
            float dist1 = normal * vIn[1] - offset;
            float dist2 = normal * vIn[2] - offset;
            float dist3 = normal * vIn[3] - offset;

            // if the points are behind the plane (i.e. inside the cube)
            if (dist0 <= 0.0f) { vOut[np++] = vIn[0]; }
            if (dist1 <= 0.0f) { vOut[np++] = vIn[1]; }
            if (dist2 <= 0.0f) { vOut[np++] = vIn[2]; }
            if (dist3 <= 0.0f) { vOut[np++] = vIn[3]; }

            // todo this next check doesn't translate the same way to 3D as the above step does
            // todo probably evaluate both separately in terms of the y's and z's

            // check if the vertices are separated by the edge on the 1st axis
            if (dist0 * dist2 < 0.0f) {
                float interp = dist0/(dist0 - dist2);
                vOut[np] = vIn[0] + (vIn[2] - vIn[0]) * interp;
                ++np;
            }

            // check if the vertices are separated by the edge on the 2nd axis
            if (dist1 * dist3 < 0.0f) {
                float interp = dist1/(dist1 - dist3);
                vOut[np] = vIn[1] + (vIn[3] - vIn[1]) * interp;
                ++np;
            }

            return np;
        };

        CollisionManifold findCollisionFeatures(Primitives::AABB const &aabb1, Primitives::AABB const &aabb2) {
            return {};
        };

        CollisionManifold findCollisionFeatures(Primitives::AABB const &aabb, Primitives::Cube const &cube) {
            return {};
        };

        CollisionManifold findCollisionFeatures(Primitives::Cube const &cube1, Primitives::Cube const &cube2) {
            // todo make sure the sign for the normal is correct

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
            // ! not 100% sure this part works properly
            // ! issue may be we can't use projection in the same way for 3D for solving this

            // Assume A's x-axis is the best axis first
            Axis axis = FACE_A_X;
            float separation = faceA.x;
            result.normal = dA.x > 0.0f ? -rotA.c1 : rotA.c1;

            // tolerance values
            float relativeTol = 0.95f;
            float absoluteTol = 0.01f;

            // check if there is another axis better than A's x axis
            if (faceA.y > relativeTol * separation + absoluteTol * hA.y) {
                axis = FACE_A_Y;
                separation = faceA.y;
                result.normal = dA.y > 0.0f ? -rotA.c2 : rotA.c2;
            }

            if (faceA.z > relativeTol * separation + absoluteTol * hA.z) {
                axis = FACE_A_Z;
                separation = faceA.z;
                result.normal = dA.z > 0.0f ? -rotA.c3 : rotA.c3;
            }

            // B's axes
            if (faceB.x > relativeTol * separation + absoluteTol * hB.x) {
                axis = FACE_B_X;
                separation = faceB.x;
                result.normal = dB.x > 0.0f ? -rotB.c1 : rotB.c1;
            }

            if (faceB.y > relativeTol * separation + absoluteTol * hB.y) {
                axis = FACE_B_Y;
                separation = faceB.y;
                result.normal = dB.y > 0.0f ? -rotB.c2 : rotB.c2;
            }

            if (faceB.z > relativeTol * separation + absoluteTol * hB.z) {
                axis = FACE_B_Z;
                separation = faceB.z;
                result.normal = dB.z > 0.0f ? -rotB.c3 : rotB.c3;
            }

            // * Setup clipping plane data based on the best axis

            ZMath::Vec3D frontNormal, sideNormal1, sideNormal2;
            ZMath::Vec3D incidentFace[4]; // 4 vertices for the collision in 3D
            float front, negSide1, negSide2, posSide1, posSide2;

            // * Compute the clipping lines and line segment to be clipped

            switch(axis) {
                case FACE_A_X: {
                    // * Project onto the best axis (so in this case A's x-axis)
                    // * We now know the min and max values for the remaining face must be contained in the vertices; therefore, we can cosntruct our min and max vectors for the face
                    // * We can then use this to solve the problem further

                    // ? Current roadblock: Need to find a way to determine distance to the side but there are two sides to do so with
                    // ? This doesn't allow for the same simple check as in 2D
                    // ? Therefore, we can either solve this new problem as a 2D one or find a more elegant solution
                    // ? I will try to find the latter but will resort to the former if needed

                    // Determine the side normals for y and z, which we'll call yNormal and zNormal.
                    // This should just be the second and third column in the rotation matrix for A (rotA).

                    // I believe this should now work. I should test individual portions of function
                    // I'll have to test more stuff later

                    frontNormal = result.normal;
                    front = cube1.rb.pos * frontNormal + hA.x;
                    sideNormal1 = rotA.c2; // yNormal
                    sideNormal2 = rotA.c3; // zNormal
                    float ySide = cube1.rb.pos * sideNormal1;
                    float zSide = cube1.rb.pos * sideNormal2;

                    negSide1 = -ySide + hA.y; // negSideY
                    posSide1 = ySide + hA.y; // posSideY
                    negSide2 = -zSide + hA.z; // negSideZ
                    posSide2 = zSide + hA.z; // posSideZ

                    computeIncidentFace(incidentFace, hB, cube2.rb.pos, rotB, frontNormal);
                    break;
                }

                case FACE_A_Y: {
                    frontNormal = result.normal;
                    front = cube1.rb.pos * frontNormal + hA.y;
                    sideNormal1 = rotA.c1; // xNormal
                    sideNormal2 = rotA.c3; // zNormal
                    float xSide = cube1.rb.pos * sideNormal1;
                    float zSide = cube1.rb.pos * sideNormal2;

                    negSide1 = -xSide + hA.x; // negSideX
                    posSide1 = xSide + hA.x; // posSideX
                    negSide2 = -zSide + hA.z; // negSideZ
                    posSide2 = zSide + hA.z; // posSideZ

                    computeIncidentFace(incidentFace, hB, cube2.rb.pos, rotB, frontNormal);
                    break;
                }

                case FACE_A_Z: {
                    frontNormal = result.normal;
                    front = cube1.rb.pos * frontNormal + hA.z;
                    sideNormal1 = rotA.c1; // xNormal
                    sideNormal2 = rotA.c2; // yNormal
                    float xSide = cube1.rb.pos * sideNormal1;
                    float ySide = cube1.rb.pos * sideNormal2;

                    negSide1 = -xSide + hA.x; // negSideX
                    posSide1 = xSide + hA.x; // posSideX
                    negSide2 = -ySide + hA.y; // negSideY
                    posSide2 = ySide + hA.y; // posSideY

                    computeIncidentFace(incidentFace, hB, cube2.rb.pos, rotB, frontNormal);
                    break;
                }

                case FACE_B_X:{
                    frontNormal = result.normal;
                    front = cube2.rb.pos * frontNormal + hB.x;
                    sideNormal1 = rotB.c2; // yNormal
                    sideNormal2 = rotB.c3; // zNormal
                    float ySide = cube2.rb.pos * sideNormal1;
                    float zSide = cube2.rb.pos * sideNormal2;

                    negSide1 = -ySide + hB.y; // negSideY
                    posSide1 = ySide + hB.y; // posSideY
                    negSide2 = -zSide + hB.z; // negSideZ
                    posSide2 = zSide + hB.z; // posSideZ

                    computeIncidentFace(incidentFace, hA, cube1.rb.pos, rotA, frontNormal);
                    break;
                }

                case FACE_B_Y: {
                    frontNormal = result.normal;
                    front = cube2.rb.pos * frontNormal + hB.y;
                    sideNormal1 = rotB.c1; // xNormal
                    sideNormal2 = rotB.c3; // zNormal
                    float xSide = cube2.rb.pos * sideNormal1;
                    float zSide = cube2.rb.pos * sideNormal2;

                    negSide1 = -xSide + hB.x; // negSideX
                    posSide1 = xSide + hB.x; // posSideX
                    negSide2 = -zSide + hB.z; // negSideZ
                    posSide2 = zSide + hB.z; // posSideZ

                    computeIncidentFace(incidentFace, hA, cube1.rb.pos, rotA, frontNormal);
                    break;
                }

                case FACE_B_Z: {
                    frontNormal = result.normal;
                    front = cube2.rb.pos * frontNormal + hB.z;
                    sideNormal1 = rotB.c1; // xNormal
                    sideNormal2 = rotB.c2; // yNormal
                    float xSide = cube2.rb.pos * sideNormal1;
                    float ySide = cube2.rb.pos * sideNormal2;

                    negSide1 = -xSide + hB.x; // negSideX
                    posSide1 = xSide + hB.x; // posSideX
                    negSide2 = -ySide + hB.y; // negSideY
                    posSide2 = ySide + hB.y; // posSideY

                    computeIncidentFace(incidentFace, hA, cube1.rb.pos, rotA, frontNormal);
                    break;
                }
            }

            // * Clip the incident edge with box planes.

            // We need 4 sets of clip points for 3D as we have 2 negative and 2 positive sides to check.
            // todo we could probably make this just two sets that we alternate
            // todo do some math and make diagrams to ensure 
            ZMath::Vec3D clipPoints1[4];
            ZMath::Vec3D clipPoints2[4];
            ZMath::Vec3D clipPoints3[4];
            ZMath::Vec3D clipPoints4[4];

            // todo check to make sure the normals passed in are actually correct
            // ! ensure I do this in the proper order, too.
            // todo add in better comments when I understand this better to make sure I understand it well

            // ! unsure if I should do it with the incident face for both axes separately or chain together the axes

            // Clip with the incident face
            int np = clipSegmentToLine(clipPoints1, incidentFace, -sideNormal1, negSide1);

            if (np < 4) {
                result.hit = 0;
                return result;
            }

            // Clip with negSide1
            np = clipSegmentToLine(clipPoints2, clipPoints1, sideNormal1, posSide1);

            if (np < 4) {
                result.hit = 0;
                return result;
            }

            // Clip with the incident face
            np = clipSegmentToLine(clipPoints3, incidentFace, -sideNormal2, negSide2);

            if (np < 4) {
                result.hit = 0;
                return result;
            }

            // Clip with negSide2
            np = clipSegmentToLine(clipPoints4, clipPoints3, sideNormal2, posSide2);

            if (np < 4) {
                result.hit = 0;
                return result;
            }

            // ! unsure what to do from here

            result.hit = 1;
            return result;
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
