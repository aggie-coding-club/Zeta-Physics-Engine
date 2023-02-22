// ? Define functions from our primitives header here.

#include "primitives.h"
#include <cmath>

namespace Primitives {
    // * ===================
    // * Line3D
    // * ===================

    // * ====================================================================================================================

    void Line3D::setStart(ZMath::Vec3D const &pos) { start.set(pos); };

    void Line3D::setEnd(ZMath::Vec3D const &pos) { end.set(pos); };

    ZMath::Vec3D Line3D::getStart() { return start; };
    ZMath::Vec3D Line3D::getEnd() { return end; };

    ZMath::Vec3D Line3D::getStart() const { return start; };
    ZMath::Vec3D Line3D::getEnd() const { return end; };

    // * ====================================================================================================================

    // * =================
    // * Plane
    // * =================

    // ! No functions to move to here yet

    // * ====================================================================================================================

    // * =================
    // * Sphere
    // * =================

    void Sphere::setRadius(float r) { rho = r; };
    void Sphere::setCenter(ZMath::Vec3D const &c) { center.set(c); };

    float Sphere::getRadius() { return rho; };
    ZMath::Vec3D Sphere::getCenter() { return center; };

    float Sphere::getRaidus() const { return rho; };
    ZMath::Vec3D Sphere::getCenter() const { return center; };

    // * ====================================================================================================================

    // * =============
    // * AABB
    // * =============
    ZMath::Vec3D AABB::getMin() { return pos - halfSize; }
    ZMath::Vec3D AABB::getMax() { return pos + halfSize; }
    ZMath::Vec3D AABB::getHalfSize() { return halfSize; }

    ZMath::Vec3D* AABB::getVertices() {};

    // * ====================================================================================================================

    // * ===============
    // * Cube
    // * ===============

    void Cube::setTheta(float theta) { rb.setTheta(theta); };

    void Cube::setPhi(float phi) { rb.setPhi(phi); };

    float Cube::getTheta() { return rb.getTheta(); };

    float Cube::getPhi() { return rb.getPhi(); };

    ZMath::Vec3D Cube::getPos() { return rb.getPos(); }

    ZMath::Vec3D Cube::getLocalMin() { return rb.getPos() - halfSize; };

    ZMath::Vec3D Cube::getLocalMax() { return rb.getPos() + halfSize; };

    ZMath::Vec3D Cube::getHalfSize() { return halfSize; };

    ZMath::Vec3D* Cube::getVertices() {
        ZMath::Vec3D* v = new ZMath::Vec3D[8];
        ZMath::Vec3D const pos = rb.getPos();

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
            ZMath::rotateXY(v[i], pos, rb.getTheta());
            ZMath::rotateXZ(v[i], pos, rb.getPhi());
        }

        return v;
    };

    // * ====================================================================================================================
}
