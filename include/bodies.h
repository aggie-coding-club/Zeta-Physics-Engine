// ? Stores all the different types of bodies attached to primitives.

#ifndef BODIES_H
#define BODIES_H

#include "zmath.h"

namespace Primitives {
    struct RigidBody3D {
        float theta; // rotation with respect to the XY plane.
        float phi; // rotation with respect to the XZ plane.
        float mass; // Must remain constant.
        float invMass; // 1/mass. Must remain constant.

        ZMath::Vec3D pos; // centerpoint of the rigidbody.
        ZMath::Vec3D velocity = ZMath::Vec3D(); // velocity of the rigidbody.
        ZMath::Vec3D netForce = ZMath::Vec3D(); // sum of all forces acting on the rigidbody.

        void update(ZMath::Vec3D const &g, float dt) {
            // * Add code to update a rigidbody here.
        };
    };

    struct StaticBody3D {
        ZMath::Vec3D pos; // centerpoint of the staticbody.
        float theta; // rotation with respect to the XY plane.
        float phi; // rotation with respect to the XZ plane.
        float mass; // Must remain constant.
        float invMass; // 1/mass. Must remain constant.
    };
}

#endif // !RIGIDBODY_H