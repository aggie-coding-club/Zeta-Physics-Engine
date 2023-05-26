// ? Stores all the different types of bodies attached to primitives.

#ifndef BODIES_H
#define BODIES_H

#include "primitives.h"

namespace Primitives {
    enum RigidBodyCollider {
        RIGID_SPHERE_COLLIDER,
        RIGID_AABB_COLLIDER,
        RIGID_CUBE_COLLIDER,
        RIGID_CUSTOM_COLLIDER,
        RIGID_NONE
    };

    enum StaticBodyCollider {
        STATIC_PLANE_COLLIDER,
        STATIC_SPHERE_COLLIDER,
        STATIC_AABB_COLLIDER,
        STATIC_CUBE_COLLIDER,
        STATIC_CUSTOM_COLLIDER,
        STATIC_NONE
    };

    struct RigidBody3D {
        // Remember to specify the necessary fields before using the RigidBody3D if using the default constructor.
        // Consult documentation for those fields if needed.
        RigidBody3D() {}; // Default constructor to make the compiler happy (for efficiency).

        /**
         * @brief Create a 3D RigidBody.
         * 
         * @param pos Centerpoint of the rigidbody.
         * @param mass Mass of the rigidbody.
         * @param cor The coefficient of restituion of the rigidbody. This represents a loss of kinetic energy due to heat and should be
         *              between 0 and 1 inclusive with 1 being perfectly elastic.
         * @param colliderType The type of collider attached to the rigidbody. This should be set to RIGID_NONE if there will not be one attached.
         *                       Remember to manually assign a value to sphere, aabb, or cube depending on the collider type specified. DO NOT
         *                       assign a value to a collider other than the one corresponding to the type specified.
         */
        RigidBody3D(ZMath::Vec3D const &pos, float mass, float cor, float linearDamping, RigidBodyCollider colliderType) 
                : pos(pos), mass(mass), invMass(1.0f/mass), cor(cor), linearDamping(linearDamping), colliderType(colliderType) {};

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

        // Linear damping.
        // Acts as linear friction on the rigidbody.
        float linearDamping;

        ZMath::Vec3D pos; // centerpoint of the rigidbody.
        ZMath::Vec3D vel = ZMath::Vec3D(); // velocity of the rigidbody.
        ZMath::Vec3D netForce = ZMath::Vec3D(); // sum of all forces acting on the rigidbody.

        void update(ZMath::Vec3D const &g, float dt) {
            // ? assuming g is gravity, and it is already negative
            netForce += g * mass;
            vel += (netForce * invMass) * dt;
            pos += vel * dt;
            netForce = ZMath::Vec3D();

            // todo refactor so that we can store the colliders inside the bodies without having to update the 
            // todo    positions of the colliders and bodies separately.
            // ! temporary solution to test out the new architecture

            // Update the pos of the collider.
            // If statements are more readable than a switch here.
            if      (colliderType == RIGID_SPHERE_COLLIDER) { sphere.c = pos; }
            else if (colliderType == RIGID_AABB_COLLIDER)   { aabb.pos = pos; }
            else if (colliderType == RIGID_CUBE_COLLIDER)   { cube.pos = pos; }
        };
    };

    struct StaticBody3D {
        StaticBody3D() {}; // Default constructor to make the compiler happy (for efficiency).

        /**
         * @brief Create a 3D staticbody.
         * 
         * @param pos The centerpoint of the staticbody.
         * @param colliderType The type of collider attached to the staticbody. This should be set to STATIC_NONE if there will not be one attached.
         *                       Remember to manually assign a value to plane, sphere, aabb, or cube depending on the collider type specified.
         *                       DO NOT assign a value to a collider other than the one corresponding to the type specified.
         */
        StaticBody3D(ZMath::Vec3D const &pos, StaticBodyCollider colliderType) : pos(pos), colliderType(colliderType) {};

        // * Information related to the static body.

        ZMath::Vec3D pos; // centerpoint of the staticbody.

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
