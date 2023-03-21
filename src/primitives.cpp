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

    ZMath::Vec3D Plane::getNormal() {
        ZMath::Vec3D min = ZMath::Vec3D(sb.pos.x - halfSize.x, sb.pos.y - halfSize.y, sb.pos.z);
        ZMath::Vec3D max = ZMath::Vec3D(sb.pos.x + halfSize.x, sb.pos.y + halfSize.y, sb.pos.z);

        ZMath::rotateXY(min, sb.pos, sb.theta);
        ZMath::rotateXZ(min, sb.pos, sb.phi);
        ZMath::rotateXY(max, sb.pos, sb.theta);
        ZMath::rotateXZ(max, sb.pos, sb.phi);

        return (max - sb.pos).cross(min - sb.pos);
    };

    ZMath::Vec3D Plane::getLocalMin() { return ZMath::Vec3D(sb.pos.x - halfSize.x, sb.pos.y - halfSize.y, sb.pos.z); };
    ZMath::Vec3D Plane::getLocalMax() { return ZMath::Vec3D(sb.pos.x + halfSize.x, sb.pos.y + halfSize.y, sb.pos.z); };
    ZMath::Vec2D Plane::getHalfsize() { return halfSize; };

    ZMath::Vec3D* Plane::getVertices() {
        ZMath::Vec3D* v = new ZMath::Vec3D[4]; // 4 as it is rectangular

        v[0] = ZMath::Vec3D(sb.pos.x - halfSize.x, sb.pos.y - halfSize.y, sb.pos.z);
        v[1] = ZMath::Vec3D(sb.pos.x - halfSize.x, sb.pos.y + halfSize.y, sb.pos.z);
        v[2] = ZMath::Vec3D(sb.pos.x + halfSize.x, sb.pos.y + halfSize.y, sb.pos.z);
        v[3] = ZMath::Vec3D(sb.pos.x + halfSize.x, sb.pos.y - halfSize.y, sb.pos.z);

        // rotate each vertex
        for (int i = 0; i < 4; i++) {
            ZMath::rotateXY(v[i], sb.pos, sb.theta);
            ZMath::rotateXZ(v[i], sb.pos, sb.phi);
        }

        return v;
    };

    ZMath::Vec3D Plane::getNormal() const {
        ZMath::Vec3D min = ZMath::Vec3D(sb.pos.x - halfSize.x, sb.pos.y - halfSize.y, sb.pos.z);
        ZMath::Vec3D max = ZMath::Vec3D(sb.pos.x + halfSize.x, sb.pos.y + halfSize.y, sb.pos.z);

        ZMath::rotateXY(min, sb.pos, sb.theta);
        ZMath::rotateXZ(min, sb.pos, sb.phi);
        ZMath::rotateXY(max, sb.pos, sb.theta);
        ZMath::rotateXZ(max, sb.pos, sb.phi);

        return (max - sb.pos).cross(min - sb.pos);
    };

    ZMath::Vec3D Plane::getLocalMin() const { return ZMath::Vec3D(sb.pos.x - halfSize.x, sb.pos.y - halfSize.y, sb.pos.z); };
    ZMath::Vec3D Plane::getLocalMax() const { return ZMath::Vec3D(sb.pos.x + halfSize.x, sb.pos.y + halfSize.y, sb.pos.z); };
    ZMath::Vec2D Plane::getHalfSize() const { return halfSize; };

    ZMath::Vec3D* Plane::getVertices() const {
        ZMath::Vec3D* v = new ZMath::Vec3D[4]; // 4 as it is rectangular

        v[0] = ZMath::Vec3D(sb.pos.x - halfSize.x, sb.pos.y - halfSize.y, sb.pos.z);
        v[1] = ZMath::Vec3D(sb.pos.x - halfSize.x, sb.pos.y + halfSize.y, sb.pos.z);
        v[2] = ZMath::Vec3D(sb.pos.x + halfSize.x, sb.pos.y + halfSize.y, sb.pos.z);
        v[3] = ZMath::Vec3D(sb.pos.x + halfSize.x, sb.pos.y - halfSize.y, sb.pos.z);

        // rotate each vertex
        for (int i = 0; i < 4; i++) {
            ZMath::rotateXY(v[i], sb.pos, sb.theta);
            ZMath::rotateXZ(v[i], sb.pos, sb.phi);
        }

        return v;
    };

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

    ZMath::Vec3D* AABB::getVertices() const {
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

    // * ====================================================================================================================

    // * ===============
    // * Cube
    // * ===============

    ZMath::Vec3D Cube::getLocalMin() { return rb.pos - halfSize; };

    ZMath::Vec3D Cube::getLocalMax() { return rb.pos + halfSize; };

    ZMath::Vec3D Cube::getHalfSize() { return halfSize; };

    ZMath::Vec3D* Cube::getVertices() {
        ZMath::Vec3D* v = new ZMath::Vec3D[8];

        // ! test out using .set() after we have a model
        // todo maybe reorder
        v[0] = rb.pos - halfSize;
        v[1] = ZMath::Vec3D(rb.pos.x - halfSize.x, rb.pos.y - halfSize.y, rb.pos.z +  halfSize.z);
        v[2] = ZMath::Vec3D(rb.pos.x + halfSize.x, rb.pos.y - halfSize.y, rb.pos.z + halfSize.z);
        v[3] = ZMath::Vec3D(rb.pos.x + halfSize.x, rb.pos.y - halfSize.y, rb.pos.z - halfSize.z);
        v[4] = ZMath::Vec3D(rb.pos.x - halfSize.x, rb.pos.y + halfSize.y, rb.pos.z + halfSize.z);
        v[5] = ZMath::Vec3D(rb.pos.x - halfSize.x, rb.pos.y + halfSize.y, rb.pos.z - halfSize.z);
        v[6] = ZMath::Vec3D(rb.pos.x + halfSize.x, rb.pos.y + halfSize.y, rb.pos.z - halfSize.z);
        v[7] = rb.pos + halfSize;

        for (int i = 0; i < 8; i++) {
            ZMath::rotateXY(v[i], rb.pos, rb.theta);
            ZMath::rotateXZ(v[i], rb.pos, rb.phi);
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
