// ? Stores all the different types of bodies attached to primitives.

#ifndef BODIES_H
#define BODIES_H

#include "zmath.h"

namespace Primitives {
    // todo make these into structs (well at least rigidbody and staticbody)

    class RigidBody3D {
        private:
            float mass; // mass must remain constant 
            float mass_inverse;
        
        public:
            ZMath::Vec3D pos; // center point of the rigidbody
            float theta; // rotation with respect to the XY plane
            float phi; // rotation with respect to the XZ plane
            ZMath::Vec3D velocity;
            ZMath::Vec3D netForce; // net force as a vector
            
            // @brief Create an unrotated 3D rigid body.
            // 
            // @param p Center position of the rigid body.
            RigidBody3D(ZMath::Vec3D const &p, float mass) : pos(p), mass(mass), mass_inverse(1 / mass), theta(0.0f), phi(0.0f) {};

            // @brief Create a rotated 3D rigid body.
            // 
            // @param p Center position of the rigid body.
            // @param angXY Angle the rigid body is rotated with respect to the XY plane.
            // @param angXZ Angle the rigid body is rotated with respect to the XZ plane.
            RigidBody3D(ZMath::Vec3D const &p, float mass, float angXY, float angXZ) : pos(p), mass(mass), mass_inverse(1 / mass), theta(angXY), phi(angXZ) {};

            float & get_mass() {return mass;}
            float & get_inverse_mass() {return mass_inverse;}
    };

    class StaticBody3D {
        public:
            const ZMath::Vec3D pos; // centerpoint of the staticbody
            const float theta; // rotation with respect to the XY plane
            const float phi; // rotation with respect to the XZ plane
            
            // @brief Create an unrotated StaticBody3D object.
            // 
            // @param p Center position of the static body.
            StaticBody3D(ZMath::Vec3D const &p) : pos(p), theta(0.0f), phi(0.0f) {};

            // @brief Create a rotated StaticBody3D object.
            // 
            // @param p Center position of the static body.
            // @param angXY Angle rotated with respect to the XY plane.
            // @param angXZ Angle rotated with respect to the XZ plane.
            StaticBody3D(ZMath::Vec3D const &p, float angXY, float angXZ) : pos(p), theta(angXY), phi(angXZ) {};
    };

    // ! Just a flag class. I'm sure there's a better way to do this so I'll update it once I get the chance to look into it.
    // ! Rn I'm dealing with too much architecture all at once to figure this out so once I don't have to worry about that so much I will.
    class Collider3D {};
}

#endif // !RIGIDBODY_H