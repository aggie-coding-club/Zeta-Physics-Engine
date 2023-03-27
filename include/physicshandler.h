#ifndef PHYSICS_HANDLER_H
#define PHYSICS_HANDLER_H

#include "primitives.h"

namespace PhysicsHandler {
    namespace { // make this struct private to this file
        // ? This struct can be expanded to accomidate kineticbodies too as we can just add a second list
        // ? For now, default to allocating 8 slots for RigidBodies. Probably up once we start implementing more stuff.

        struct Objects {
            Primitives::RigidBody3D* rigidbodies; // list of active rigidbodies
            int capacity; // current max capacity
            int count; // number of ridigbodies 
        };
    }

    class Handler {
        private:
            ZMath::Vec3D g; // gravity
            Objects objects; // objects to update

        public:
            // * ===================================
            // * Constructors, Destructors, Etc.
            // * ===================================

            // todo add stuff for the rule of 5

            /**
             * @brief Make a physics handler with a default gravity value of -9.8.
             * 
             */
            Handler() : g(ZMath::Vec3D(0, 0, -9.8f)) {
                objects.rigidbodies = new Primitives::RigidBody3D[8];
                objects.capacity = 8;
                objects.count = 0;
            };

            /**
             * @brief Make a physics handler with custom gravity.
             * 
             * @param grav (Vec3D) The force applied by gravity.
             */
            Handler(ZMath::Vec3D const &grav) : g(grav) {
                objects.rigidbodies = new Primitives::RigidBody3D[8];
                objects.capacity = 8;
                objects.count = 0;
            };

            /**
             * @brief Make a physics handler from an exisiting handler.
             * 
             * @param handler (Handler) The physics handler to copy.
             */
            Handler(Handler const &handler) : g (handler.g) {
                objects.capacity = handler.objects.capacity;
                objects.count = handler.objects.count;

                for (int i = 0; i < objects.count; i++) { objects.rigidbodies[i] = handler.objects.rigidbodies[i]; }
            };

            /**
             * @brief Make a physics handler from an existing handler.
             * 
             * @param handler (Handler) The physics handler to move.
             */
            Handler(Handler&& handler) : g(handler.g) {
                objects.rigidbodies = handler.objects.rigidbodies;
                objects.capacity = handler.objects.capacity;
                objects.count = handler.objects.count;

                handler.objects.rigidbodies = nullptr;
            };

            Handler& operator = (Handler const &handler) {
                g = handler.g;

                delete[] objects.rigidbodies;

                objects.rigidbodies = new Primitives::RigidBody3D[handler.objects.capacity];
                objects.capacity = handler.objects.capacity;
                objects.count = handler.objects.count;

                for (int i = 0; i < objects.count; i++) { objects.rigidbodies[i] = handler.objects.rigidbodies[i]; }

                return *this;
            };

            Handler& operator = (Handler&& handler) {
                if (this != &handler) { // ensure no self assignment
                    g = handler.g;

                    delete[] objects.rigidbodies;

                    objects.rigidbodies = handler.objects.rigidbodies;
                    objects.capacity = handler.objects.capacity;
                    objects.count = handler.objects.count;

                    handler.objects.rigidbodies = nullptr;
                }

                return *this;
            };

            ~Handler() { delete[] objects.rigidbodies; };


            // * ===========================
            // * Object List Functions
            // * ===========================

            // Add a rigidbody to the list of objects to be updated by the physics handler.
            void addRigidbody(Primitives::RigidBody3D const &rb) {
                if (objects.count == objects.capacity) {
                    objects.capacity *= 2;
                    Primitives::RigidBody3D* temp = new Primitives::RigidBody3D[objects.capacity];

                    for (int i = 0; i < objects.count; i++) { temp[i] = objects.rigidbodies[i]; }

                    delete[] objects.rigidbodies;
                    objects.rigidbodies = temp;
                }

                objects.rigidbodies[objects.count] = rb;
                objects.count++;
            };

            // Remove a rigidbody at the index from the list of objects to be updated by the physics handler.
            void removeRigidbody(int index) {
                for (int i = index; i < objects.count - 1; i++) { objects.rigidbodies[i] = objects.rigidbodies[i + 1]; }
                objects.count--;
            };


            // * ============================
            // * Main Physics Functions
            // * ============================

            void update(float dt) {
                // * Add code to handle collision manifolds and impulse resolution here

                // Update our rigidbodies
                for (int i = 0; i < objects.count; i++) { Primitives::updateRigidbody(objects.rigidbodies[i], g, dt); }
            };
    };
}

#endif // ! PHYSICS_HANDLER_H
