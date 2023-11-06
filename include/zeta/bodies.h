// ? Stores all the different types of bodies attached to primitives.

#pragma once

#include <utility>
#include "primitives.h"

namespace Zeta {
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

    class RigidBody3D {
        public:
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
             */
            RigidBody3D(ZMath::Vec3D const &pos, float mass, float cor, float linearDamping, RigidBodyCollider colliderType, void* collider) 
                    : pos(pos), mass(mass), invMass(1.0f/mass), cor(cor), linearDamping(linearDamping), 
                      colliderType(colliderType), collider(collider) {};


            // * ===================
            // * Rule of 5 Stuff
            // * ===================

            // Create a 3D rigidbody from another 3D rigidbody.
            RigidBody3D(RigidBody3D const &rb) {
                pos = rb.pos;
                mass = rb.mass;
                invMass = rb.invMass;
                cor = rb.cor;
                linearDamping = rb.linearDamping;
                colliderType = rb.colliderType;

                switch(colliderType) {
                    case RIGID_SPHERE_COLLIDER: { collider = new Sphere(*((Sphere*) rb.collider)); break; }
                    case RIGID_AABB_COLLIDER:   { collider = new AABB(*((AABB*) rb.collider));     break; }
                    case RIGID_CUBE_COLLIDER:   { collider = new Cube(*((Cube*) rb.collider));     break; }
                    case RIGID_NONE:            { collider = nullptr;                              break; }
                }
            };

            // Create a 3D rigidbody from another 3D rigidbody.
            RigidBody3D(RigidBody3D &&rb) {
                pos = std::move(rb.pos);
                mass = rb.mass;
                invMass = rb.invMass;
                cor = rb.cor;
                linearDamping = rb.linearDamping;
                colliderType = rb.colliderType;
                collider = rb.collider;
                rb.collider = nullptr;
            };

            RigidBody3D& operator = (RigidBody3D const &rb) {
                if (collider) {
                    switch(colliderType) {
                        case RIGID_SPHERE_COLLIDER: { delete (Sphere*) collider; break; }
                        case RIGID_AABB_COLLIDER:   { delete (AABB*) collider;   break; }
                        case RIGID_CUBE_COLLIDER:   { delete (Cube*) collider;   break; }
                    }
                }

                pos = rb.pos;
                mass = rb.mass;
                invMass = rb.invMass;
                cor = rb.cor;
                linearDamping = rb.linearDamping;
                colliderType = rb.colliderType;

                switch(colliderType) {
                    case RIGID_SPHERE_COLLIDER: { collider = new Sphere(*((Sphere*) rb.collider)); break; }
                    case RIGID_AABB_COLLIDER:   { collider = new AABB(*((AABB*) rb.collider));     break; }
                    case RIGID_CUBE_COLLIDER:   { collider = new Cube(*((Cube*) rb.collider));     break; }
                    case RIGID_NONE:            { collider = nullptr;                              break; }
                }

                return *this;
            };

            RigidBody3D& operator = (RigidBody3D &&rb) {
                if (this != &rb) {
                    pos = std::move(rb.pos);
                    mass = rb.mass;
                    invMass = rb.invMass;
                    cor = rb.cor;
                    linearDamping = rb.linearDamping;
                    colliderType = rb.colliderType;
                    collider = rb.collider;
                    rb.collider = nullptr;
                }

                return *this;
            };

            ~RigidBody3D() {
                switch(colliderType) {
                    case RIGID_SPHERE_COLLIDER: { delete (Sphere*) collider; break; }
                    case RIGID_AABB_COLLIDER:   { delete (AABB*) collider;   break; }
                    case RIGID_CUBE_COLLIDER:   { delete (Cube*) collider;   break; }
                }
            };


            // * =======================
            // * Fields and Functions
            // * =======================

            // * Handle and store the collider.

            RigidBodyCollider colliderType;
            void* collider;

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
            ZMath::Vec3D vel; // velocity of the rigidbody.
            ZMath::Vec3D netForce; // sum of all forces acting on the rigidbody.

            void update(ZMath::Vec3D const &g, float dt) {
                // ? assuming g is gravity, and it is already negative
                netForce += g * mass;
                vel += (netForce * invMass) * dt;
                pos += vel * dt;

                vel *= linearDamping;
                netForce = ZMath::Vec3D();

                // Update the pos of the collider.
                switch(colliderType) {
                    case RIGID_SPHERE_COLLIDER: { ((Sphere*) collider)->c = pos; break; }
                    case RIGID_AABB_COLLIDER:   { ((AABB*) collider)->pos = pos; break; }
                    case RIGID_CUBE_COLLIDER:   { ((Cube*) collider)->pos = pos; break; }
                }
            };
    };

    class StaticBody3D {
        public:
            StaticBody3D() {}; // Default constructor to make the compiler happy (for efficiency).

            /**
             * @brief Create a 3D staticbody.
             * 
             * @param pos The centerpoint of the staticbody.
             * @param colliderType The type of collider attached to the staticbody. This should be set to STATIC_NONE if there will not be one attached.
             * @param collider A pointer to the collider of the static body. If this does not match the colliderType specified, it will
             *                   cause undefined behvior to occur. If you specify STATIC_NONE, this should be set to nullptr. 
             */
            StaticBody3D(ZMath::Vec3D const &pos, StaticBodyCollider colliderType, void* collider)
                    : pos(pos), colliderType(colliderType), collider(collider) {};


            // * ===================
            // * Rule of 5 Stuff
            // * ===================

            // Create a 3D staticbody from another staticbody.
            StaticBody3D(StaticBody3D const &sb) {
                pos = sb.pos;
                colliderType = sb.colliderType;

                switch(colliderType) {
                    case STATIC_PLANE_COLLIDER:  { collider = new Plane(*((Plane*) sb.collider));   break; }
                    case STATIC_SPHERE_COLLIDER: { collider = new Sphere(*((Sphere*) sb.collider)); break; }
                    case STATIC_AABB_COLLIDER:   { collider = new AABB(*((AABB*) sb.collider));     break; }
                    case STATIC_CUBE_COLLIDER:   { collider = new Cube(*((Cube*) sb.collider));     break; }
                    case STATIC_NONE:            { collider = nullptr;                              break; }
                }
            };

            // Create a 3D staticbody from another staticbody.
            StaticBody3D(StaticBody3D &&sb) {
                pos = std::move(sb.pos);
                colliderType = sb.colliderType;
                collider = sb.collider;
                sb.collider = nullptr;
            };

            StaticBody3D& operator = (StaticBody3D const &sb) {
                if (collider) {
                    switch(colliderType) {
                        case STATIC_PLANE_COLLIDER:  { delete (Plane*) collider;  break; }
                        case STATIC_SPHERE_COLLIDER: { delete (Sphere*) collider; break; }
                        case STATIC_AABB_COLLIDER:   { delete (AABB*) collider;   break; }
                        case STATIC_CUBE_COLLIDER:   { delete (Cube*) collider;   break; }
                    }
                }

                pos = sb.pos;
                colliderType = sb.colliderType;

                switch(colliderType) {
                    case STATIC_PLANE_COLLIDER:  { collider = new Plane(*((Plane*) sb.collider));   break; }
                    case STATIC_SPHERE_COLLIDER: { collider = new Sphere(*((Sphere*) sb.collider)); break; }
                    case STATIC_AABB_COLLIDER:   { collider = new AABB(*((AABB*) sb.collider));     break; }
                    case STATIC_CUBE_COLLIDER:   { collider = new Cube(*((Cube*) sb.collider));     break; }
                    case STATIC_NONE:            { collider = nullptr;                              break; }
                }

                return *this;
            };

            StaticBody3D& operator = (StaticBody3D &&sb) {
                if (this != &sb) {
                    pos = std::move(sb.pos);
                    colliderType = sb.colliderType;
                    collider = sb.collider;
                    sb.collider = nullptr;
                }

                return *this;
            };

            ~StaticBody3D() {
                switch(colliderType) {
                    case STATIC_PLANE_COLLIDER:  { delete (Plane*) collider;  break; }
                    case STATIC_SPHERE_COLLIDER: { delete (Sphere*) collider; break; }
                    case STATIC_AABB_COLLIDER:   { delete (AABB*) collider;   break; }
                    case STATIC_CUBE_COLLIDER:   { delete (Cube*) collider;   break; }
                }
            };


            // * ========================
            // * Fields and Functions
            // * ========================

            // * Information related to the static body.

            ZMath::Vec3D pos; // centerpoint of the staticbody.

            // * Handle and store the collider.

            StaticBodyCollider colliderType;
            void* collider;
        };
}
