// ? Stores all the different types of bodies attached to primitives.

#pragma once

#include <utility>
#include "primitives.h"

namespace Zeta {
    enum RigidBodyCollider {
        RIGID_SPHERE_COLLIDER,
        RIGID_AABB_COLLIDER,
        RIGID_CUBE_COLLIDER,
        RIGID_TRI_PY_COLLIDER,
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

    enum KinematicBodyCollider {
        KINEMATIC_PLANE_COLLIDER,
        KINEMATIC_SPHERE_COLLIDER,
        KINEMATIC_AABB_COLLIDER,
        KINEMATIC_CUBE_COLLIDER,
        KINEMATIC_TRI_PY_COLLIDER, 
        KINEMATIC_CUSTOM_COLLIDER,
        KINEMATIC_NONE
    };

    class RigidBody3D {
        public:
            // * =====================
            // * Public Attributes
            // * =====================

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


            // * ================
            // * Constructors
            // * ================

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
             *                   cause undefined behavior to occur. If you specify RIGID_NONE, this should be set to nullptr. 
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
                    case RIGID_SPHERE_COLLIDER: { collider = new Sphere(*((Sphere*) rb.collider));                       break; }
                    case RIGID_AABB_COLLIDER:   { collider = new AABB(*((AABB*) rb.collider));                           break; }
                    case RIGID_CUBE_COLLIDER:   { collider = new Cube(*((Cube*) rb.collider));                           break; }
                    case RIGID_TRI_PY_COLLIDER: { collider = new TriangularPyramid(*((TriangularPyramid*) rb.collider)); break; }
                    case RIGID_NONE:            { collider = nullptr;                                                    break; }
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
                if (this != &rb) { 
                    if (collider) {
                        switch(colliderType) {
                            case RIGID_SPHERE_COLLIDER: { delete (Sphere*) collider;            break; }
                            case RIGID_AABB_COLLIDER:   { delete (AABB*) collider;              break; }
                            case RIGID_CUBE_COLLIDER:   { delete (Cube*) collider;              break; }
                            case RIGID_TRI_PY_COLLIDER: { delete (TriangularPyramid*) collider; break; }
                        }
                    }

                    pos = rb.pos;
                    mass = rb.mass;
                    invMass = rb.invMass;
                    cor = rb.cor;
                    linearDamping = rb.linearDamping;
                    colliderType = rb.colliderType;

                    // zero the velocity and net force
                    vel.zero();
                    netForce.zero();

                    switch(colliderType) {
                        case RIGID_SPHERE_COLLIDER: { collider = new Sphere(*((Sphere*) rb.collider));                       break; }
                        case RIGID_AABB_COLLIDER:   { collider = new AABB(*((AABB*) rb.collider));                           break; }
                        case RIGID_CUBE_COLLIDER:   { collider = new Cube(*((Cube*) rb.collider));                           break; }
                        case RIGID_TRI_PY_COLLIDER: { collider = new TriangularPyramid(*((TriangularPyramid*) rb.collider)); break; }
                        case RIGID_NONE:            { collider = nullptr;                                                    break; }
                    }
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

                    // zero the velocity and net force
                    vel.zero();
                    netForce.zero();
                }

                return *this;
            };

            ~RigidBody3D() {
                switch(colliderType) {
                    case RIGID_SPHERE_COLLIDER: { delete (Sphere*) collider;            break; }
                    case RIGID_AABB_COLLIDER:   { delete (AABB*) collider;              break; }
                    case RIGID_CUBE_COLLIDER:   { delete (Cube*) collider;              break; }
                    case RIGID_TRI_PY_COLLIDER: { delete (TriangularPyramid*) collider; break; }
                }
            };


            // * ===================
            // * Update Function
            // * ===================

            void update(ZMath::Vec3D const &g, float dt) {
                // ? assuming g is gravity, and it is already negative
                netForce += g * mass;
                vel += (netForce * invMass) * dt;
                pos += vel * dt;

                vel *= linearDamping;
                netForce.zero();

                // Update the pos of the collider.
                switch(colliderType) {
                    case RIGID_SPHERE_COLLIDER: { ((Sphere*) collider)->c = pos;              break; }
                    case RIGID_AABB_COLLIDER:   { ((AABB*) collider)->pos = pos;              break; }
                    case RIGID_CUBE_COLLIDER:   { ((Cube*) collider)->pos = pos;              break; }
                    case RIGID_TRI_PY_COLLIDER: { ((TriangularPyramid*) collider)->pos = pos; break; }
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
                if (this != &sb) {
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

    class KinematicBody3D {
        public:
            KinematicBody3D() {}; // Default constructor to make the compiler happy (for efficiency).

            /**
                * @brief Create a 3D KinematicBody.
                * 
                * @param pos Centerpoint of the kinematicbody.
                * @param mass Mass of the kinematicbody.
                * @param linearDamping The linear damping of the kinematicbody. This should fall on (0, 1].
                * @param colliderType The type of collider attached to the kinematicbody. This should be set to KINEMATIC_NONE if there will not be one attached.
                * @param collider A pointer to the collider of the kinematicbody. If this does not match the colliderType specified, it will
                *                   cause undefined behavior to occur. If you specify KINEMATIC_NONE, this should be set to nullptr. 
                */
            KinematicBody3D(ZMath::Vec3D const &pos, float mass, float cor, float linearDamping, KinematicBodyCollider colliderType, void* collider) 
                    : pos(pos), mass(mass), invMass(1.0f/mass), cor(cor), linearDamping(linearDamping),
                        colliderType(colliderType), collider(collider) {};


            // * ===================
            // * Rule of 5 Stuff
            // * ===================

            KinematicBody3D(KinematicBody3D const &kb) {
                pos = kb.pos;
                colliderType = kb.colliderType;
                mass = kb.mass;
                invMass = kb.invMass;
                cor = kb.cor;
                linearDamping = kb.linearDamping;

                switch(colliderType) {
                    case KINEMATIC_SPHERE_COLLIDER: { collider = new Sphere(*(Sphere*) kb.collider);                        break; }
                    case KINEMATIC_AABB_COLLIDER:   { collider = new AABB(*(AABB*) kb.collider);                            break; }
                    case KINEMATIC_CUBE_COLLIDER:   { collider = new Cube(*(Cube*) kb.collider);                            break; }
                    case KINEMATIC_TRI_PY_COLLIDER: { collider = new TriangularPyramid(*(TriangularPyramid*) kb.collider);  break; }
                    case KINEMATIC_NONE:            { collider = nullptr;                                                   break; }
                }
            };

            KinematicBody3D(KinematicBody3D &&kb) {
                pos = std::move(kb.pos);
                colliderType = kb.colliderType;
                collider = kb.collider;

                mass = kb.mass;
                invMass = kb.invMass;
                cor = kb.cor;
                linearDamping = kb.linearDamping;

                kb.collider = nullptr;
            };

            KinematicBody3D& operator = (KinematicBody3D const &kb) {
                if (this != &kb) {
                    if (collider) {
                        switch(colliderType) {
                            case KINEMATIC_SPHERE_COLLIDER: { delete (Sphere*) collider;            break; }
                            case KINEMATIC_AABB_COLLIDER:   { delete (AABB*) collider;              break; }
                            case KINEMATIC_CUBE_COLLIDER:   { delete (Cube*) collider;              break; }
                            case KINEMATIC_TRI_PY_COLLIDER: { delete (TriangularPyramid*) collider; break; }
                        }
                    }

                    pos = kb.pos;
                    colliderType = kb.colliderType;
                    mass = kb.mass;
                    invMass = kb.invMass;
                    cor = kb.cor;
                    linearDamping = kb.linearDamping;

                    switch(colliderType) {
                        case KINEMATIC_SPHERE_COLLIDER:   { collider = new Sphere(*(Sphere*) kb.collider);                       break; }
                        case KINEMATIC_AABB_COLLIDER:     { collider = new AABB(*(AABB*) kb.collider);                           break; }
                        case KINEMATIC_CUBE_COLLIDER:     { collider = new Cube(*(Cube*) kb.collider);                           break; }
                        case KINEMATIC_TRI_PY_COLLIDER:   { collider = new TriangularPyramid(*(TriangularPyramid*) kb.collider); break; }
                        case KINEMATIC_NONE:              { collider = nullptr;                                                  break; }
                    }

                    // zero the velocity and net force
                    vel.zero();
                    netForce.zero();
                }

                return *this;
            };

            KinematicBody3D& operator = (KinematicBody3D &&kb) {
                if (this != &kb) {
                    pos = std::move(kb.pos);
                    colliderType = kb.colliderType;
                    collider = kb.collider;
                    kb.collider = nullptr;

                    mass = kb.mass;
                    invMass = kb.invMass;
                    cor = kb.cor;
                    linearDamping = kb.linearDamping;

                    // zero the velocity and net force
                    vel.zero();
                    netForce.zero();
                }

                return *this;
            };

            ~KinematicBody3D() {
                switch(colliderType) {
                    case KINEMATIC_SPHERE_COLLIDER:   { delete (Sphere*) collider;            break; }
                    case KINEMATIC_AABB_COLLIDER:     { delete (AABB*) collider;              break; }
                    case KINEMATIC_CUBE_COLLIDER:     { delete (Cube*) collider;              break; }
                    case KINEMATIC_TRI_PY_COLLIDER:   { delete (TriangularPyramid*) collider; break; }
                }
            };


            // * ========================
            // * Fields and Functions
            // * ========================

            // * Information related to the kinematic body.

            float mass; // Must remain constant.
            float invMass; // 1/mass. Must remain constant.

            // Coefficient of Restitution.
            // Represents a loss of kinetic energy due to heat.
            // Between 0 and 1 for our purposes.
            // 1 = perfectly elastic.
            float cor;

            // Linear damping.
            // Acts as linear friction on the kinematicbody.
            float linearDamping;

            ZMath::Vec3D pos; // centerpoint of the kinematicbody.
            ZMath::Vec3D vel; // velocity of the kinematicbody.
            ZMath::Vec3D netForce; // sum of all forces acting on the kinematicbody.

            // * Handle and store the collider.

            KinematicBodyCollider colliderType;
            void* collider;

            void update(ZMath::Vec3D const &g, float dt) {
                // ? This assumes that g (gravity) is already negative.
                netForce += g * mass;
                vel += (netForce * invMass) * dt;
                pos += vel *dt;

                vel *= linearDamping;
                netForce.zero();

                // Update the pos of the collider.
                switch(colliderType) {
                    case KINEMATIC_SPHERE_COLLIDER: { ((Sphere*) collider)->c = pos;              break; }
                    case KINEMATIC_AABB_COLLIDER:   { ((AABB*) collider)->pos = pos;              break; }
                    case KINEMATIC_CUBE_COLLIDER:   { ((Cube*) collider)->pos = pos;              break; }
                    case KINEMATIC_TRI_PY_COLLIDER: { ((TriangularPyramid*) collider)->pos = pos; break; }
                }
            };
    };
}
