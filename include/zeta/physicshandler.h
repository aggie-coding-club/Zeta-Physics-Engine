#pragma once

#include "collisions.h"
#include <stdexcept>

// todo go through project and make it more memory efficient.
// todo for example: add in more std::move calls and avoid assigning an object to another object directly if possible

// todo need to add static body handling here

namespace Zeta {
    // * ====================================
    // * Common Framerates for Handler
    // * ====================================

    #define FPS_24 0.0417f
    #define FPS_30 0.0333f
    #define FPS_40 0.025f
    #define FPS_50 0.02f
    #define FPS_60 0.0167f

    // * =========================
    // * Impulse Resolution
    // * =========================

    // Resolve a collision between two rigidbodies.
    void applyImpulse(RigidBody3D* rb1, RigidBody3D* rb2, CollisionManifold const &manifold) {
        // delta v = J/m
        // For this calculation we need to acocunt for the relative velocity between the two objects
        // v_r = v_1 - v_2
        // To determine the total velocity of the collision: v_j = -(1 + cor)v_r dot collisionNormal
        // Impulse then equals: J = v_j/(invMass_1 + invMass_2)
        // v_1' = v_1 + invMass_1 * J * collisionNormal
        // v_2' = v_2 - invMass_2 * J * collisionNormal. Note the - is to account for the direction which the normal is pointing.
        // It's opposite for one of the two objects.

        float J = ((ZMath::abs(rb1->vel - rb2->vel) * -(1 + rb1->cor * rb2->cor)) * manifold.normal)/(rb1->invMass + rb2->invMass);

        rb1->vel -= manifold.normal * (rb1->invMass * J);
        rb2->vel += manifold.normal * (rb2->invMass * J);
    };

    // todo test if this is reasonable for the impulse resolution
    // Resolve a collision between a rigidbody and a staticbody.
    void applyImpulse(RigidBody3D* rb, StaticBody3D* sb, CollisionManifold const &manifold) {
        float J = ((ZMath::abs(rb->vel) * -(1 + rb->cor)) * manifold.normal)/rb->invMass;
        rb->vel -= manifold.normal * (rb->invMass * J);
    };


    // * ==============
    // * Wrappers
    // * ==============

    namespace { // make this struct private to this file
        // these are also likely just placeholders until we can find a good value
        static const int startingSlots = 64;
        static const int halfStartingSlots = 32;

        // ? For now, default to allocating 64 slots for Objects. Probably up once we start implementing more stuff.

        struct RigidBodies {
            RigidBody3D** rigidBodies = nullptr; // list of active rigid bodies
            int capacity; // current max capacity
            int count; // number of rigid bodies 
        };

        struct CollisionWrapper {
            RigidBody3D** bodies1 = nullptr; // list of colliding bodies (Object A)
            RigidBody3D** bodies2 = nullptr; // list of colliding bodies (Object B)
            CollisionManifold* manifolds = nullptr; // list of the collision manifolds between the objects

            int capacity; // current max capacity
            int count; // number of collisions
        };
    }


    // * ========================
    // * Main Physics Handler
    // * ========================

    class Handler {
        private:
            // * =================
            // * Attributes
            // * =================

            RigidBodies rbs; // rigid bodies to update
            CollisionWrapper colWrapper; // collision information
            float updateStep; // amount of dt to update after
            static const int IMPULSE_ITERATIONS = 6; // number of times to apply the impulse update.


            // * ==============================
            // * Functions for Ease of Use
            // * ==============================

            inline void addCollision(RigidBody3D* rb1, RigidBody3D* rb2, CollisionManifold const &manifold) {
                if (colWrapper.count == colWrapper.capacity) { // 99.9% of the time this part of the code will not execute. This is for an edge case.
                    colWrapper.capacity *= 2;

                    RigidBody3D** temp1 = new RigidBody3D*[colWrapper.capacity];
                    RigidBody3D** temp2 = new RigidBody3D*[colWrapper.capacity];
                    CollisionManifold* temp3 = new CollisionManifold[colWrapper.capacity];

                    for (int i = 0; i < colWrapper.count; i++) {
                        temp1[i] = colWrapper.bodies1[i];
                        temp2[i] = colWrapper.bodies2[i];
                        temp3[i] = std::move(colWrapper.manifolds[i]);
                    }

                    delete[] colWrapper.bodies1;
                    delete[] colWrapper.bodies2;
                    delete[] colWrapper.manifolds;

                    colWrapper.bodies1 = temp1;
                    colWrapper.bodies2 = temp2;
                    colWrapper.manifolds = temp3;
                }

                colWrapper.bodies1[colWrapper.count] = rb1;
                colWrapper.bodies2[colWrapper.count] = rb2;
                colWrapper.manifolds[colWrapper.count] = manifold;
                colWrapper.count++;
            };

            inline void clearCollisions() {
                // ? We do not need to check for nullptrs because if this function is reached it is guarenteed none of the pointers inside of here will be NULL

                int halfRbs = rbs.capacity/2;

                delete[] colWrapper.bodies1;
                delete[] colWrapper.bodies2;

                for (int i = 0; i < colWrapper.count; ++i) { delete[] colWrapper.manifolds[i].contactPoints; }
                delete[] colWrapper.manifolds;

                colWrapper.bodies1 = new RigidBody3D*[halfRbs];
                colWrapper.bodies2 = new RigidBody3D*[halfRbs];
                colWrapper.manifolds = new CollisionManifold[halfRbs];

                colWrapper.capacity = halfRbs;
                colWrapper.count = 0;
            };

        public:
            // * =====================
            // * Public Attributes
            // * =====================

            ZMath::Vec3D g; // gravity

            // * ===================================
            // * Constructors, Destructors, Etc.
            // * ===================================

            // Make a physics handler with a default gravity of -9.8 and an update speed of 60FPS.

            /**
             * @brief Create a physics handler.
             * 
             * @param g (Vec3D) The force applied by gravity. Default of <0, 0, -9.8f>.
             * @param timeStep (float) The amount of time in seconds that must pass before the handler updates physics.
             *    Default speed of 60FPS. Anything above 60FPS is not recommended as it can cause lag in lower end hardware.
             */
            Handler(ZMath::Vec3D const &g = ZMath::Vec3D(0, 0, -9.8f), float timeStep = FPS_60) : g(g), updateStep(timeStep) {
                if (updateStep < FPS_60) { updateStep = FPS_60; } // hard cap at 60 FPS

                rbs.rigidBodies = new RigidBody3D*[startingSlots];
                rbs.capacity = startingSlots;
                rbs.count = 0;

                colWrapper.bodies1 = new RigidBody3D*[halfStartingSlots];
                colWrapper.bodies2 = new RigidBody3D*[halfStartingSlots];
                colWrapper.manifolds = new CollisionManifold[halfStartingSlots];
                colWrapper.capacity = halfStartingSlots;
                colWrapper.count = 0;
            };

            // Do not allow for construction from an existing physics handler.
            Handler(Handler const &handler) { throw std::runtime_error("PhysicsHandler object CANNOT be constructed from another PhysicsHandler."); };
            Handler(Handler&& handler) { throw std::runtime_error("PhysicsHandler object CANNOT be constructed from another PhysicsHandler."); };

            // The physics handler cannot be reassigned.
            Handler& operator = (Handler const &handler) { throw std::runtime_error("PhysicsHandler object CANNOT be reassigned to another PhysicsHandler."); };
            Handler& operator = (Handler&& handler) { throw std::runtime_error("PhysicsHandler object CANNOT be reassigned to another PhysicsHandler."); };

            ~Handler() {
                // If one of the pointers is not NULL, none of them are.
                if (rbs.rigidBodies) {
                    for (int i = 0; i < rbs.count; ++i) { delete rbs.rigidBodies[i]; }
                    delete[] rbs.rigidBodies;

                    // ? Note: we do not need to delete each RigidBody pointer in bodies1 and bodies2 as the main rigidBodies list
                    // ?       is guarenteed to contain those same pointers.
                    delete[] colWrapper.bodies1;
                    delete[] colWrapper.bodies2;

                    // ? We do not need to check for nullptr for contactPoints as if colWrapper.count > 0, it is guarenteed manifolds[i].contactPoints != nullptr.
                    for (int i = 0; i < colWrapper.count; ++i) { delete[] colWrapper.manifolds[i].contactPoints; }
                    delete[] colWrapper.manifolds;
                }
            };


            // * ============================
            // * RigidBody List Functions
            // * ============================

            // Add a rigid body to the list of rigid bodies to be updated.
            void addRigidBody(RigidBody3D* rb) {
                if (rbs.count == rbs.capacity) {
                    rbs.capacity *= 2;
                    RigidBody3D** temp = new RigidBody3D*[rbs.capacity];

                    for (int i = 0; i < rbs.count; ++i) { temp[i] = rbs.rigidBodies[i]; }

                    delete[] rbs.rigidBodies;
                    rbs.rigidBodies = temp;
                }

                rbs.rigidBodies[rbs.count++] = rb;
            };

            // Add a list of rigid bodies to be updated
            void addRigidBody(RigidBody3D** rbs, int size) {
                if (this->rbs.count + size - 1 >= this->rbs.capacity) {
                    this->rbs.capacity += size;
                    RigidBody3D** temp = new RigidBody3D*[this->rbs.capacity];

                    for (int i = 0; i < this->rbs.count; ++i) { temp[i] = this->rbs.rigidBodies[i]; }

                    delete[] this->rbs.rigidBodies;
                    this->rbs.rigidBodies = temp;
                }

                for (int i = 0; i < size; ++i) { this->rbs.rigidBodies[this->rbs.count++] = rbs[i]; }
            };

            // Remove a rigid body.
            // This returns 1 if the rigid body is found and removed and 0 if it was not found.
            // If the rigid body is found, the data pointed to by rb gets deleted by this function.
            bool removeRigidBody(RigidBody3D* rb) {
                for (int i = rbs.count; i >= 0; --i) {
                    if (rbs.rigidBodies[i] == rb) {
                        delete rb;
                        for (int j = i; j < rbs.count - 1; ++j) { rbs.rigidBodies[j] = rbs.rigidBodies[j + 1]; }
                        rbs.count--;
                        return 1;
                    }
                }

                return 0;
            };


            // * ============================
            // * Main Physics Functions
            // * ============================

            // Update the physics.
            // dt will be updated to the appropriate value after the updates run for you so DO NOT modify it yourself.
            int update(float &dt) {
                int count = 0;

                while (dt >= updateStep) {
                    // Broad phase: collision detection
                    for (int i = 0; i < rbs.count - 1; i++) {
                        for (int j = i + 1; j < rbs.count; j++) {
                            CollisionManifold result = findCollisionFeatures(rbs.rigidBodies[i], rbs.rigidBodies[j]);
                            if (result.hit) { addCollision(rbs.rigidBodies[i], rbs.rigidBodies[j], result); }
                        }
                    }

                    // todo update to not be through iterative deepening -- look into this in the future
                    // todo use spacial partitioning
                    // Narrow phase: Impulse resolution
                    for (int k = 0; k < IMPULSE_ITERATIONS; k++) {
                        for (int i = 0; i < colWrapper.count; i++) {
                            applyImpulse(colWrapper.bodies1[i], colWrapper.bodies2[i], colWrapper.manifolds[i]);
                        }
                    }

                    clearCollisions();

                    // Update our rigidbodies
                    for (int i = 0; i < rbs.count; ++i) { rbs.rigidBodies[i]->update(g, updateStep); }

                    dt -= updateStep;
                    ++count;
                }

                return count;
            };
    };
}
