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
    };

    struct StaticBody3D {
        ZMath::Vec3D pos; // centerpoint of the staticbody.
        float theta; // rotation with respect to the XY plane.
        float phi; // rotation with respect to the XZ plane.
        float mass; // Must remain constant.
        float invMass; // 1/mass. Must remain constant.
    };

    void updateRigidbody(RigidBody3D &rb, ZMath::Vec3D const &g, float dt) {
        // * Add code to update a rigidbody here
    };

    // ! Just a flag class. I'm sure there's a better way to do this so I'll update it once I get the chance to look into it.
    // ! Rn I'm dealing with too much architecture all at once to figure this out so once I don't have to worry about that so much I will.
    class Collider3D {
        public:
            enum types {
                SPHERE_COLLIDER,
                AABB_COLLIDER,
                CUBE_COLLIDER,
                CUSTOM_COLLIDER
            };

            int type = CUSTOM_COLLIDER + 1; // initialize it to not have a collider type
    };
}

#endif // !RIGIDBODY_H