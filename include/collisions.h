#ifndef COLLISIONS_H
#define COLLISIONS_H

#include "intersections.h"

// We can use the normals for each as possible separation axes
// We have to account for certain edge cases when moving this to 3D

namespace Collisions {
    // * =========================
    // * Collision Manifolds
    // * =========================

    // ? Note: if we have objects A and B colliding, the collison normal will point towards B and away from A.

    // todo maybe make the contactPoints list an array of 4 as that's the max number
    // this would allow for more efficiency in cube v cube manifolds

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

            ZMath::Vec3D sphereDiff = sphere2.rb.pos - sphere1.rb.pos;
            float d = sphereDiff.mag(); // allows us to only take the sqrt once

            result.pDist = (sphere1.r + sphere2.r - d) * 0.5f;
            result.normal = sphereDiff * (1.0f/d);
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

            // The closest point to the sphere's center will be our contact point.
            // Therefore, we just set our contact point to closest.

            result.numPoints = 1;
            result.contactPoints = new ZMath::Vec3D[1];
            result.contactPoints[0] = closest;

            // determine the penetration distance and collision normal

            ZMath::Vec3D diff = closest - sphere.rb.pos;
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

            // the closest point to the sphere's center will be our contact point rotated back into global coordinates coordinates

            ZMath::rotateXY(closest, cube.rb.pos, cube.rb.theta);
            ZMath::rotateXZ(closest, cube.rb.pos, cube.rb.phi);

            result.numPoints = 1;
            result.contactPoints = new ZMath::Vec3D[1];
            result.contactPoints[0] = closest;

            // determine the penetration distance and the collision normal

            ZMath::Vec3D diff = closest - sphere.rb.pos;
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
            ZMath::Vec3D n = -(rot.transpose() * normal); // ! may not need to flip the sign since our normal points towards B from A
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
         * @brief Compute the clipping points given input points.
         * 
         * @param vOut Array which gets filled with the clipping points.
         * @param vIn Array containing the input points.
         * @param n1 Side normal 1.
         * @param n2 Side normal 2.
         * @param offset1 Distance to the side corresponding with side normal 1.
         * @param offset2 Distance to the side corresponding with side normal 2.
         * @return (int) The number of clipping points. If this does not return 4, there is not an intersection on this axis.
         */
        int clipSegmentToLine(ZMath::Vec3D vOut[4], ZMath::Vec3D vIn[4], const ZMath::Vec3D &n1, const ZMath::Vec3D &n2, float offset1, float offset2) {
            // begin with 0 output points
            int np = 0;

            // todo fs need to test this function extensively
            // todo may only need to compute the dist for [0] and [2] and use that for all the data
            // todo make sure the signs are correct

            // calculate the distance
            // first set of distances
            float d0 = n1 * vIn[0] - offset1;
            float d1 = n1 * vIn[1] - offset1;
            float d2 = n1 * vIn[2] - offset1;
            float d3 = n1 * vIn[3] - offset1;

            // second set of distances
            float d4 = n2 * vIn[0] - offset2;
            float d5 = n2 * vIn[1] - offset2;
            float d6 = n2 * vIn[2] - offset2;
            float d7 = n2 * vIn[3] - offset2;

            // todo print out all the distances to check them as many should be equivalent

            // * Compute the clipping points.

            // first input point
            // if the points are inside the reference cube, add them as clipping points
            if (d0 <= 0.0f && d4 <= 0.0f) { 
                vOut[np++] = vIn[0];

            // check if the vertices are separated by the edge of the reference cube
            } else if (d0 <= 0.0f && d4 * d6 < 0.0f) {
                vOut[np++] = vIn[0] + (vIn[2] - vIn[0]) * (d4/(d4 - d6));

            } else if (d4 <= 0.0f && d0 * d2 < 0.0f) {
                vOut[np++] = vIn[0] + (vIn[2] - vIn[0]) * (d0/(d0 - d2));

            } else if (d0 * d2 < 0.0f && d4 * d6 < 0.0f) {
                vOut[np++] = vIn[0] + (vIn[2] - vIn[0]) * ((d0/(d0 - d2)) + (d4/(d4 - d6)));
            }

            // second input point
            // if the points are inside the reference cube, add them as clipping points
            if (d1 <= 0.0f && d5 <= 0.0f) {
                vOut[np++] = vIn[1];
            
            // check if the vertices are separated by the edge of the reference cube
            } else if (d1 <= 0.0f && d5 * d7 < 0.0f) {
                vOut[np++] = vIn[1] + (vIn[3] - vIn[1]) * (d5/(d5 - d7));

            } else if (d5 <= 0.0f && d1 * d3 < 0.0f) {
                vOut[np++] = vIn[1] + (vIn[3] - vIn[1]) * (d1/(d1 - d3));

            } else if (d1 * d3 < 0.0f && d5 * d7 < 0.0f) {
                vOut[np++] = vIn[1] + (vIn[3] - vIn[1]) * ((d1/(d1 - d3)) + (d5/(d5 - d7)));
            }

            // third input point
            // if the points are inside the reference cube, add them as clipping points
            if (d2 <= 0.0f && d6 <= 0.0f) {
                vOut[np++] = vIn[2];
            
            // check if the vertices are separated by the edge of the reference cube
            } else if (d2 <= 0.0f && d4 * d6 < 0.0f) {
                vOut[np++] = vIn[0] + (vIn[2] - vIn[0]) * (d4/(d4 - d6));

            } else if (d6 <= 0.0f && d0 * d2 < 0.0f) {
                vOut[np++] = vIn[0] + (vIn[2] - vIn[0]) * (d0/(d0 - d2));

            } else if (d0 * d2 < 0.0f && d4 * d6 < 0.0f) {
                vOut[np++] = vIn[0] + (vIn[2] - vIn[0]) * ((d0/(d0 - d2)) + (d4/(d4 - d6)));
            }

            // fourth input point
            // if the points are inside the reference cube, add them as clipping points
            if (d3 <= 0.0f && d7 <= 0.0f) {
                vOut[np++] = vIn[3];
            
            // check if the vertices are separated by the edge of the reference cube
            } else if (d3 <= 0.0f && d5 * d7 < 0.0f) {
                vOut[np++] = vIn[1] + (vIn[3] - vIn[1]) * (d5/(d5 - d7));

            } else if (d7 <= 0.0f && d1 * d3 < 0.0f) {
                vOut[np++] = vIn[1] + (vIn[3] - vIn[1]) * (d1/(d1 - d3));

            } else if (d1 * d3 < 0.0f && d5 * d7 < 0.0f) {
                vOut[np++] = vIn[1] + (vIn[3] - vIn[1]) * ((d1/(d1 - d3)) + (d5/(d5 - d7)));
            }

            return np;
        };

        CollisionManifold findCollisionFeatures(Primitives::AABB const &aabb1, Primitives::AABB const &aabb2) {
            return {};
        };

        CollisionManifold findCollisionFeatures(Primitives::AABB const &aabb, Primitives::Cube const &cube) {
            return {};
        };

        // ? Normal points towards B and away from A

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
            
            // todo do the math to figure out if we actually need to do the abs
            // todo not sure what math to do for this though

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

            // * Find the best axis (i.e. the axis with the least amount of penetration).

            // Assume A's x-axis is the best axis first
            Axis axis = FACE_A_X;
            float separation = faceA.x;
            result.normal = dA.x > 0.0f ? rotA.c1 : -rotA.c1;

            // tolerance values
            float relativeTol = 0.95f;
            float absoluteTol = 0.01f;

            // ? check if there is another axis better than A's x axis by checking if the penetration along
            // ?  the current axis being checked is greater than that of the current penetration
            // ?  (as greater value = less negative = less penetration).

            // A's remaining axes
            if (faceA.y > relativeTol * separation + absoluteTol * hA.y) {
                axis = FACE_A_Y;
                separation = faceA.y;
                result.normal = dA.y > 0.0f ? rotA.c2 : -rotA.c2;
            }

            if (faceA.z > relativeTol * separation + absoluteTol * hA.z) {
                axis = FACE_A_Z;
                separation = faceA.z;
                result.normal = dA.z > 0.0f ? rotA.c3 : -rotA.c3;
            }

            // B's axes
            if (faceB.x > relativeTol * separation + absoluteTol * hB.x) {
                axis = FACE_B_X;
                separation = faceB.x;
                result.normal = dB.x > 0.0f ? rotB.c1 : -rotB.c1;
            }

            if (faceB.y > relativeTol * separation + absoluteTol * hB.y) {
                axis = FACE_B_Y;
                separation = faceB.y;
                result.normal = dB.y > 0.0f ? rotB.c2 : -rotB.c2;
            }

            if (faceB.z > relativeTol * separation + absoluteTol * hB.z) {
                axis = FACE_B_Z;
                separation = faceB.z;
                result.normal = dB.z > 0.0f ? rotB.c3 : -rotB.c3;
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

                case FACE_B_X: {
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

            ZMath::Vec3D clipPoints1[4];
            ZMath::Vec3D clipPoints2[4];

            // Clip to side 1
            int np = clipSegmentToLine(clipPoints1, incidentFace, -sideNormal1, -sideNormal2, negSide1, negSide2);

            if (np < 4) {
                result.hit = 0;
                return result;
            }

            // Clip to the negative side 1
            np = clipSegmentToLine(clipPoints2, clipPoints1, sideNormal1, sideNormal2, posSide1, posSide2);

            if (np < 4) {
                result.hit = 0;
                return result;
            }

            // * ClipPoints2 now contains the clipping points.
            // * Compute the contact points.
            
            // store the conatct points in here and add them to the dynamic array after they are determined
            ZMath::Vec3D contactPoints[4];
            np = 0;
            result.pDist = 0.0f;

            // todo check my work for this one. I'm not certain on it.

            for (int i = 0; i < 4; ++i) {
                separation = frontNormal * clipPoints2[i] - front;

                if (separation <= 0) {
                    contactPoints[np++] = clipPoints2[i] - frontNormal * separation;
                    float pDist = std::fabs(separation);
                    if (result.pDist < pDist) { result.pDist = pDist; }
                }
            }

            // Ensure there are contact points.
            // ! This check is probs unnecessary
            // ! Can do the math to check later
            if (!np) {
                result.hit = 0;
                return result;
            }

            // * update the manifold to contain the results.

            result.hit = 1;
            result.numPoints = np;
            result.contactPoints = new ZMath::Vec3D[np];

            for (int i = 0; i < np; ++i) { result.contactPoints[i] = contactPoints[i]; }
            
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
