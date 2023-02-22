#ifndef RIGIDBODY_H
#define RIGIDBODY_H

#include "zmath.h"

namespace Primitives {
    class RigidBody3D {
        private:
            ZMath::Vec3D pos; // center point of the rigidbody
            float theta, phi; // rotation with respect to the XY and XZ axes

        public:
            // * @brief Create an unrotated 3D rigid body.
            // * 
            // * @param p Center position of the rigid body.
            RigidBody3D(ZMath::Vec3D const &p) : pos(p), theta(0.0f), phi(0.0f) {};

            // * @brief Create a rotated 3D rigid body.
            // * 
            // * @param p Center position of the rigid body.
            // * @param angXY Angle the rigid body is rotated with respect to the XY plane.
            // * @param angXZ Angle the rigid body is rotated with respect to the XZ plane.
            RigidBody3D(ZMath::Vec3D const &p, float angXY, float angXZ) : pos(p), theta(angXY), phi(angXZ) {};

            // ! Optimize away later
            // * ======================
            // * Getters and Setters.
            // * ======================

            // * Get the center point of the rigid body.
            ZMath::Vec3D getPos() { return pos; };

            // * Get the center point of the rigid body.
            ZMath::Vec3D getPos() const { return pos; };

            // * Get the angle the rigid body is rotated with respect to the XY plane.
            float getTheta() { return theta; };

            // * Get the angle the rigid body is rotated with respect to the XZ plane.
            float getPhi() { return phi; };

            // * Set the center point.
            void setPos(ZMath::Vec3D const &p) { pos.set(p); };

            // * Set the angle rotated with respect to the XY plane.
            void setTheta(float theta) { this->theta = theta; };

            // * Set the angle rotated with respect to the XZ plane.
            void setPhi(float phi) { this->phi = phi; };
    };
}

#endif // !RIGIDBODY_H