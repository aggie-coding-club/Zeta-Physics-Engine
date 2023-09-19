#pragma once

#include "intersections.h"

// We can use the normals for each as possible separation axes
// We have to account for certain edge cases when moving this to 3D

namespace Zeta {
    // * =========================
    // * Collision Manifolds
    // * =========================

    // ? Note: if we have objects A and B colliding, the collison normal will point towards B and away from A.

    struct CollisionManifold {
        ZMath::Vec3D normal; // collision normal
        ZMath::Vec3D* contactPoints; // contact points of the collision
        float pDist; // penetration distance
        int numPoints; // number of contact points
        bool hit; // do they intersect
    };

    // * Enums used for denotating the edges of the cubes
    enum Axis {
        FACE_A_X,
        FACE_A_Y,
        FACE_A_Z,
        FACE_B_X,
        FACE_B_Y,
        FACE_B_Z
    };

    // * ===================================
    // * Collision Manifold Calculators
    // * ===================================

    // ? Normal points towards B away from A.

    // todo go through and replace direct assignment of vectors to using .set()

    static CollisionManifold findCollisionFeatures(Plane const &plane, Sphere const &sphere) {
        CollisionManifold result;

        ZMath::Vec3D closest = sphere.c - plane.pos;
        ZMath::Vec2D min = plane.getLocalMin(), max = plane.getLocalMax();

        // rotate the center of the sphere into the plane's local coords
        closest = plane.rot * closest + plane.pos;

        closest.x = ZMath::clamp(closest.x, min.x, max.x);
        closest.y = ZMath::clamp(closest.y, min.y, max.y);
        closest.z = plane.pos.z;

        result.hit = closest.distSq(sphere.c) <= sphere.r*sphere.r;

        if (!result.hit) { return result; }

        // The closest point to the sphere's center will be our contact point.
        // Therefore, we just set our contact point to closest.

        result.numPoints = 1;
        result.contactPoints = new ZMath::Vec3D[1];
        result.contactPoints[0] = closest;

        // determine the penetration distance and collision normal

        ZMath::Vec3D diff = closest - sphere.c;
        float d = diff.mag(); // allows us to only take the sqrt once
        result.pDist = sphere.r - d;
        result.normal = diff * (1.0f/d);

        return result;
    };


    // * ====================================================
    // * Helper Functions for 3D Box Collision Manifolds
    // * ====================================================

    /**
     * @brief Determine the 4 vertices comprising the incident face when the incident cube is an AABB.
     * 
     * @param v Array which gets filled with the 4 vertices making up the incident face.
     * @param h Halfsize of the incident AABB.
     * @param pos The position of the incident AABB.
     * @param normal The normal vector of the collision (points towards B away from A).
     */
    static void computeIncidentFaceAABB(ZMath::Vec3D v[4], const ZMath::Vec3D& h, const ZMath::Vec3D& pos, const ZMath::Vec3D& normal) {
        // Take the absolute value of the normal for comparisons.
        ZMath::Vec3D nAbs = ZMath::abs(normal);

        // Determine the vertices in terms of halfsize.
        // Vertex array starts in the bottom left corner when considering the face as a 2D box and goes around counterclockwise.
        if (nAbs.x > nAbs.y && nAbs.x > nAbs.z) { // x > y && x > z
            if (normal.x > 0.0f) { // incident cube is intersecting on its -x side
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
            if (normal.y > 0.0f) { // incident cube is intersecting on its -y side
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
            if (normal.z > 0.0f) { // incident cube is intersecting on its -z side
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

        // translate the vertices to their proper positions
        v[0] = pos + v[0];
        v[1] = pos + v[1];
        v[2] = pos + v[2];
        v[3] = pos + v[3];
    };

    /**
     * @brief Determine the 4 vertices making up the incident face.
     * 
     * @param v Array which gets filled with the 4 vertices comprising the incident face.
     * @param h Halfsize of the incident cube.
     * @param pos The position of the incident cube.
     * @param rot The rotation matrix of the incident cube.
     * @param normal The normal vector of the collision.
     */
    static void computeIncidentFace(ZMath::Vec3D v[4], const ZMath::Vec3D& h, const ZMath::Vec3D& pos, 
                                    const ZMath::Mat3D& rot, const ZMath::Vec3D& normal) {

        // Rotate the normal to the incident cube's local space.
        ZMath::Vec3D n = rot.transpose() * normal;
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
    static int clipSegmentToLine(ZMath::Vec3D vOut[4], ZMath::Vec3D vIn[4], const ZMath::Vec3D &n1, const ZMath::Vec3D &n2, float offset1, float offset2) {
        // begin with 0 output points
        int np = 0;

        // calculate the distance
        // first set of distances
        float d0 = n1 * vIn[0] - offset1;
        float d1 = n1 * vIn[1] - offset1;

        // second set of distances
        float d2 = n2 * vIn[0] - offset2;
        float d3 = n2 * vIn[3] - offset2;

        // * Compute the clipping points.
        // ? If the points are outside the reference cube's clipping plane (more or less inside the cube), add them as clipping points.
        // ? Otherwise, check if the vertices are separated by the edge of the reference cube used for this clipping plane.
        
        // cache the interpolation values for efficiency
        float i1 = (d0/(d1 + d0));
        float i2 = (d2/(d3 + d2));
        float i3 = (d1/(d1 + d0));
        float i4 = (d3/(d3 + d2));

        // first input point
        if (d0 <= 0.0f && d2 <= 0.0f) { vOut[np++] = vIn[0]; }
        else if (d0 * d1 < 0.0f && d2 * d3 < 0.0f) { vOut[np++] = vIn[0] + (vIn[1] - vIn[0]) * i1 + (vIn[3] - vIn[0]) * i2; }
        else if (d0 * d1 < 0.0f) { vOut[np++] = vIn[0] + (vIn[1] - vIn[0]) * i1; } 
        else if (d2 * d3 < 0.0f) { vOut[np++] = vIn[0] + (vIn[3] - vIn[0]) * i2; }

        // second input point
        if (d1 <= 0.0f && d2 <= 0.0f) { vOut[np++] = vIn[1]; }
        else if (d0 * d1 < 0.0f && d2 * d3 < 0.0f) { vOut[np++] = vIn[1] + (vIn[0] - vIn[1]) * i3 + (vIn[2] - vIn[1]) * i2; }
        else if (d0 * d1 < 0.0f) { vOut[np++] = vIn[1] + (vIn[0] - vIn[1]) * i3; }
        else if (d2 * d3 < 0.0f) { vOut[np++] = vIn[1] + (vIn[2] - vIn[1]) * i2; }

        // third input point
        if (d1 <= 0.0f && d3 <= 0.0f) { vOut[np++] = vIn[2]; }
        else if (d0 * d1 < 0.0f && d2 * d3 < 0.0f) { vOut[np++] = vIn[2] + (vIn[3] - vIn[2]) * i3 + (vIn[1] - vIn[2]) * i4; }
        else if (d0 * d1 < 0.0f) { vOut[np++] = vIn[2] + (vIn[3] - vIn[2]) * i3; }
        else if (d2 * d3 < 0.0f) { vOut[np++] = vIn[2] + (vIn[1] - vIn[2]) * i4; }

        // fourth input point
        if (d0 <= 0.0f && d3 <= 0.0f) { vOut[np++] = vIn[3]; }
        else if (d0 * d1 < 0.0f && d2 * d3 < 0.0f) { vOut[np++] = vIn[3] + (vIn[2] - vIn[3]) * i1 + (vIn[0] - vIn[3]) * i4; }
        else if (d0 * d1 < 0.0f) { vOut[np++] = vIn[3] + (vIn[2] - vIn[3]) * i1; }
        else if (d2 * d3 < 0.0f) { vOut[np++] = vIn[3] + (vIn[0] - vIn[3]) * i4; }

        return np;
    };


    static CollisionManifold findCollisionFeatures(Plane const &plane, AABB const &aabb) {
        CollisionManifold result;

        // halfsize of the plane (A) and aabb (B)
        ZMath::Vec2D planeH = plane.getHalfSize();
        ZMath::Vec3D hA(planeH.x, planeH.y, 0.0f), hB = aabb.getHalfSize();

        // * Determine the rotation matrices of A and B

        // rotate anything from global space (B's local space) to A's local space
        ZMath::Mat3D rotAT = plane.rot.transpose();

        // determine the difference between the positions
        // Note: global space is the AABB's local space
        ZMath::Vec3D dB = aabb.pos - plane.pos;
        ZMath::Vec3D dA = rotAT * dB;

        // * Check for intersections with the separating axis theorem

        // amount of penetration along A's axes
        ZMath::Vec3D faceA = ZMath::abs(dA) - hA - rotAT * hB;
        if (faceA.x > 0 || faceA.y > 0 || faceA.z > 0) {
            result.hit = 0;
            return result;
        }

        // amount of penetration along B's axes
        ZMath::Vec3D faceB = ZMath::abs(dB) - hB - plane.rot * hA;
        if (faceB.x > 0 || faceB.y > 0 || faceB.z > 0) {
            result.hit = 0;
            return result;
        }

        // * Find the best axis (i.e. the axis with the least penetration)

        // Assume A's x-axis is the best axis first
        Axis axis = FACE_A_X;
        float separation = faceA.x;
        result.normal = dA.x > 0.0f ? plane.rot.c1 : -plane.rot.c1;

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
            result.normal = dA.y > 0.0f ? plane.rot.c2 : -plane.rot.c2;
        }

        if (faceA.z > relativeTol * separation + absoluteTol * hA.z) {
            axis = FACE_A_Z;
            separation = faceA.z;
            result.normal = dA.z > 0.0f ? plane.rot.c3 : -plane.rot.c3;
        }

        // B's axes
        if (faceB.x > relativeTol * separation + absoluteTol * hB.x) {
            axis = FACE_B_X;
            separation = faceB.x;
            result.normal = dB.x > 0.0f ? ZMath::Vec3D(1, 0, 0) : ZMath::Vec3D(-1, 0, 0);
        }

        if (faceB.y > relativeTol * separation + absoluteTol * hB.y) {
            axis = FACE_B_Y;
            separation = faceB.y;
            result.normal = dB.y > 0.0f ? ZMath::Vec3D(0, 1, 0) : ZMath::Vec3D(0, -1, 0);
        }

        if (faceB.z > relativeTol * separation + absoluteTol * hB.z) {
            axis = FACE_B_Z;
            separation = faceB.z;
            result.normal = dB.z > 0.0f ? ZMath::Vec3D(0, 0, 1) : ZMath::Vec3D(0, 0, -1);
        }

        // * Setup clipping plane data based on the best axis

        ZMath::Vec3D sideNormal1, sideNormal2;
        ZMath::Vec3D incidentFace[4]; // 4 vertices for the collision in 3D
        float front, negSide1, negSide2, posSide1, posSide2;

        // * Compute the clipping lines and line segment to be clipped

        // todo check my work for this tomorrow or after a couple days

        switch(axis) {
            case FACE_A_X: {
                front = plane.pos * result.normal + hA.x;
                sideNormal1 = plane.rot.c2; // yNormal
                sideNormal2 = plane.rot.c3; // zNormal
                float ySide = plane.pos * sideNormal1;
                float zSide = plane.pos * sideNormal2;

                negSide1 = -ySide + hA.y; // negSideY
                posSide1 = ySide + hA.y; // posSideY
                negSide2 = -zSide + hA.z; // negSideZ
                posSide2 = zSide + hA.z; // posSideZ

                computeIncidentFaceAABB(incidentFace, hB, aabb.pos, result.normal);
                break;
            }

            case FACE_A_Y: {
                front = plane.pos * result.normal + hA.y;
                sideNormal1 = plane.rot.c1; // xNormal
                sideNormal2 = plane.rot.c3; // zNormal
                float xSide = plane.pos * sideNormal1;
                float zSide = plane.pos * sideNormal2;

                negSide1 = -xSide + hA.x; // negSideX
                posSide1 = xSide + hA.x; // posSideX
                negSide2 = -zSide + hA.z; // negSideZ
                posSide2 = zSide + hA.z; // posSideZ

                computeIncidentFaceAABB(incidentFace, hB, aabb.pos, result.normal);
                break;
            }

            case FACE_A_Z: {
                front = plane.pos * result.normal + hA.z;
                sideNormal1 = plane.rot.c1; // xNormal
                sideNormal2 = plane.rot.c2; // yNormal
                float xSide = plane.pos * sideNormal1;
                float ySide = plane.pos * sideNormal2;

                negSide1 = -xSide + hA.x; // negSideX
                posSide1 = xSide + hA.x; // posSideX
                negSide2 = -ySide + hA.y; // negSideY
                posSide2 = ySide + hA.y; // posSideY

                computeIncidentFaceAABB(incidentFace, hB, aabb.pos, result.normal);
                break;
            }

            case FACE_B_X: {
                front = aabb.pos * result.normal + hB.x;
                sideNormal1.set(0, 1, 0); // yNormal
                sideNormal2.set(0, 0, 1); // zNormal
                float ySide = aabb.pos * sideNormal1;
                float zSide = aabb.pos * sideNormal2;

                negSide1 = -ySide + hB.y; // negSideY
                posSide1 = ySide + hB.y; // posSideY
                negSide2 = -zSide + hB.z; // negSideZ
                posSide2 = zSide + hB.z; // posSideZ

                // ? We know when the plane serves as the incident face, it must be the plane's only 3D face.
                // ? In other words, we take the plane's 4 vertices.

                // Determine the incident face in terms of halfSize. hA.z will always be 0.
                incidentFace[0].set(-hA.x, -hA.y, 0.0f);
                incidentFace[1].set(-hA.x, hA.y, 0.0f);
                incidentFace[2].set(hA.x, hA.y, 0.0f);
                incidentFace[3].set(hA.x, -hA.y, 0.0f);

                // Rotate the incident face into global coordinates
                incidentFace[0].set(plane.pos + plane.rot * incidentFace[0]);
                incidentFace[1].set(plane.pos + plane.rot * incidentFace[1]);
                incidentFace[2].set(plane.pos + plane.rot * incidentFace[2]);
                incidentFace[3].set(plane.pos + plane.rot * incidentFace[3]);

                break;
            }

            case FACE_B_Y: {
                front = aabb.pos * result.normal + hB.y;
                sideNormal1.set(1, 0, 0); // xNormal
                sideNormal2.set(0, 0, 1); // zNormal
                float xSide = aabb.pos * sideNormal1;
                float zSide = aabb.pos * sideNormal2;

                negSide1 = -xSide + hB.x; // negSideX
                posSide1 = xSide + hB.x; // posSideX
                negSide2 = -zSide + hB.z; // negSideZ
                posSide2 = zSide + hB.z; // posSideZ

                // ? We know when the plane serves as the incident face, it must be the plane's only 3D face.
                // ? In other words, we take the plane's 4 vertices.

                // Determine the incident face in terms of halfSize. hA.z will always be 0.
                incidentFace[0].set(-hA.x, -hA.y, 0.0f);
                incidentFace[1].set(-hA.x, hA.y, 0.0f);
                incidentFace[2].set(hA.x, hA.y, 0.0f);
                incidentFace[3].set(hA.x, -hA.y, 0.0f);

                // Rotate the incident face into global coordinates
                incidentFace[0].set(plane.pos + plane.rot * incidentFace[0]);
                incidentFace[1].set(plane.pos + plane.rot * incidentFace[1]);
                incidentFace[2].set(plane.pos + plane.rot * incidentFace[2]);
                incidentFace[3].set(plane.pos + plane.rot * incidentFace[3]);

                break;
            }

            case FACE_B_Z: {
                front = aabb.pos * result.normal + hB.z;
                sideNormal1.set(1, 0, 0); // xNormal
                sideNormal2.set(0, 1, 0); // yNormal
                float xSide = aabb.pos * sideNormal1;
                float ySide = aabb.pos * sideNormal2;

                negSide1 = -xSide + hB.x; // negSideX
                posSide1 = xSide + hB.x; // posSideX
                negSide2 = -ySide + hB.y; // negSideY
                posSide2 = ySide + hB.y; // posSideY

                // ? We know when the plane serves as the incident face, it must be the plane's only 3D face.
                // ? In other words, we take the plane's 4 vertices.

                // Determine the incident face in terms of halfSize. hA.z will always be 0.
                incidentFace[0].set(-hA.x, -hA.y, 0.0f);
                incidentFace[1].set(-hA.x, hA.y, 0.0f);
                incidentFace[2].set(hA.x, hA.y, 0.0f);
                incidentFace[3].set(hA.x, -hA.y, 0.0f);

                // Rotate the incident face into global coordinates
                incidentFace[0].set(plane.pos + plane.rot * incidentFace[0]);
                incidentFace[1].set(plane.pos + plane.rot * incidentFace[1]);
                incidentFace[2].set(plane.pos + plane.rot * incidentFace[2]);
                incidentFace[3].set(plane.pos + plane.rot * incidentFace[3]);

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

        for (int i = 0; i < 4; ++i) {
            separation = result.normal * clipPoints2[i] - front;

            if (separation <= 0) {
                contactPoints[np++] = clipPoints2[i] - result.normal * separation;
                if (result.pDist < separation) { result.pDist = separation; }
            }
        }

        // * update the manifold to contain the results.

        result.pDist = -result.pDist;
        result.hit = 1;
        result.numPoints = np;
        result.contactPoints = new ZMath::Vec3D[np];

        for (int i = 0; i < np; ++i) { result.contactPoints[i] = contactPoints[i]; }
        
        return result;
    };

    static CollisionManifold findCollisionFeatures(Plane const &plane, Cube const &cube) {
        CollisionManifold result;

        // halfsize of the plane (A) and cube (B)
        ZMath::Vec2D planeH = plane.getHalfSize();
        ZMath::Vec3D hA(planeH.x, planeH.y, 0.0f), hB = cube.getHalfSize();

        // * Determine the rotation matrices of A and B

        // rotate anything from global space to A's local space
        ZMath::Mat3D rotAT = plane.rot.transpose();

        // rotate anything from global space to B's local space
        ZMath::Mat3D rotBT = cube.rot.transpose();

        // determine the difference between the positions
        ZMath::Vec3D dP = cube.pos - plane.pos;
        ZMath::Vec3D dA = rotAT * dP;
        ZMath::Vec3D dB = rotBT * dP;

        // * Rotation matrices for switching between local spaces

        // ! When scenes are developed test if we actually need the absolute value

        // Rotate anything from B's local space into A's
        ZMath::Mat3D C = ZMath::abs(rotAT * cube.rot);

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
        ZMath::Vec3D faceB = ZMath::abs(dB) - hB - CT * hA;
        if (faceB.x > 0 || faceB.y > 0 || faceB.z > 0) {
            result.hit = 0;
            return result;
        }

        // * Find the best axis (i.e. the axis with the least penetration)

        // Assume A's x-axis is the best axis first
        Axis axis = FACE_A_X;
        float separation = faceA.x;
        result.normal = dA.x > 0.0f ? plane.rot.c1 : -plane.rot.c1;

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
            result.normal = dA.y > 0.0f ? plane.rot.c2 : -plane.rot.c2;
        }

        if (faceA.z > relativeTol * separation + absoluteTol * hA.z) {
            axis = FACE_A_Z;
            separation = faceA.z;
            result.normal = dA.z > 0.0f ? plane.rot.c3 : -plane.rot.c3;
        }

        // B's axes
        if (faceB.x > relativeTol * separation + absoluteTol * hB.x) {
            axis = FACE_B_X;
            separation = faceB.x;
            result.normal = dB.x > 0.0f ? cube.rot.c1 : -cube.rot.c1;
        }

        if (faceB.y > relativeTol * separation + absoluteTol * hB.y) {
            axis = FACE_B_Y;
            separation = faceB.y;
            result.normal = dB.y > 0.0f ? cube.rot.c2 : -cube.rot.c2;
        }

        if (faceB.z > relativeTol * separation + absoluteTol * hB.z) {
            axis = FACE_B_Z;
            separation = faceB.z;
            result.normal = dB.z > 0.0f ? cube.rot.c3 : -cube.rot.c3;
        }

        // * Setup clipping plane data based on the best axis

        ZMath::Vec3D sideNormal1, sideNormal2;
        ZMath::Vec3D incidentFace[4]; // 4 vertices for the collision in 3D
        float front, negSide1, negSide2, posSide1, posSide2;

        // * Compute the clipping lines and line segment to be clipped

        // todo check my work for this tomorrow or after a couple days

        switch(axis) {
            case FACE_A_X: {
                front = plane.pos * result.normal + hA.x;
                sideNormal1 = plane.rot.c2; // yNormal
                sideNormal2 = plane.rot.c3; // zNormal
                float ySide = plane.pos * sideNormal1;
                float zSide = plane.pos * sideNormal2;

                negSide1 = -ySide + hA.y; // negSideY
                posSide1 = ySide + hA.y; // posSideY
                negSide2 = -zSide + hA.z; // negSideZ
                posSide2 = zSide + hA.z; // posSideZ

                computeIncidentFace(incidentFace, hB, cube.pos, cube.rot, result.normal);
                break;
            }

            case FACE_A_Y: {
                front = plane.pos * result.normal + hA.y;
                sideNormal1 = plane.rot.c1; // xNormal
                sideNormal2 = plane.rot.c3; // zNormal
                float xSide = plane.pos * sideNormal1;
                float zSide = plane.pos * sideNormal2;

                negSide1 = -xSide + hA.x; // negSideX
                posSide1 = xSide + hA.x; // posSideX
                negSide2 = -zSide + hA.z; // negSideZ
                posSide2 = zSide + hA.z; // posSideZ

                computeIncidentFace(incidentFace, hB, cube.pos, cube.rot, result.normal);
                break;
            }

            case FACE_A_Z: {
                front = plane.pos * result.normal + hA.z;
                sideNormal1 = plane.rot.c1; // xNormal
                sideNormal2 = plane.rot.c2; // yNormal
                float xSide = plane.pos * sideNormal1;
                float ySide = plane.pos * sideNormal2;

                negSide1 = -xSide + hA.x; // negSideX
                posSide1 = xSide + hA.x; // posSideX
                negSide2 = -ySide + hA.y; // negSideY
                posSide2 = ySide + hA.y; // posSideY

                computeIncidentFace(incidentFace, hB, cube.pos, cube.rot, result.normal);
                break;
            }

            case FACE_B_X: {
                front = cube.pos * result.normal + hB.x;
                sideNormal1 = cube.rot.c2; // yNormal
                sideNormal2 = cube.rot.c3; // zNormal
                float ySide = cube.pos * sideNormal1;
                float zSide = cube.pos * sideNormal2;

                negSide1 = -ySide + hB.y; // negSideY
                posSide1 = ySide + hB.y; // posSideY
                negSide2 = -zSide + hB.z; // negSideZ
                posSide2 = zSide + hB.z; // posSideZ

                // ? We know when the plane serves as the incident face, it must be the plane's only 3D face.
                // ? In other words, we take the plane's 4 vertices.

                // Determine the incident face in terms of halfSize. hA.z will always be 0.
                incidentFace[0].set(-hA.x, -hA.y, 0.0f);
                incidentFace[1].set(-hA.x, hA.y, 0.0f);
                incidentFace[2].set(hA.x, hA.y, 0.0f);
                incidentFace[3].set(hA.x, -hA.y, 0.0f);

                // Rotate the incident face into global coordinates
                incidentFace[0].set(plane.pos + plane.rot * incidentFace[0]);
                incidentFace[1].set(plane.pos + plane.rot * incidentFace[1]);
                incidentFace[2].set(plane.pos + plane.rot * incidentFace[2]);
                incidentFace[3].set(plane.pos + plane.rot * incidentFace[3]);

                break;
            }

            case FACE_B_Y: {
                front = cube.pos * result.normal + hB.y;
                sideNormal1 = cube.rot.c1; // xNormal
                sideNormal2 = cube.rot.c3; // zNormal
                float xSide = cube.pos * sideNormal1;
                float zSide = cube.pos * sideNormal2;

                negSide1 = -xSide + hB.x; // negSideX
                posSide1 = xSide + hB.x; // posSideX
                negSide2 = -zSide + hB.z; // negSideZ
                posSide2 = zSide + hB.z; // posSideZ

                // ? We know when the plane serves as the incident face, it must be the plane's only 3D face.
                // ? In other words, we take the plane's 4 vertices.

                // Determine the incident face in terms of halfSize. hA.z will always be 0.
                incidentFace[0].set(-hA.x, -hA.y, 0.0f);
                incidentFace[1].set(-hA.x, hA.y, 0.0f);
                incidentFace[2].set(hA.x, hA.y, 0.0f);
                incidentFace[3].set(hA.x, -hA.y, 0.0f);

                // Rotate the incident face into global coordinates
                incidentFace[0].set(plane.pos + plane.rot * incidentFace[0]);
                incidentFace[1].set(plane.pos + plane.rot * incidentFace[1]);
                incidentFace[2].set(plane.pos + plane.rot * incidentFace[2]);
                incidentFace[3].set(plane.pos + plane.rot * incidentFace[3]);

                break;
            }

            case FACE_B_Z: {
                front = cube.pos * result.normal + hB.z;
                sideNormal1 = cube.rot.c1; // xNormal
                sideNormal2 = cube.rot.c2; // yNormal
                float xSide = cube.pos * sideNormal1;
                float ySide = cube.pos * sideNormal2;

                negSide1 = -xSide + hB.x; // negSideX
                posSide1 = xSide + hB.x; // posSideX
                negSide2 = -ySide + hB.y; // negSideY
                posSide2 = ySide + hB.y; // posSideY

                // ? We know when the plane serves as the incident face, it must be the plane's only 3D face.
                // ? In other words, we take the plane's 4 vertices.

                // Determine the incident face in terms of halfSize. hA.z will always be 0.
                incidentFace[0].set(-hA.x, -hA.y, 0.0f);
                incidentFace[1].set(-hA.x, hA.y, 0.0f);
                incidentFace[2].set(hA.x, hA.y, 0.0f);
                incidentFace[3].set(hA.x, -hA.y, 0.0f);

                // Rotate the incident face into global coordinates
                incidentFace[0].set(plane.pos + plane.rot * incidentFace[0]);
                incidentFace[1].set(plane.pos + plane.rot * incidentFace[1]);
                incidentFace[2].set(plane.pos + plane.rot * incidentFace[2]);
                incidentFace[3].set(plane.pos + plane.rot * incidentFace[3]);

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

        for (int i = 0; i < 4; ++i) {
            separation = result.normal * clipPoints2[i] - front;

            if (separation <= 0) {
                contactPoints[np++] = clipPoints2[i] - result.normal * separation;
                if (result.pDist < separation) { result.pDist = separation; }
            }
        }

        // * update the manifold to contain the results.

        result.pDist = -result.pDist;
        result.hit = 1;
        result.numPoints = np;
        result.contactPoints = new ZMath::Vec3D[np];

        for (int i = 0; i < np; ++i) { result.contactPoints[i] = contactPoints[i]; }
        
        return result;
    };

    static CollisionManifold findCollisionFeatures(Sphere const &sphere1, Sphere const &sphere2) {
        CollisionManifold result;

        float r = sphere1.r + sphere2.r;
        ZMath::Vec3D sphereDiff = sphere2.c - sphere1.c;

        result.hit = sphereDiff.magSq() <= r*r;

        if (!result.hit) { return result; }
        
        float d = sphereDiff.mag(); // allows us to only take the sqrt once

        result.pDist = (sphere1.r + sphere2.r - d) * 0.5f;
        result.normal = sphereDiff * (1.0f/d);

        // determine the contact point
        result.numPoints = 1;
        result.contactPoints = new ZMath::Vec3D[result.numPoints];
        result.contactPoints[0] = sphere1.c + (result.normal * (sphere1.r - result.pDist));

        return result;
    };

    static CollisionManifold findCollisionFeatures(Sphere const &sphere, AABB const &aabb) {
        CollisionManifold result;

        // ? We know a sphere and AABB would intersect if the distance from the closest point to the center on the AABB
        // ?  from the center is less than or equal to the radius of the sphere.
        // ? We can determine the closet point by clamping the value of the sphere's center between the min and max of the AABB.
        // ? From here, we can check the distance from this point to the sphere's center.

        ZMath::Vec3D closest = sphere.c;
        ZMath::Vec3D min = aabb.getMin(), max = aabb.getMax();

        closest = ZMath::clamp(closest, min, max);
        result.hit = closest.distSq(sphere.c) <= sphere.r*sphere.r;

        if (!result.hit) { return result; }

        // The closest point to the sphere's center will be our contact point.
        // Therefore, we just set our contact point to closest.

        result.numPoints = 1;
        result.contactPoints = new ZMath::Vec3D[1];
        result.contactPoints[0] = closest;

        // determine the penetration distance and collision normal

        ZMath::Vec3D diff = closest - sphere.c;
        float d = diff.mag(); // allows us to only take the sqrt once
        result.pDist = sphere.r - d;
        result.normal = diff * (1.0f/d);

        return result;
    };

    static CollisionManifold findCollisionFeatures(Sphere const &sphere, Cube const &cube) {
        CollisionManifold result;

        ZMath::Vec3D closest = sphere.c - cube.pos;
        ZMath::Vec3D min = cube.getLocalMin(), max = cube.getLocalMax();

        // rotate the center of the sphere into the UVW coordinates of our cube
        closest = cube.rot * closest + cube.pos;
        
        // perform the check as if it was an AABB vs Sphere
        closest = ZMath::clamp(closest, min, max);
        result.hit = closest.distSq(sphere.c) <= sphere.r*sphere.r;

        if (!result.hit) { return result; }

        // the closest point to the sphere's center will be our contact point rotated back into global coordinates coordinates

        closest -= cube.pos;
        closest = cube.rot.transpose() * closest + cube.pos;

        result.numPoints = 1;
        result.contactPoints = new ZMath::Vec3D[1];
        result.contactPoints[0] = closest;

        // determine the penetration distance and the collision normal

        ZMath::Vec3D diff = closest - sphere.c;
        float d = diff.mag(); // allows us to only take the sqrt once
        result.pDist = sphere.r - d;
        result.normal = diff * (1.0f/d);

        return result;
    };

    // ? Normal points towards B and away from A

    static CollisionManifold findCollisionFeatures(AABB const &aabb1, AABB const &aabb2) {
        CollisionManifold result;

        // half size of AABB a and b respectively
        ZMath::Vec3D hA = aabb1.getHalfSize(), hB = aabb2.getHalfSize();

        // * Check for intersections using the separating axis theorem.
        // because both are axis aligned, global space is the same as the local space of both AABBs.

        // distance between the two
        ZMath::Vec3D dP = aabb2.pos - aabb1.pos;
        ZMath::Vec3D absDP = ZMath::abs(dP);

        // penetration along A's (and B's) axes
        ZMath::Vec3D faceA = absDP - hA - hB;
        if (faceA.x > 0 || faceA.y > 0 || faceA.z > 0) {
            result.hit = 0;
            return result;
        }

        // ? Since they are axis aligned, the penetration between the two will be the same on any given axis.
        // ?  Therefore, we only need to check for A.

        // * Find the best axis (i.e. the axis with the least amount of penetration).

        // Assume A's x-axis is the best axis first
        Axis axis = FACE_A_X;
        float separation = faceA.x;
        result.normal = dP.x > 0.0f ? ZMath::Vec3D(1, 0, 0) : ZMath::Vec3D(-1, 0, 0);

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
            result.normal = dP.y > 0.0f ? ZMath::Vec3D(0, 1, 0) : ZMath::Vec3D(0, -1, 0);
        }

        if (faceA.z > relativeTol * separation + absoluteTol * hA.z) {
            axis = FACE_A_Z;
            separation = faceA.z;
            result.normal = dP.z > 0.0f ? ZMath::Vec3D(0, 0, 1) : ZMath::Vec3D(0, 0, -1);
        }

        // * Setup clipping plane data based on the best axis

        ZMath::Vec3D sideNormal1, sideNormal2;
        ZMath::Vec3D incidentFace[4]; // 4 vertices for the collision in 3D
        float front, negSide1, negSide2, posSide1, posSide2;

        // * Compute the clipping lines and line segment to be clipped

        switch(axis) {
            case FACE_A_X: {
                front = aabb1.pos * result.normal + hA.x;
                sideNormal1 = ZMath::Vec3D(0, 1, 0); // yNormal
                sideNormal2 = ZMath::Vec3D(0, 0, 1); // zNormal

                negSide1 = aabb1.pos.y - hA.y; // negSideY
                posSide1 = aabb1.pos.y + hA.y; // posSideY
                negSide2 = aabb1.pos.z - hA.z; // negSideZ
                posSide2 = aabb1.pos.z + hA.z; // posSideZ

                computeIncidentFaceAABB(incidentFace, hB, aabb2.pos, result.normal);
                break;
            }

            case FACE_A_Y: {
                front = aabb1.pos * result.normal + hA.y;
                sideNormal1 = ZMath::Vec3D(1, 0, 0); // xNormal
                sideNormal2 = ZMath::Vec3D(0, 0, 1); // zNormal

                negSide1 = aabb1.pos.x - hA.x; // negSideX
                posSide1 = aabb1.pos.x + hA.x; // posSideX
                negSide2 = aabb1.pos.z - hA.z; // negSideZ
                posSide2 = aabb1.pos.z + hA.z; // posSideZ

                computeIncidentFaceAABB(incidentFace, hB, aabb2.pos, result.normal);
                break;
            }

            case FACE_A_Z: {
                front = aabb1.pos * result.normal + hA.z;
                sideNormal1 = ZMath::Vec3D(1, 0, 0); // xNormal
                sideNormal2 = ZMath::Vec3D(0, 1, 0); // yNormal

                negSide1 = aabb1.pos.x - hA.x; // negSideX
                posSide1 = aabb1.pos.x + hA.x; // posSideX
                negSide2 = aabb1.pos.y - hA.y; // negSideY
                posSide2 = aabb1.pos.y + hA.y; // posSideY

                computeIncidentFaceAABB(incidentFace, hB, aabb2.pos, result.normal);
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

        for (int i = 0; i < 4; ++i) {
            separation = result.normal * clipPoints2[i] - front;

            if (separation <= 0) {
                contactPoints[np++] = clipPoints2[i] - result.normal * separation;
                if (result.pDist < separation) { result.pDist = separation; }
            }
        }

        // * update the manifold to contain the results.

        result.pDist = -result.pDist;
        result.hit = 1;
        result.numPoints = np;
        result.contactPoints = new ZMath::Vec3D[np];

        for (int i = 0; i < np; ++i) { result.contactPoints[i] = contactPoints[i]; }
        
        return result;
    };

    // ? Normal points towards B and away from A

    static CollisionManifold findCollisionFeatures(AABB const &aabb, Cube const &cube) {
        CollisionManifold result;

        // half size of a and b respectively
        ZMath::Vec3D hA = aabb.getHalfSize(), hB = cube.getHalfSize();

        // * determine the rotation matrices of A and B
        // * global space is A's local space so we do not need a rotation matrix for it

        // rotate anything from gobal space to B's local space
        ZMath::Mat3D rotBT = cube.rot.transpose();

        // determine the difference between the positions
        ZMath::Vec3D dA = cube.pos - aabb.pos;
        ZMath::Vec3D dB = rotBT * dA;

        // * Check for intersections with the separating axis theorem

        // amount of penetration along A's axes
        ZMath::Vec3D faceA = ZMath::abs(dA) - hA - hB;
        if (faceA.x > 0 || faceA.y > 0 || faceA.z > 0) {
            result.hit = 0;
            return result;
        }

        // amount of penetration along B's axes
        ZMath::Vec3D faceB = ZMath::abs(dB) - hB - rotBT * hA;
        if (faceB.x > 0 || faceB.y > 0 || faceB.z > 0) {
            result.hit = 0;
            return result;
        }

        // * Find the best axis (i.e. the axis with the least amount of penetration).

        // Assume A's x-axis is the best axis first
        Axis axis = FACE_A_X;
        float separation = faceA.x;
        result.normal = dA.x > 0.0f ? ZMath::Vec3D(1, 0, 0) : ZMath::Vec3D(-1, 0, 0);

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
            result.normal = dA.y > 0.0f ? ZMath::Vec3D(0, 1, 0) : ZMath::Vec3D(0, -1, 0);
        }

        if (faceA.z > relativeTol * separation + absoluteTol * hA.z) {
            axis = FACE_A_Z;
            separation = faceA.z;
            result.normal = dA.z > 0.0f ? ZMath::Vec3D(0, 0, 1) : ZMath::Vec3D(0, 0, -1);
        }

        // B's axes
        if (faceB.x > relativeTol * separation + absoluteTol * hB.x) {
            axis = FACE_B_X;
            separation = faceB.x;
            result.normal = dB.x > 0.0f ? cube.rot.c1 : -cube.rot.c1;
        }

        if (faceB.y > relativeTol * separation + absoluteTol * hB.y) {
            axis = FACE_B_Y;
            separation = faceB.y;
            result.normal = dB.y > 0.0f ? cube.rot.c2 : -cube.rot.c2;
        }

        if (faceB.z > relativeTol * separation + absoluteTol * hB.z) {
            axis = FACE_B_Z;
            separation = faceB.z;
            result.normal = dB.z > 0.0f ? cube.rot.c3 : -cube.rot.c3;
        }

        // * Setup clipping plane data based on the best axis

        ZMath::Vec3D sideNormal1, sideNormal2;
        ZMath::Vec3D incidentFace[4]; // 4 vertices for the collision in 3D
        float front, negSide1, negSide2, posSide1, posSide2;

        // * Compute the clipping lines and line segment to be clipped

        switch(axis) {
            case FACE_A_X: {
                front = aabb.pos * result.normal + hA.x;
                sideNormal1 = ZMath::Vec3D(0, 1, 0); // yNormal
                sideNormal2 = ZMath::Vec3D(0, 0, 1); // zNormal

                negSide1 = aabb.pos.y - hA.y; // negSideY
                posSide1 = aabb.pos.y + hA.y; // posSideY
                negSide2 = aabb.pos.z - hA.z; // negSideZ
                posSide2 = aabb.pos.z + hA.z; // posSideZ

                computeIncidentFace(incidentFace, hB, cube.pos, cube.rot, result.normal);
                break;
            }

            case FACE_A_Y: {
                front = aabb.pos * result.normal + hA.y;
                sideNormal1 = ZMath::Vec3D(1, 0, 0); // xNormal
                sideNormal2 = ZMath::Vec3D(0, 0, 1); // zNormal

                negSide1 = aabb.pos.x - hA.x; // negSideX
                posSide1 = aabb.pos.x + hA.x; // posSideX
                negSide2 = aabb.pos.z - hA.z; // negSideZ
                posSide2 = aabb.pos.z + hA.z; // posSideZ

                computeIncidentFace(incidentFace, hB, cube.pos, cube.rot, result.normal);
                break;
            }

            case FACE_A_Z: {
                front = aabb.pos * result.normal + hA.z;
                sideNormal1 = ZMath::Vec3D(1, 0, 0); // xNormal
                sideNormal2 = ZMath::Vec3D(0, 1, 0); // yNormal

                negSide1 = aabb.pos.x - hA.x; // negSideX
                posSide1 = aabb.pos.x + hA.x; // posSideX
                negSide2 = aabb.pos.y - hA.y; // negSideY
                posSide2 = aabb.pos.y + hA.y; // posSideY

                computeIncidentFace(incidentFace, hB, cube.pos, cube.rot, result.normal);
                break;
            }

            case FACE_B_X: {
                front = cube.pos * result.normal + hB.x;
                sideNormal1 = cube.rot.c2; // yNormal
                sideNormal2 = cube.rot.c3; // zNormal
                float ySide = cube.pos * sideNormal1;
                float zSide = cube.pos * sideNormal2;

                negSide1 = -ySide + hB.y; // negSideY
                posSide1 = ySide + hB.y; // posSideY
                negSide2 = -zSide + hB.z; // negSideZ
                posSide2 = zSide + hB.z; // posSideZ

                computeIncidentFaceAABB(incidentFace, hA, aabb.pos, result.normal);
                break;
            }

            case FACE_B_Y: {
                front = cube.pos * result.normal + hB.y;
                sideNormal1 = cube.rot.c1; // xNormal
                sideNormal2 = cube.rot.c3; // zNormal
                float xSide = cube.pos * sideNormal1;
                float zSide = cube.pos * sideNormal2;

                negSide1 = -xSide + hB.x; // negSideX
                posSide1 = xSide + hB.x; // posSideX
                negSide2 = -zSide + hB.z; // negSideZ
                posSide2 = zSide + hB.z; // posSideZ

                computeIncidentFaceAABB(incidentFace, hA, aabb.pos, result.normal);
                break;
            }

            case FACE_B_Z: {
                front = cube.pos * result.normal + hB.z;
                sideNormal1 = cube.rot.c1; // xNormal
                sideNormal2 = cube.rot.c2; // yNormal
                float xSide = cube.pos * sideNormal1;
                float ySide = cube.pos * sideNormal2;

                negSide1 = -xSide + hB.x; // negSideX
                posSide1 = xSide + hB.x; // posSideX
                negSide2 = -ySide + hB.y; // negSideY
                posSide2 = ySide + hB.y; // posSideY

                computeIncidentFaceAABB(incidentFace, hA, aabb.pos, result.normal);
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

        for (int i = 0; i < 4; ++i) {
            separation = result.normal * clipPoints2[i] - front;

            if (separation <= 0) {
                contactPoints[np++] = clipPoints2[i] - result.normal * separation;
                if (result.pDist < separation) { result.pDist = separation; }
            }
        }

        // * update the manifold to contain the results.

        result.pDist = -result.pDist;
        result.hit = 1;
        result.numPoints = np;
        result.contactPoints = new ZMath::Vec3D[np];

        for (int i = 0; i < np; ++i) { result.contactPoints[i] = contactPoints[i]; }
        
        return result;
    };

    // ? Normal points towards B and away from A.

    static CollisionManifold findCollisionFeatures(Cube const &cube1, Cube const &cube2) {
        CollisionManifold result;

        // half size of cube a and b respectively
        ZMath::Vec3D hA = cube1.getHalfSize(), hB = cube2.getHalfSize();

        // * determine the rotation matrices of A and B

        // rotate anything from global space to A's local space
        ZMath::Mat3D rotAT = cube1.rot.transpose();

        // rotate anything from gobal space to B's local space
        ZMath::Mat3D rotBT = cube2.rot.transpose();

        // determine the difference between the positions
        ZMath::Vec3D dP = cube2.pos - cube1.pos;
        ZMath::Vec3D dA = rotAT * dP;
        ZMath::Vec3D dB = rotBT * dP;

        // * rotation matrices for switching between local spaces

        // ! When scenes are developed test if we actually need the absolute value

        // Rotate anything from B's local space into A's
        ZMath::Mat3D C = ZMath::abs(rotAT * cube2.rot);

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
        ZMath::Vec3D faceB = ZMath::abs(dB) - hB - CT * hA;
        if (faceB.x > 0 || faceB.y > 0 || faceB.z > 0) {
            result.hit = 0;
            return result;
        }

        // * Find the best axis (i.e. the axis with the least amount of penetration).

        // Assume A's x-axis is the best axis first.
        Axis axis = FACE_A_X;
        float separation = faceA.x;
        result.normal = dA.x > 0.0f ? cube1.rot.c1 : -cube1.rot.c1;

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
            result.normal = dA.y > 0.0f ? cube1.rot.c2 : -cube1.rot.c2;
        }

        if (faceA.z > relativeTol * separation + absoluteTol * hA.z) {
            axis = FACE_A_Z;
            separation = faceA.z;
            result.normal = dA.z > 0.0f ? cube1.rot.c3 : -cube1.rot.c3;
        }

        // B's axes
        if (faceB.x > relativeTol * separation + absoluteTol * hB.x) {
            axis = FACE_B_X;
            separation = faceB.x;
            result.normal = dB.x > 0.0f ? cube2.rot.c1 : -cube2.rot.c1;
        }

        if (faceB.y > relativeTol * separation + absoluteTol * hB.y) {
            axis = FACE_B_Y;
            separation = faceB.y;
            result.normal = dB.y > 0.0f ? cube2.rot.c2 : -cube2.rot.c2;
        }

        if (faceB.z > relativeTol * separation + absoluteTol * hB.z) {
            axis = FACE_B_Z;
            separation = faceB.z;
            result.normal = dB.z > 0.0f ? cube2.rot.c3 : -cube2.rot.c3;
        }

        // * Setup clipping plane data based on the best axis

        ZMath::Vec3D sideNormal1, sideNormal2;
        ZMath::Vec3D incidentFace[4]; // 4 vertices for the collision in 3D
        float front, negSide1, negSide2, posSide1, posSide2;

        // * Compute the clipping lines and line segment to be clipped

        switch(axis) {
            case FACE_A_X: {
                front = cube1.pos * result.normal + hA.x;
                sideNormal1 = cube1.rot.c2; // yNormal
                sideNormal2 = cube1.rot.c3; // zNormal
                float ySide = cube1.pos * sideNormal1;
                float zSide = cube1.pos * sideNormal2;

                negSide1 = -ySide + hA.y; // negSideY
                posSide1 = ySide + hA.y; // posSideY
                negSide2 = -zSide + hA.z; // negSideZ
                posSide2 = zSide + hA.z; // posSideZ

                computeIncidentFace(incidentFace, hB, cube2.pos, cube2.rot, result.normal);
                break;
            }

            case FACE_A_Y: {
                front = cube1.pos * result.normal + hA.y;
                sideNormal1 = cube1.rot.c1; // xNormal
                sideNormal2 = cube1.rot.c3; // zNormal
                float xSide = cube1.pos * sideNormal1;
                float zSide = cube1.pos * sideNormal2;

                negSide1 = -xSide + hA.x; // negSideX
                posSide1 = xSide + hA.x; // posSideX
                negSide2 = -zSide + hA.z; // negSideZ
                posSide2 = zSide + hA.z; // posSideZ

                computeIncidentFace(incidentFace, hB, cube2.pos, cube2.rot, result.normal);
                break;
            }

            case FACE_A_Z: {
                front = cube1.pos * result.normal + hA.z;
                sideNormal1 = cube1.rot.c1; // xNormal
                sideNormal2 = cube1.rot.c2; // yNormal
                float xSide = cube1.pos * sideNormal1;
                float ySide = cube1.pos * sideNormal2;

                negSide1 = -xSide + hA.x; // negSideX
                posSide1 = xSide + hA.x; // posSideX
                negSide2 = -ySide + hA.y; // negSideY
                posSide2 = ySide + hA.y; // posSideY

                computeIncidentFace(incidentFace, hB, cube2.pos, cube2.rot, result.normal);
                break;
            }

            case FACE_B_X: {
                front = cube2.pos * result.normal + hB.x;
                sideNormal1 = cube2.rot.c2; // yNormal
                sideNormal2 = cube2.rot.c3; // zNormal
                float ySide = cube2.pos * sideNormal1;
                float zSide = cube2.pos * sideNormal2;

                negSide1 = -ySide + hB.y; // negSideY
                posSide1 = ySide + hB.y; // posSideY
                negSide2 = -zSide + hB.z; // negSideZ
                posSide2 = zSide + hB.z; // posSideZ

                computeIncidentFace(incidentFace, hA, cube1.pos, cube1.rot, result.normal);
                break;
            }

            case FACE_B_Y: {
                front = cube2.pos * result.normal + hB.y;
                sideNormal1 = cube2.rot.c1; // xNormal
                sideNormal2 = cube2.rot.c3; // zNormal
                float xSide = cube2.pos * sideNormal1;
                float zSide = cube2.pos * sideNormal2;

                negSide1 = -xSide + hB.x; // negSideX
                posSide1 = xSide + hB.x; // posSideX
                negSide2 = -zSide + hB.z; // negSideZ
                posSide2 = zSide + hB.z; // posSideZ

                computeIncidentFace(incidentFace, hA, cube1.pos, cube1.rot, result.normal);
                break;
            }

            case FACE_B_Z: {
                front = cube2.pos * result.normal + hB.z;
                sideNormal1 = cube2.rot.c1; // xNormal
                sideNormal2 = cube2.rot.c2; // yNormal
                float xSide = cube2.pos * sideNormal1;
                float ySide = cube2.pos * sideNormal2;

                negSide1 = -xSide + hB.x; // negSideX
                posSide1 = xSide + hB.x; // posSideX
                negSide2 = -ySide + hB.y; // negSideY
                posSide2 = ySide + hB.y; // posSideY

                computeIncidentFace(incidentFace, hA, cube1.pos, cube1.rot, result.normal);
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

        for (int i = 0; i < 4; ++i) {
            separation = result.normal * clipPoints2[i] - front;

            if (separation <= 0) {
                contactPoints[np++] = clipPoints2[i] - result.normal * separation;
                if (result.pDist < separation) { result.pDist = separation; }
            }
        }

        // * update the manifold to contain the results.

        result.pDist = -result.pDist;
        result.hit = 1;
        result.numPoints = np;
        result.contactPoints = new ZMath::Vec3D[np];

        for (int i = 0; i < np; ++i) { result.contactPoints[i] = contactPoints[i]; }
        
        return result;
    };

    // Find the collision features and resolve the impulse between two rigidbodies.
    static CollisionManifold findCollisionFeatures(RigidBody3D* rb1, RigidBody3D* rb2) {
        switch (rb1->colliderType) {
            case RIGID_SPHERE_COLLIDER: {
                if (rb2->colliderType == RIGID_SPHERE_COLLIDER) { return findCollisionFeatures(rb1->collider.sphere, rb2->collider.sphere); }
                if (rb2->colliderType == RIGID_AABB_COLLIDER) { return findCollisionFeatures(rb1->collider.sphere, rb2->collider.aabb); }
                if (rb2->colliderType == RIGID_CUBE_COLLIDER) { return findCollisionFeatures(rb1->collider.sphere, rb2->collider.cube); }

                break;
            }

            case RIGID_AABB_COLLIDER: {
                if (rb2->colliderType == RIGID_SPHERE_COLLIDER) {
                    CollisionManifold manifold = findCollisionFeatures(rb2->collider.sphere, rb1->collider.aabb);
                    manifold.normal = -manifold.normal; // flip the direction as the original order passed in was reversed
                    return manifold;
                }

                if (rb2->colliderType == RIGID_AABB_COLLIDER) { return findCollisionFeatures(rb1->collider.aabb, rb2->collider.aabb); }
                if (rb2->colliderType == RIGID_CUBE_COLLIDER) { return findCollisionFeatures(rb1->collider.aabb, rb2->collider.cube); }

                break;
            }

            case RIGID_CUBE_COLLIDER: {
                if (rb2->colliderType == RIGID_SPHERE_COLLIDER) {
                    CollisionManifold manifold = findCollisionFeatures(rb2->collider.sphere, rb1->collider.cube);
                    manifold.normal = -manifold.normal; // flip the direction as the original order passed in was reversed
                    return manifold;
                }

                if (rb2->colliderType == RIGID_AABB_COLLIDER) {
                    CollisionManifold manifold = findCollisionFeatures(rb2->collider.aabb, rb1->collider.cube);
                    manifold.normal = -manifold.normal; // flip the direction as the original order passed in was reversed
                    return manifold;
                }

                if (rb2->colliderType == RIGID_CUBE_COLLIDER) { return findCollisionFeatures(rb1->collider.cube, rb2->collider.cube); }

                break;
            }

            // * User defined types go here.
        }

        return {ZMath::Vec3D(), nullptr, -1.0f, 0, 0};
    };

    // Find the collision features and resolve the impulse between a staticbody and a rigidbody.
    // The collision normal will point towards the rigid body and away from the static body.
    static CollisionManifold findCollisionFeatures(StaticBody3D* sb, RigidBody3D* rb) {
        // ? The normal points towards B and away from A so we want to pass the rigid body's colliders second.

        switch (sb->colliderType) {
            case STATIC_PLANE_COLLIDER: {
                switch (rb->colliderType) {
                    case RIGID_SPHERE_COLLIDER: { return findCollisionFeatures(sb->collider.plane, rb->collider.sphere); }
                    case RIGID_AABB_COLLIDER: { return findCollisionFeatures(sb->collider.plane, rb->collider.aabb); }
                    case RIGID_CUBE_COLLIDER: { return findCollisionFeatures(sb->collider.plane, rb->collider.cube); }
                }

                break;
            }

            case STATIC_SPHERE_COLLIDER: {
                switch (rb->colliderType) {
                    case RIGID_SPHERE_COLLIDER: { return findCollisionFeatures(sb->collider.sphere, rb->collider.sphere); }
                    case RIGID_AABB_COLLIDER: { return findCollisionFeatures(sb->collider.sphere, rb->collider.aabb); }
                    case RIGID_CUBE_COLLIDER: { return findCollisionFeatures(sb->collider.sphere, rb->collider.cube); }
                }

                break;
            }

            case STATIC_AABB_COLLIDER: {
                switch (rb->colliderType) {
                    case RIGID_SPHERE_COLLIDER: {
                        CollisionManifold manifold = findCollisionFeatures(rb->collider.sphere, sb->collider.aabb);
                        manifold.normal = -manifold.normal; // flip the direction as the original order passed in was reversed
                        return manifold;
                    }

                    case RIGID_AABB_COLLIDER: { return findCollisionFeatures(sb->collider.aabb, rb->collider.aabb); }
                    case RIGID_CUBE_COLLIDER: { return findCollisionFeatures(sb->collider.aabb, rb->collider.cube); }
                }

                break;
            }

            case STATIC_CUBE_COLLIDER: {
                switch (rb->colliderType) {
                    case RIGID_SPHERE_COLLIDER: {
                        CollisionManifold manifold = findCollisionFeatures(rb->collider.sphere, sb->collider.cube);
                        manifold.normal = -manifold.normal; // flip the direction as the original order passed in was reversed
                        return manifold;
                    }

                    case RIGID_AABB_COLLIDER: {
                        CollisionManifold manifold = findCollisionFeatures(rb->collider.aabb, sb->collider.cube);
                        manifold.normal = -manifold.normal; // flip the direction as the original order passed in was reversed
                        return manifold;
                    }

                    case RIGID_CUBE_COLLIDER: { return findCollisionFeatures(sb->collider.cube, rb->collider.cube); }
                }

                break;
            }

            case STATIC_CUSTOM_COLLIDER: {
                // * User defined types go here.
                break;
            }
        }

        return {ZMath::Vec3D(), nullptr, -1.0f, 0, 0};
    };
}
