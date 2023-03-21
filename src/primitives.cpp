// ? Define functions from our primitives header here.

#include "primitives.h"

namespace Primitives {
    // * ===================
    // * Line3D
    // * ===================

    // * Does not have any functions to define in here.
    // ! Will leave this here temporarily in case of refactoring

    // * ====================================================================================================================

    // * =================
    // * Plane
    // * =================

    // ! No functions to move to here yet

    // * ====================================================================================================================

    // * =================
    // * Sphere
    // * =================

    // * Does not have any functions to define in here.
    // ! Will leave this here temporarily in case of refactoring

    // * ====================================================================================================================

    // * ==================================
    // * AABB (Axis Aligned Bounding Box)
    // * ==================================

    ZMath::Vec3D AABB::getMin() { return pos - halfSize; }
    ZMath::Vec3D AABB::getMax() { return pos + halfSize; }
    ZMath::Vec3D AABB::getHalfSize() { return halfSize; }

    ZMath::Vec3D* AABB::getVertices() {
        ZMath::Vec3D* vertices = new ZMath::Vec3D[4];

        vertices[0] = {pos.x - halfSize.x, pos.y - halfSize.y, pos.z + halfSize.z}; // bottom left
        vertices[1] = {pos.x - halfSize.x, pos.y + halfSize.y, pos.z + halfSize.z}; // top left
        vertices[2] = {pos.x + halfSize.x, pos.y + halfSize.y, pos.z + halfSize.z}; // top right
        vertices[3] = {pos.x + halfSize.x, pos.y - halfSize.y, pos.z + halfSize.z};  // bottom right
        
        vertices[4] = {pos.x - halfSize.x, pos.y - halfSize.y, pos.z - halfSize.z}; // bottom left
        vertices[5] = {pos.x - halfSize.x, pos.y + halfSize.y, pos.z - halfSize.z}; // top left
        vertices[6] = {pos.x + halfSize.x, pos.y + halfSize.y, pos.z - halfSize.z}; // top right
        vertices[7] = {pos.x + halfSize.x, pos.y - halfSize.y, pos.z - halfSize.z}; // bottom right


        return vertices;
    };

    ZMath::Vec3D AABB::getMin() const { return pos - halfSize; }
    ZMath::Vec3D AABB::getMax() const { return pos + halfSize; }
    ZMath::Vec3D AABB::getHalfSize() const { return halfSize; }

    ZMath::Vec3D* AABB::getVertices() const {};

    // * ====================================================================================================================

    // * ===============
    // * Cube
    // * ===============

    ZMath::Vec3D Cube::getLocalMin() { return rb.pos - halfSize; };

    ZMath::Vec3D Cube::getLocalMax() { return rb.pos + halfSize; };

    ZMath::Vec3D Cube::getHalfSize() { return halfSize; };

    ZMath::Vec3D* Cube::getVertices() {
        ZMath::Vec3D* v = new ZMath::Vec3D[8];
        ZMath::Vec3D const pos = rb.pos;

        // ! test out using .set() after we have a model
        // todo maybe reorder
        v[0] = pos - halfSize;
        v[1] = ZMath::Vec3D(pos.x - halfSize.x, pos.y - halfSize.y, pos.z +  halfSize.z);
        v[2] = ZMath::Vec3D(pos.x + halfSize.x, pos.y - halfSize.y, pos.z + halfSize.z);
        v[3] = ZMath::Vec3D(pos.x + halfSize.x, pos.y - halfSize.y, pos.z - halfSize.z);
        v[4] = ZMath::Vec3D(pos.x - halfSize.x, pos.y + halfSize.y, pos.z + halfSize.z);
        v[5] = ZMath::Vec3D(pos.x - halfSize.x, pos.y + halfSize.y, pos.z - halfSize.z);
        v[6] = ZMath::Vec3D(pos.x + halfSize.x, pos.y + halfSize.y, pos.z - halfSize.z);
        v[7] = pos + halfSize;

        for (int i = 0; i < 8; i++) {
            ZMath::rotateXY(v[i], pos, rb.theta);
            ZMath::rotateXZ(v[i], pos, rb.phi);
        }

        return v;
    };

    ZMath::Vec3D Cube::getLocalMin() const { return rb.pos - halfSize; };

    ZMath::Vec3D Cube::getLocalMax() const { return rb.pos + halfSize; };

    ZMath::Vec3D Cube::getHalfSize() const { return halfSize; };

    ZMath::Vec3D* Cube::getVertices() const {
        ZMath::Vec3D* v = new ZMath::Vec3D[8];
        ZMath::Vec3D const pos = rb.pos;

        // ! test out using .set() after we have a model
        // todo maybe reorder
        v[0] = pos - halfSize;
        v[1] = ZMath::Vec3D(pos.x - halfSize.x, pos.y - halfSize.y, pos.z + halfSize.z);
        v[2] = ZMath::Vec3D(pos.x + halfSize.x, pos.y - halfSize.y, pos.z + halfSize.z);
        v[3] = ZMath::Vec3D(pos.x + halfSize.x, pos.y - halfSize.y, pos.z - halfSize.z);
        v[4] = ZMath::Vec3D(pos.x - halfSize.x, pos.y + halfSize.y, pos.z + halfSize.z);
        v[5] = ZMath::Vec3D(pos.x - halfSize.x, pos.y + halfSize.y, pos.z - halfSize.z);
        v[6] = ZMath::Vec3D(pos.x + halfSize.x, pos.y + halfSize.y, pos.z - halfSize.z);
        v[7] = pos + halfSize;

        for (int i = 0; i < 8; i++) {
            ZMath::rotateXY(v[i], pos, rb.theta);
            ZMath::rotateXZ(v[i], pos, rb.phi);
        }

        return v;
    };

    // * ====================================================================================================================
}
