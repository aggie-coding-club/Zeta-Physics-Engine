#ifndef RIGIDBODY_H
#define RIGIDBODY_H

#include "zmath.h"

namespace Primitives {
    class RigidBody3D {
        private:
            float mass; // mass must remain constant 
            float mass_inverse;
        
        public:
            ZMath::Vec3D pos; // center point of the rigidbody
            float theta; // rotation with respect to the XY plane
            float phi; // rotation with respect to the XZ plane
            ZMath::Vec3D velocity;
            
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
}

#endif // !RIGIDBODY_H