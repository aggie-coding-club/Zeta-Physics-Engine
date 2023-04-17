// ? Stores all the different types of bodies attached to primitives.

#ifndef BODIES_H
#define BODIES_H

#include "zmath.h"

namespace Primitives {
    struct RigidBody3D {
        float theta; // rotation with respect to the XY plane.
        float phi; // rotation with respect to the XZ plane.
        float mass = 5.0f; // Must remain constant.
        float invMass = 0.2f; // 1/mass. Must remain constant.

        // Coefficient of Restitution.
        // Represents a loss of kinetic energy due to heat.
        // Between 0 and 1 for our purposes.
        // 1 = perfectly elastic.
        float cor;

        ZMath::Vec3D pos; // centerpoint of the rigidbody.
        ZMath::Vec3D vel = ZMath::Vec3D(); // velocity of the rigidbody.
        ZMath::Vec3D netForce = ZMath::Vec3D(); // sum of all forces acting on the rigidbody.

        void update(ZMath::Vec3D const &g, float dt) {
            // * Add code to update a rigidbody here.
            // ? assuming g is gravity, and it is already negative
            netForce += g * mass;
            vel += (netForce * invMass) * ((int)(dt/0.0167f) * 0.0167f);
            pos += vel * ((int)(dt/0.0167f) * 0.0167f);
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
