#ifndef PHYSICS_HANDLER_H
#define PHYSICS_HANDLER_H

#include "primitives.h"

namespace PhysicsHandler {
    // todo might wanna move this object wrapper elsewhere.
    // todo Additionally, will redesign in the future, this is just to get it working.

    class Object {
        private:
            Primitives::Collider3D collider;

        public:
            Object() {};
            Object(Primitives::Collider3D collider) : collider(collider) {};

            void update(ZMath::Vec3D g, float dt) {
                if (collider.type == Primitives::SPHERE_COLLIDER) { Primitives::updateRigidbody(collider.sphere.rb, g, dt); }
                else if (collider.type == Primitives::AABB_COLLIDER) { Primitives::updateRigidbody(collider.aabb.rb, g, dt); }
                else if (collider.type == Primitives::CUBE_COLLIDER) { Primitives::updateRigidbody(collider.cube.rb, g, dt); }
            };

            Primitives::Collider3D getCollider() { return collider; };
    };

    namespace { // make this struct private to this file
        // ? This struct can be expanded to accomidate kineticbodies too as we can just add a second list
        // ? For now, default to allocating 8 slots for RigidBodies. Probably up once we start implementing more stuff.

        struct Objects {
            Object* objects; // list of active objects
            int capacity; // current max capacity
            int count; // number of ridigbodies 
        };
    }

    class Handler {
        private:
            ZMath::Vec3D g; // gravity
            Objects objs; // objects to update

        public:
            // * ===================================
            // * Constructors, Destructors, Etc.
            // * ===================================

            /**
             * @brief Make a physics handler with a default gravity value of -9.8.
             * 
             */
            Handler() : g(ZMath::Vec3D(0, 0, -9.8f)) {
                objs.objects = new Object[8];
                objs.capacity = 8;
                objs.count = 0;
            };

            /**
             * @brief Make a physics handler with custom gravity.
             * 
             * @param grav (Vec3D) The force applied by gravity.
             */
            Handler(ZMath::Vec3D const &grav) : g(grav) {
                objs.objects = new Object[8];
                objs.capacity = 8;
                objs.count = 0;
            };

            /**
             * @brief Make a physics handler from an exisiting handler.
             * 
             * @param handler (Handler) The physics handler to copy.
             */
            Handler(Handler const &handler) : g (handler.g) {
                objs.capacity = handler.objs.capacity;
                objs.count = handler.objs.count;

                for (int i = 0; i < objs.count; i++) { objs.objects[i] = handler.objs.objects[i]; }
            };

            /**
             * @brief Make a physics handler from an existing handler.
             * 
             * @param handler (Handler) The physics handler to move.
             */
            Handler(Handler&& handler) : g(handler.g) {
                objs.objects = handler.objs.objects;
                objs.capacity = handler.objs.capacity;
                objs.count = handler.objs.count;

                handler.objs.objects = nullptr;
            };

            Handler& operator = (Handler const &handler) {
                g = handler.g;

                delete[] objs.objects;

                objs.objects = new Object[handler.objs.capacity];
                objs.capacity = handler.objs.capacity;
                objs.count = handler.objs.count;

                for (int i = 0; i < objs.count; i++) { objs.objects[i] = handler.objs.objects[i]; }

                return *this;
            };

            Handler& operator = (Handler&& handler) {
                if (this != &handler) { // ensure no self assignment
                    g = handler.g;

                    delete[] objs.objects;

                    objs.objects = handler.objs.objects;
                    objs.capacity = handler.objs.capacity;
                    objs.count = handler.objs.count;

                    handler.objs.objects = nullptr;
                }

                return *this;
            };

            ~Handler() { delete[] objs.objects; };


            // * ===========================
            // * Object List Functions
            // * ===========================

            // Add a rigidbody to the list of objects to be updated by the physics handler.
            void addRigidbody(Object const &obj) {
                if (objs.count == objs.capacity) {
                    objs.capacity *= 2;
                    Object* temp = new Object[objs.capacity];

                    for (int i = 0; i < objs.count; i++) { temp[i] = objs.objects[i]; }

                    delete[] objs.objects;
                    objs.objects = temp;
                }

                objs.objects[objs.count] = obj;
                objs.count++;
            };

            // Remove a rigidbody at the index from the list of objects to be updated by the physics handler.
            void removeRigidbody(int index) {
                for (int i = index; i < objs.count - 1; i++) { objs.objects[i] = objs.objects[i + 1]; }
                objs.count--;
            };


            // * ============================
            // * Main Physics Functions
            // * ============================

            void update(float dt) {
                // * Add code to handle collision manifolds and impulse resolution here

                // Update our rigidbodies
                for (int i = 0; i < objs.count; i++) { objs.objects[i].update(g, dt); }
            };
    };
}

#endif // ! PHYSICS_HANDLER_H
