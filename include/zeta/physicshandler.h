#pragma once

#include "collisions.h"
#include "octree.h"
#include <stdexcept>

// todo also provide an implementation based on 3 fixed levels of dense grids with row based optimizations as an alternative

// todo go through the destructors and make it so the actual bodies are only deleted if the user calls a cleanup or deleteBodies function
// todo make it so removing a body doesn't also free the memory of that body


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

namespace Zeta {
    // Resolve a collision between two rigidbodies.
    static void applyImpulse(RigidBody3D* rb1, RigidBody3D* rb2, CollisionManifold const &manifold) {
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
    static void applyImpulse(RigidBody3D* rb, StaticBody3D* sb, CollisionManifold const &manifold) {
        float J = ((ZMath::abs(rb->vel) * -(1 + rb->cor)) * manifold.normal)/rb->invMass;
        rb->vel -= manifold.normal * (rb->invMass * J);
    };
}

#ifdef DISABLE_SPATIAL_PARTITIONING
// physics handler without spatial partitioning

namespace Zeta {
    // * ==============
    // * Wrappers
    // * ==============

    // these are also likely just placeholders until we can find a good value
    static const int startingSlots = 64;
    static const int halfStartingSlots = 32;

    // ? For now, default to allocating 64 slots for Objects. Probably up once we start implementing more stuff.

    typedef struct RigidBodies {
        RigidBody3D** rigidBodies = nullptr; // list of active rigid bodies
        int capacity; // current max capacity
        int count; // number of rigid bodies 
    } RBS;

    typedef struct StaticBodies {
        StaticBody3D** staticBodies = nullptr; // list of active static bodies
        int capacity; // current max capacity
        int count; // number of static bodies
    } SBS;

    typedef struct RigidCollisionWrapper {
        RigidBody3D** bodies1 = nullptr; // list of colliding bodies (Object A)
        RigidBody3D** bodies2 = nullptr; // list of colliding bodies (Object B)
        Manifold* manifolds = nullptr; // list of the collision manifolds between the objects

        int capacity; // current max capacity
        int count; // number of collisions
    } RCol;

    typedef struct RigidStaticCollisionWrapper {
        RigidBody3D** rbs = nullptr;
        StaticBody3D** sbs = nullptr;
        Manifold* manifolds = nullptr;

        int capacity;
        int count;
    } RSCol;


    // * ========================
    // * Main Physics Handler
    // * ========================

    class Handler {
        private:
            // * =================
            // * Attributes
            // * =================

            RBS rbs; // rigid bodies to update
            SBS sbs; // static bodies to check for collisions with
            RCol rCol; // collisions between rigid bodies
            RSCol rsCol; // collisions between rigid and static bodies

            float updateStep; // amount of dt to update after
            static const int IMPULSE_ITERATIONS = 6; // number of times to apply the impulse update.


            // * ==============================
            // * Functions for Ease of Use
            // * ==============================

            inline void addCollision(RigidBody3D* rb1, RigidBody3D* rb2, Manifold const &manifold) {
                if (rCol.count == rCol.capacity) { // 99.9% of the time this part of the code will not execute. This is for an edge case.
                    rCol.capacity *= 2;

                    RigidBody3D** temp1 = new RigidBody3D*[rCol.capacity];
                    RigidBody3D** temp2 = new RigidBody3D*[rCol.capacity];
                    Manifold* temp3 = new Manifold[rCol.capacity];

                    for (int i = 0; i < rCol.count; i++) {
                        temp1[i] = rCol.bodies1[i];
                        temp2[i] = rCol.bodies2[i];
                        temp3[i] = std::move(rCol.manifolds[i]);
                    }

                    delete[] rCol.bodies1;
                    delete[] rCol.bodies2;
                    delete[] rCol.manifolds;

                    rCol.bodies1 = temp1;
                    rCol.bodies2 = temp2;
                    rCol.manifolds = temp3;
                }

                rCol.bodies1[rCol.count] = rb1;
                rCol.bodies2[rCol.count] = rb2;
                rCol.manifolds[rCol.count++] = manifold;
            };

            inline void addCollision(RigidBody3D* rb, StaticBody3D* sb, Manifold const &manifold) {
                if (rsCol.count == rsCol.capacity) {
                    rsCol.capacity *= 2;

                    RigidBody3D** temp1 = new RigidBody3D*[rsCol.capacity];
                    StaticBody3D** temp2 = new StaticBody3D*[rsCol.capacity];
                    Manifold* temp3 = new Manifold[rsCol.capacity];

                    for (int i = 0; i < rsCol.count; ++i) {
                        temp1[i] = rsCol.rbs[i];
                        temp2[i] = rsCol.sbs[i];
                        temp3[i] = std::move(rsCol.manifolds[i]);
                    }

                    delete[] rsCol.rbs;
                    delete[] rsCol.sbs;
                    delete[] rsCol.manifolds;

                    rsCol.rbs = temp1;
                    rsCol.sbs = temp2;
                    rsCol.manifolds = temp3;
                }

                rsCol.rbs[rsCol.count] = rb;
                rsCol.sbs[rsCol.count] = sb;
                rsCol.manifolds[rsCol.count++] = manifold;
            };

            inline void clearCollisions() {
                // ? We do not need to check for nullptrs because if this function is reached it is guarenteed none of the pointers inside of here will be NULL

                // * rCol

                int halfRbs = rbs.capacity/2;

                delete[] rCol.bodies1;
                delete[] rCol.bodies2;

                for (int i = 0; i < rCol.count; ++i) { delete[] rCol.manifolds[i].contactPoints; }
                delete[] rCol.manifolds;

                rCol.bodies1 = new RigidBody3D*[halfRbs];
                rCol.bodies2 = new RigidBody3D*[halfRbs];
                rCol.manifolds = new Manifold[halfRbs];

                rCol.capacity = halfRbs;
                rCol.count = 0;


                // * rsCol

                delete[] rsCol.rbs;
                delete[] rsCol.sbs;

                for (int i = 0; i < rsCol.count; ++i) { delete[] rsCol.manifolds[i].contactPoints; }
                delete[] rsCol.manifolds;

                rsCol.rbs = new RigidBody3D*[halfRbs];
                rsCol.sbs = new StaticBody3D*[halfRbs];
                rsCol.manifolds = new Manifold[halfRbs];

                rsCol.capacity = halfRbs;
                rsCol.count = 0;
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

                // * Bodies

                rbs.rigidBodies = new RigidBody3D*[startingSlots];
                rbs.capacity = startingSlots;
                rbs.count = 0;

                sbs.staticBodies = new StaticBody3D*[startingSlots];
                sbs.capacity = startingSlots;
                sbs.count = 0;


                // * Collisions

                rCol.bodies1 = new RigidBody3D*[halfStartingSlots];
                rCol.bodies2 = new RigidBody3D*[halfStartingSlots];
                rCol.manifolds = new Manifold[halfStartingSlots];
                rCol.capacity = halfStartingSlots;
                rCol.count = 0;

                rsCol.rbs = new RigidBody3D*[halfStartingSlots];
                rsCol.sbs = new StaticBody3D*[halfStartingSlots];
                rsCol.manifolds = new Manifold[halfStartingSlots];
                rsCol.capacity = halfStartingSlots;
                rsCol.count = 0;
            };

            // Do not allow for construction from an existing physics handler.
            Handler(Handler const &handler) { throw std::runtime_error("PhysicsHandler object CANNOT be constructed from another PhysicsHandler."); };
            Handler(Handler&& handler) { throw std::runtime_error("PhysicsHandler object CANNOT be constructed from another PhysicsHandler."); };

            // The physics handler cannot be reassigned.
            Handler& operator = (Handler const &handler) { throw std::runtime_error("PhysicsHandler object CANNOT be reassigned to another PhysicsHandler."); };
            Handler& operator = (Handler&& handler) { throw std::runtime_error("PhysicsHandler object CANNOT be reassigned to another PhysicsHandler."); };

            ~Handler() { // todo could probs combine some of the loops together
                // If one of the pointers is not NULL, none of them are.
                if (rbs.rigidBodies) {
                    for (int i = 0; i < rbs.count; ++i) { delete rbs.rigidBodies[i]; }
                    delete[] rbs.rigidBodies;

                    // ? Note: we do not need to delete each RigidBody pointer in bodies1 and bodies2 as the main rigidBodies list
                    // ?       is guarenteed to contain those same pointers.
                    delete[] rCol.bodies1;
                    delete[] rCol.bodies2;
                    
                    delete[] rsCol.rbs;
                    delete[] rsCol.sbs;

                    // ? We do not need to check for nullptr for contactPoints as if rCol.count > 0, it is guarenteed manifolds[i].contactPoints != nullptr.
                    for (int i = 0; i < rCol.count; ++i) { delete[] rCol.manifolds[i].contactPoints; }
                    delete[] rCol.manifolds;

                    for (int i = 0; i < rsCol.count; ++i) { delete[] rsCol.manifolds[i].contactPoints; }
                    delete[] rsCol.manifolds;
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
            void addRigidBodies(RigidBody3D** rbs, int size) {
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

            // Will go through an array of rigid bodies, look for them in the handler and remove if founc
            // Returns -1 if all rigid bodies found and removed
            // If not all rigid bodies in the array are in the handler, it will return the index of the first rigid body not found in the handler 
            int removeRigidBodies(RigidBody3D** rbs, int size) {
                for(int i = 0; i < size; ++i) {
                    for(int j = 0; j < this->rbs.count; ++j) {
                        if(this->rbs.rigidBodies[j] == rbs[i]) {
                            delete rbs[i];
                            for(int k = j; k < this->rbs.count - 1; ++k) {
                                this->rbs.rigidBodies[k] = this->rbs.rigidBodies[k + 1];
                            }
                            this->rbs.count--;
                            break;
                        } else if(j == this->rbs.count - 1 && this->rbs.rigidBodies[j] != rbs[i]) {
                            return i;
                        }
                    }
                }
                return -1;
            }
            // * ============================
            // * StaticBody List Functions
            // * ============================

            // Add a static body to the list of rigid bodies to be updated.
            void addStaticBody(StaticBody3D* sb) {
                if (sbs.count == sbs.capacity) {
                    sbs.capacity *= 2;
                    StaticBody3D** temp = new StaticBody3D*[sbs.capacity];

                    for (int i = 0; i < sbs.count; ++i) { temp[i] = sbs.staticBodies[i]; }

                    delete[] sbs.staticBodies;
                    sbs.staticBodies = temp;
                }

                sbs.staticBodies[sbs.count++] = sb;
            };

            // Add a list of static bodies to be updated
            void addStaticBodies(StaticBody3D** sbs, int size) {
                if (this->sbs.count + size - 1 >= this->sbs.capacity) {
                    this->sbs.capacity += size;
                    StaticBody3D** temp = new StaticBody3D*[this->sbs.capacity];

                    for (int i = 0; i < this->sbs.count; ++i) { temp[i] = this->sbs.staticBodies[i]; }

                    delete[] this->sbs.staticBodies;
                    this->sbs.staticBodies = temp;
                }

                for (int i = 0; i < size; ++i) { this->sbs.staticBodies[this->sbs.count++] = sbs[i]; }
            };

            // Remove a static body.
            // This returns 1 if the static body is found and removed and 0 if it was not found.
            // If the static body is found, the data pointed to by sb gets deleted by this function.
            bool removeStaticBody(StaticBody3D* sb) {
                for (int i = sbs.count; i >= 0; --i) {
                    if (sbs.staticBodies[i] == sb) {
                        delete sb;
                        for (int j = i; j < sbs.count - 1; ++j) { sbs.staticBodies[j] = sbs.staticBodies[j + 1]; }
                        sbs.count--;
                        return 1;
                    }
                }

                return 0;
            };

            //Tries to remove all static bodies in an array of static bodies
            //Returns 1 if all found in the handler and deleted
            //Returns 0 if any of the bodies in sbs are not found in the handler (none of them are deleted in this case)
            bool removeStaticBodies(StaticBody3D** sbs, int size) {
                int* indices = new int[size];
                for(int i = 0; i < size; ++i) {
                    for(int j = 0; j < this->sbs.count; ++j) {
                        if(this->sbs.staticBodies[j] == sbs[i]) {
                            indices[i] = j;
                            break;
                        } else if(j == this->sbs.count - 1 && this->sbs.staticBodies[j] != sbs[i]) {
                            delete[] indices;
                            return 0;
                        }
                    }
                }
                for(int i = 0; i < size; ++i) {
                    delete sbs[i];
                    for(int j = indices[i]; j < this->sbs.count - 1; ++j) { 
                        this->sbs.staticBodies[j] = this->sbs.staticBodies[j + 1]; 
                    }
                    this->sbs.count--;
                }
                delete[] indices;
                return 1;
            }

            // * ============================
            // * Main Physics Functions
            // * ============================

            // Update the physics.
            // dt will be updated to the appropriate value after the updates run for you so DO NOT modify it yourself.
            int update(float &dt) {
                int count = 0;

                // todo combine the loops together later with an equation
                while (dt >= updateStep) {
                    // Broad phase: collision detection
                    for (int i = 0; i < rbs.count - 1; ++i) {
                        for (int j = i + 1; j < rbs.count; ++j) {
                            Manifold result = findCollisionFeatures(rbs.rigidBodies[i], rbs.rigidBodies[j]);
                            if (result.hit) { addCollision(rbs.rigidBodies[i], rbs.rigidBodies[j], result); }
                        }

                        for (int j = 0; j < sbs.count; ++j) {
                            Manifold result = findCollisionFeatures(sbs.staticBodies[j], rbs.rigidBodies[i]);
                            if (result.hit) { addCollision(rbs.rigidBodies[i], sbs.staticBodies[j], result); }
                        }
                    }

                    // todo update to not be through iterative deepening -- look into this in the future
                    // todo use spacial partitioning
                    // Narrow phase: Impulse resolution
                    for (int k = 0; k < IMPULSE_ITERATIONS; ++k) {
                        for (int i = 0; i < rCol.count; ++i) {
                            applyImpulse(rCol.bodies1[i], rCol.bodies2[i], rCol.manifolds[i]);
                        }

                        for (int i = 0; i < rsCol.count; ++i) {
                            applyImpulse(rsCol.rbs[i], rsCol.sbs[i], rsCol.manifolds[i]);
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

#else
// physics handler using spatial partitioning

namespace Zeta {
    enum BodyType {
        RIGID_BODY,
        STATIC_BODY,
        KINEMATIC_BODY
    };


    // * ========================
    // * Main Physics Handler
    // * ========================

    class Handler {
        private:
            // these are also likely just placeholders until we can find a good value
            const static uint8_t startingSlots = 64;
            const static uint8_t halfStartingSlots = 32;

            struct Body {
                void* body;
                BodyType type;
            };


            // * =================
            // * Attributes
            // * =================

            Octree partitions;

            Body* bodies;
            uint32_t capacity;
            uint32_t count;

            float updateStep; // amount of dt to update after


            // * ==============================
            // * Functions for Ease of Use
            // * ==============================

            inline void grow() {
                capacity << 1;
                Body* temp = new Body[capacity];

                for (uint32_t i = 0; i < count; ++i) { temp[i] = std::move(bodies[i]); }

                delete[] bodies;
                bodies = temp;
            };

            inline void grow(uint32_t size) {
                capacity += size;
                Body* temp = new Body[capacity];

                for (uint32_t i = 0; i < count; ++i) { temp[i] = std::move(bodies[i]); }

                delete[] bodies;
                bodies = temp;
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
                if (timeStep < FPS_60) { updateStep = FPS_60; } // hard cap at 60 FPS

                // Initialize the bodies list.
                bodies = new Body[startingSlots];
                capacity = startingSlots;
                count = 0;
            };

            // Do not allow for construction from an existing physics handler.
            Handler(Handler const &handler) { throw std::runtime_error("PhysicsHandler object CANNOT be constructed from another PhysicsHandler."); };
            Handler(Handler&& handler) { throw std::runtime_error("PhysicsHandler object CANNOT be constructed from another PhysicsHandler."); };

            // The physics handler cannot be reassigned.
            Handler& operator = (Handler const &handler) { throw std::runtime_error("PhysicsHandler object CANNOT be reassigned to another PhysicsHandler."); };
            Handler& operator = (Handler&& handler) { throw std::runtime_error("PhysicsHandler object CANNOT be reassigned to another PhysicsHandler."); };

            // Note: The destructor does NOT delete the actual bodies inside of the array.
            // This is in case you need to use the bodies after the handler is out of scope.
            // You will need to cleanup the memory from the bodies or call the cleanup function to do it for you.
            ~Handler() { delete[] bodies; };


            // * =======================
            // * Body List Functions
            // * =======================

            // Add a body to the list of bodies to be updated.
            // Note: adding a nullptr to the handler will result in undefined behavior.
            void addBody(void* body, BodyType type) {
                if (count == capacity) { grow(); }
                bodies[count++] = {body, type};
            };

            // Add a list of bodies to be updated.
            // Note: adding any number of nullptrs to the handler will result in undefined behavior.
            void addBodies(void** bodies, BodyType type, uint32_t size) {
                if (count + size - 1 >= capacity) { grow(size); }
                for (uint32_t i = 0; i < size; ++i) { this->bodies[count++] = {bodies[i], type}; }
            };

            // Remove a body.
            // This returns 1 if the body is found and removed and 0 if it was not found.
            // The body will NOT be freed when found. You MUST free the memory yourself.
            bool removeBody(void* body) {
                for (uint32_t i = 0; i < count; ++i) {
                    if (body == bodies[i].body) { // this should be fine since both are just addresses anyway
                        for (uint32_t j = i; j < count - 1; ++j) { bodies[j] = bodies[j + 1]; }
                        --count;
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

                // todo combine the loops together later with an equation
                while (dt >= updateStep) {
                    

                    dt -= updateStep;
                    ++count;
                }

                return count;
            };
    };
}

#endif
