// ? Stores all the different types of bodies attached to primitives.

#ifndef BODIES_H
#define BODIES_H

#include "primitives.h"

namespace Primitives {
    enum RigidBodyCollider {
        SPHERE_COLLIDER,
        AABB_COLLIDER,
        CUBE_COLLIDER,
        CUSTOM_COLLIDER,
        NONE
    };

    enum StaticBodyCollider {
        PLANE_COLLIDER,
        SPHERE_COLLIDER,
        AABB_COLLIDER,
        CUBE_COLLIDER,
        CUSTOM_COLLIDER,
        NONE
    };

    struct RigidBody3D {
        RigidBody3D() = default; // Default constructor to make the compiler happy (for efficiency).

        // * Handle and store the collider.

        RigidBodyCollider colliderType;
        union {
            Sphere sphere;
            AABB aabb;
            Cube cube;
            // * Add custom colliders here.
        };

        // * Handle and store the physics.

        float mass; // Must remain constant.
        float invMass; // 1/mass. Must remain constant.

        // Coefficient of Restitution.
        // Represents a loss of kinetic energy due to heat.
        // Between 0 and 1 for our purposes.
        // 1 = perfectly elastic.
        float cor;

        ZMath::Vec3D pos; // centerpoint of the rigidbody.
        ZMath::Vec3D vel = ZMath::Vec3D(); // velocity of the rigidbody.
        ZMath::Vec3D netForce = ZMath::Vec3D(); // sum of all forces acting on the rigidbody.

        // todo pass timeStep so that we can make it actually accurate
        void update(ZMath::Vec3D const &g, float dt) {
            // * Add code to update a rigidbody here.
            // ? assuming g is gravity, and it is already negative
            float t = (float)(int)(dt/0.0167f) * 0.0167f;

            netForce += g * mass;
            vel += (netForce * invMass) * t;
            pos += vel * t;
            netForce = ZMath::Vec3D();

            // todo refactor so that we can store the colliders inside the bodies without having to update the 
            // todo    positions of the colliders and bodies separately.
            // ! temporary solution to test out the new architecture

            // Update the pos of the collider.
            // If statements are more readable than a switch here.
            if      (colliderType == SPHERE_COLLIDER) { sphere.c = pos; }
            else if (colliderType == AABB_COLLIDER)   { aabb.pos = pos; }
            else if (colliderType == CUBE_COLLIDER)   { cube.pos = pos; }
        };
    };

    struct StaticBody3D {
        StaticBody3D() = default; // Default constructor to make the compiler happy (for efficiency).

        // * Information related to the static body.

        ZMath::Vec3D pos; // centerpoint of the staticbody.

        // todo might not need to store the mass.
        float mass; // Must remain constant.
        float invMass; // 1/mass. Must remain constant.

        // * Handle and store the collider.

        StaticBodyCollider colliderType;
        union {
            Plane plane;
            Sphere sphere;
            AABB aabb;
            Cube cube;
        };
    };
}

#endif // !RIGIDBODY_H
