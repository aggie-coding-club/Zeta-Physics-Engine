#ifndef STATICBODY_H
#define STATICBODY_H

#include "primitives.h"

namespace Primitives {
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
}

#endif // ! STATICBODY_H
