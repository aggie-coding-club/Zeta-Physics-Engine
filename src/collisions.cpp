#include "collisions.h"
#include <cmath>

namespace Collisions {
    ZMath::Vec3D collisionNormal(Primitives::Sphere const &sphere1, Primitives::Sphere const &sphere2, float &dist) {
        // ! get rid of dist call if possible as it uses a sqrt
        float d = sphere1.rb.pos.dist(sphere2.rb.pos);
        dist = sphere1.r + sphere2.r - d;
        return sphere2.rb.pos - sphere1.rb.pos; //! may need to be normalized but we'll see if we can get away without doing so        
    };
    
    ZMath::Vec3D collisionNormal(Primitives::Sphere const &sphere, Primitives::AABB const &aabb, float &dist) {

    };

    ZMath::Vec3D collisionNormal(Primitives::Sphere const &sphere, Primitives::Cube const &cube, float &dist) {

    };
    
    ZMath::Vec3D collisionNormal(Primitives::AABB const &aabb1, Primitives::AABB const &aabb2, float &dist) {

    };

    ZMath::Vec3D collisionNormal(Primitives::AABB const &aabb, Primitives::Cube const &cube, float &dist) {

    };

    ZMath::Vec3D collisionNormal(Primitives::Cube const &cube1, Primitives::Cube const &cube2, float &dist) {

    };
}
