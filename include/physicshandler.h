#ifndef PHYSICS_HANDLER_H
#define PHYSICS_HANDLER_H

#include <collisions.h>

// todo test the physics handler

namespace PhysicsHandler {
    // * =========================
    // * Impulse Resolution
    // * =========================

    // Resolve a collision between two rigidbodies.
    void applyImpulse(Primitives::RigidBody3D &rb1, Primitives::RigidBody3D &rb2, Collisions::CollisionManifold const &manifold) {
        // delta v = J/m
        // For this calculation we need to acocunt for the relative velocity between the two objects
        // v_r = v_1 - v_2
        // To determine the total velocity of the collision: v_j = -(1 + cor)v_r dot collisionNormal
        // Impulse then equals: J = v_j/(invMass_1 + invMass_2)
        // v_1' = v_1 + invMass_1 * J * collisionNormal
        // v_2' = v_2 - invMass_2 * J * collisionNormal. Note the - is to account for the direction which the normal is pointing.
        // It's opposite for one of the two objects.

        // ! signs might be messed up

        float J = (((rb1.vel - rb2.vel) * -(1 + rb1.cor * rb2.cor)) * manifold.normal)/(rb1.invMass + rb2.invMass);
        // ! only add the commented line if we end up finding this is insufficient for impulse resolution
        // J /= manifold.numPoints; // we know the numPoints must be at least 1 for a collision to have occurred

        rb1.vel += manifold.normal * (rb1.invMass * J);
        rb2.vel -= manifold.normal * (rb2.invMass * J);
    };


    // todo might wanna move this object wrapper elsewhere.

    // * ==============
    // * Wrappers
    // * ==============

    class Object {
        public:
            // The collider associated with the physics object
            Primitives::Collider3D collider;

            // Create a physics object.
            Object() {};

            // Create a physics object with a predefined collider.
            Object(const Primitives::Collider3D &collider) {
                (this->collider).type = collider.type;

                switch (collider.type) {
                    case Primitives::SPHERE_COLLIDER:
                        (this->collider).sphere = collider.sphere;
                        break;

                    case Primitives::AABB_COLLIDER:
                        (this->collider).aabb = collider.aabb;
                        break;

                    case Primitives::CUBE_COLLIDER:
                        (this->collider).cube = collider.cube;
                        break;
                }
            };

            void update(ZMath::Vec3D const &g, float dt) {
                if (collider.type == Primitives::SPHERE_COLLIDER) { collider.sphere.rb.update(g, dt); }
                else if (collider.type == Primitives::AABB_COLLIDER) { collider.aabb.rb.update(g, dt); }
                else if (collider.type == Primitives::CUBE_COLLIDER) { collider.cube.rb.update(g, dt); }
            };
    };

    namespace { // make this struct private to this file
        // todo Definitely refactor to only need to store rigidbodies
        // todo could probably also shrink the lists at certain times
        // todo possibly start the lists at lower values
        // ? For now, default to allocating 8 slots for Objects. Probably up once we start implementing more stuff.

        struct Objects {
            Object* objects; // list of active objects
            int capacity; // current max capacity
            int count; // number of objects 
        };

        struct CollisionWrapper {
            Primitives::RigidBody3D* bodies1; // list of colliding bodies (Object A)
            Primitives::RigidBody3D* bodies2; // list of colliding bodies (Object B)
            Collisions::CollisionManifold* manifolds; // list of the collision manifolds between the objects

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

            ZMath::Vec3D g; // gravity
            Objects objs; // objects to update
            CollisionWrapper colWrapper; // collision information
            float updateStep; // amount of dt to update after
            int IMPULSE_ITERATIONS = 6; // number of times to apply the impulse update.


            // * ==============================
            // * Functions for Ease of Use
            // * ==============================

            inline void addCollision(Primitives::RigidBody3D const &rb1, Primitives::RigidBody3D const &rb2, Collisions::CollisionManifold const &manifold) {
                if (colWrapper.count == colWrapper.capacity) {
                    colWrapper.capacity *= 2;

                    Primitives::RigidBody3D* temp1 = new Primitives::RigidBody3D[colWrapper.capacity];
                    Primitives::RigidBody3D* temp2 = new Primitives::RigidBody3D[colWrapper.capacity];
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
                delete[] colWrapper.bodies1;
                delete[] colWrapper.bodies2;
                delete[] colWrapper.manifolds;

                colWrapper.bodies1 = new Primitives::RigidBody3D[4];
                colWrapper.bodies2 = new Primitives::RigidBody3D[4];
                colWrapper.manifolds = new Collisions::CollisionManifold[4];

                colWrapper.capacity = 4;
                colWrapper.count = 0;
            };

        public:
            // * ===================================
            // * Constructors, Destructors, Etc.
            // * ===================================

            // Make a physics handler with a default gravity of -9.8 and an update speed of 60FPS.
            Handler() : g(ZMath::Vec3D(0, 0, -9.8f)), updateStep(0.0167f) {
                objs.objects = new Object[8];
                objs.capacity = 8;
                objs.count = 0;

                colWrapper.bodies1 = new Primitives::RigidBody3D[4];
                colWrapper.bodies2 = new Primitives::RigidBody3D[4];
                colWrapper.manifolds = new Collisions::CollisionManifold[4];
                colWrapper.capacity = 4;
                colWrapper.count = 0;
            };

            /**
             * @brief Make a physics handler with custom gravity and an update speed of 60FPS.
             * 
             * @param grav (Vec3D) The force applied by gravity.
             */
            Handler(ZMath::Vec3D const &grav) : g(grav), updateStep(0.0167f) {
                objs.objects = new Object[8];
                objs.capacity = 8;
                objs.count = 0;

                colWrapper.bodies1 = new Primitives::RigidBody3D[4];
                colWrapper.bodies2 = new Primitives::RigidBody3D[4];
                colWrapper.manifolds = new Collisions::CollisionManifold[4];
                colWrapper.capacity = 4;
                colWrapper.count = 0;
            };

            /**
             * @brief Make a physics handler with custom gravity and a custom update speed.
             * 
             * @param grav (Vec3D) The force applied by gravity.
             * @param updateStep (float) The amount of time in seconds that must pass before the handler updates.
             */
            Handler(ZMath::Vec3D const &grav, float updateStep) : g(grav), updateStep(updateStep) {
                objs.objects = new Object[8];
                objs.capacity = 8;
                objs.count = 0;

                colWrapper.bodies1 = new Primitives::RigidBody3D[4];
                colWrapper.bodies2 = new Primitives::RigidBody3D[4];
                colWrapper.manifolds = new Collisions::CollisionManifold[4];
                colWrapper.capacity = 4;
                colWrapper.count = 0;
            };

            /**
             * @brief Make a physics handler from an exisiting handler.
             * 
             * @param handler (Handler) The physics handler to copy.
             */
            Handler(Handler const &handler) : g (handler.g), updateStep(handler.updateStep) {
                objs.capacity = handler.objs.capacity;
                objs.count = handler.objs.count;
                objs.objects = new Object[objs.capacity];

                for (int i = 0; i < objs.count; i++) { objs.objects[i] = handler.objs.objects[i]; }

                // ==================================================================================

                colWrapper.capacity = handler.colWrapper.capacity;
                colWrapper.count = handler.colWrapper.count;
                colWrapper.bodies1 = new Primitives::RigidBody3D[colWrapper.capacity];
                colWrapper.bodies2 = new Primitives::RigidBody3D[colWrapper.capacity];
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
                objs.objects = handler.objs.objects;
                objs.capacity = handler.objs.capacity;
                objs.count = handler.objs.count;

                colWrapper.bodies1 = handler.colWrapper.bodies1;
                colWrapper.bodies2 = handler.colWrapper.bodies2;
                colWrapper.manifolds = handler.colWrapper.manifolds;
                colWrapper.capacity = handler.colWrapper.capacity;
                colWrapper.count = handler.colWrapper.count;

                handler.objs.objects = nullptr;
                handler.colWrapper.bodies1 = nullptr;
                handler.colWrapper.bodies2 = nullptr;
                handler.colWrapper.manifolds = nullptr;

                handler.objs.capacity = 0;
                handler.objs.count = 0;
                handler.colWrapper.capacity = 0;
                handler.colWrapper.count = 0;
            };

            Handler& operator = (Handler const &handler) {
                g = handler.g;
                updateStep = handler.updateStep;

                delete[] objs.objects;
                delete[] colWrapper.bodies1;
                delete[] colWrapper.bodies2;
                delete[] colWrapper.manifolds;

                objs.capacity = handler.objs.capacity;
                objs.count = handler.objs.count;
                objs.objects = new Object[objs.capacity];

                for (int i = 0; i < objs.count; i++) { objs.objects[i] = handler.objs.objects[i]; }

                // ===================================================================================

                colWrapper.capacity = handler.colWrapper.capacity;
                colWrapper.count = handler.colWrapper.count;
                colWrapper.bodies1 = new Primitives::RigidBody3D[colWrapper.capacity];
                colWrapper.bodies2 = new Primitives::RigidBody3D[colWrapper.capacity];
                colWrapper.manifolds = new Collisions::CollisionManifold[colWrapper.capacity];

                for (int i = 0; i < colWrapper.count; i++) {
                    colWrapper.bodies1[i] = handler.colWrapper.bodies1[i];
                    colWrapper.bodies2[i] = handler.colWrapper.bodies2[i];
                    colWrapper.manifolds[i] = handler.colWrapper.manifolds[i];
                }

                return *this;
            };

            Handler& operator = (Handler&& handler) {
                if (this != &handler) { // ensure no self assignment
                    g = handler.g;
                    updateStep = handler.updateStep;

                    delete[] objs.objects;
                    delete[] colWrapper.bodies1;
                    delete[] colWrapper.bodies2;
                    delete[] colWrapper.manifolds;

                    objs.objects = handler.objs.objects;
                    objs.capacity = handler.objs.capacity;
                    objs.count = handler.objs.count;

                    colWrapper.bodies1 = handler.colWrapper.bodies1;
                    colWrapper.bodies2 = handler.colWrapper.bodies2;
                    colWrapper.manifolds = handler.colWrapper.manifolds;
                    colWrapper.capacity = handler.colWrapper.capacity;
                    colWrapper.count = handler.colWrapper.count;

                    handler.objs.objects = nullptr;
                    handler.colWrapper.bodies1 = nullptr;
                    handler.colWrapper.bodies2 = nullptr;
                    handler.colWrapper.manifolds = nullptr;

                    handler.objs.capacity = 0;
                    handler.objs.count = 0;
                    handler.colWrapper.capacity = 0;
                    handler.colWrapper.count = 0;
                }

                return *this;
            };

            ~Handler() {
                delete[] objs.objects;
                delete[] colWrapper.bodies1;
                delete[] colWrapper.bodies2;
                delete[] colWrapper.manifolds;
            };


            // * ===========================
            // * Object List Functions
            // * ===========================

            // Add an object to the list of objects to be updated by the physics handler.
            void addObject(Object const &obj) {
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

            // Remove an object at the index from the list of objects to be updated by the physics handler.
            void removeObject(int index) {
                for (int i = index; i < objs.count - 1; i++) { objs.objects[i] = objs.objects[i + 1]; }
                objs.count--;
            };


            // * ============================
            // * Main Physics Functions
            // * ============================

            void update(float dt) {
                // Broad phase: collision detection
                for (int i = 0; i < objs.count - 1; i++) {
                    for (int j = i + 1; j < objs.count; j++) {
                        Collisions::CollisionManifold result = Collisions::findCollisionFeatures(objs.objects[i].collider, objs.objects[j].collider);

                        if (result.hit) {
                            // todo refactor to be more efficient
                            // ! This is just to get it working

                            switch (objs.objects[i].collider.type) {
                                case Primitives::SPHERE_COLLIDER:
                                    if (objs.objects[j].collider.type == Primitives::SPHERE_COLLIDER) { addCollision(objs.objects[i].collider.sphere.rb, objs.objects[j].collider.sphere.rb, result); }
                                    if (objs.objects[j].collider.type == Primitives::AABB_COLLIDER) { addCollision(objs.objects[i].collider.sphere.rb, objs.objects[j].collider.aabb.rb, result); }
                                    if (objs.objects[j].collider.type == Primitives::CUBE_COLLIDER) { addCollision(objs.objects[i].collider.sphere.rb, objs.objects[j].collider.cube.rb, result); }
                                    break;
                                
                                case Primitives::AABB_COLLIDER:
                                    if (objs.objects[j].collider.type == Primitives::SPHERE_COLLIDER) { addCollision(objs.objects[i].collider.aabb.rb, objs.objects[j].collider.sphere.rb, result); }
                                    if (objs.objects[j].collider.type == Primitives::AABB_COLLIDER) { addCollision(objs.objects[i].collider.aabb.rb, objs.objects[j].collider.aabb.rb, result); }
                                    if (objs.objects[j].collider.type == Primitives::CUBE_COLLIDER) { addCollision(objs.objects[i].collider.aabb.rb, objs.objects[j].collider.cube.rb, result); }
                                    break;

                                case Primitives::CUBE_COLLIDER:
                                    if (objs.objects[j].collider.type == Primitives::SPHERE_COLLIDER) { addCollision(objs.objects[i].collider.cube.rb, objs.objects[j].collider.sphere.rb, result); }
                                    if (objs.objects[j].collider.type == Primitives::AABB_COLLIDER) { addCollision(objs.objects[i].collider.cube.rb, objs.objects[j].collider.aabb.rb, result); }
                                    if (objs.objects[j].collider.type == Primitives::CUBE_COLLIDER) { addCollision(objs.objects[i].collider.cube.rb, objs.objects[j].collider.cube.rb, result); }
                                    break;

                                default:
                                    // * User defined types go here.
                                    break;
                            }
                        }
                    }
                }

                // todo update to not be through iterative deepening -- look into this in the future
                // Narrow phase: Impulse resolution
                for (int k = 0; k < IMPULSE_ITERATIONS; k++) {
                    for (int i = 0; i < colWrapper.count; i++) {
                        applyImpulse(colWrapper.bodies1[i], colWrapper.bodies2[i], colWrapper.manifolds[i]);
                    }
                }

                clearCollisions();

                // Update our rigidbodies
                for (int i = 0; i < objs.count; i++) { objs.objects[i].update(g, dt); }
            };
    };
}

#endif // ! PHYSICS_HANDLER_H
