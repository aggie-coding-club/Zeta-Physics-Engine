#ifndef PHYSICS_HANDLER_H
#define PHYSICS_HANDLER_H

#include "collisions.h"

namespace PhysicsHandler {
    // * ====================================
    // * Common Framerates for Handler
    // * ====================================

    #define FPS_24 0.0417f
    #define FPS_30 0.0333f
    #define FPS_50 0.02f
    #define FPS_60 0.0167f

    // * =========================
    // * Impulse Resolution
    // * =========================

    // Resolve a collision between two rigidbodies.
    void applyImpulse(Primitives::RigidBody3D* rb1, Primitives::RigidBody3D* rb2, Collisions::CollisionManifold const &manifold) {
        // delta v = J/m
        // For this calculation we need to acocunt for the relative velocity between the two objects
        // v_r = v_1 - v_2
        // To determine the total velocity of the collision: v_j = -(1 + cor)v_r dot collisionNormal
        // Impulse then equals: J = v_j/(invMass_1 + invMass_2)
        // v_1' = v_1 + invMass_1 * J * collisionNormal
        // v_2' = v_2 - invMass_2 * J * collisionNormal. Note the - is to account for the direction which the normal is pointing.
        // It's opposite for one of the two objects.

        float J = ((ZMath::abs(rb1->vel - rb2->vel) * -(1 + rb1->cor * rb2->cor)) * manifold.normal)/(rb1->invMass + rb2->invMass);
        // ! only add the commented line if we end up finding this is insufficient for impulse resolution
        // J /= manifold.numPoints; // we know the numPoints must be at least 1 for a collision to have occurred

        rb1->vel -= manifold.normal * (rb1->invMass * J);
        rb2->vel += manifold.normal * (rb2->invMass * J);
    };


    // * ==============
    // * Wrappers
    // * ==============

    namespace { // make this struct private to this file
        // todo rename to something better
        // these are also likely just placeholders until we can find a good value
        static const int startingSlots = 64;
        static const int halfSlots = 32;

        // ? For now, default to allocating 64 slots for Objects. Probably up once we start implementing more stuff.

        struct RigidBodies {
            Primitives::RigidBody3D** rigidBodies = nullptr; // list of active rigid bodies
            int capacity; // current max capacity
            int count; // number of rigid bodies 
        };

        // todo rename to something better
        struct CollisionWrapper {
            Primitives::RigidBody3D** bodies1 = nullptr; // list of colliding bodies (Object A)
            Primitives::RigidBody3D** bodies2 = nullptr; // list of colliding bodies (Object B)
            Collisions::CollisionManifold* manifolds = nullptr; // list of the collision manifolds between the objects

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
            static const int IMPULSE_ITERATIONS = 3; // number of times to apply the impulse update.


            // * ==============================
            // * Functions for Ease of Use
            // * ==============================

            inline void addCollision(Primitives::RigidBody3D* rb1, Primitives::RigidBody3D* rb2, Collisions::CollisionManifold const &manifold) {
                if (colWrapper.count == colWrapper.capacity) {
                    colWrapper.capacity *= 2;

                    Primitives::RigidBody3D** temp1 = new Primitives::RigidBody3D*[colWrapper.capacity];
                    Primitives::RigidBody3D** temp2 = new Primitives::RigidBody3D*[colWrapper.capacity];
                    Collisions::CollisionManifold* temp3 = new Collisions::CollisionManifold[colWrapper.capacity];

                    for (int i = 0; i < colWrapper.count; i++) {
                        temp1[i] = colWrapper.bodies1[i];
                        temp2[i] = colWrapper.bodies2[i];
                        temp3[i] = colWrapper.manifolds[i];
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
                // todo could refactor to base this around the current count of rigidbodies
                delete[] colWrapper.bodies1;
                delete[] colWrapper.bodies2;
                delete[] colWrapper.manifolds;

                colWrapper.bodies1 = new Primitives::RigidBody3D*[halfSlots];
                colWrapper.bodies2 = new Primitives::RigidBody3D*[halfSlots];
                colWrapper.manifolds = new Collisions::CollisionManifold[halfSlots];

                colWrapper.capacity = halfSlots;
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
                rbs.rigidBodies = new Primitives::RigidBody3D*[startingSlots];
                rbs.capacity = startingSlots;
                rbs.count = 0;

                colWrapper.bodies1 = new Primitives::RigidBody3D*[halfSlots];
                colWrapper.bodies2 = new Primitives::RigidBody3D*[halfSlots];
                colWrapper.manifolds = new Collisions::CollisionManifold[halfSlots];
                colWrapper.capacity = halfSlots;
                colWrapper.count = 0;
            };

            /**
             * @brief Make a physics handler from an exisiting handler.
             * 
             * @param handler (Handler) The physics handler to copy.
             */
            Handler(Handler const &handler) : g(handler.g), updateStep(handler.updateStep) {
                rbs.capacity = handler.rbs.capacity;
                rbs.count = handler.rbs.count;
                rbs.rigidBodies = new Primitives::RigidBody3D*[rbs.capacity];

                for (int i = 0; i < rbs.count; ++i) { rbs.rigidBodies[i] = handler.rbs.rigidBodies[i]; }

                // ==================================================================================

                colWrapper.capacity = handler.colWrapper.capacity;
                colWrapper.count = handler.colWrapper.count;
                colWrapper.bodies1 = new Primitives::RigidBody3D*[colWrapper.capacity];
                colWrapper.bodies2 = new Primitives::RigidBody3D*[colWrapper.capacity];
                colWrapper.manifolds = new Collisions::CollisionManifold[colWrapper.capacity];

                for (int i = 0; i < colWrapper.count; i++) {
                    colWrapper.bodies1[i] = handler.colWrapper.bodies1[i];
                    colWrapper.bodies2[i] = handler.colWrapper.bodies2[i];
                    colWrapper.manifolds[i] = handler.colWrapper.manifolds[i];
                }
            };

            /**
             * @brief Make a physics handler from an existing handler.
             * 
             * @param handler (Handler) The physics handler to move.
             */
            Handler(Handler&& handler) : g(handler.g), updateStep(handler.updateStep) {
                rbs.rigidBodies = handler.rbs.rigidBodies;
                rbs.capacity = handler.rbs.capacity;
                rbs.count = handler.rbs.count;

                colWrapper.bodies1 = handler.colWrapper.bodies1;
                colWrapper.bodies2 = handler.colWrapper.bodies2;
                colWrapper.manifolds = handler.colWrapper.manifolds;
                colWrapper.capacity = handler.colWrapper.capacity;
                colWrapper.count = handler.colWrapper.count;

                handler.rbs.rigidBodies = nullptr;
                handler.colWrapper.bodies1 = nullptr;
                handler.colWrapper.bodies2 = nullptr;
                handler.colWrapper.manifolds = nullptr;

                handler.rbs.capacity = 0;
                handler.rbs.count = 0;
                handler.colWrapper.capacity = 0;
                handler.colWrapper.count = 0;
            };

            // The physics handler cannot be reassigned.
            Handler& operator = (Handler const &handler) { throw std::runtime_error("PhysicsHandler instance cannot be reassigned."); };
            Handler& operator = (Handler&& handler) { throw std::runtime_error("PhysicsHandler instance cannot be reassigned."); };

            ~Handler() {
                // If one of the pointers is not NULL, none of them are.
                if (rbs.rigidBodies) {
                    for (int i = 0; i < rbs.count; ++i) { delete rbs.rigidBodies[i]; }
                    delete[] rbs.rigidBodies;

                    for (int i = 0; i < colWrapper.count; ++i) {
                        delete colWrapper.bodies1[i];
                        delete colWrapper.bodies2[i];
                    }

                    delete[] colWrapper.bodies1;
                    delete[] colWrapper.bodies2;
                    delete[] colWrapper.manifolds;
                }
            };


            // * ============================
            // * RigidBody List Functions
            // * ============================

            // Add a rigid body to the list of rigid bodies to be updated.
            void addRigidBody(Primitives::RigidBody3D* rb) {
                if (rbs.count == rbs.capacity) {
                    rbs.capacity *= 2;
                    Primitives::RigidBody3D** temp = new Primitives::RigidBody3D*[rbs.capacity];

                    for (int i = 0; i < rbs.count; ++i) { temp[i] = rbs.rigidBodies[i]; }

                    delete[] rbs.rigidBodies;
                    rbs.rigidBodies = temp;
                }

                rbs.rigidBodies[rbs.count++] = rb;
            };

            // Remove a rigid body at the given index.
            void removeRigidBody(int index) {
                delete rbs.rigidBodies[index];
                for (int i = index; i < rbs.count; ++i) { rbs.rigidBodies[i] = rbs.rigidBodies[i + 1]; }
                rbs.count--;
            };


            // * ============================
            // * Main Physics Functions
            // * ============================

            // Update the physics.
            // dt will be updated to the appropriate value after the updates run for you so DO NOT modify it yourself.
            void update(float &dt) {
                while (dt >= updateStep) {
                    // Broad phase: collision detection
                    for (int i = 0; i < rbs.count - 1; i++) {
                        for (int j = i + 1; j < rbs.count; j++) {
                            Collisions::CollisionManifold result = Collisions::findCollisionFeatures(rbs.rigidBodies[i], rbs.rigidBodies[j]);
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
                }
            };
    };
}

#endif // ! PHYSICS_HANDLER_H
