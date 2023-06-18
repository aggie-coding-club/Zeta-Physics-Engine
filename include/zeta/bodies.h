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
         * @param linearDamping The linear damping of the rigid body. This should fall on (0, 1].
         * @param colliderType The type of collider attached to the rigidbody. This should be set to RIGID_NONE if there will not be one attached.
         * @param collider A pointer to the collider of the rigid body. If this does not match the colliderType specified, it will
         *                   cause undefined behvior to occur. If you specify RIGID_NONE, this should be set to nullptr. 
         *                   This will call delete on it afterwards so do not try to use the pointer passed after creating the rigid body.
         */
        RigidBody3D(ZMath::Vec3D const &pos, float mass, float cor, float linearDamping, RigidBodyCollider colliderType, void* collider) 
                : pos(pos), mass(mass), invMass(1.0f/mass), cor(cor), linearDamping(linearDamping), colliderType(colliderType)
        {
            switch(colliderType) {
                case RIGID_SPHERE_COLLIDER: { this->collider.sphere = *((Sphere*) collider); }
                case RIGID_AABB_COLLIDER: { this->collider.aabb = *((AABB*) collider); }
                case RIGID_CUBE_COLLIDER: { this->collider.cube = *((Cube*) collider); }
                // * User defined colliders go here.
            }

            // if (collider) { delete collider; }
        };

        // * Handle and store the collider.

        RigidBodyCollider colliderType;
        union Collider {
            Collider() {};

            Sphere sphere;
            AABB aabb;
            Cube cube;
            // * Add custom colliders here.
        } collider;

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

            vel *= linearDamping;
            netForce = ZMath::Vec3D();

            // Update the pos of the collider.
            // If statements are more readable than a switch here.
            if      (colliderType == RIGID_SPHERE_COLLIDER) { collider.sphere.c = pos; }
            else if (colliderType == RIGID_AABB_COLLIDER)   { collider.aabb.pos = pos; }
            else if (colliderType == RIGID_CUBE_COLLIDER)   { collider.cube.pos = pos; }
        };
    };

    struct StaticBody3D {
        StaticBody3D() {}; // Default constructor to make the compiler happy (for efficiency).

        /**
         * @brief Create a 3D staticbody.
         * 
         * @param pos The centerpoint of the staticbody.
         * @param colliderType The type of collider attached to the staticbody. This should be set to STATIC_NONE if there will not be one attached.
         * @param collider A pointer to the collider of the static body. If this does not match the colliderType specified, it will
         *                   cause undefined behvior to occur. If you specify STATIC_NONE, this should be set to nullptr. 
         *                   This will call delete on it afterwards so do not try to use the pointer passed after creating the static body.
         */
        StaticBody3D(ZMath::Vec3D const &pos, StaticBodyCollider colliderType, void* collider) : pos(pos), colliderType(colliderType) {
            switch(colliderType) {
                case STATIC_PLANE_COLLIDER: { this->collider.plane = *((Plane*) collider); }
                case STATIC_SPHERE_COLLIDER: { this->collider.sphere = *((Sphere*) collider); }
                case STATIC_AABB_COLLIDER: { this->collider.aabb = *((AABB*) collider); }
                case STATIC_CUBE_COLLIDER: { this->collider.cube = *((Cube*) collider); }
                // * User defined colliders go here.
            }

            // if (collider) { delete collider; }
        };

        // * Information related to the static body.

        ZMath::Vec3D pos; // centerpoint of the staticbody.

        // * Handle and store the collider.

        StaticBodyCollider colliderType;
        union Collider {
            Collider() {};

            Plane plane;
            Sphere sphere;
            AABB aabb;
            Cube cube;
        } collider;
    };
}

#endif // !RIGIDBODY_H
